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

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <thread>
#include <chrono>

#include "App.h"
#include "Packet_m.h"

using namespace omnetpp;

Define_Module(App);


void App::initialize()
{
    send_counter = 0;
    receive_counter = 0;
    par("nodeID") = getIndex();

    send_counter_vec.setName("Send-counter-vector");
    receive_counter_vec.setName("Receive-counter-vector");
    packet_sizes.setName("Packet-sizes");
    hop_counter.setName("Hop-counter");

    // to see counters during simulation
    WATCH(send_counter);
    WATCH(receive_counter);

    // generate selfmsg to send messages repeatedly
    cMessage *selfmsg = new cMessage("Timer");
    scheduleAt(simTime(), selfmsg);
}


void App::handleMessage(cMessage *packet)
{
    if (packet->isSelfMessage())
    {
        // send actual msg (triggered by selfmsg)
        Packet *newpacket = generateMessage();

        // use forwardMessage to update hopCount immediately
        forwardMessage(newpacket);

        send_counter++;
        send_counter_vec.recordWithTimestamp(simTime(), (double)send_counter);

        // send a selfmsg again to repeat process
        double interval = (double)par("sendDelay") / 1000000;
        scheduleAt(simTime()+interval, packet);
    }
    else
    {
        receive_counter++;
        receive_counter_vec.record((double)receive_counter);

        // check and cast if packet belongs to class Packet (subclass of cMessage)
        Packet *ttpacket = check_and_cast<Packet *>(packet);

        if (ttpacket->getDestination() == getIndex())
        {
            // message arrived:
            bubble("ARRIVED at destination!");

            // add size to histogram
            int bit_length = ttpacket->getBitLength();
            packet_sizes.collect(bit_length);

            // record hop_count in hist
            int hop_count = ttpacket->getHopCount();
            hop_counter.collect(hop_count);

            delete ttpacket;
        }
        else
        {
            forwardMessage(ttpacket);
        }
    }
}


Packet *App::generateMessage()
{
    // produce source and destination addresses
    int src = par("nodeID");  // our module index
    int n = getVectorSize();  // module vector size

    // choose destination and bit length randomly
    int dest = intuniform(0, n-1);
    int packetBitLength = 64 * intuniform(1, 64);

    // create message/ packet object and set params
    Packet *packet = new Packet();
    packet->setSource(src);
    packet->setDestination(dest);
    packet->setBitLength(packetBitLength);

    return packet;
}


void App::forwardMessage(Packet *packet)
{
    // increment hop-count
    packet->setHopCount(packet->getHopCount()+1);

    EV << "Forwarding message\n";
    send(packet, "out");
}


void App::finish()
{
    // Scalar data collected by histogram-objects has to be recorded manually
    // to be accessed afterwards
    packet_sizes.recordAs("Packet-sizes");
    hop_counter.recordAs("Hop-count");
}
