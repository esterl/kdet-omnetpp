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

/*** Aux functions ***/
inline double max(double a, double b) {
    return (a > b) ? a : b;
}

/*********************/

Define_Module(TrustedAuthority);

void TrustedAuthority::initialize() {
    numNodes = gateSize("inReports");
    droppedPackets = new int[numNodes];
    inPackets = new int[numNodes];
    outPackets = new int[numNodes];
    faulty = new bool[numNodes];
    evaluations = new std::vector<CoreEvaluation*>[numNodes];
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
    // CSV
    cvsFile.open(par("resultsFile").stringValue());
    cvsFile << "Timestamp, Core, Detected, Estimation, Real, Collusion" << endl;
}

void TrustedAuthority::finish() {
    delete[] inPackets;
    delete[] outPackets;
    delete[] droppedPackets;
    delete[] faulty;
    clearEvaluations();
    delete[] evaluations;
    cvsFile.close();
}

void TrustedAuthority::handleMessage(cMessage *msg) {
    if (msg->arrivedOn("inReports")) {
        // Save Core evaluation
        int gate = msg->getArrivalGate()->getIndex();
        CoreEvaluation* evaluation = check_and_cast<CoreEvaluation*>(msg);
        IPv4Address addr = evaluation->getReporter();
        IPtoIndex[addr.getInt()] = gate;
        evaluations[gate].push_back(evaluation);
    } else if (msg->arrivedOn("inGroundTruth")) {
        // Update values
        int gate = msg->getArrivalGate()->getIndex();
        DropperReport* report = check_and_cast<DropperReport*>(msg);
        inPackets[gate] = report->getInPackets();
        outPackets[gate] = report->getOutPackets();
        droppedPackets[gate] = report->getDroppedPackets();
        faulty[gate] = report->getFaulty();
        delete report;
    } else if (msg->arrivedOn("clock")) {
        // Schedule evaluation just a bit later
        scheduleAt(simTime() + 0.03, msg);
    } else {
        // Evaluate KDet performance
        evaluateKDet();
        // Clear reports
        clearEvaluations();
        delete msg;
    }
}

std::string coreToString(std::set<IPv4Address> core) {
    std::ostringstream result;
    for (auto it = core.begin(); it != core.end(); it++) {
        result << (*it) << ";";
    }
    return result.str();
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

std::pair<bool, double> getNodeEvaluation(
        std::vector<CoreEvaluation*>& evaluationList, IPSet core) {
    bool detected = false;
    double estimation = 0.0;
    for (auto evaluation = evaluationList.begin();
            evaluation != evaluationList.end(); evaluation++) {
        // Look for the evaluation of the requested core
        IPList evaluationCore = (*evaluation)->getCore();
        if (IPSet(evaluationCore.begin(), evaluationCore.end()) == core) {
            // Is there any report missing?
            BoolMap received = (*evaluation)->getReceivedReports();
            for (auto it = received.begin(); it != received.end(); it++) {
                if (!(it->second))
                    detected = true;
            }
            // Get Estimation
            estimation = (*evaluation)->getDropEstimation();
        }
    }
    return std::make_pair(detected, estimation);
}

void TrustedAuthority::evaluateCore(IPSet core, IPSet boundary) {
    bool detected = false;
    // Base entry
    std::ostringstream os;
    os << simTime() << "," << "'" << coreToString(core) << "'" << ","
            << isFaulty(core) << ",";
    std::ostringstream estimations;
    estimations << "'";
    bool bogus = false;
    for (auto node = boundary.begin(); node != boundary.end(); node++) {
        // Get the evaluations from that node:
        std::pair<bool, double> evaluation = getNodeEvaluation(
                evaluations[IPtoIndex[(*node).getInt()]], core);
        estimations << evaluation.second << ",";
        // Update detected
        detected = evaluation.first | (evaluation.second > getThreshold(core));
        // Update collusion
        if (faulty[IPtoIndex[node->getInt()]] & collusion(*node, core))
            bogus = true;
    }
    estimations << "'";
    cvsFile << os.str() << estimations.str() << ","
            << getRealDropProbability(core) << "," << bogus << endl;
}

std::string TrustedAuthority::getRealDropProbability(IPSet core) {
    std::ostringstream os;
    os << "'";
    for (auto node = core.begin(); node != core.end(); node++) {
        unsigned index = IPtoIndex[node->getInt()];
        os << double(droppedPackets[index]) / max(inPackets[index], 1) << ",";
    }
    os << "'";
    return os.str();
}

bool TrustedAuthority::collusion(IPv4Address boundaryNode,
        std::set<IPv4Address> core) {
// Is there any neighbor of boundaryNode in the core & faulty?
    std::set<IPv4Address> neighbors = graphServer->getNeighbors(boundaryNode);
    for (auto it = neighbors.begin(); it != neighbors.end(); it++) {
        if (faulty[IPtoIndex[it->getInt()]] & core.count(*it) != 0)
            return true;
    }
    return false;
}
double TrustedAuthority::getThreshold(std::set<IPv4Address> core) {
    return threshold;
}

bool TrustedAuthority::isFaulty(std::set<IPv4Address> core) {
    for (auto node = core.begin(); node != core.end(); node++) {
        int index = IPtoIndex[node->getInt()];
        coreReal.record(
                double(droppedPackets[index]) / max(inPackets[index], 1));
        if (double(droppedPackets[index]) / max(inPackets[index], 1) > alpha)
            return true;
    }
    return false;
}

void TrustedAuthority::clearEvaluations() {
    for (unsigned i = 0; i < numNodes; i++) {
        for (auto it = evaluations[i].begin(); it != evaluations[i].end(); it++)
            delete (*it);
        evaluations[i].clear();
    }
}
