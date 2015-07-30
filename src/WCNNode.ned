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

import kdet.utils.topology.TopologyAnnouncer;
import kdet.protocol.IKDet;
import kdet.utils.Dropper;


module WCNNode extends AdhocHost
{
    parameters:
        string trafGenType;
        string kdetImplementation = default("KDetProtocol");
        bool faulty = default(false);
        dropper.faulty = faulty;
        monitor.faulty = faulty;
        networkLayer.arp.globalARP = true;
    gates:
        input fromGraphServer;
        output toGraphServer;
        output TAReports;
        output TAGroundTruth;
		input clock[3];		
        // For FakeFlooding
        // output othersOut;
        // input othersIn;
    submodules:
        monitor: <kdetImplementation> like IKDet {
            @display("p=635,353");
        }
        topology: TopologyAnnouncer {
            @display("p=635,256");
        }
        trafGen: <trafGenType> like IIPvXTrafficGenerator {
            @display("p=255,134");
        }
        dropper: Dropper;
    connections:
        fromGraphServer --> monitor.graphServerIn;
        topology.out --> toGraphServer;
        monitor.detectorOut --> TAReports;
        networkLayer.transportOut++ --> trafGen.ipIn;
        networkLayer.transportIn++ <-- trafGen.ipOut;
        dropper.out --> TAGroundTruth;
        // Fake flooding
        // monitor.IPout --> othersOut;
        // monitor.IPin <-- othersIn;
        // Robust flooding
        monitor.IPout --> networkLayer.transportIn++;
        monitor.IPin <-- networkLayer.transportOut++;
        // Clock
        for i=0..1 {
            clock[i] --> monitor.clock[i];
        }
        clock[2] --> dropper.clock;
}
