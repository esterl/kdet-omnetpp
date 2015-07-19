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
    IP = IPvXAddressResolver().addressOf(getParentModule()->getParentModule(),
            IPvXAddressResolver::ADDR_PREFER_IPv4).get4();
    //WATCH(reports);
    faulty = par("faulty");
}

void Detector::finish() {
    clearReports();
}

void Detector::handleMessage(cMessage *msg) {
    if (msg->arrivedOn("clock")) {
        // Schedule evaluation just a bit later, so that we have all sketches
        scheduleAt(
                simTime()
                        + simtime_t(getParentModule()->par("interval")) * .8,
                msg);
    } else if (msg->isSelfMessage()) {
        evaluateCores();
        delete msg;
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
        // Share evaluation
        CoreEvaluation* evaluationMsg = new CoreEvaluation("Core evaluation");
        evaluationMsg->setReporter(getIP());
        evaluateCore(cores[i], boundaries[i], evaluationMsg);
        evaluationMsg->setCore(
                std::vector<IPv4Address>(cores[i].begin(), cores[i].end()));
        evaluationMsg->setBogus(faulty);
        send(evaluationMsg, "detectionOut");
    }
}

void Detector::evaluateCore(std::set<IPv4Address> core,
        std::set<IPv4Address> boundary, CoreEvaluation* msg) {
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
                        globalSummary->add(summaries[coreNode->getInt()]);
                    }
                }
            }
        }
    }
    double dropPerc, inPkts, outPkts;
    if (globalSummary != NULL) {
        // Estimate the drop %
        dropPerc = globalSummary->estimateDrop(core);
        inPkts = globalSummary->estimateIn(core);
        outPkts = globalSummary->estimateOut(core);
        delete globalSummary;
    } else {
        dropPerc = 0.0;
        inPkts = 0.0;
        outPkts = 0.0;
    }
    msg->setDropEstimation(dropPerc);
    msg->setInEstimation(inPkts);
    msg->setOutEstimation(outPkts);
    msg->setReceivedReports(receivedSketches);
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
