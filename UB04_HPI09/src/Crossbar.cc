#include <string.h>
#include <omnetpp.h>

#include "Packet_m.h"

using namespace omnetpp;


class Inport : public cSimpleModule
{
    cOutVector qlength;
    cOutVector qtime;
    cQueue *in_queue;

    // to get info if channel/s behind "out" is busy
    cChannel *txChannel;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};


class Arbiter : public cSimpleModule
{
    cOutVector outstanding_requests;
    cOutVector service_time;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};


class Multiplexer : public cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};


class Outbuffer : public cSimpleModule
{
    cOutVector qlength;
    cOutVector qtime;
    cQueue *out_queue;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};


Define_Module(Inport);
Define_Module(Arbiter);
Define_Module(Multiplexer);
Define_Module(Outbuffer);


/*
 * lookup-table for msg-kinds
 * kind     meaning
 * 100      request for transmission (Inport->Outport-Arbiter)
 * 200      grant -- access granted (Outport-Arbiter->Inport)
 * 300      release -- transmission finished (Inport->Outport-Arbiter)
 */


void Inport::initialize()
{
    qlength.setName("Qlength");
    qtime.setName("Qtime");
    in_queue = new cQueue("Inport-queue");
}


void Inport::handleMessage(cMessage *msg)
{
    // send slfmsg to repeatedly check queue --> needed for msgs which arrived
    // during arbiterCtrl-cycle and thus are stored without request to arbiter
    if (msg->isSelfMessage())
    {
        // check queue
        if (!in_queue->isEmpty())
        {
            auto *ttpacket = (Packet*)in_queue->get(0);
            auto destination = ttpacket->getDestination();

            // create and send request
            auto *request = new cMessage("REQUEST");
            request->setKind(100);
            send(request, "arbiterCtrl", destination);

            // delete ttpacket needed?

            delete msg;
        }
        else
        {
            delete msg;
        }
    }
    // grant from arbiter
    else if (msg->getKind() == 200)
    {
        auto *ttpacket = (Packet*)in_queue->pop();
        auto destination = ttpacket->getDestination();

        send(ttpacket, "out", destination);

        // get transmission channel and transmission finish time
        txChannel = gate("out", destination)->getTransmissionChannel();
        auto txFinishTime = txChannel->getTransmissionFinishTime();

        // send release msg to arbiter after transmission finished
        auto *release = new cMessage("RELEASE");
        release->setKind(300);
        sendDelayed(release, txFinishTime-simTime(), "out", destination);

        // also send selfmsg to check if there are more pkgs waiting in in_queue
        auto *selfmsg = new cMessage("CHECK-QUEUE");
        scheduleAt(simTime(), selfmsg);

        delete msg;
    }
    // if in_queue is empty and external msg: send request immediately
    else if (in_queue->isEmpty())
    {
        // cast msg to custom class Packet
        Packet *ttpacket = check_and_cast<Packet *>(msg);
        auto destination = ttpacket->getDestination();

        auto *request = new cMessage("REQUEST");
        request->setKind(100);
        send(request, "arbiterCtrl", destination);

        // insert packet into queue and delete local cast
        in_queue->insert(ttpacket);

        //delete ttpacket;
    }
    // if in_queue is not empty and external msg: just store in in_queue
    else
    {
        Packet *ttpacket = check_and_cast<Packet *>(msg);
        in_queue->insert(ttpacket);

        delete ttpacket;
    }
}

// TODO: destructor to deallocate queue


void Arbiter::initialize()
{
    outstanding_requests.setName("Outstanding-requests");
    service_time.setName("Service-time");
}


void Arbiter::handleMessage(cMessage *msg)
{
    // TODO
    // check how old
}


void Multiplexer::initialize()
{
    // TODO
}


void Multiplexer::handleMessage(cMessage *msg)
{
    // TODO
    // map: input[i] -> out
}


void Outbuffer::initialize()
{
    qlength.setName("Qlength");
    qtime.setName("Qtime");
}


void Outbuffer::handleMessage(cMessage *msg)
{
    // TODO
    // check if output to extern is busy via isBusy() / getTransmissionTime()
    // send slfmsg to repeatedly check queue
}
