//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "Inport.h"

Define_Module(Inport);

Inport::~Inport() {
    cancelAndDelete(createRelease);
    delete buffer;
}

void Inport::initialize() {
    buffer = new cQueue("PktStore");
    createRelease = new ReqGrant("Create Release");

    qlen = registerSignal("qlen");
    qtime = registerSignal("qtime");

    gate("in")->setDeliverOnReceptionStart(par("cutThrough"));
}

void Inport::handleMessage(cMessage *msg) {
    if (msg->arrivedOn("in")) {
        if (!reqActive) {

            Packet *pkt = check_and_cast<Packet*>(msg);
            int dest = pkt->getDestination();
            int src = pkt->getSource();
            std::stringstream ss;
            ss << "Req from " << src;
            ReqGrant *req = new ReqGrant(ss.str().c_str());

            req->setType(REQUEST);
            req->setOutports(dest);
            req->setSource(src);

            EV_INFO << "No Req outstanding. Generate Request to arbiter "
                           << dest << endl;
            send(req, "arbiterCtrl$o", dest);

            msg->setTimestamp();
            buffer->insert(msg);

            //one packet is temporary stored, so "real" qsize is one less
            emit(qlen, buffer->getLength());

            reqActive = true;
        } else {
            msg->setTimestamp();
            buffer->insert(msg);
            EV_INFO << "Requests are outstanding. Queue Request" << endl;
            EV_INFO << "Request waiting: " << buffer->getLength() << endl;
            //one packet is temporary stored, so "real" qsize is one less
            emit(qlen, buffer->getLength());
        }
    } else if (msg->arrivedOn("arbiterCtrl$i")) {
        ReqGrant *rg = check_and_cast<ReqGrant*>(msg);
        if (rg->getType() != GRANT) {
            EV_ERROR << "Received something other than a grant in an inport!"
                            << std::endl;
        }
        Packet *pkt = check_and_cast<Packet*>(buffer->pop());
        emit(qtime, simTime() - pkt->getTimestamp());
        int dest = pkt->getDestination();

        EV_INFO << "Received Grant. Sending packet to outport=" << dest << endl;

        send(pkt, "outp", dest);
        simtime_t duration = pkt->getDuration();
        //simtime_t duration = gate("outp",dest)->getTransmissionChannel()->getTransmissionFinishTime();

        createRelease->setOutports(dest);

        EV_INFO << "Packet duration is: " << duration * 1000000 << "us" << endl;
        //EV_INFO << "Received Grant. Packet duration is: " << (duration - simTime()) * 1000000 << endl;

        scheduleAt(simTime() + duration, createRelease);
        delete msg;

        //one packet is temporary stored, so "real" qsize is one less
        emit(qlen, buffer->getLength());
    } else {
        // self messages
        if (msg == createRelease) {

            ReqGrant *rel = new ReqGrant("Release");

            int dest = createRelease->getOutports();

            EV_INFO << "Transfer complete. Send Release to arbiter= " << dest
                           << endl;

            rel->setType(RELEASE);
            rel->setOutports(dest);

            send(rel, "arbiterCtrl$o", dest);

            if (buffer->isEmpty()) {
                reqActive = false;
                EV_INFO << "No Requests are waiting." << endl;

            } else {
                //outstanding requests in buffer
                Packet *pkt = check_and_cast<Packet*>(buffer->front());
                int dest = pkt->getDestination();
                int src = pkt->getSource();
                std::stringstream ss;
                ss << "Req from " << src;
                ReqGrant *req = new ReqGrant(ss.str().c_str());

                req->setType(REQUEST);
                req->setOutports(dest);
                req->setSource(src);

                EV_INFO  << "Req are outstanding. Generate new Request to arbiter="
                         << dest << " from queue" << endl;

                send(req, "arbiterCtrl$o", dest);
                reqActive = true;
            }
        }
    }
}
