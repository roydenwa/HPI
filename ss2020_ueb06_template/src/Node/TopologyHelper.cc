/*
 * TopologyHelper.cpp
 *
 *  Created on: Apr 21, 2020
 *      Author: fbeenen
 */

#include "TopologyHelper.h"
#include <list>
#include <sstream>
#include <string.h>


TopologyHelper::TopologyHelper() {

}

TopologyHelper::~TopologyHelper() {
}

int TopologyHelper::getNodeCountSingle(const char *nodeSpecifier) {
    std::vector<int> dims = getNodeCount(nodeSpecifier);
    int accu = 1;
    for (int i : dims) {
        accu *= i;
    }
    return accu;
}

std::vector<int> TopologyHelper::getNodeCount(const char *nodeSpecifier) {
    std::vector<int> nodeCnt;
    int specLen = strlen(nodeSpecifier);
    std::stringstream ss;
    int onesFromIdx = 0;
    for (int i = 0; i < specLen; ++i) {
        if (nodeSpecifier[i] == ' ') {
            nodeCnt.push_back(std::stoi(ss.str()));
            if (nodeCnt.back() != 1) {
                onesFromIdx = nodeCnt.size() - 1;
            }
            ss.str(""); // Clear the SS
        } else if (nodeSpecifier[i] >= '0' && nodeSpecifier[i] <= '9'){
            ss << nodeSpecifier[i];
        }
    }
    if (nodeSpecifier[specLen - 1] != ' ') {
        nodeCnt.push_back(std::stoi(ss.str()));
        if (nodeCnt.back() != 1) {
            onesFromIdx = nodeCnt.size() - 1;
        }
    }

    // Filter the tailing ones from the result.
    std::vector<int> result;
    for (int i = 0; i <= onesFromIdx; ++i) {
        result.push_back(nodeCnt.at(i));
    }

    return result;
}

std::vector<int> TopologyHelper::calculateNodeLocationFromId(int nodeId, const char *nodeSpecifier) {
    std::list<int> result;
    std::vector<int> nodeCnt = getNodeCount(nodeSpecifier);
    int numerator = nodeId;

    for (int i = nodeCnt.size() - 1; i >= 0; --i) {
        int denom = 1;
        for (int  d = 0; d < i; ++d) {
            denom *= nodeCnt.at(d);
        }
        result.push_front(numerator / denom);
        numerator %= denom;
    }

    return std::vector<int>(result.begin(), result.end());
}

