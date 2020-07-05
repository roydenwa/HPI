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

#include "Arbiter.h"

Define_Module(Arbiter);

void Arbiter::initialize()
{
    //buffer. waiting for abitration / transfer
    outstanding = new cQueue("Outstanding requests");

    //register statistic signals
    qlen = registerSignal("qlen");
    qtime = registerSignal("qtime");
}

Arbiter::~Arbiter()
{
    // cleanup queue
    delete outstanding;
}



void Arbiter::handleMessage(cMessage *msg)
{
    //always cast msg to ReqGrant protocol
    ReqGrant *pkt = check_and_cast<ReqGrant* >(msg);

    if (pkt->getType() == REQUEST)
    {
        if(!grantActive) //instant grant
        {
            // no grand is active and Request received
            // send grant immediately to requester

            //toggle flag
            grantActive = true;

            // generate packet of type grant
            // save arrival gate == requesting inport in packet
            // send packet back to source of REQUEST
            int source = msg->getArrivalGate()->getIndex();
            std::stringstream ss;
            ss << "Grant to " << source;
            ReqGrant *grant = new ReqGrant(ss.str().c_str());
            grant->setType(GRANT);


            send(grant,"arbiterCtrl$o",source);
            EV_INFO << "Instant sent grant to source=" << source << endl;

            // delete requester packet
            delete msg;
        }
        else
        {
            // grant is active. Insert into queue

            // get values for debug output
            int source = pkt->getSource();
            int port = pkt->getOutports();

            // save timestamp for queue wait time and insert packet
            pkt->setTimestamp();
            outstanding->insert(pkt);

            // update queue length statistic
            emit(qlen,outstanding->getLength());
            EV_INFO << "Grant active. Queued request from " << source << " with port=" << port << endl;
        }
    }
    else // (pkt->getType() == RELEASE)
    {
        if (outstanding->isEmpty())
        {
            // Received packet is RELEASE and
            // no packet is waiting for arbitration
            grantActive = false;
            EV_INFO << "No requests queued" << endl;
        }
        else
        {
            // received packet is RELEASE and packets are waiting.
            // get waiting REQUEST from queue and send new grant

            // toggle flag and get waiting packet
            grantActive = true;
            ReqGrant *pkt = check_and_cast<ReqGrant* >(outstanding->pop());

            //save source of packet
            int source = pkt->getSource();

            // update wait time and length statistics
            emit(qlen,outstanding->getLength());
            emit(qtime,simTime() - pkt->getTimestamp());

            //delete release packet
            delete pkt;

            // generate Grant and send it to requesting source
            std::stringstream ss;
            ss << "Grant to " << source;
            ReqGrant *grant = new ReqGrant(ss.str().c_str());
            grant->setType(GRANT);


            send(grant,"arbiterCtrl$o",source);

            EV_INFO << "Sent grant to source=" << source << endl;
        }

        //delete RELEASE packet
        delete msg;
    }
}
