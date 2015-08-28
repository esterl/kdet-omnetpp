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

#include "CoreSummary.h"

std::vector<IPv4Address> CoreSummary::getID() {
    std::vector<IPv4Address> id;
    id.push_back(reporter);
    for (auto it = core.begin(); it != core.end(); it++)
        id.push_back(*it);
    return id;
}
std::set<IPv4Address> CoreSummary::getSendTo(IPv4Address localIP,
        std::set<IPv4Address> neighbors) {
    if (localIP == reporter) {
        std::set<IPv4Address> result;
        for (auto it = core.begin(); it != core.end(); it++) {
            if (neighbors.count(*it) > 0) {
                result.insert(*it);
            }
        }
        return result;
    } else if (core.count(localIP) > 0) {
        return neighbors;
    }
    return std::set<IPv4Address>();
}
