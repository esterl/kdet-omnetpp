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

#include <CoreReport.h>

Hash_CW2<uint8_t> CoreReport::hash = Hash_CW2<uint8_t>(2 << 16); // Results will be within 2^13
uint32_t CoreReport::prime = 131071; //Mersenne prime 17
uint64_t CoreReport::seedPows[5] = { 1, 4804, 9920, 76907, 103150 }; // Random seed and its powers % p

CoreReport::CoreReport(const CoreReport& other) :
        CoreReport_Base(other) {
    setSummary(other.summary_var);
}

CoreReport::~CoreReport() {
    if (summary_var != NULL)
        delete summary_var;
}

CoreReport* CoreReport::dup() const {
    CoreReport* duppedReport = new CoreReport();
    duppedReport->TTL_var = TTL_var;
    duppedReport->bogus_var = bogus_var;
    duppedReport->index_var = index_var;
    duppedReport->version_var = version_var;
    duppedReport->TTL_var = TTL_var;
    duppedReport->bogus_var = bogus_var;
    duppedReport->reporter_var = reporter_var;
    duppedReport->bytes_var = bytes_var;
    duppedReport->setKind(getKind());
    duppedReport->summary_var = check_and_cast<CoreSketchSummary*>(
            summary_var->copy());
    return duppedReport;
}

bool CoreReport::equals_to(Report* other) {
    CoreReport* otherPtr = dynamic_cast<CoreReport*>(other);
    if (otherPtr == NULL)
        return false;
    return reporter_var == otherPtr->reporter_var
            && summary_var->getCore() == otherPtr->summary_var->getCore();
}

size_t CoreReport::Hash() const {
    uint8_t lastByte = reporter_var.getDByte(3);
    size_t result = CoreReport::seedPows[0] * lastByte;
    const std::set<IPv4Address> core = summary_var->getCore();
    unsigned i = 1;
    for (auto it = core.begin(); it != core.end(); it++) {
        uint8_t lastByte = it->getDByte(3);
        result += mersenne_modulus<size_t>(seedPows[i] * lastByte, 17);
        i++;
    }
    return mersenne_modulus<size_t>(result, 17);

}

std::set<IPv4Address> CoreReport::sendTo(IPv4Address local,
        std::set<IPv4Address> neighbors) {
    if (reporter_var == local) {
        // Send to neighbors that are part of the core:
        std::set<IPv4Address> core = summary_var->getCore();
        std::set<IPv4Address> result;
        for (auto it = core.begin(); it != core.end(); it++) {
            if (neighbors.count(*it) != 0) {
                result.insert(*it);
            }
        }
        return result;
    } else {
        return neighbors;
    }
}

std::string CoreReport::getName() {
    std::stringstream ss;
    ss << "[" << reporter_var << " <";
    const std::set<IPv4Address> core = summary_var->getCore();
    for (auto it = core.begin(); it != core.end(); it++) {
        ss << *it << ",";
    }
    ss << ">] : " << version_var;
    return ss.str();
}

CoreSketchSummary* CoreReport::getSummary() {
    return summary_var;
}

void CoreReport::setSummary(CoreSketchSummary* summary) {
    if (summary == summary_var)
        return;
    // Delete old summary
    if (summary_var != NULL)
        delete summary_var;
    summary_var = dynamic_cast<CoreSketchSummary*>(summary->copy());
    bytes_var = summary_var->getOptimizedBytes();
}
