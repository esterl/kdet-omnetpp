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
                @display("r=,,#707070");
        }
        proxies[numProxies]: WCNNode {
            parameters:
                @display("r=,,#007070");
        }
        channelControl: ChannelControl {
            parameters:
                @display("p=60,50");
        }
        configurator: IPv4NetworkConfigurator {
            @display("p=140,50");
        }
        graphServer: GraphServer;
        ta : TrustedAuthority;
    connections:
        for i=0..numNodes-1 {
            hosts[i].toGraphServer --> graphServer.in++;
            hosts[i].fromGraphServer <-- graphServer.out++;
            hosts[i].toTrustedAuthority --> ta.in++;
        }
        for i=0..numProxies-1 {
            proxies[i].toGraphServer --> graphServer.in++;
            proxies[i].fromGraphServer <-- graphServer.out++;
            proxies[i].toTrustedAuthority --> ta.in++;
        }
}
