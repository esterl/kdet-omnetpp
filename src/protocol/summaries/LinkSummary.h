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

#ifndef LINKSUMMARY_H_
#define LINKSUMMARY_H_

#include "Summary.h"

/**
 * Traffic summary based on the second strategy: it keeps information about
 * all the traffic that comes from and to a given link, as well as which node
 * is the source or destination of the packet.
 */
class LinkSummary : public Summary{
public:
    LinkSummary(IPv4Address reporterIP,
            IPv4Address neighborIP) {reporter=reporterIP; neighbor=neighborIP;};
    virtual ~LinkSummary() { };
    virtual IPv4Address getNeighbor(){ return neighbor; };
    virtual std::vector<IPv4Address> getID();
    virtual std::set<IPv4Address> getSendTo(IPv4Address localIP,
            std::set<IPv4Address> neighbors);
    virtual void optimize(std::set<IPv4Address> kHopsNeighbors) = 0;
protected:
    IPv4Address neighbor;
};

#endif /* LINKSUMMARY_H_ */
