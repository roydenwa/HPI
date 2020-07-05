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

#include "App.h"

Define_Module(App);

void App::initialize()
{
    // initialize timer self message
    sendNewPacket = new cMessage("sendNewPacket");

    // save parameters in Class attributes
    numNodes = par("numNodes");
    nodeID = par("nodeID");

    //register signals for statistics
    sent = registerSignal("sent");
    rcvd = registerSignal("rcvd");
    eed = registerSignal("eed");

    // schedule first self message to get message generation started
    scheduleAt(simTime() + par("sendInterval").doubleValue(),sendNewPacket);

    gate("in")->setDeliverOnReceptionStart(par("cutThrough"));
}

App::~App()
{
    //remove packet from FutureEventList and delete it
    cancelAndDelete(sendNewPacket);
}

void App::handleMessage(cMessage *msg)
{
    if (msg == sendNewPacket) // is msg our timer self message?
    {
        // read volatile parameter
        double delta;
        delta = par("sendInterval").doubleValue();

        //schedule timer self message again in the future
        scheduleAt(simTime() + delta, sendNewPacket);

        EV_INFO << "Schedule next timer in: " << delta * 1000000 << "us" << endl;

        // randomize new Packet
        Packet* pkt = generatePacket();

        // collect statistics
        emit(sent,pkt);

        //send packet on outport
        send(pkt,"out");
    }
    else // msg is a packet
    {
        //cast cMessage object to our specialized Packet object
        Packet *pkt = check_and_cast<Packet*>(msg);

        if (pkt->getDestination() == nodeID)
        {
            EV_INFO << "Message received." << endl;

            //collect statistics
            emit(rcvd,pkt);
            emit(eed,simTime() - pkt->getCreationTime());

            // msg received at destination. Drop packet
            delete msg;
        }
        else
        {
            //wrong destination. Send it out again
            EV_INFO << "Message resend " << endl;
            send(pkt,"out");
        }
    }
}

Packet* App::generatePacket()
{
    Packet* pkt = new Packet();
    int dest = intuniform(0,numNodes - 1);

    int pktSize = par("pktSize").intValue();
    int src = nodeID;

    // randomize until destination is not source.
    // Code by Dirk Frey
    // Removed by Florian Beenen; we allow sending to
    // ourself in this exercise.
    // Comment: This implementation is not the reference, how problems
    // like this should be handled ;).
    /*do {
        dest = intuniform(0,numNodes - 1);
    } while (dest == src);*/

    // randomize new packet
    pkt->setBitLength(pktSize);
    pkt->setSource(src);
    pkt->setDestination(dest);

    EV_INFO << "Generated new packet with: Dest = " << dest << " size = " << pktSize << endl;

    return pkt;
}

