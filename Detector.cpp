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

#include "Detector.h"
#include "IPvXAddressResolver.h"
#include "CoreEvaluation_m.h"

Define_Module(Detector);

void Detector::initialize() {
    // Initialize local variables
    interval = getParentModule()->par("interval");
    IP = IPvXAddressResolver().addressOf(getParentModule()->getParentModule(),
            IPvXAddressResolver::ADDR_PREFER_IPv4).get4();
    // Start first detection interval
    timeout = new cMessage("Detector timeout");
    simtime_t waitTime = getParentModule()->par("waitTime");
    scheduleAt(simTime() + waitTime + (interval/2), timeout);
}

void Detector::finish() {
    cancelAndDelete(timeout);
    clearReports();
}

void Detector::handleMessage(cMessage *msg) {
    if (msg->isSelfMessage()) {
        evaluateCores();
        // Re-schedule evaluation
        scheduleAt(simTime() + interval, msg);
        // Empty reports
        clearReports();
    } else if (msg->arrivedOn("graphServerIn")) {
        updateCores(check_and_cast<CoresUpdate*>(msg));
    } else if (msg->arrivedOn("reportsIn")) {
        updateReports(check_and_cast<Report*>(msg));
    } else {
        delete msg;
    }
}

void Detector::updateReports(Report* report) {
    // TODO Report properly signed? (here or at RobustFlooding)
    // Is there a report from that node?
    IPv4Address source = report->getReporter();
    if (reports.count(source.getInt()) == 0) {
        reports[source.getInt()] = report;
    } else {
        // TODO if there are two different versions of the report, it should be invalidated
        if ((reports[source.getInt()])->getId() < report->getId()) {
            delete report;
        } else {
            delete reports[source.getInt()];
            reports[source.getInt()] = report;
        }
    }
}

void Detector::updateCores(CoresUpdate* update) {
    cores = update->getCores();
    boundaries = update->getBoundaries();
    delete update;
}

void Detector::evaluateCores() {
    for (unsigned i = 0; i < cores.size(); i++) {
        std::pair<std::map<int, bool>, double> evaluation = evaluateCore(
                cores[i], boundaries[i]);
        // Share evaluation
        CoreEvaluation* evaluationMsg = new CoreEvaluation("Core evaluation");
        evaluationMsg->setReporter(getIP());
        evaluationMsg->setReceivedReports(evaluation.first);
        evaluationMsg->setDropEstimation(evaluation.second);
        evaluationMsg->setCore(
                std::vector<IPv4Address>(cores[i].begin(), cores[i].end()));
        send(evaluationMsg, "detectionOut");
    }
}

std::pair<std::map<int, bool>, double> Detector::evaluateCore(
        std::set<IPv4Address> core, std::set<IPv4Address> boundary) {
    bool print = (getIP() == IPv4Address("10.0.0.2"))
            & (core.count(IPv4Address("10.0.0.3")) == 1);
//    if (print) {
//        std::cout << "Evaluating core with boundary: ";
//        for (auto it = boundary.begin(); it != boundary.end(); it++)
//            std::cout << it->str() << " ";
//        std::cout << endl;
//        std::cout << "Available reports: ";
//        for (auto it = reports.begin(); it != reports.end(); it++) {
//            std::cout << " " << it->second->getReporter();
//            LinkSummariesHash summaries = it->second->getSummaries();
//            std::cout << " (";
//            for (auto it2 = summaries.begin(); it2 != summaries.end(); it2++){
//                std:: cout << "[" << IPv4Address(it2->first) << "] ";
//                std::cout << it2->second->getHost() << "-->" << it2->second->getNeighbor() << " ";
//            }
//            std::cout << " )" << endl;
//        }
//        std::cout << endl;
//    }
    LinkSummary* globalSummary = NULL;
    std::map<int, bool> receivedSketches;
    for (auto boundaryNode = boundary.begin(); boundaryNode != boundary.end();
            boundaryNode++) {
        // Is there a report for that node?
        if (reports.count(boundaryNode->getInt()) == 0) {
            receivedSketches[boundaryNode->getInt()] = false;
        } else {
            receivedSketches[boundaryNode->getInt()] = true;
            LinkSummariesHash summaries =
                    reports[boundaryNode->getInt()]->getSummaries();
            for (auto coreNode = core.begin(); coreNode != core.end();
                    coreNode++) {
                if (summaries.count(coreNode->getInt()) != 0) {
                    if (globalSummary == NULL) {
                        globalSummary = summaries[coreNode->getInt()]->copy();
                    } else {
                        globalSummary = (*globalSummary)
                                + summaries[coreNode->getInt()];
                    }
                }
            }
        }
    }
    double dropPerc;
    if (globalSummary) {
        // Estimate the drop %
        dropPerc = globalSummary->estimateDrop(core);
        delete globalSummary;
    } else {
        std::cout << "No summaries" << endl;
        dropPerc = 0.0;
    }
    return make_pair(receivedSketches, dropPerc);
}

void Detector::clearReports() {
    for (auto it = reports.begin(); it != reports.end(); it++) {
        delete it->second;
    }
    reports.clear();
}

IPv4Address Detector::getIP() {
    if (IP.isUnspecified()) {
        IP = IPvXAddressResolver().addressOf(
                getParentModule()->getParentModule(),
                IPvXAddressResolver::ADDR_PREFER_IPv4).get4();
    }
    return IP;
}
