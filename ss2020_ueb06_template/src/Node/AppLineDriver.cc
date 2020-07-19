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

#include "AppLineDriver.h"
#include "../debug.h"
#include "../Packets/IntFlowCtrl_m.h"
#include "../Packets/IntFlowCtrlRouteRequest_m.h"
#include "../Packets/FlitHead_m.h"
#include "../Packets/VirtualChannelFrame_m.h"


#define MSG_KIND_DL_CHECK 1


Define_Module(AppLineDriver);

AppLineDriver::~AppLineDriver() {
    delete hlQueue;
    delete txFlits;
    delete rxFlits;
}

void AppLineDriver::sendNextFlit() {
    IntFlowCtrl *sendNextFlit = new IntFlowCtrl();
    VirtualChannelFrame *sendNextFlitWrap = new VirtualChannelFrame();
    sendNextFlitWrap->setVirtualChannel(0);
    sendNextFlit->setAction(INT_NEXT_FLIT);
    sendNextFlitWrap->encapsulate(sendNextFlit);
    send(sendNextFlitWrap, "flitFlowCtrl$o");
}

void AppLineDriver::deadlockCheck() {
    cMessage *first = (cMessage *) txFlits->front();
    if (first != nullptr) {
        int firstId = first->getId();
        simtime_t stuckFor = SimTime(0);
        if (firstId == bufferFirstMessageId) {
            stuckFor = simTime() - first->getArrivalTime();
        } else {
            bufferFirstMessageId = first->getId();
        }
        emit(sigAldPacketStuckTime, stuckFor);
    }
}


HLPacket* AppLineDriver::assembleHLPacket() {
    HLPacket *result = nullptr;
    bool firstIsHead = (!rxFlits->isEmpty()) && (((Flit*)rxFlits->front())->getType() == FLIT_HEAD);
    bool lastIsTail = (!rxFlits->isEmpty()) && (((Flit*)rxFlits->back())->getType() == FLIT_TAIL);
    if (firstIsHead && lastIsTail) {
        result = new HLPacket();
        FlitHead *head = (FlitHead *)rxFlits->front();
        result->setSource(head->getSource());
        result->setDestination(head->getDestination());
        result->setHopCnt(head->getHopCnt());
        head = nullptr;

        std::vector<char> payload;
        int seqCnt = 0;
        bool seqMismatch = false;
        while(!rxFlits->isEmpty()) {
            Flit *flit = (Flit*) rxFlits->pop();
            for (unsigned int i = 0; i < flit->getPayloadArraySize(); ++i) {
                payload.push_back(flit->getPayload(i));
            }
            if (!seqMismatch && flit->getSeqnum() != seqCnt) {
                INFO(("Invalid Seqnum: Got %d but expected %d\n", flit->getSeqnum(), seqCnt ));
                seqMismatch = true;
            } else {
                ++seqCnt;
            }
            delete flit;
        }
        result->setPayloadArraySize(payload.size());
        result->setByteLength(payload.size());
        for (unsigned int i = 0; i < payload.size(); ++i) {
            result->setPayload(i, payload.at(i));
        }
    } else {
        INFO(("ERROR: While assembling HL-Packet: FirstIsHead %d, LastIsTail %d.\n", firstIsHead, lastIsTail));
    }
    return result;
}

void AppLineDriver::disassembleHLPacket(HLPacket *hlp) {
    if (!txTransmissionActive) {
        FlitHead *head = new FlitHead();
        int seqnum = 0;
        head->setSource(hlp->getSource());
        head->setDestination(hlp->getDestination());
        head->setType(FLIT_HEAD);
        head->setHopCnt(hlp->getHopCnt());
        int remainingBytes = hlp->getPayloadArraySize();
        Flit *currFlit = head;
        int hlpOffset = 0;
        while(remainingBytes > 0) {
            int flitSize = std::min(remainingBytes, (int)par("FLIT_BYTES"));
            currFlit->setPayloadArraySize(flitSize);
            currFlit->setSeqnum(seqnum++);
            for (int i = 0; i < flitSize; ++i) {
                currFlit->setPayload(i, hlp->getPayload(hlpOffset + i));
            }
            currFlit->setByteLength(flitSize);
            if (hlpOffset != 0) { // Not the first flit. The first Flit is the head.
                if (flitSize == remainingBytes) {
                    currFlit->setType(FLIT_TAIL);
                } else {
                    currFlit->setType(FLIT_DATA);
                }
            }
            currFlit->setArrivalTime(simTime()); // For Deadlock-Checking
            txFlits->insert(currFlit);
            hlpOffset += flitSize;
            remainingBytes -= flitSize;
            if (remainingBytes > 0) { // only allocate a new Flit, if it is needed.
                currFlit = new Flit();
            }
        }
        // Data is contained in only one (Header)Flit. We need to append
        // an empty Tail Flit
        if ((int)(hlp->getPayloadArraySize()) <= (int)par("FLIT_BYTES")) {
            Flit *tail = new Flit();
            tail->setType(FLIT_TAIL);
            tail->setByteLength(0);
            tail->setPayloadArraySize(0);
            tail->setSeqnum(seqnum++);
            txFlits->insert(tail);
        }
    } else {
        INFO(("ERROR: Cannot disassemble the next HL-Packet without the TX-Buffer being empty\n"));
    }
}

void AppLineDriver::initRouting() {
    if (!txTransmissionActive && !txFlits->isEmpty()) {
        bool isHead = (((Flit*) txFlits->front())->getType()) == FLIT_HEAD;
        if (isHead) {
            FlitHead *head = (FlitHead*) txFlits->front();
            IntFlowCtrlRouteRequest *req = new IntFlowCtrlRouteRequest();
            req->setSource(head->getSource());
            req->setDestination(head->getDestination());
            req->setAction(INT_ROUTE_REQUEST);
            VirtualChannelFrame *reqWrapper = new VirtualChannelFrame();
            reqWrapper->setVirtualChannel(0);
            reqWrapper->encapsulate(req);
            send(reqWrapper, "flitFlowCtrl$o");
            txTransmissionActive = true;
        } else {
            INFO(("ERROR: First Element in the TX-Queue is not a Head-Flit!\n"));
        }
    }
}

void AppLineDriver::initNextTransmission() {
    if (!txTransmissionActive) { // Initiate Transmissions
        if (txFlits->isEmpty()) { // If the queue is empty, we can check if there are HLPackets to be disassembled.
            if (!hlQueue->isEmpty()) { // If the queue is empty, we have nothing to do.
                HLPacket *nextHlp = (HLPacket*) hlQueue->pop();
                disassembleHLPacket(nextHlp);
                delete nextHlp;
                initRouting(); // Send the Routing request
            }
        } else {
            INFO(("ERROR: txFlits was not empty during transmission initialization. This is a bug.\n"));
        }
    }
}

void AppLineDriver::serviceTxQueue() {
    if (txTransmissionActive && !txLastFlitSent) { // Send all the flits
        if (!txFlits->isEmpty()) {
            Flit *flit = (Flit*) txFlits->pop();
            if (flit->getType() == FLIT_TAIL) {
                txLastFlitSent = true;
            }
            VirtualChannelFrame *flitWrapper = new VirtualChannelFrame();
            flitWrapper->setVirtualChannel(0);
            flitWrapper->encapsulate(flit);
            send(flitWrapper, "flitData$o");
        } else {
            INFO(("ERROR: Transmission is active and allowed but no data available.n\n"));
        }
    } else if (txTransmissionActive && txLastFlitSent) { // Release the connection
        VirtualChannelFrame *releaseWrapper = new VirtualChannelFrame();
        releaseWrapper->setVirtualChannel(0);
        IntFlowCtrl *release = new IntFlowCtrl();
        release->setAction(INT_RELEASE);
        releaseWrapper->encapsulate(release);
        send(releaseWrapper, "flitFlowCtrl$o");
        txLastFlitSent = false;
        txTransmissionActive = false;
        initNextTransmission(); // Maybe send next HLPacket next.
    }
}

void AppLineDriver::initialize() {
    rxTransmissionActive = false;
    txTransmissionActive = false;
    txLastFlitSent = false;
    bufferFirstMessageId = -1; // For Deadlock Check
    sigAldPacketStuckTime = registerSignal("sigAldPacketStuckTime");
    hlQueue = new cQueue("hlQueue");
    txFlits = new cQueue("txFlits");
    rxFlits = new cQueue("rxFlits");

    cMessage *dlCheckTimer = new cMessage("AppLineDriver: DL-Check-Timer");
    dlCheckTimer->setKind(MSG_KIND_DL_CHECK);
    scheduleAt(simTime() + SimTime((double)par("DL_CHECK_INTERVAL")), dlCheckTimer);
}


void AppLineDriver::handleMessage(cMessage *msg) {
    if (msg->arrivedOn("hlData$i")) {
        HLPacket *hlp = (HLPacket *) msg;
        unsigned int capacity = (unsigned int) par("HL_BUFFER_CAPACITY");
        if ((((unsigned int)hlQueue->getLength()) <= capacity) || (capacity == 0)) {
            hlQueue->insert(hlp);
            if (!txTransmissionActive) {
                initNextTransmission();
            }
        } else {
            delete msg;
            INFO(("Dropping HLP because of capacity overflow\n"));
        }
    } else if (msg->arrivedOn("flitData$i")) {
        VirtualChannelFrame *vcf = (VirtualChannelFrame *) msg;
        if (rxTransmissionActive) {
            Flit *flit = (Flit *) vcf->decapsulate();
            rxFlits->insert(flit);
            sendNextFlit();
        } else {
            INFO(("ERROR: Received data without ongoing transmission.\n"));
        }
        delete msg;
    } else if (msg->arrivedOn("flitFlowCtrl$i")) {
        VirtualChannelFrame *vcf = (VirtualChannelFrame *) msg;
        IntFlowCtrl *ifc = (IntFlowCtrl *)vcf->getEncapsulatedPacket();
        if (ifc->getAction() == INT_NEXT_FLIT) {
            if (!txTransmissionActive) {
                INFO(("ERROR: Received NextFlit-Info, but no TX-Transmission was active\n"));
            }
            serviceTxQueue();
        } else if (ifc->getAction() == INT_TX_REQUEST) { // Another channel wants to send us data.
            if (!rxTransmissionActive) {
                rxTransmissionActive = true;
                sendNextFlit();
            } else {
                INFO(("ERROR: Received TX-Request, but a transmission was already active.\n"));
            }
        } else if (ifc->getAction() == INT_RELEASE) {
            if (rxTransmissionActive) {
                rxTransmissionActive = false;
                HLPacket *hlp = assembleHLPacket();
                // rxFlits Queue is wiped clean after this.
                if (hlp != nullptr) {
                    send(hlp, "hlData$o");
                } else {
                    INFO(("ERROR: Packet assembly failed.\n"));
                }
            } else {
                INFO(("ERROR: Received Release, but no transmission was active.\n"));
            }
        }
        delete msg;
    } else if (msg->isSelfMessage()) {
        if (msg->getKind() == MSG_KIND_DL_CHECK) {
            deadlockCheck();
            scheduleAt(simTime() + SimTime((double)par("DL_CHECK_INTERVAL")), msg);
        } else {
            INFO(("ERROR: Invalid Self-Message type found.\n"));
            delete msg;
        }
    } else {
        INFO(("ERROR: Invalid input gate in the AppLineDriver\n"));
    }
}
