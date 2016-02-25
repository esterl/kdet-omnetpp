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

#include "Report.h"

namespace kdet{
Hash_CW2<uint8_t> Report::hash = Hash_CW2<uint8_t>(2 << 16); // Results will be within 2^13
uint32_t Report::prime = 131071; //Mersenne prime 17
uint64_t Report::seedPows[10] = { 1, 4804, 9920, 76907, 103150, 84220, 107774,
        15846, 103004, 38191 }; // Random seed and its powers % p

Report::Report() :
        Report_Base() {
    summary_var = NULL;
}

Report::Report(const char *name, int kind) :
        Report_Base(name, kind) {
    summary_var = NULL;
}

Report::Report(const Report& other) :
        Report_Base(other) {
    summary_var = NULL;
    setSummary(other.summary_var);
}

Report::~Report(){
    delete summary_var;
}

Report* Report::dup() const {
    return new Report(*this);
}

bool Report::equals_to(Report* other) {
    return summary_var->getID() == other->summary_var->getID();
}

size_t Report::Hash() const {
    std::vector<inet::IPv4Address> summaryID = summary_var->getID();
    // TODO throw runtime error if addresses too big
    size_t result = 0;
    for (unsigned i = 0; i < summaryID.size(); i++) {
        uint8_t lastByte = summaryID[i].getDByte(3);
        result += mersenne_modulus<size_t>(seedPows[i] * lastByte, 17);
    }
    return mersenne_modulus<size_t>(result, 17);
}

std::set<inet::IPv4Address> Report::sendTo(inet::IPv4Address local,
        std::set<inet::IPv4Address> neighbors) {
    return summary_var->getSendTo(local, neighbors);
}

std::string Report::getName() {
    std::vector<inet::IPv4Address> summaryID = summary_var->getID();
    std::stringstream ss;
    ss << "[";
    for (auto it = summaryID.begin(); it != summaryID.end(); it++) {
        ss << *it << ",";
    }
    ss << ">] : " << simTime();
    return ss.str();
}

Summary* Report::getSummary() const {
    return summary_var;
}

void Report::setSummary(Summary* summary) {
    if (summary == summary_var)
        return;
    // Delete old summary
    if (summary_var != NULL)
        delete summary_var;
    summary_var = summary->copy();
    bytes_var = summary_var->getOptimizedBytes();
}
}
