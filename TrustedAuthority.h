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

#ifndef __KDET_TRUSTEDAUTHORITY_H_
#define __KDET_TRUSTEDAUTHORITY_H_

#include <omnetpp.h>
#include <unordered_map>
#include "GraphServer.h"
#include "CoreEvaluation_m.h"

/**
 * TODO - Generated class
 */
class TrustedAuthority : public cSimpleModule
{
  protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
    virtual void evaluateKDet();
    virtual void evaluateCore(std::set<IPv4Address> core,
        std::set<IPv4Address> boundary);
    virtual double getThreshold(std::set<IPv4Address> core);
    bool isFaulty(std::set<IPv4Address> core);
    unsigned numNodes;
    int* droppedPackets;
    int* inPackets;
    int* outPackets;
    CoreEvaluation** evaluations;
    double alpha;
    double beta;
    double threshold;
    std::unordered_map<int, int> IPtoIndex;
    cMessage* timer;
    GraphServer* graphServer;
    // Output statistics
    cOutVector coreEstimation;
    cOutVector coreIsFaulty;
    cOutVector coreSize;
    cOutVector coreDetected;
};

#endif
