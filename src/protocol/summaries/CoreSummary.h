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

#ifndef CORESUMMARY_H_
#define CORESUMMARY_H_

#include "Summary.h"
#include <set>

/**
 * TODO Doc
 */
class CoreSummary: public Summary {
public:
    CoreSummary(const IPv4Address reporterIP = IPv4Address::UNSPECIFIED_ADDRESS,
            const std::set<IPv4Address>& coreIPs = std::set<IPv4Address>()) {
        reporter = reporterIP;
        core = coreIPs;
    }
    ;
    virtual ~CoreSummary() {};
    virtual std::set<IPv4Address> getCore() {
        return core;
    }
    ;
    virtual std::vector<IPv4Address> getID();
    virtual std::set<IPv4Address> getSendTo(IPv4Address localIP,
            std::set<IPv4Address> neighbors);
protected:
    std::set<IPv4Address> core;
};

#endif /* CORESUMMARY_H_ */
