/***************************************************************************
 *   Copyright (C) 2015 by Ester Lopez                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef __KDET_TRUSTEDAUTHORITY_H_
#define __KDET_TRUSTEDAUTHORITY_H_

#include <omnetpp.h>
#include <unordered_map>
#include "GraphServer.h"
#include "CoreEvaluation_m.h"
#include <iostream>
#include <fstream>

namespace kdet {
/**
 * The trusted authority receives Core evaluations from every node and
 * saves the results into CSV files.
 */
class TrustedAuthority: public cSimpleModule {
protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
    virtual void evaluateKDet();
    virtual void evaluateCore(std::set<inet::IPv4Address> core,
            std::set<inet::IPv4Address> boundary, double&, double&, bool&);
    std::string getRealValues(IPSet core);
    bool collusion(inet::IPv4Address boundaryNode,
            std::set<inet::IPv4Address> core);
    virtual double getThreshold(std::set<inet::IPv4Address> core);
    bool isFaulty(std::set<inet::IPv4Address> core);
    void clearEvaluations();
    unsigned numNodes;
    int* droppedPackets;
    int* inPackets;
    int* outPackets;
    bool* faulty;
    std::vector<CoreEvaluation*>* evaluations;
    double alpha;
    double beta;
    double threshold;
    std::unordered_map<int, int> IPtoIndex;
    GraphServer* graphServer;
    // Output statistics
    cOutVector coreEstimation;
    cOutVector coreIsFaulty;
    cOutVector coreReal;
    cOutVector coreSize;
    cOutVector coreDetected;
    // CSV file
    std::ofstream coreCSV;
    std::ofstream nodeCSV;
};
}
#endif
