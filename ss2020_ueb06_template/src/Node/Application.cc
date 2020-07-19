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

#include "Application.h"
#include "TopologyHelper.h"
#include "../debug.h"

Define_Module(Application);


void Application::initialize() {
    NODE_CNT = TopologyHelper::getNodeCountSingle((const char *)par("NODE_COUNT"));
    if (par("ROUTE_TEST")) {
        probeAllNodes();
    } else {
        scheduleAt(simTime(), new cMessage("App: Selfmsg"));
    }
}

void Application::probeAllNodes() {
    //TODO: Implement this.
}

void Application::checkRouteability() {
    bool success = true;

    //TODO: Insert checking code here.

    if (!success) {
        INFO(("Node %d has not received packets from all other nodes!\n", (int)par("NODE_ID")));
    }
}

void Application::finish() {
    if (par("ROUTE_TEST")) {
        checkRouteability();
    }
}

HLPacket *Application::generatePacket() {
    HLPacket *hlp = new HLPacket();
    int size = (int) par("PKT_SIZE");
    int flitSize = (int)par("FLIT_BYTES");
    /* This is a hack to make the
     * packet size divisible by the flit size.
     * This is not really necessary for anything but it
     * makes the calculation of XON and XOFF easier.
     */
    size -= (size % flitSize);
    // Message should consist of 2 Flits (header + tail).
    // Both fully filled.
    if (size < 2*flitSize) {
        size = 2*flitSize;
    }
    hlp->setSource((int) par("NODE_ID"));
    int dest = rand() % NODE_CNT; // Allow localhost-messages.
    hlp->setDestination(dest);
    hlp->setPayloadArraySize(size);
    hlp->setByteLength(size);
    for (int i = 0; i < size; ++i) {
        char c = ('a' + i) % ('z' - 'a' + 1);
        hlp->setPayload(i, c);
    }
    return hlp;
}

void Application::handleMessage(cMessage *msg) {
    if (msg->isSelfMessage()) {
        simtime_t nextSendTime = simTime() + (double)par("SEND_DELAY");
        HLPacket *hlp = generatePacket();
        send(hlp, "data$o");
        scheduleAt(nextSendTime, msg);
    } else { // Message arrived on data-line
       HLPacket *hlp = (HLPacket *) msg;
       if (hlp->getDestination() != (int) par("NODE_ID")) {
           INFO(("Packet misrouted. Should be delivered to %d, but got sent to %d\n",
                   hlp->getDescriptor(), (int) par("NODE_ID")));
       }
       delete msg;
    }
}
