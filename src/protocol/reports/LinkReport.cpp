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


#include <LinkReport.h>
#include <functional>
#include <sstream>
Register_Class(LinkReport);

void LinkReport::copy(const LinkReport& other) {
    if (this == &other)
        return;
    // Delete summaries
    for (auto it = summaries_var.begin(); it != summaries_var.end(); it++) {
        delete it->second;
    }
    summaries_var.clear();
    setIndex(other.getIndex());
    setVersion(other.getVersion());
    setTTL(other.getTTL());
    setBogus(other.getBogus());
    setReporter(other.getReporter());
    setSummaries(other.summaries_var);
}

LinkReport::LinkReport(const LinkReport& other) :
        LinkReport_Base(other) {
    setSummaries(other.summaries_var);
}

LinkReport::~LinkReport() {
    // Delete summaries
    for (auto it = summaries_var.begin(); it != summaries_var.end(); it++) {
        delete it->second;
    }
    summaries_var.clear();
}

LinkReport& LinkReport::operator=(const LinkReport& other) {
    if (this == &other)
        return *this;
    cPacket::operator=(other);
    copy(other);
    return *this;
}

LinkReport* LinkReport::dup() const {
    return new LinkReport(*this);
}

void LinkReport::setSummaries(const LinkSummariesHash& summaries) {
    if (&summaries == &summaries_var)
        return;
    // Delete old summaries
    for (auto it = summaries_var.begin(); it != summaries_var.end(); it++) {
        delete it->second;
    }
    summaries_var.clear();
    for (auto it = summaries.begin(); it != summaries.end(); it++) {
        summaries_var[it->first] = dynamic_cast<LinkSummary*>(it->second->copy());
    }
    // update Bytes
    updateBytes();
}

LinkSummariesHash LinkReport::getSummaries() {
    return summaries_var;
}

void LinkReport::optimizeSummaries(std::set<IPv4Address> coreNodes) {
    for (auto it = summaries_var.begin(); it != summaries_var.end(); it++) {
        it->second->optimizeSummary(coreNodes);
    }
    updateBytes();
}

void LinkReport::updateBytes() {
    bytes_var = 0;
    for (auto it = summaries_var.begin(); it != summaries_var.end(); it++) {
        bytes_var += it->second->getBytes();
    }
}

bool LinkReport::equals_to(Report* other) {
    return reporter_var == other->getReporter();
}

size_t LinkReport::Hash() const{
    std::hash<unsigned> hash_fn;
    return hash_fn(reporter_var.getInt());
}

std::set<IPv4Address> LinkReport::sendTo(IPv4Address local,
        std::set<IPv4Address> neighbors) {
    return neighbors;
}


std::string LinkReport::getName() {
    std::stringstream ss;
    ss << reporter_var.str() << ":" << version_var;
    return ss.str();
}
