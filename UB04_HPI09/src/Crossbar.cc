#include <string.h>
#include <omnetpp.h>

#include "Packet_m.h"

using namespace omnetpp;


class Inport : public cSimpleModule
{
    cOutVector qlength;
    cOutVector qtime;
    cQueue *in_queue;

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
    // if queue not empty -> store arriving msgs in queue
    // send slfmsg to repeatedly check queue

    if (msg->getKind() == 200)
    {
        auto *ttpacket = (Packet*)in_queue->pop();
        auto destination = ttpacket->getDestination();

        send(ttpacket, "out", destination);

        // getTransmissionFinishTime
        // TODO

        // schedule at release msg to arbiter
        auto *release = new cMessage("RELEASE");
        release->setKind(300);

        // send(release, "arbiterCtrl", destination);

        delete msg;
    }
    else
    {
        // external msg
        auto *request = new cMessage("REQUEST");
        request->setKind(100);

        // cast msg to custom class Packet
        Packet *ttpacket = check_and_cast<Packet *>(msg);

        auto destination = ttpacket->getDestination();
        send(request, "arbiterCtrl", destination);

        // insert packet into queue and delete local cast
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
