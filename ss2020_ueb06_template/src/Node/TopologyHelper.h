/*
 * TopologyHelper.h
 *
 *  Created on: Apr 21, 2020
 *      Author: fbeenen
 */

#ifndef NODE_TOPOLOGYHELPER_H_
#define NODE_TOPOLOGYHELPER_H_

#include <vector>

class TopologyHelper {
public:
    TopologyHelper();
    virtual ~TopologyHelper();
    static std::vector<int> getNodeCount(const char *nodeSpecifier);
    static std::vector<int> calculateNodeLocationFromId(int nodeId, const char *nodeSpecifier);
    static int getNodeCountSingle(const char *nodeSpecifier);
};

#endif /* NODE_TOPOLOGYHELPER_H_ */
