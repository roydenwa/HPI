/*
 * LogicalPort.cpp
 *
 *  Created on: Apr 19, 2020
 *      Author: fbeenen
 */

#include "LogicalPort.h"

LogicalPort::LogicalPort(int pid, int vid) {
    this->pid = pid;
    this->vid = vid;
}

LogicalPort::LogicalPort() {
    LogicalPort(0, 0);
}

LogicalPort::~LogicalPort() {
}

void LogicalPort::setPort(int pid) {
    this->pid = pid;
}

void LogicalPort::setVirtualChannel(int vid) {
    this->vid = vid;
}

int LogicalPort::getPort() {
    return this->pid;
}

int LogicalPort::getVirtualChannel() {
    return this->vid;
}
