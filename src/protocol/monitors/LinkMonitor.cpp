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

#include <monitors/LinkMonitor.h>
#include "LinkSummary.h"

namespace kdet {
Define_Module(LinkMonitor);

void LinkMonitor::setCores(std::vector<std::set<inet::IPv4Address>> cores) {
    resetSummaries();
    // Make sure it works:
    for (auto it : summaries) {
        delete it.second;
    }
    summaries.clear();
    for (auto core : cores) {
        if (core.size() == 1) {
            auto neighbor = core.begin();
            summaries[neighbor->getInt()] = new LinkSummary(IP, *neighbor);
        }
    }
    TrafficMonitor::setCores(cores);
}

void LinkMonitor::handleMessage(cMessage *msg) {
    for (auto it = summaries.begin(); it != summaries.end(); it++) {
        std::set<inet::IPv4Address> coreAddresses;
        // See if there is any core for that link that needs to share its summaries:
        for (unsigned i = 0; i < cores.size(); i++) {
            if ((shareSummaries[i] || !par("randomized").boolValue())
                    && cores[i].count(inet::IPv4Address(it->first)) != 0) {
                coreAddresses.insert(cores[i].begin(), cores[i].end());
            }
        }
        if (coreAddresses.size() > 0) {
            Report* report = new Report();
            report->setReporter(IP);
            LinkSummary* linkSummary = check_and_cast<LinkSummary*>(it->second);
            linkSummary->optimize(coreAddresses);
            report->setSummary(linkSummary);
            report->setBogus(faulty);
            report->setName(report->getName().c_str());
            send(report, "reports");
            linkSummary->clear();
        }
    }
    delete msg;
}

void LinkMonitor::finish() {
    double bytes = 0.;
    for (auto it = summaries.begin(); it != summaries.end(); it++) {
        bytes += it->second->getBytes();
        delete it->second;
    }
    summaries.clear();
    recordScalar("MemoryBytes", bytes);
    TrafficMonitor::finish();
}

void LinkMonitor::resetSummaries() {
    for (auto it = summaries.begin(); it != summaries.end(); it++) {
        it->second->clear();
    }
}

std::vector<Summary*> LinkMonitor::findSummaries(inet::IPv4Address addr) {
    if (summaries.count(addr.getInt()) == 0) {
        summaries[addr.getInt()] = new LinkSummary(IP, addr);
    }
    std::vector<Summary*> result;
    result.push_back(summaries[addr.getInt()]);
    return result;
}
}
