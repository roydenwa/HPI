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

#ifndef __SS2020_UEB06_LINEDRIVER_H_
#define __SS2020_UEB06_LINEDRIVER_H_

#include <omnetpp.h>
#include "../Packets/Flit_m.h"
#include "../Packets/VirtualChannelFrame_m.h"

using namespace omnetpp;

class LineDriver : public cSimpleModule
{
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual ~LineDriver();
  private:
     cArray *queues;
     cArray *nextTxFlit;
     std::vector<bool> extSendAllowed;
     std::vector<bool> intSendAllowed;
     std::vector<bool> intTransmissionActive;
     std::vector<bool> extTransmissionActive;
     std::vector<bool> intTransmissionLastFlitSent;
     std::vector<int> bufferFirstMessageId;
     bool extSendNextMessageSMScheduled;
     int extLastTransmitVid;
     int queueCapacity;
     std::vector<simsignal_t> sigPacketStuckTimes;
     void sendNextFlit(int vid);
     void sendSelfMessage(simtime_t time, int vid, int kind=0);
     void deadlockCheck();
     void serviceExtTx();
     void serviceIntTx(int vId);
     void handleVcf(VirtualChannelFrame *vcf);
};

#endif
