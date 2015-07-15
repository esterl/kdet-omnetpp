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
    nodeCSV.open(
            std::string("results/node_") + par("resultsFile").stringValue());
    nodeCSV << "Timestamp, Node, CoreEstimationIn, CoresEstimationDropped, "
            << "Collusion, NodeDrop, NodeIn, NodeOut" << endl;
    coreCSV.open(
            std::string("results/core_") + par("resultsFile").stringValue());
    coreCSV << "Timestamp, Core, Detected, DropEstimation, InEstimation, "
            << "OutEstimation, DropReal, InReal, OutReal, Collusion" << endl;
}

void TrustedAuthority::finish() {
    delete[] inPackets;
    delete[] outPackets;
    delete[] droppedPackets;
    delete[] faulty;
    clearEvaluations();
    delete[] evaluations;
    nodeCSV.close();
    coreCSV.close();
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

    std::vector<double> *estimatedIn = new std::vector<double>[numNodes];
    std::vector<double> *estimatedDropped = new std::vector<double>[numNodes];
    std::vector<bool> *isBogus = new std::vector<bool>[numNodes];
    for (unsigned i = 0; i < cores.size(); i++) {
        double dropped, inPkts;
        bool bogusEval;
        evaluateCore(cores[i], boundaries[i], dropped, inPkts, bogusEval);
        for (auto node = cores[i].begin(); node != cores[i].end(); node++) {
            estimatedIn[IPtoIndex[node->getInt()]].push_back(inPkts);
            estimatedDropped[IPtoIndex[node->getInt()]].push_back(dropped);
            isBogus[IPtoIndex[node->getInt()]].push_back(bogusEval);
        }
    }
    // Record values in nodeCSV
    for (unsigned i = 0; i < numNodes; i++) {
        for (unsigned j = 0; j < estimatedIn[i].size(); j++) {
            // "Timestamp, Node, CoreEstimationIn, CoresEstimationDropped"
            nodeCSV << simTime() << ", " << i << "," << estimatedIn[i][j]
                    << ", " << estimatedDropped[i][j] << "," << isBogus[i][j]
                    << ", " << droppedPackets[i] << "," << inPackets[i] << ", "
                    << outPackets[i] << endl;
        }
    }
    delete[] estimatedIn;
    delete[] estimatedDropped;
}

std::pair<bool, CoreEvaluation*> getNodeEvaluation(
        std::vector<CoreEvaluation*>& evaluationList, IPSet core) {
    bool detected = false;
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
            return std::make_pair(detected, *evaluation);
        }
    }
    CoreEvaluation* evaluation = NULL;
    return std::make_pair(detected, evaluation);
}

void TrustedAuthority::evaluateCore(IPSet core, IPSet boundary,
        double& dropEstimation, double& inEstimation, bool& bogusEval) {
    bool detected = false;
// Base entry
    std::ostringstream os;
    os << simTime() << "," << "'" << coreToString(core) << "'" << ","
            << isFaulty(core) << ",";
    double outEstimation;
    dropEstimation = -1;
    inEstimation = -1;
    outEstimation = -1;
    bogusEval = false;
    for (auto node = boundary.begin(); node != boundary.end(); node++) {
        // Get the evaluations from that node:
        std::pair<bool, CoreEvaluation*> evaluation = getNodeEvaluation(
                evaluations[IPtoIndex[(*node).getInt()]], core);
        if (evaluation.second != NULL) {
            if (dropEstimation == -1 || inEstimation == -1
                    || outEstimation == -1) {
                dropEstimation = evaluation.second->getDropEstimation();
                inEstimation = evaluation.second->getInEstimation();
                outEstimation = evaluation.second->getOutEstimation();
            } else {
                if (dropEstimation != evaluation.second->getDropEstimation())
                    std::cout
                            << "Received different drop estimations for the same core"
                            << endl;
                if (inEstimation != evaluation.second->getInEstimation())
                    std::cout
                            << "Received different in estimations for the same core"
                            << endl;
                if (outEstimation != evaluation.second->getOutEstimation())
                    std::cout
                            << "Received different in estimations for the same core"
                            << endl;
            }
        } else {
            std::cout << "Estimation not received from " << *node << endl;
        }
        // Update detected
        detected = evaluation.first
                | (evaluation.second->getDropEstimation() > getThreshold(core));
        // Update collusion
        if (faulty[IPtoIndex[node->getInt()]] & collusion(*node, core))
            bogusEval = true;
    }
    coreCSV << os.str() << dropEstimation << "," << inEstimation << ","
            << outEstimation << "," << getRealValues(core) << "," << bogus
            << endl;
}

std::string TrustedAuthority::getRealValues(IPSet core) {
    std::ostringstream os;
    double dropped = 0;
    double in = 0;
    double out = 0;
    for (auto node = core.begin(); node != core.end(); node++) {
        unsigned index = IPtoIndex[node->getInt()];
        dropped += droppedPackets[index];
        in += inPackets[index];
        out += outPackets[index];
    }
    os << dropped << ", " << in << ", " << out;
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
