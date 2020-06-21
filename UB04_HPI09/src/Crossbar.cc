#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;


class Inport : public cSimpleModule
{
    cOutVector qlength;
    cOutVector qtime;

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

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};


Define_Module(Inport);
Define_Module(Arbiter);
Define_Module(Multiplexer);
Define_Module(Outbuffer);


void Inport::initialize()
{
    qlength.setName("Qlength");
    qtime.setName("Qtime");
}


void Inport::handleMessage(cMessage *msg)
{
    // TODO
}


void Arbiter::initialize()
{
    outstanding_requests.setName("Outstanding-requests");
    service_time.setName("Service-time");
}


void Arbiter::handleMessage(cMessage *msg)
{
    // TODO
}


void Multiplexer::initialize()
{
    // TODO
}


void Multiplexer::handleMessage(cMessage *msg)
{
    // TODO
}


void Outbuffer::initialize()
{
    qlength.setName("Qlength");
    qtime.setName("Qtime");
}


void Outbuffer::handleMessage(cMessage *msg)
{
    // TODO
}
