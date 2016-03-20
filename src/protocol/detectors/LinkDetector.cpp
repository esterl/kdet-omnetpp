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

namespace kdet {
Define_Module(LinkDetector);

void LinkDetector::updateReports(Report* report) {
    LinkSummary* summary = check_and_cast<LinkSummary*>(report->getSummary());
    // Is there a report from that node?
    inet::IPv4Address source = report->getReporter();
    if (reports.count(source.getInt()) == 0) {
        std::unordered_map<int, Report*> reports_map;
        reports_map[summary->getNeighbor().getInt()] = report;
        reports[source.getInt()] = reports_map;
    } else {
        if (reports[source.getInt()].count(summary->getNeighbor().getInt())
                != 0) {
            delete reports[source.getInt()][summary->getNeighbor().getInt()];
        }
        reports[source.getInt()][summary->getNeighbor().getInt()] = report;
    }
}

void LinkDetector::evaluateCore(unsigned i, CoreEvaluation* msg) {
    std::set<inet::IPv4Address> core = cores[i];
    std::set<inet::IPv4Address> boundary = boundaries[i];
    LinkSummary* globalSummary = NULL;
    std::map<int, bool> receivedSketches;
    std::cout << simTime() << ": Evaluating ";
    for (auto node : core)
        std::cout << node << " ";
    std::cout << std::endl;
    for (auto boundaryNode = boundary.begin(); boundaryNode != boundary.end();
            boundaryNode++) {
        std::cout << " Boundary " << *boundaryNode << std::endl;
        // Is there any report from that node?
        receivedSketches[boundaryNode->getInt()] = false;
        if (reports.count(boundaryNode->getInt()) != 0) {
            std::unordered_map<int, Report*> nodeReports =
                    reports[boundaryNode->getInt()];
            for (auto coreNode = core.begin(); coreNode != core.end();
                    coreNode++) {
                // Is there any report from boundaryNode -- coreNode?
                if (nodeReports.count(coreNode->getInt()) > 0) {
                    std::cout << "Report from " << *boundaryNode << "--" << *coreNode << std::endl;
                    nodeReports[coreNode->getInt()]->getSummary()->print();
                    receivedSketches[boundaryNode->getInt()] = true;
                    if (globalSummary == NULL) {
                        globalSummary =
                                check_and_cast<LinkSummary*>(
                                        nodeReports[coreNode->getInt()]->getSummary()->copy());
                    } else {
                        globalSummary->add(
                                nodeReports[coreNode->getInt()]->getSummary());
                    }
                }
            }
        }
    }
    double dropEstimation, inEstimation, outEstimation;
    double dropPkts, inPkts, outPkts;
    if (globalSummary != NULL) {
        std::cout << " Global ";
        globalSummary->print();
        // Estimate the drop %
        dropEstimation = globalSummary->estimateDrop(core);
        inEstimation = globalSummary->estimateIn(core);
        outEstimation = globalSummary->estimateOut(core);
        dropPkts = globalSummary->getDrop(core);
        inPkts = globalSummary->getIn(core);
        outPkts = globalSummary->getOut(core);
        delete globalSummary;
    } else {
        dropEstimation = 0.0;
        inEstimation = 0.0;
        outEstimation = 0.0;
        dropPkts = 0.0;
        inPkts = 0.0;
        outPkts = 0.0;
    }
    msg->setDropEstimation(dropEstimation);
    msg->setInEstimation(inEstimation);
    msg->setOutEstimation(outEstimation);
    msg->setDropReal(dropPkts);
    msg->setInReal(inPkts);
    msg->setOutReal(outPkts);
    msg->setReceivedReports(receivedSketches);
}

void LinkDetector::clearReports() {
    for (auto it = reports.begin(); it != reports.end(); it++) {
        for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++) {
            delete it2->second;
        }
        it->second.clear();
    }
    reports.clear();
}
}
