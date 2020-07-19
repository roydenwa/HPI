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

#ifndef __SS2020_UEB06_XBAR_H_
#define __SS2020_UEB06_XBAR_H_

#include <omnetpp.h>
#include "LogicalPort.h"
#include "../Packets/VirtualChannelFrame_m.h"

using namespace omnetpp;

class Xbar : public cSimpleModule
{
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual ~Xbar();
  private:
    cArray *rTableSrcToDest;
    cArray *rTableDestToSrc;
    cQueue *routeRequests;
    std::vector<int> MY_NODE_COORD;
    std::vector<int> NODE_COUNT;
    bool destChannelBusy(int dPid, int dVid);
    LogicalPort* getRouteDestination(int sPid, int sVid);
    LogicalPort* getRouteSource(int dPid, int dVid);
    LogicalPort* calculateRouting(int sAddr, int dAddr, int sPid, int sVid);
    void addRoute(int sPid, int sVid, LogicalPort *destLp);
    void addRoute(int sPid, int sVid, int dPid, int dVid);
    bool deleteRoute(int sPid, int sVid);
    void relaySrcToDest(int sPid, int sVid, cPacket *innerPacket, bool flowCtrl);
    void relayDestToSrc(int dPid, int dVid, cPacket *innerPacket, bool flowCtrl);
    void serviceRouting();
    LogicalPort* calculateRoutingDOR3DMesh(int dAddr);
    std::vector<int> calculateNodeLocationFromId(int nodeId);
    std::vector<int> getNodeCount();
    LogicalPort* calculateRoutingDORMesh(int dAddr);
    LogicalPort* calculateRoutingDORCube(int dAddr, int sPid, int sVid);
};

#endif
