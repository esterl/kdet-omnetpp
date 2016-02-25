///***************************************************************************
// *   Copyright (C) 2015 by Ester Lopez                                     *
// *                                                                         *
// *   This program is free software; you can redistribute it and/or modify  *
// *   it under the terms of the GNU General Public License as published by  *
// *   the Free Software Foundation; either version 2 of the License, or     *
// *   (at your option) any later version.                                   *
// *                                                                         *
// *   This program is distributed in the hope that it will be useful,       *
// *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
// *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
// *   GNU General Public License for more details.                          *
// *                                                                         *
// *   You should have received a copy of the GNU General Public License     *
// *   along with this program; if not, write to the                         *
// *   Free Software Foundation, Inc.,                                       *
// *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
// ***************************************************************************/

package kdet.examples;

import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;
import inet.physicallayer.ieee80211.packetlevel.Ieee80211ScalarRadioMedium;

import kdet.WCNNode;
import kdet.utils.topology.GraphServer;
import kdet.protocol.TrustedAuthority;
import kdet.utils.Clock;

network kdet
{
    parameters:
        int numNodes = default(16);
        int numProxies = default(4);
    submodules:
        hosts[numNodes]: WCNNode {
            parameters:
                @display("r=,,#707070;");
        }
        proxies[numProxies]: WCNNode {
            parameters:
                @display("r=,,#007070;");
        }
        radioMedium: Ieee80211ScalarRadioMedium {
            parameters:
                @display("p=60,50");
        }
        configurator: IPv4NetworkConfigurator {
            @display("p=140,50");
        }
        graphServer: GraphServer {
            @display("p=407,250");
        }
        ta: TrustedAuthority {
            @display("p=60,161");
        }
        clock: Clock {
            @display("p=60,250");
        }
    connections:
        for i=0..numNodes-1 {
            hosts[i].toGraphServer --> graphServer.in++;
            hosts[i].fromGraphServer <-- graphServer.out++;
            hosts[i].TAReports --> ta.inReports++;
            hosts[i].TAGroundTruth --> ta.inGroundTruth++;
        }

        for i=0..numProxies-1 {
            proxies[i].toGraphServer --> graphServer.in++;
            proxies[i].fromGraphServer <-- graphServer.out++;
            proxies[i].TAReports --> ta.inReports++;
            proxies[i].TAGroundTruth --> ta.inGroundTruth++;
        }
        for i=0..2, for j=0..numNodes-1 {
            clock.out++ --> hosts[j].clock[i];
        }
        for i=0..2, for j=0..numProxies-1 {
            clock.out++ --> proxies[j].clock[i];
        }
        clock.out++ --> ta.clock;

}
