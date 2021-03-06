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

#ifndef __UB04_HPI09_APP_H_
#define __UB04_HPI09_APP_H_


#include "Packet_m.h"
#include <omnetpp.h>

using namespace omnetpp;

/**
 * TODO - Generated class
 */
class App : public cSimpleModule
{
private:
    int send_counter;
    int receive_counter;
    double recieved_bits;

    // for statistics
    cHistogram packet_sizes;
    cOutVector send_counter_vec;
    cOutVector receive_counter_vec;
    cHistogram hop_counter;
    cOutVector end2end_delay;
    cOutVector throughput_vec;

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *packet) override;
    virtual void finish() override;

    virtual Packet *generateMessage();
    virtual void forwardMessage(Packet *msg);
};

#endif
