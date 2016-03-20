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

#include "CoreMonitor.h"
#include "CoreSummary.h"
#include "Report.h"

namespace kdet {
Define_Module(CoreMonitor);

void CoreMonitor::initialize(int stage) {
    TrafficMonitor::initialize(stage);
    if (stage == 3) {
        WATCH_VECTOR(summaries);
    }
}

void CoreMonitor::handleMessage(cMessage *msg) {
    // Send a report per summary
    for (unsigned i = 0; i < summaries.size(); i++) {
        if (shareSummaries[i]) {
            Report* report = new Report();
            report->setReporter(IP);
            report->setSummary(summaries[i]);
            report->setBogus(faulty);
            report->setName(report->getName().c_str());
            send(report, "reports");
            summaries[i]->clear();
        }
    }
    delete msg;
}

void CoreMonitor::finish() {
    double bytes = 0.0;
    for (auto it = summaries.begin(); it != summaries.end(); it++) {
        bytes += (*it)->getBytes();
        delete (*it);
    }
    summaries.clear();
    recordScalar("MemoryBytes", bytes);
    TrafficMonitor::finish();
}

void CoreMonitor::resetSummaries() {
    for (auto it : summaries) {
        summaries.clear();
    }
}

void CoreMonitor::deleteSummaries() {
    for (auto it : summaries ) {
        delete it;
    }
    summaries.clear();
}

void CoreMonitor::setCores(std::vector<std::set<inet::IPv4Address>> newCores) {
    cores = newCores;
    deleteSummaries();
    for (auto it : cores) {
        summaries.push_back(new CoreSummary(IP, it));
    }
}

std::vector<Summary*> CoreMonitor::findSummaries(inet::IPv4Address addr) {
    std::vector<Summary*> result;
    for (auto it : summaries) {
        CoreSummary* summary = check_and_cast<CoreSummary*>(it);
        if (summary->getCore().count(addr) != 0) {
            result.push_back(summary);
        }
    }
    return result;
}

}
