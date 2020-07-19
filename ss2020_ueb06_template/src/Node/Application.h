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

#ifndef __SS2020_UEB06_APPLICATION_H_
#define __SS2020_UEB06_APPLICATION_H_

#include <omnetpp.h>
#include "../Packets/HLPacket_m.h"

using namespace omnetpp;

class Application : public cSimpleModule
{
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
  private:
    HLPacket *generatePacket();
    void probeAllNodes();
    void checkRouteability();
    int NODE_CNT;

    // statistics
    cHistogram hop_count;
    cHistogram packet_sizes;
    cHistogram end2end_delay;
};

#endif
