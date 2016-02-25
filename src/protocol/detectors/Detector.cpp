/*   Copyright (C) 2015 by Ester Lopez                                     *
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

#include "Detector.h"
#include "L3AddressResolver.h"
#include "CoreEvaluation_m.h"

namespace kdet{
void Detector::initialize(int stage) {
    if (stage == inet::INITSTAGE_LAST) {
        // Initialize local variables
        IP = inet::L3AddressResolver().addressOf(
                getParentModule()->getParentModule(),
                inet::L3AddressResolver::ADDR_IPv4).toIPv4();
        faulty = par("faulty");
        monitor = check_and_cast<TrafficMonitor*>(
                getParentModule()->getSubmodule("monitor"));
        WATCH_VECTOR(thresholds);
        WATCH_VECTOR(evaluateNextIter);
    }
}

void Detector::finish() {
    clearReports();
}

void Detector::handleMessage(cMessage *msg) {
    if (msg->arrivedOn("clock")) {
        // Schedule evaluation just a bit later, so that we have all sketches
        scheduleAt(
                simTime() + simtime_t(getParentModule()->par("interval")) * .8,
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

void Detector::updateCores(CoresUpdate* update) {
    cores = update->getCores();
    boundaries = update->getBoundaries();
    evaluateNextIter = std::vector<bool>(cores.size(), true);
    double threshold = par("checkProbability").doubleValue() * double(UINT_MAX);
    min_threshold = unsigned(threshold);
    thresholds = std::vector<unsigned>(cores.size(), min_threshold);
    randGenerators.clear();
    std::vector<unsigned> secrets = update->getSecrets();
    for (auto it = secrets.begin(); it < secrets.end(); it++) {
        randGenerators.push_back(std::mt19937(*it));
    }
    monitor->setCores(cores);
    monitor->setShareSummaries(evaluateNextIter);
    delete update;
}

void Detector::evaluateCores() {
    for (unsigned i = 0; i < cores.size(); i++) {
        if (evaluateNextIter[i]) {
            // Share evaluation
            CoreEvaluation* evaluationMsg = new CoreEvaluation(
                    "Core evaluation");
            evaluationMsg->setReporter(IP);
            evaluateCore(i, evaluationMsg);
            evaluationMsg->setCore(
                    std::vector<inet::IPv4Address>(cores[i].begin(),
                            cores[i].end()));
            evaluationMsg->setBogus(faulty);
            send(evaluationMsg, "detectionOut");
            thresholds[i] = updateThreshold(thresholds[i], evaluationMsg);
        }
        evaluateNextIter[i] = getEvaluateNextIter(i);
    }
    //Notify the detector about which cores are to be evaluated next:

    monitor->setShareSummaries(evaluateNextIter);
}

unsigned Detector::updateThreshold(unsigned previous, CoreEvaluation* msg) {
    double inPkts = msg->getInEstimation();
    double outPkts = msg->getOutEstimation();
    double droppedPkts = msg->getDropEstimation();
    // If there has been no traffic, don't change the threshold
    if (inPkts == 0 && outPkts == 0) {
        return previous;
    }
    double maxPkts = ((inPkts > outPkts) ? inPkts : outPkts);
    // TODO configurable threshold
    if (droppedPkts / maxPkts > 0.05 * 0.1 * 2 / (.15)) {
        return previous * 2;
    } else {
        return previous / 2;
    }
}

bool Detector::getEvaluateNextIter(unsigned index) {
    if (par("randomized").boolValue()) {
        unsigned randNum = randGenerators[index]();
        // TODO max num iter without test
        return randNum < thresholds[index];
    } else {
        return true;
    }
}
}
