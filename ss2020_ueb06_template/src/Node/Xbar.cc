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

#include "Xbar.h"
#include "../Packets/IntFlowCtrl_m.h"
#include "../Packets/IntFlowCtrlRouteRequest_m.h"
#include "../Packets/Flit_m.h"
#include "TopologyHelper.h"
#include "../debug.h"

Define_Module(Xbar);


void Xbar::initialize() {
    routeRequests = new cQueue("routeRequests");
    rTableSrcToDest = new cArray("rTableSrcToDest");
    rTableDestToSrc = new cArray("rTableDestToSrc");

    for (int port = 0; port <= (int)par("PORTS"); ++port) {
        rTableSrcToDest->add(new cArray("vcMap"));
        rTableDestToSrc->add(new cArray("vcMap"));
    }
    MY_NODE_COORD = TopologyHelper::calculateNodeLocationFromId((int)par("NODE_ID"), (const char *)par("NODE_COUNT"));
    NODE_COUNT = TopologyHelper::getNodeCount((const char *)par("NODE_COUNT"));

    WATCH_VECTOR(NODE_COUNT);
    WATCH_VECTOR(MY_NODE_COORD);
}

Xbar::~Xbar() {
    delete routeRequests;
    delete rTableSrcToDest;
    delete rTableDestToSrc;
}

/**
 * Implements the DOR algorithm on a Mesh. Should return a
 * LogicalPort (contains destination Port and Virtual Channel).
 * If no routing could be determined, nullptr is returned.
 * This method uses only 1 virtual channel (vID=0).
 * @param dAddr ID of the target node. This is a number. It must
 *              be translated into coordinate-space in order to
 *              be useful.
 * @return The LogicalPort with VID=0 (always) and the target port.
 *         The routing priority is defined through the index of the
 *         dimensions (X is routed first, then Y, then Z...).
 *         If dAddr equals the current node ID, this method should
 *         return nullptr.
 */
LogicalPort* Xbar::calculateRoutingDORMesh(int dAddr) {
    const char *nodeSpecifier = (const char *)par("NODE_COUNT");
    // If the destination is Node (3,5), destNodeCoord would hold:
    // destNodeCoord[0] = 3 ; destNodeCoord[1] = 5.
    std::vector<int> destNodeCoord = TopologyHelper::calculateNodeLocationFromId(dAddr, nodeSpecifier);

    //TODO: Implement this. Needs roughly 10 lines of code.

    return nullptr;
}

/**
 * This method implements the DOR algorithm on a generic cube.
 * It generates the LogicalPort of the target port and Virtual Channel, where
 * this packet should be routed next. The implementation may choose to alter
 * the virtual channel during routing.
 * The implementation should make use of virtual channels to guarantee
 * deadlock-free routing.
 * If the routing dimension changes (e.g. from X to Y), the VID should be
 * reset to 0, in order to make the routing identical over each dimension.
 *
 * @param dAddr ID of the target node. This is a number. It must
 *              be translated into coordinate-space in order to
 *              be useful.
 * @param sPid Source Port ID. The ID of the port (LineDriver or ApplicationLineDriver)
 *             on which the packet was received.
 *             The first sPids correspond to the respective LineDriver ports.
 *             If sPid==par("PORTS"), the packet originated from the
 *             ApplicationLineDriver.
 * @param sVid Source Virtual Channel ID. The ID of the virtual channel on which
 *             the packet was received.
 * @return The LogicalPort with the target VID and the target port.
 *         The routing priority is defined through the index of the
 *         dimensions (X is routed first, then Y, then Z...).
 *         If dAddr equals the current node ID, this method should
 *         return nullptr.
 */
LogicalPort* Xbar::calculateRoutingDORCube(int dAddr, int sPid, int sVid)  {
    const char *nodeSpecifier = (const char *)par("NODE_COUNT");
    std::vector<int> destNodeCoord = TopologyHelper::calculateNodeLocationFromId(dAddr, nodeSpecifier);

    //TODO: Implement this. Needs roughly 30 lines of code.

    return nullptr;
}

LogicalPort* Xbar::calculateRouting(int sAddr, int dAddr, int sPid, int sVid) {
    LogicalPort *resultPort = nullptr;
    if ((dAddr == (int)par("NODE_ID")) && !(destChannelBusy((int)par("PORTS"), 0))) {
        resultPort = new LogicalPort ( (int)par("PORTS"), 0);
    } else {
        if ((bool)par("TOPO_CUBE")){
            resultPort = calculateRoutingDORCube(dAddr, sPid, sVid);
        } else {
            resultPort = calculateRoutingDORMesh(dAddr);
        }
    }
    // Only do the routing, if the destination is free.
    if ((resultPort != nullptr) &&
            destChannelBusy(resultPort->getPort(), resultPort->getVirtualChannel())) {
        delete resultPort;
        resultPort = nullptr;
    }
    return resultPort;
}


bool Xbar::destChannelBusy(int dPid, int dVid) {
    return (getRouteSource(dPid, dVid) != nullptr);
}

LogicalPort* Xbar::getRouteDestination(int sPid, int sVid) {
    return (LogicalPort *) (((cArray *)rTableSrcToDest->get(sPid))->get(sVid));
}

LogicalPort* Xbar::getRouteSource(int dPid, int dVid) {
    return (LogicalPort *)  (((cArray *)rTableDestToSrc->get(dPid))->get(dVid));
}


void Xbar::addRoute(int sPid, int sVid, int dPid, int dVid) {
    LogicalPort *lp = new LogicalPort(dPid, dVid);
    addRoute(sPid, sVid, lp);
}

void Xbar::addRoute(int sPid, int sVid, LogicalPort *destLp) {
    int dPid = destLp->getPort();
    int dVid = destLp->getVirtualChannel();
    bool s2dClear = (getRouteDestination(sPid, sVid) == nullptr);
    bool d2sClear = (getRouteSource(dPid, dVid) == nullptr);
    if (s2dClear && d2sClear) {
        LogicalPort *sLp = new LogicalPort(sPid, sVid);
        ((cArray *)rTableSrcToDest->get(sPid))->addAt(sVid, destLp);
        ((cArray *)rTableDestToSrc->get(dPid))->addAt(dVid, sLp);
    } else {
        INFO(("ERROR: Trying to add a route that would overwrite an existing route. Delete the old route first.\n"));
    }
}

bool Xbar::deleteRoute(int sPid, int sVid) {
    LogicalPort *dLp = getRouteDestination(sPid, sVid);
    if (dLp == nullptr) {
        return false;
    } else {
        int dPid = dLp->getPort();
        int dVid = dLp->getVirtualChannel();
        bool d2sPresent = (getRouteSource(dPid, dVid) != nullptr);
        if (d2sPresent) {
            cObject *o1 = ((cArray *)rTableSrcToDest->get(sPid))->remove(sVid);
            cObject *o2 = ((cArray *)rTableDestToSrc->get(dPid))->remove(dVid);
            delete o1;
            delete o2;
        } else {
            INFO(("ERROR: Trying to delete a route with not D2S Mapping.\n"));
        }
        return true;
    }
}

void Xbar::relaySrcToDest(int sPid, int sVid, cPacket *innerPacket, bool flowCtrl) {
    LogicalPort *dPort = getRouteDestination(sPid, sVid);
    if (dPort != nullptr) {
        VirtualChannelFrame *wrap = new VirtualChannelFrame();
        wrap->setVirtualChannel(dPort->getVirtualChannel());
        wrap->encapsulate(innerPacket);
        const char *outGate = flowCtrl ? "flowCtrl$o" : "data$o";
        send(wrap, outGate, dPort->getPort());
    } else {
        INFO(("ERROR: Cannot relay (S2D) Message, because no routing-mapping exists\n"));
    }
}

void Xbar::relayDestToSrc(int dPid, int dVid, cPacket *innerPacket, bool flowCtrl) {
    LogicalPort *sPort = getRouteSource(dPid, dVid);
    if (sPort != nullptr) {
        VirtualChannelFrame *wrap = new VirtualChannelFrame();
        wrap->setVirtualChannel(sPort->getVirtualChannel());
        wrap->encapsulate(innerPacket);
        const char *outGate = flowCtrl ? "flowCtrl$o" : "data$o";
        send(wrap, outGate, sPort->getPort());
    } else {
        INFO(("ERROR: Cannot relay (D2S) Message, because no routing-mapping exists\n"));
    }
}

// We try to route every request in the queue, delete those which can be serviced
// and leave the rest untouched.
void Xbar::serviceRouting() {
    std::list<VirtualChannelFrame*> serviced;

    cQueue::Iterator it(*routeRequests);
    while (!it.end()) {
        VirtualChannelFrame *vcf = (VirtualChannelFrame*) *it;
        int gateId = vcf->getArrivalGate()->getIndex();
        int vid = vcf->getVirtualChannel();
        // Use getEncapsulated packet because the vcf must remain untouched, because
        // it potentially remains in the queue.
        IntFlowCtrlRouteRequest *routeRequest =
                (IntFlowCtrlRouteRequest*) vcf->getEncapsulatedPacket();
        LogicalPort *targetLP = calculateRouting(routeRequest->getSource(),
                routeRequest->getDestination(), gateId, vid);
        bool routeSuccess = (targetLP != nullptr);
        if (routeSuccess) {
            // Actually insert the routing
            addRoute(gateId, vid, targetLP);
            // Send TX-Request to target port
            VirtualChannelFrame *txReqWrapper = new VirtualChannelFrame();
            txReqWrapper->setVirtualChannel(targetLP->getVirtualChannel());
            IntFlowCtrl *txReq = new IntFlowCtrl();
            txReq->setAction(INT_TX_REQUEST);
            txReqWrapper->encapsulate(txReq);
            send(txReqWrapper, "flowCtrl$o", targetLP->getPort());
            // Remove Route requests that have been serviced.
            serviced.push_back(vcf);
        }
        ++it;
    }
    for (VirtualChannelFrame *&vcf : serviced) {
        routeRequests->remove(vcf);
        delete vcf;
    }
}

void Xbar::handleMessage(cMessage *msg) {
    bool deleteMsg = true;
    VirtualChannelFrame *vcf = (VirtualChannelFrame*) msg;
    int vid = vcf->getVirtualChannel();
    if (msg->arrivedOn("flowCtrl$i")) {
        int gateId = msg->getArrivalGate()->getIndex();
        IntFlowCtrl *fcm = (IntFlowCtrl*) vcf->getEncapsulatedPacket();
        if (fcm->getAction() == INT_NEXT_FLIT) {
            // There must be a mapping in the routing table.
            // This is a message from the Destination to the Source.
            // We can simply relay the message.
            // IMPORTANT: Do NOT use fcm for this.
            // fcm is linked to vcf. We need to extract the inner
            // packet first before relaying
            relayDestToSrc(gateId, vid, vcf->decapsulate(), true);
        } else if (fcm->getAction() == INT_ROUTE_REQUEST) {
            routeRequests->insert(vcf);
            serviceRouting();
            deleteMsg = false;
        } else if (fcm->getAction() == INT_RELEASE) {
            relaySrcToDest(gateId, vid, vcf->decapsulate(), true); // Send the Release to the DPort.
            deleteRoute(gateId, vid); // Remove the route.
            serviceRouting(); // Check, if we can route something new.
        } else {
            INFO(("ERROR: Invalid IntFlowCtrl Type in XBAR.\n"));
        }
    } else if (msg->arrivedOn("data$i")) {
        int gateId = msg->getArrivalGate()->getIndex();
        relaySrcToDest(gateId, vid, vcf->decapsulate(), false);
    } else {
        INFO(("ERROR: XBar received packed on unknown input gate.\n"));
    }
    if (deleteMsg) {
        delete msg;
    }
}
