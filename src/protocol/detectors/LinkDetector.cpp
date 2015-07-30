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

#include <LinkDetector.h>

#include "CoreEvaluation_m.h"

Define_Module(LinkDetector);

void LinkDetector::updateReports(Report* report) {
    LinkReport *linkReport = check_and_cast<LinkReport*>(report);
    // Is there a report from that node?
    IPv4Address source = report->getReporter();
    if (reports.count(source.getInt()) == 0) {
        reports[source.getInt()] = linkReport;
    } else {
        delete reports[source.getInt()];
        reports[source.getInt()] = linkReport;
    }
}

void LinkDetector::evaluateCore(unsigned i, CoreEvaluation* msg) {
    std::set<IPv4Address> core = cores[i];
    std::set<IPv4Address> boundary = boundaries[i];
    LinkSummary* globalSummary = NULL;
    std::map<int, bool> receivedSketches;
    for (auto boundaryNode = boundary.begin(); boundaryNode != boundary.end();
            boundaryNode++) {
//        // Is there a report for that node?
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
                        globalSummary = check_and_cast<LinkSummary*>(
                                summaries[coreNode->getInt()]->copy());
                    } else {
                        globalSummary->add(summaries[coreNode->getInt()]);
                    }
                }
            }
        }
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

void LinkDetector::clearReports() {
    for (auto it = reports.begin(); it != reports.end(); it++) {
        delete it->second;
    }
    reports.clear();
}

