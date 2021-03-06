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


using namespace omnetpp;


class Simple2: public cSimpleModule
{
private:
  int counter;
  int message_counter;
  char out_string[128];

protected:
    virtual void send_message(cMessage *msg);
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Simple2);

void Simple2::send_message(cMessage *msg)
{
    // helper func to choose output gate randomly
    if (rand() % 2 == 0) {
        send(msg, "out1");
    } else {
        send(msg, "out2");
    }
}

void Simple2::initialize()
{
    counter = 0;
    message_counter = 0;

    // to see counter during simulation
    WATCH(counter);
    WATCH(message_counter);

    // start with node "tic"
    if (strcmp("tic", getName()) == 0) {
        auto message_name = "tictocMsg";

        // concat info to out_string
        snprintf(out_string, 128, "%s%d", message_name, message_counter);
        cMessage *msg = new cMessage(out_string);

        send_message(msg);
    }
}

void Simple2::handleMessage(cMessage *msg)
{
    // method is called when a msg arrives
    counter++;

    if (par("receive_only_5").boolValue() == true) {
        if (counter < 5) {
            send_message(msg);
        } else {
            counter = 0;
            message_counter++;
            delete msg;

            // create new message
            auto message_name = "tictocMsg";
            snprintf(out_string, 128, "%s%d", message_name, message_counter);
            cMessage *msg = new cMessage(out_string);

            send_message(msg);
        }
    } else {
        send_message(msg);
    }
}
