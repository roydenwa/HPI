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

#include "Buffer.h"
#include<tuple>

Define_Module(Buffer);


void Buffer::initialize()
{
    // init statistics
    num_pkgs_dropped = 0;
    pkgs_saved_vec.setName("Pkgs-saved");
    pkgs_dropped_vec.setName("Pkgs-dropped");
    time_in_buffer.setName("Time-in-buffer");

    queue = new cQueue("Bufferqueue");
    selfMessage = new cMessage();
    txChannel = gate("line$o")->getTransmissionChannel();
}


void Buffer::handleMessage(cMessage *msg)
{
    // use slfmsgs to check queue
    if (msg->isSelfMessage())
    {

        simtime_t txFinishTime = txChannel->getTransmissionFinishTime();

        if (txFinishTime <= simTime())
        {
            auto *sendMsg = (cMessage*) queue->pop();
            auto buffer_time = simTime() - sendMsg->getArrivalTime();

            time_in_buffer.record(buffer_time);
            send(sendMsg, "line$o");

            if (!queue->isEmpty())
            {
                scheduleAt(txFinishTime, msg);
            }
        }
        else
        {
            scheduleAt(txFinishTime, msg);
        }

    }
    else
    {
        if (msg->arrivedOn("in"))
        {

            simtime_t txFinishTime = txChannel->getTransmissionFinishTime();

            if (txFinishTime <= simTime() && queue->isEmpty())
            {
                auto buffer_time = simTime() - msg->getArrivalTime();
                time_in_buffer.record(buffer_time);
                send(msg, "line$o");
            }
            else
            {
                if (queue->getLength() < (int) par("buffer_size") || (int) par("buffer_size") == 0)
                {

                    if (queue->isEmpty())
                    {
                        scheduleAt(txFinishTime, selfMessage);
                    }

                    queue->insert(msg);
                }
                else
                {
                    delete msg;

                    num_pkgs_dropped++;
                    pkgs_dropped_vec.recordWithTimestamp(simTime(), (double)num_pkgs_dropped);
                }

            }

        }
        // always pass
        else if (msg->arrivedOn("line$i"))
        {
            send(msg, "out");
        }

    }
    // get size of queue == # of stored pkgs
    pkgs_saved_vec.recordWithTimestamp(simTime(), (double)queue->getLength());
}


Buffer::~Buffer()
{
    delete queue;
    delete selfMessage;
}

