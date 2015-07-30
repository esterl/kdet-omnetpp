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


#include <CoreDetector.h>

#include "IPvXAddressResolver.h"
#include "CoreEvaluation_m.h"
#include "CoreSketchSummary.h"
#include "CoreMonitor.h"

Define_Module(CoreDetector);

void CoreDetector::initialize(int stage) {
    Detector::initialize(stage);
}

void CoreDetector::updateReports(Report* report) {
    CoreReport* coreReport = check_and_cast<CoreReport*>(report);
    //Which core is it?
    unsigned index = findCore(coreReport->getSummary()->getCore());
    if (index >= cores.size()) {
        // Report for a core that is not being monitorized:
        delete report;
        return;
    }
    // Update the map of reports
    if (reports[index].count(coreReport->getReporter().getInt()) != 0) {
        delete reports[index][coreReport->getReporter().getInt()];
    }
    reports[index][coreReport->getReporter().getInt()] = coreReport;
}

void CoreDetector::updateCores(CoresUpdate* update) {
    // TODO Reports should be re-placed...
    clearReports();
    reports.resize(update->getCores().size(),
            std::unordered_map<int, CoreReport*>());
    Detector::updateCores(update);
}

void CoreDetector::evaluateCore(unsigned i, CoreEvaluation* msg) {
    std::set<IPv4Address> core = cores[i];
    std::set<IPv4Address> boundary = boundaries[i];
    std::unordered_map<int, CoreReport*> coreReports = reports[i];
    CoreSketchSummary* globalSummary = NULL;
    std::map<int, bool> receivedSketches;
    for (auto boundaryNode = boundary.begin(); boundaryNode != boundary.end();
            boundaryNode++) {
        receivedSketches[boundaryNode->getInt()] = false;
    }
    for (auto it = coreReports.begin(); it != coreReports.end(); it++) {
        if (globalSummary == NULL) {
            globalSummary = check_and_cast<CoreSketchSummary*>(
                    it->second->getSummary()->copy());
        } else {
            globalSummary->add(it->second->getSummary());
        }
        receivedSketches[it->first] = true;
    }
    double dropPkts, inPkts, outPkts;
    if (globalSummary != NULL) {
        // Estimate the drop %
        dropPkts = globalSummary->estimateDrop(core);
        inPkts = globalSummary->estimateIn(core);
        outPkts = globalSummary->estimateOut(core);
        delete globalSummary;
    } else {
        dropPkts = 0.0;
        inPkts = 0.0;
        outPkts = 0.0;
    }
    msg->setDropEstimation(dropPkts);
    msg->setInEstimation(inPkts);
    msg->setOutEstimation(outPkts);
    msg->setReceivedReports(receivedSketches);
}

void CoreDetector::clearReports() {
    for (auto it = reports.begin(); it != reports.end(); it++) {
        for (auto it2 = it->begin(); it2 != it->end(); it2++) {
            delete it2->second;
        }
        it->clear();
    }
}

unsigned CoreDetector::findCore(std::set<IPv4Address> core) {
    unsigned i;
    for (i = 0; i < cores.size(); i++) {
        if (cores[i] == core) {
            return i;
        }
    }
    return i;
}
