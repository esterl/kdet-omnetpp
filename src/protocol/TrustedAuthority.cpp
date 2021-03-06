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

#include "TrustedAuthority.h"
#include "CoreEvaluation_m.h"
#include "DropperReport_m.h"
#include "IPv4Address.h"

namespace kdet {
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
    nodeCSV.open(par("nodeResults").stringValue());
    nodeCSV << "Timestamp, Node, CoreEstimationIn, CoresEstimationDropped, "
            << "Collusion, NodeDrop, NodeIn, NodeOut" << endl;
    coreCSV.open(par("coreResults").stringValue());
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
        inet::IPv4Address addr = evaluation->getReporter();
        IPtoIndex[addr.getInt()] = gate;
        evaluations[gate].push_back(evaluation);
        // Received all evaluations?
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
        scheduleAt(simTime() + simtime_t(par("interval")) * .9, msg);
    } else {
        // Evaluate KDet performance
        evaluateKDet();
        // Clear reports
        clearEvaluations();
        delete msg;
    }
}

std::string coreToString(std::set<inet::IPv4Address> core) {
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
    std::vector<double> *realIn = new std::vector<double>[numNodes];
    std::vector<double> *realDropped = new std::vector<double>[numNodes];
    std::vector<bool> *isBogus = new std::vector<bool>[numNodes];
    for (unsigned i = 0; i < cores.size(); i++) {
        double dropEstimation, inEstimation, dropReal, inReal;
        bool bogusEval;
        evaluateCore(cores[i], boundaries[i], dropEstimation, inEstimation,
                dropReal, inReal, bogusEval);
        for (auto node = cores[i].begin(); node != cores[i].end(); node++) {
            estimatedIn[IPtoIndex[node->getInt()]].push_back(inEstimation);
            estimatedDropped[IPtoIndex[node->getInt()]].push_back(dropEstimation);
            realIn[IPtoIndex[node->getInt()]].push_back(inReal);
            realDropped[IPtoIndex[node->getInt()]].push_back(dropReal);
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
    delete[] isBogus;
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
        double& dropEstimation, double& inEstimation, double& dropReal,
        double &inReal, bool& bogusEval) {
    bool detected = true;
// Base entry
    std::ostringstream os;
    os << simTime() << "," << "'" << coreToString(core) << "'" << ","
            << isFaulty(core) << ",";
    double outEstimation;
    dropEstimation = -1;
    inEstimation = -1;
    outEstimation = -1;
    inReal = -1;
    dropReal = -1;
    bogusEval = false;
    for (auto node = boundary.begin(); node != boundary.end(); node++) {
        // Get the evaluations from that node:
        std::pair<bool, CoreEvaluation*> evaluation = getNodeEvaluation(
                evaluations[IPtoIndex[(*node).getInt()]], core);
        if (evaluation.second != NULL) {
            if (!evaluation.first) {
                if (dropEstimation == -1 || inEstimation == -1
                        || outEstimation == -1) {
                    dropEstimation = evaluation.second->getDropEstimation();
                    inEstimation = evaluation.second->getInEstimation();
                    outEstimation = evaluation.second->getOutEstimation();
                    inReal = evaluation.second->getInReal();
                    dropReal = evaluation.second->getDropReal();
                } else {
                    if (dropEstimation
                            != evaluation.second->getDropEstimation())
                        std::cout << simTime()
                                << "Received different drop estimations for the same core: "
                                << dropEstimation << " "
                                << evaluation.second->getDropEstimation()
                                << endl;
                    if (inEstimation != evaluation.second->getInEstimation())
                        std::cout << simTime()
                                << "Received different in estimations for the same core: "
                                << inEstimation << " "
                                << evaluation.second->getInEstimation() << endl;
                    if (outEstimation != evaluation.second->getOutEstimation())
                        std::cout << simTime()
                                << "Received different out estimations for the same core: "
                                << outEstimation << " "
                                << evaluation.second->getOutEstimation()
                                << endl;
                }
                // Bitmap marks a missing sketch
            } else {
                std::cout << *node << ";";
            }
            // Update detected
            detected = evaluation.first
                    | (evaluation.second->getDropEstimation()
                            > getThreshold(core));
            // Update collusion
            if (faulty[IPtoIndex[node->getInt()]] & collusion(*node, core))
                bogusEval = true;
        } else {
            std::cout << "Estimation not received from " << *node << endl;
        }

    }
    //    coreCSV << os.str() << dropEstimation << "," << inEstimation << ","
    //            << outEstimation << "," << getRealValues(core) << "," << bogusEval
    //            << endl;
    coreCSV << os.str() << dropEstimation << "," << inEstimation << ","
            << outEstimation << "," << dropReal << "," << inReal << ","
            << inReal - dropReal << "," << bogusEval << endl;
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

bool TrustedAuthority::collusion(inet::IPv4Address boundaryNode,
        std::set<inet::IPv4Address> core) {
// Is there any neighbor of boundaryNode in the core & faulty?
    std::set<inet::IPv4Address> neighbors = graphServer->getNeighbors(
            boundaryNode);
    for (auto it = neighbors.begin(); it != neighbors.end(); it++) {
        if (faulty[IPtoIndex[it->getInt()]] & core.count(*it) != 0)
            return true;
    }
    return false;
}
double TrustedAuthority::getThreshold(std::set<inet::IPv4Address> core) {
    return threshold;
}

bool TrustedAuthority::isFaulty(std::set<inet::IPv4Address> core) {
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
}
