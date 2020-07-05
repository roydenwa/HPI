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

Define_Module(Buffer);

void Buffer::initialize()
{
    // save simulation parameters
    queue_size = par("queue_size");

    // shortcut to channel. Reduce number of lookups for performance
    channel = gate("out")->getTransmissionChannel();

    // buffer to store packets
    queue = new cQueue("queue");

    // self message to save transmission finish time
    finished = new cMessage("finished");

    // register all statistic signals
    qtime = registerSignal("qtime");
    qlen = registerSignal("qlen");
    busy = registerSignal("busy");
    drop = registerSignal("drop");

    gate("in")->setDeliverOnReceptionStart(par("cutThrough"));

}

Buffer::~Buffer()
{
    // clean up queue object.
    // Queue destructor handles deletion of massages still in queue
    delete queue;

    // delete our finish timer self message
    cancelAndDelete(finished);
}

void Buffer::handleMessage(cMessage *msg)
{
    if (msg->arrivedOn("in"))
    {
        // message comes from upper layer
        // check if packets are waiting or transfer ongoing
        if (channel->isBusy() || !(queue->isEmpty()))
        {
            // transfer ongoing and queue not empty
            if (( queue_size - queue->getLength() ) > 0 || queue_size == 0)
            {
                //channel busy. queue space is available
                // Store Packet in queue

                // set timestamp for queue waiting time
                msg->setTimestamp(simTime());

                queue->insert(msg);

                //save current queue length
                emit(qlen,queue->getLength());
                EV_INFO << "Channel is Busy: isBusy: " << channel->isBusy() << "; queue length: " << queue->getLength() << endl;
            }
            else
            {
                //channel busy and queue is full
                // drop Packet
                delete msg;

                // "increment" number of dropped packets
                emit(drop,1);

                EV_INFO << "Channel is Busy: isBusy: " << channel->isBusy() << "; queue length: " << queue->getLength() << endl;
                EV_INFO << "queue is full. Drop Packet " << endl;
            }
        }
        else //channel is free and no packet is queued -> send immediately
        {
            send(msg,"out");

            // send 0 for time average calculation
            emit(qtime, SIMTIME_ZERO);

            //calculate when transfer finishes and schedule reminder/timer
            simtime_t finish_time = channel->getTransmissionFinishTime();
            scheduleAt(finish_time,finished);

            // channel is now busy, send "1" for timing average calculation
            emit(busy,true);

            EV_INFO << "Channel free, no msg queued, send now. Duration:" << (finish_time.dbl() - simTime()) * 1000000 << "us" << endl;
        }
    }
    else
    {
        //handle timer self messages
        if (queue->isEmpty())
        {
            EV_INFO << "Transmission finished, but no msg is queued" << endl;

            // channel is free, send "0" for timing average calculation
            emit(busy,false);
        }
        else
        {
            // get values from queue for debug output
            // pop message from queue
            int before = queue->getLength();
            Packet *pkt = (Packet*) queue->pop();
            int after = queue->getLength();

            // send wait time for this message
            emit(qtime,simTime() - pkt->getTimestamp());
            send(pkt,"out");

            simtime_t finish_time = channel->getTransmissionFinishTime();
            scheduleAt(finish_time,finished);

            // channel is now busy, send "1" for timing average calculation
            emit(busy,true);

            EV_INFO << "Transmission finished" << endl;
            EV_INFO << "Send queued message. Old queue size: " << before << "; still waiting: " << after << endl;
            EV_INFO << "Duration:" << finish_time.dbl()  * 1000000 << "us" << endl;
        }
    }

}
