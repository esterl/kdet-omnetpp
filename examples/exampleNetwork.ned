//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package kdet.examples;

import inet.networklayer.autorouting.ipv4.IPv4NetworkConfigurator;
import inet.world.radio.ChannelControl;

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
        channelControl: ChannelControl {
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
