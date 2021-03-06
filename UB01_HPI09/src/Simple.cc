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
#include <string.h>
#include <omnetpp.h>
#include <iostream>
#include <string>
#include <cstdio>


class Simple: public omnetpp::cSimpleModule
{
private:
  int counter;

protected:
    // The following redefined virtual function holds the algorithm.
    virtual void initialize() override;
    virtual void handleMessage(omnetpp::cMessage *msg) override;
};


Define_Module(Simple);

void Simple::initialize()
{
    counter = 0;

    // to see counter during simulation
    WATCH(counter);

    // start with node "tic"
    if (strcmp("tic", getName()) == 0) {
        auto message_name = "tictocMsg1000";
        omnetpp::cMessage *msg = new omnetpp::cMessage(message_name);

        send(msg, "out");
    }
}

void Simple::handleMessage(omnetpp::cMessage *msg)
{
    // method is called when a msg arrives
    counter++;

    if (par("receive_only_5").boolValue() == true) {
        if (counter < 5) {
            send(msg, "out");
        } else {
            delete msg;
        }
    } else {
        send(msg, "out");
    }
}
