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

package kdet;

import inet.nodes.inet.AdhocHost;
import inet.applications.generic.IIPvXTrafficGenerator;

module WCNNode extends AdhocHost
{
    parameters:
        string trafGenType;
    gates:
        input fromGraphServer;
        output toGraphServer;
        output toTrustedAuthority;
    submodules:
        monitor: KDetProtocol {
            @display("p=635,353");
        }
        topology: TopologyAnnouncer {
            @display("p=635,256");
        }
        trafGen: <trafGenType> like IIPvXTrafficGenerator {
            @display("p=255,134");
        }
    connections:
        fromGraphServer --> monitor.graphServerIn;
        monitor.IPout --> networkLayer.transportIn++;
        monitor.IPin <-- networkLayer.transportOut++;
        topology.out --> toGraphServer;
        monitor.detectorOut --> toTrustedAuthority;
        networkLayer.transportOut++ --> trafGen.ipIn;
        networkLayer.transportIn++ <-- trafGen.ipOut;
}
