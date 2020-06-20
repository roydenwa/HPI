#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;


class Inport : public cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};


class Arbiter : public cSimpleModule
{
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


Define_Module(Inport);
Define_Module(Arbiter);
Define_Module(Multiplexer);


void Inport::initialize()
{
    // TODO
}


void Inport::handleMessage(cMessage *msg)
{
    // TODO
}


void Arbiter::initialize()
{
    // TODO
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
