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

#ifndef __UB04_HPI09_BUFFER_H_
#define __UB04_HPI09_BUFFER_H_

#include <omnetpp.h>

using namespace omnetpp;

/**
 * TODO - Generated class
 */
class Buffer : public cSimpleModule
{
private:
    // for statistics
    int num_pkgs_dropped;
    cMessage *selfMessage;
    cOutVector time_in_buffer;
    cOutVector pkgs_dropped_vec;
    cOutVector pkgs_saved_vec;
    cQueue *queue;

    // to get info if channel behind "line" is busy
    cChannel *txChannel;

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

    // custom destructor
    virtual ~Buffer() override;
};

#endif
