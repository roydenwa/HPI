/*
 * LogicalPort.h
 *
 *  Created on: Apr 19, 2020
 *      Author: fbeenen
 */

#ifndef NODE_LOGICALPORT_H_
#define NODE_LOGICALPORT_H_

#include <omnetpp.h>

class LogicalPort : public omnetpp::cObject {
public:
    LogicalPort();
    LogicalPort(int pid, int vid);
    void setPort(int pid);
    void setVirtualChannel(int vid);
    int getPort();
    int getVirtualChannel();
    virtual ~LogicalPort();
private:
    int vid;
    int pid;
};

#endif /* NODE_LOGICALPORT_H_ */
