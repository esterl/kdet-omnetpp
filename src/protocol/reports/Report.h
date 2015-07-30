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


#ifndef REPORT_H_
#define REPORT_H_

#include "Report_m.h"

/**
 * Base class for the message of type reports. Derived classes must implement
 * the equals_to operator and the Hash function, so that the RobustFlooding
 * module knows which new reports can replace older versions (equals_to) and
 * save them in a unordered_map using the Hash function.
 */
class Report : public Report_Base {
public:
    Report(const char *name, int kind) : Report_Base(name, kind){};
    Report(const Report& other) : Report_Base(other){};
    virtual Report* dup() const = 0;
    virtual bool equals_to(Report* other) = 0;
    virtual size_t Hash() const = 0;
    virtual std::set<IPv4Address> sendTo(IPv4Address local, std::set<IPv4Address> neighbors) = 0;
    virtual std::string getName() = 0;
    virtual void optimizeSummaries(std::set<IPv4Address> kNeighbors) {};
};

struct ReportHash{
    size_t operator() (const Report* report) const {
        return report->Hash();
    }
};

struct ReportEqual {
    bool operator() (Report* lhs, Report* rhs) const {
        return lhs->equals_to(rhs);
    }
};

#endif /* REPORT_H_ */
