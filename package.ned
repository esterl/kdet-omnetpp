package kdet;

@license(LGPL);

import inet.networklayer.autorouting.ipv4.IPv4NetworkConfigurator;
import inet.world.radio.ChannelControl;

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
//        flooder: Flooder {
//            @display("p=407,161");
//        }
        clock: Clock {
            @display("p=60,250");
        }
    connections:
        for i=0..numNodes-1 {
            hosts[i].toGraphServer --> graphServer.in++;
            hosts[i].fromGraphServer <-- graphServer.out++;
            hosts[i].TAReports --> ta.inReports++;
            hosts[i].TAGroundTruth --> ta.inGroundTruth++;
//            hosts[i].othersOut --> flooder.in++;
//            hosts[i].othersIn <-- flooder.out++;
        }

        for i=0..numProxies-1 {
            proxies[i].toGraphServer --> graphServer.in++;
            proxies[i].fromGraphServer <-- graphServer.out++;
            proxies[i].TAReports --> ta.inReports++;
            proxies[i].TAGroundTruth --> ta.inGroundTruth++;
//            proxies[i].othersOut --> flooder.in++;
//            proxies[i].othersIn <-- flooder.out++;

        }
        for i=0..2, for j=0..numNodes-1 {
            clock.out++ --> hosts[j].clock[i];
        }
        for i=0..2, for j=0..numProxies-1 {
            clock.out++ --> proxies[j].clock[i];
        }
        clock.out++ --> ta.clock;

}
