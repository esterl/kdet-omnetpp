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

#include "TrustedAuthority.h"
#include "CoreEvaluation_m.h"
#include "DropperReport_m.h"
#include "IPv4Address.h"

Define_Module(TrustedAuthority);

void TrustedAuthority::initialize() {
    numNodes = gateSize("out");
    droppedPackets = new int[numNodes];
    inPackets = new int[numNodes];
    outPackets = new int[numNodes];
    reportRequested = new bool[numNodes];
    evaluations = new CoreEvaluation*[numNodes];
    for (unsigned i = 0; i < numNodes; i++)
        evaluations[i] = NULL;
    timer = NULL;
    graphServer = check_and_cast<GraphServer*>(
            getParentModule()->getSubmodule("graphServer"));
    coreIsFaulty.setName("coreIsFaulty");
    coreReal.setName("coreRealDropProbability");
    coreEstimation.setName("coreEstimation");
    coreSize.setName("coreSize");
    coreDetected.setName("coreDetected");
    alpha = par("alpha");
    beta = par("beta");
    threshold = par("threshold");
}

void TrustedAuthority::finish() {
    delete[] inPackets;
    delete[] outPackets;
    delete[] droppedPackets;
    for (unsigned i = 0; i < numNodes; i++)
        delete evaluations[i];
    delete[] evaluations;
}

void TrustedAuthority::handleMessage(cMessage *msg) {
    if (msg->arrivedOn("inReports")) {
        // Save Core evaluation
        int gate = msg->getArrivalGate()->getIndex();
        CoreEvaluation* evaluation = check_and_cast<CoreEvaluation*>(msg);
        IPv4Address addr = evaluation->getReporter();
        IPtoIndex[addr.getInt()] = gate;
        if (evaluations[gate] != NULL)
            delete evaluations[gate];
        evaluations[gate] = evaluation;
        // Schedule
        if (timer == NULL) {
            timer = new cMessage();
            scheduleAt(simTime() + 0.01, timer);
        }
        // Ask for ground truth:
        if (!reportRequested[gate]) {
            cMessage* msg = new cMessage();
            send(msg, "out", gate);
            reportRequested[gate] = true;
        }
    } else if (msg->arrivedOn("inGroundTruth")) {
        // Update values
        int gate = msg->getArrivalGate()->getIndex();
        DropperReport* report = check_and_cast<DropperReport*>(msg);
        inPackets[gate] = report->getInPackets();
        outPackets[gate] = report->getOutPackets();
        droppedPackets[gate] = report->getDroppedPackets();
        reportRequested[gate] = false;
        delete report;
    } else {
        // Evaluate KDet performance
        evaluateKDet();
        delete msg;
        timer = NULL;
    }
}

void TrustedAuthority::evaluateKDet() {
    // Get list of cores & boundaries
    IPSetList cores = graphServer->getAllCores();
    IPSetList boundaries = graphServer->getAllBoundaries();

    for (unsigned i = 0; i < cores.size(); i++) {
        evaluateCore(cores[i], boundaries[i]);
        coreSize.record(cores[i].size());
        coreIsFaulty.record(isFaulty(cores[i]));
    }
}

void TrustedAuthority::evaluateCore(std::set<IPv4Address> core,
        std::set<IPv4Address> boundary) {
    bool detected = false;
    for (auto node = boundary.begin(); node != boundary.end(); node++) {
        // Get the evaluation from that node:
        CoreEvaluation* evaluation = evaluations[IPtoIndex[(*node).getInt()]];
        // Look for the evaluation of the given core:
        IPList evalCore_l = evaluation->getCore();
        IPSet evalCore = std::set<IPv4Address>(evalCore_l.begin(),
                evalCore_l.end());
        if (evalCore == core) {
            // Is there any report missing?
            BoolMap recReports = evaluation->getReceivedReports();
            for (auto it = recReports.begin(); it != recReports.end(); it++) {
                if (!(it->second)) {
                    detected = true;
                }
            }
            // Is the estimation worst than expected?
            double estimation = evaluation->getDropEstimation();
            coreEstimation.record(estimation);
            detected = detected | (estimation > getThreshold(core));
            coreDetected.record(detected);
        }

    }
}

double TrustedAuthority::getThreshold(std::set<IPv4Address> core) {
    return threshold;
}

inline double max(double a, double b) {
    return (a > b) ? a : b;
}

bool TrustedAuthority::isFaulty(std::set<IPv4Address> core) {
    for (auto node = core.begin(); node != core.end(); node++) {
        int index = IPtoIndex[node->getInt()];
        coreReal.record(
                double(droppedPackets[index]) / max(inPackets[index], 1));
        std::cout << "Dropped packets by " << *node << " "
                << double(droppedPackets[index]) << " input packets: "
                << inPackets[index] << endl;
        if (double(droppedPackets[index]) / max(inPackets[index], 1) > alpha)
            return true;
    }
    return false;
}
