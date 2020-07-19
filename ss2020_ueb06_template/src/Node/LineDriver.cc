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


#include "LineDriver.h"
#include "../Packets/ExtFlowCtrl_m.h"
#include "../Packets/FlitHead_m.h"
#include "../Packets/IntFlowCtrl_m.h"
#include "../Packets/IntFlowCtrlRouteRequest_m.h"
#include "../debug.h"

Define_Module(LineDriver);

#define MSG_KIND_DL_CHECK 1
#define MSG_KIND_SERVICE_EXT 2

#define getQueue(vid) ( (cQueue*)(queues->get(vid)))

LineDriver::~LineDriver() {
    delete queues;
    delete nextTxFlit;
}

void LineDriver::initialize() {
    int vcs = par("VIRTUAL_CHANNELS");
    queues = new cArray("queues");
    nextTxFlit = new cArray("nextTxFlit");
    extLastTransmitVid = 0;
    WATCH_VECTOR(extSendAllowed);
    WATCH_VECTOR(intSendAllowed);
    WATCH_VECTOR(intTransmissionActive);
    WATCH_VECTOR(extTransmissionActive);
    WATCH_VECTOR(intTransmissionLastFlitSent);
    WATCH_VECTOR(bufferFirstMessageId);

    buffer_load.setName("Buffer-load");

    for (int i = 0; i < vcs; ++i) {
        queues->add(new cQueue());
        extSendAllowed.push_back(true);
        intSendAllowed.push_back(false);
        intTransmissionActive.push_back(false);
        extTransmissionActive.push_back(false);
        intTransmissionLastFlitSent.push_back(false);
        bufferFirstMessageId.push_back(-1);

        std::stringstream ss;
        ss << "sigPacketStuckTime-vid" << i;
        sigPacketStuckTimes.push_back(registerSignal(ss.str().c_str()));

        std::stringstream ss2;
        ss2 << "packetStuckTime-vid" << i;

        cResultFilter *warmupFilter =
                cResultFilterType::get("warmup")->create();
        cResultRecorder *vectorRecorder =
                cResultRecorderType::get("vector")->create();
        opp_string_map *attrs = new opp_string_map;
        (*attrs)["title"] = "Stuck Timer";
        (*attrs)["unit"] = "s";
        vectorRecorder->init(this, ss2.str().c_str(), "vector", nullptr, attrs);
        subscribe(sigPacketStuckTimes.at(i), warmupFilter);
        warmupFilter->addDelegate(vectorRecorder);
    }
    extSendNextMessageSMScheduled = false;
    queueCapacity = par("BUFFER_SIZE");

    cMessage *dlCheckTimer = new cMessage("LineDriver: DL-Check-Timer");
    dlCheckTimer->setKind(MSG_KIND_DL_CHECK);
    scheduleAt(simTime() + SimTime((double)par("DL_CHECK_INTERVAL")), dlCheckTimer);
}

void LineDriver::sendNextFlit(int vid) {
    IntFlowCtrl *sendNextFlit = new IntFlowCtrl();
    VirtualChannelFrame *sendNextFlitWrap = new VirtualChannelFrame();
    sendNextFlitWrap->setVirtualChannel(vid);
    sendNextFlit->setAction(INT_NEXT_FLIT);
    sendNextFlitWrap->encapsulate(sendNextFlit);
    send(sendNextFlitWrap, "intFlowCtrl$o");
}

void LineDriver::sendSelfMessage(simtime_t time, int vid, int kind) {
    VirtualChannelFrame *vcf = new VirtualChannelFrame("Selfmessage");
    vcf->setVirtualChannel(vid);
    vcf->setKind(kind);
    scheduleAt(time, vcf);
}

void LineDriver::deadlockCheck() {
    for (int vid = 0; vid < (int)par("VIRTUAL_CHANNELS"); ++vid) {
        cMessage *first = (cMessage *)(getQueue(vid)->front());
        if (first != nullptr) {
            int firstId = first->getId();
            simtime_t stuckFor = SimTime(0);
            if (firstId == bufferFirstMessageId.at(vid))  {
                stuckFor = simTime() - first->getArrivalTime();
            } else {
                bufferFirstMessageId.at(vid) = first->getId();
            }
            emit(sigPacketStuckTimes.at(vid), stuckFor);
        }
    }
}

// Handle Transmissions from XBAR Direction towards EXT.
// We do not care about the current vID, because we want to
// transmit round-robin.

void LineDriver::serviceExtTx() {
    simtime_t finishTime =
            gate("extData$o")->getTransmissionChannel()->getTransmissionFinishTime();
    if (finishTime <= simTime()) { // Channel is clear. We can send something now.
        bool nextSlotFound = false;

        // Find the next valid transmission slot. Always start search one VID after the last.
        for (int i = 1; (i <= (int) par("VIRTUAL_CHANNELS")) && !nextSlotFound;
                ++i) {
            int idx = (extLastTransmitVid + i) % (int) par("VIRTUAL_CHANNELS");
            bool itemAvail = (nextTxFlit->get(idx) != nullptr);
            bool sendAllowed = extSendAllowed.at(idx);
            nextSlotFound = itemAvail && sendAllowed;
            if (nextSlotFound) {
                extLastTransmitVid = idx;
                if (!extTransmissionActive.at(idx)) {
                    INFO(("ERROR: Was looking for sendable data towards EXT and found data without active transmission on VID %d\n", idx));
                }
            }
        }
        if (nextSlotFound) { // Else we have nothing to do: Channel clear and no data to transmit.
            Flit *flit = (Flit*) nextTxFlit->get(extLastTransmitVid); // This is guaranteed to be != nullptr
            VirtualChannelFrame *flitWrapper = new VirtualChannelFrame();
            nextTxFlit->remove(extLastTransmitVid);
            flitWrapper->setVirtualChannel(extLastTransmitVid);
            flitWrapper->encapsulate(flit);
            send(flitWrapper, "extData$o");
            sendNextFlit(extLastTransmitVid); // Make the source send the next Flit over.
            // We reset this here, because the only reasons, why a slot cannot be found is,
            // if the sending is not allowed there (XOFF) or if there is no data.
            // Both are external issues and thus we will get external triggers
            // if those are resolved. So no self-messages required.
            extSendNextMessageSMScheduled = false;
        }
    }
    // Recalculate.
    finishTime = gate("extData$o")->getTransmissionChannel()->getTransmissionFinishTime();
    // Get a timer once the channel is clear and if we still have stuff to send (on some VID).

    // We only need to schedule, if there is something to send on some VID.
    // Again we do not need selfmessages, if a channel is blocked externally.
    bool stuffToSend = false;
    int vcs = (int)par("VIRTUAL_CHANNELS");
    for (int i = 0; i < vcs && !stuffToSend; ++i) {
        stuffToSend |= (nextTxFlit->get(i) != nullptr) && extSendAllowed.at(i);
    }
    // We do not want to flood too many selfmessages, when the channel is blocked.
    if (stuffToSend &&  !extSendNextMessageSMScheduled) {
        sendSelfMessage(finishTime, 0, MSG_KIND_SERVICE_EXT);  // We dont care about the VID.
        extSendNextMessageSMScheduled = true;
    }
}


// Handle transmission from receive-buffer towards XBAR.
void LineDriver::serviceIntTx(int vId) {
    if (intTransmissionLastFlitSent.at(vId) && intSendAllowed.at(vId)) { // Close Connection, if needed.
        VirtualChannelFrame *releaseWrapper = new VirtualChannelFrame();
        IntFlowCtrl *release = new IntFlowCtrl();
        release->setAction(INT_RELEASE);
        releaseWrapper->setVirtualChannel(vId);
        releaseWrapper->encapsulate(release);
        send(releaseWrapper, "intFlowCtrl$o");
        intSendAllowed.at(vId) = false; // This is the last assignment to this variable in a cycle. Reset to false.
        if (intTransmissionActive.at(vId) && intTransmissionLastFlitSent.at(vId)) {
            intTransmissionActive.at(vId) = false;
            intTransmissionLastFlitSent.at(vId) = false;
        } else {
            INFO(("ERROR: Trying to close a connection that was not open.\n"));
        }
    }

    if (!getQueue(vId)->isEmpty()) { // Send the next flit in INT Direction
        Flit *nextFlit = (Flit*) getQueue(vId)->front(); // Only pop when transmitting flits, not when establishing route.
        if (intTransmissionActive.at(vId) && intSendAllowed.at(vId)) { // Send the actual Flits.
            if (nextFlit->getType() == FLIT_TAIL) {
                intTransmissionLastFlitSent.at(vId) = true;
            }
            getQueue(vId)->pop();
            VirtualChannelFrame *nextFlitWrap = new VirtualChannelFrame();
            nextFlitWrap->setVirtualChannel(vId);
            nextFlitWrap->encapsulate(nextFlit);
            send(nextFlitWrap, "intData$o");
            intSendAllowed.at(vId) = false;

            if ((getQueue(vId)->getLength() == (int) par("XON_THRES"))
                    && ((int) par("XON_THRES") > 0)) { // Send the XON Frame.
                VirtualChannelFrame *xonWrapper = new VirtualChannelFrame();
                xonWrapper->setVirtualChannel(vId);
                ExtFlowCtrl *xon = new ExtFlowCtrl();
                xon->setAction(XON);
                xonWrapper->encapsulate(xon);
                send(xonWrapper, "extFlowCtrl$o");
            }
        } else if (!intTransmissionActive.at(vId)
                && (nextFlit->getType() == FLIT_HEAD)) { // Send the Routing request
            VirtualChannelFrame *requestWrapper = new VirtualChannelFrame();
            FlitHead *flitHead = (FlitHead*) nextFlit;
            flitHead->setHopCnt(flitHead->getHopCnt() + 1); // Increment Hop-Counter
            requestWrapper->setVirtualChannel(vId);
            IntFlowCtrlRouteRequest *request = new IntFlowCtrlRouteRequest();
            request->setSource(flitHead->getSource());
            request->setDestination(flitHead->getDestination());
            requestWrapper->encapsulate(request);
            send(requestWrapper, "intFlowCtrl$o");
            intTransmissionActive.at(vId) = true;
        }
    }
}

void LineDriver::handleVcf(VirtualChannelFrame *vcf) {
       int vId = vcf->getVirtualChannel();
       if (vcf->arrivedOn("extFlowCtrl$i")) {
           ExtFlowCtrl *fcm = (ExtFlowCtrl *) vcf->getEncapsulatedPacket();
           extSendAllowed.at(vId) = (fcm->getAction() == XON);
           if (fcm->getAction() == XON) {
               serviceExtTx(); // Try to service some EXT Queue.
           }
       } else if (vcf->arrivedOn("intFlowCtrl$i")) {
           IntFlowCtrl *ifcm = (IntFlowCtrl *) vcf->getEncapsulatedPacket();
           if (ifcm->getAction() == INT_TX_REQUEST) { // Some other channel wants to transmit via this channel
               if (!extTransmissionActive.at(vId) && (nextTxFlit->get(vId) == nullptr)) {
                   extTransmissionActive.at(vId) = true;
                   sendNextFlit(vId);
               } else {
                   INFO(("ERROR: Received Transmission init while other transmission is still online on VID %d\n",vId));
               }
           } else if (ifcm->getAction() == INT_RELEASE) { // Transmission should be terminated.
               if (extTransmissionActive.at(vId) && nextTxFlit->get(vId) == nullptr) {
                   extTransmissionActive.at(vId) = false;
               } else {
                   INFO(("ERROR: Channel should be terminated but there is still an untransmitted flit or the channel was not active.\n"));
               }
           } else if (ifcm->getAction() == INT_NEXT_FLIT) { // This channel is the sender (towards INT) and shall transmit the next flit.
               if (intTransmissionActive.at(vId)) {
                   intSendAllowed.at(vId) = true;
                   serviceIntTx(vId); // Service the queue.
               } else {
                   INFO(("ERROR: Channel got NextFlit indicator without ongoing transmission.\n"));
               }
           }
       } else if (vcf->arrivedOn("extData$i")) {
           if ((getQueue(vId)->getLength() <= queueCapacity) && (queueCapacity > 0)) {
               Flit* flit = (Flit *) vcf->decapsulate(); //Extract the Flit from the VCF.
               getQueue(vId)->insert(flit);
               flit->setArrivalTime(simTime()); // Set the arrival time for Deadlock-Monitoring
               serviceIntTx(vId); // Service the Queue.
               if ((getQueue(vId)->getLength() == (int) par("XOFF_THRES")) && ((int) par("XOFF_THRES") > 0)) { // Send the XOFF Frame
                   VirtualChannelFrame *xoffWrapper = new VirtualChannelFrame();
                   xoffWrapper->setVirtualChannel(vId);
                   ExtFlowCtrl *xoff = new ExtFlowCtrl();
                   xoff->setAction(XOFF);
                   xoffWrapper->encapsulate(xoff);
                   send(xoffWrapper, "extFlowCtrl$o");
               }
           } else {
               INFO(("ERROR: Queue capacity exceeded at VID %d, LineDriver %d, Node %d\n", vId, getIndex(), (int)getAncestorPar("NODE_ID")));
           }
       } else if (vcf->arrivedOn("intData$i")) {
           if (extTransmissionActive.at(vId) && (nextTxFlit->get(vId) == nullptr)) {
                  Flit *flit = (Flit *) vcf->decapsulate(); // Extract the flit from the VCF.
                  nextTxFlit->addAt(vId, flit);
                  serviceExtTx(); // Send a Flit round-robin style
           } else {
               INFO(("ERROR: Got Flit from XBAR without prior Request or One-Flit-TXBuffer is occupied.\n"));
           }
       }

       // The received message can be discarded.
       // Only the data-flits are saved over multiple executions of this
       // function and they are extracted from the msg/vcf.
       delete vcf;
}

void LineDriver::handleMessage(cMessage *msg) {

    if (msg->isSelfMessage()) {
        if (msg->getKind() == MSG_KIND_SERVICE_EXT) {
            serviceExtTx(); // Timer for the EXT transmissions.
            delete msg;
        } else if (msg->getKind() == MSG_KIND_DL_CHECK) {
            // Reschedule timer
            deadlockCheck();
            scheduleAt(simTime() + SimTime((double)par("DL_CHECK_INTERVAL")), msg);
        }
    } else {
        // statistics for buffers:
        // "Bei Bedarf kann diese Statistik als Summe aller virtuellen Kanäle erfasst werden"
        int buffer_sum;

        for (int i = 0; i < (int)par("VIRTUAL_CHANNELS"); i++)
        {
            auto queue = (cQueue*)queues->get(i);
            buffer_sum += queue->getLength();
        }

        buffer_load.recordWithTimestamp(simTime(), (double)buffer_sum);

        handleVcf((VirtualChannelFrame*) msg); // Gets ownership of msg... deletes it by itself.
    }
}
