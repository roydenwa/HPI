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
    virtual ~Inport() override;
};


class Arbiter : public cSimpleModule
{
    cOutVector outstanding_requests;
    cOutVector service_time;
    cQueue *request_queue;

    // set while transmission is granted
    bool blocked;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual ~Arbiter() override;
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

    // to get info if channel/s behind "out" is busy
    cChannel *outChannel;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual ~Outbuffer() override;
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
        auto *selfmsg = new cMessage("CHECK-IN-QUEUE");
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

        //delete ttpacket;
    }
}


Inport::~Inport()
{
    delete in_queue;
}


void Arbiter::initialize()
{
    request_queue = new cQueue("Request-queue");
    blocked = false;

    outstanding_requests.setName("Outstanding-requests");
    service_time.setName("Service-time");
}


void Arbiter::handleMessage(cMessage *msg)
{
    // check request queue
    if (msg->isSelfMessage())
    {
        if (!request_queue->isEmpty())
        {
            if (!blocked)
            {
                auto next_request = (cMessage*)request_queue->pop();
                auto sourcePort = next_request->getSenderGate()->getIndex();

                auto *grant = new cMessage("GRANT");
                grant->setKind(200);
                send(grant, "in_out", sourcePort);

                // block arbiter
                blocked = true;

                delete msg;
            }
            // if blocked send selfmsg again
            else
            {
                scheduleAt(simTime(), msg);
            }
        }
        else
        {
            delete msg;
        }
    }
    // request from inport
    else if (msg->getKind() == 100)
    {
        if (!blocked)
        {
            auto sourcePort = msg->getSenderGate()->getIndex();

            auto *grant = new cMessage("GRANT");
            grant->setKind(200);
            send(grant, "in_out", sourcePort);

            // block arbiter
            blocked = true;

            delete msg;
        }
        // store request if currently blocked
        else
        {
            request_queue->insert(msg);
        }

    }
    // release from inport
    else if (msg->getKind() == 300)
    {
        blocked = false;

        // check request_queue
        if (!request_queue->isEmpty())
        {
            auto next_request = (cMessage*)request_queue->pop();
            auto sourcePort = next_request->getSenderGate()->getIndex();

            auto *grant = new cMessage("GRANT");
            grant->setKind(200);
            send(grant, "in_out", sourcePort);

            // block arbiter
            blocked = true;

            // send selfmsg to check if more requests are pending
            auto *selfmsg = new cMessage("CHECK-REQUEST-QUEUE");
            scheduleAt(simTime(), selfmsg);
        }
    }
}


Arbiter::~Arbiter()
{
    delete request_queue;
}


void Multiplexer::initialize()
{
    // TODO
}


void Multiplexer::handleMessage(cMessage *msg)
{
    // TODO
    // map: input[i] -> out
    send(msg, "out");
}


void Outbuffer::initialize()
{
    out_queue = new cQueue("Output-queue");
    outChannel = gate("out")->getTransmissionChannel();

    qlength.setName("Qlength");
    qtime.setName("Qtime");
}


void Outbuffer::handleMessage(cMessage *msg)
{
    // TODO
    // check if output to extern is busy via isBusy() / getTransmissionTime()
    // send slfmsg to repeatedly check queue

    // check if outgate is busy


    if (msg->isSelfMessage())
    {
        // check if queue is empty
        if (!out_queue->isEmpty())
        {

        }
    }
    else
    {
        if (!outChannel->isBusy())
        {
            send(msg, "out");
        }
        else
        {

        }
        if (out_queue->getLength() < (int)par("buffer_size") || (int)par("buffer_size") == 0)
        {

        }
    }


}


Outbuffer::~Outbuffer()
{
    delete out_queue;
}
