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

#include "App.h"
#include "Packet_m.h"

Define_Module(App);


void App::initialize()
{
    // node[0] starts
    if (getIndex() == 0)
    {
            Packet *packet = generateMessage();
            scheduleAt(0.0, packet);
    }
}


void App::handleMessage(cPacket *packet)
{
    Packet *ttpacket = check_and_cast<Packet *>(packet);

    if (ttpacket->getDestination() == getIndex())
    {
        // message arrived:
        bubble("ARRIVED at destination!");
        delete ttpacket;
    }
    else
    {
        forwardMessage(ttpacket);
    }
}


Packet *App::generateMessage()
{
    // produce source and destination addresses
    int src = getIndex();  // our module index
    int n = getVectorSize();  // module vector size

    // choose destination and bit length randomly
    int dest = intuniform(0, n);
    int packetBitLength = 64 * intuniform(0, 64);

    // TODO: send packet even if dest == src

    // Create message object and set source and destination field.
    Packet *packet = new Packet();
    packet->setSource(src);
    packet->setDestination(dest);
    packet->setBitLength(packetBitLength);

    return packet;
}
