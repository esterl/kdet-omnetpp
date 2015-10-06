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
#include "Summary.h"
#include <hash.h>

/**
 * Base class for the message of type reports. Derived classes must implement
 * the equals_to operator and the Hash function, so that the RobustFlooding
 * module knows which new reports can replace older versions (equals_to) and
 * save them in a unordered_map using the Hash function.
 */
class Report : public Report_Base {
public:
    Report();
    Report(const char *name, int kind);
    Report(const Report& other);
    virtual Report* dup() const;
    virtual bool equals_to(Report* other);
    virtual size_t Hash() const;
    virtual std::set<IPv4Address> sendTo(IPv4Address local, std::set<IPv4Address> neighbors);
    virtual std::string getName();
    Summary* getSummary() const;
    void setSummary(Summary* summary);
protected:
    Summary *summary_var;
    // For the hash function
    static Hash_CW2<uint8_t> hash; // Results will be within 2^13
    static uint32_t prime; //Mersenne prime 17
    static uint64_t seedPows[10]; // Random seed and its powers % p (max k == 9)
};

Register_Class(Report);

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
