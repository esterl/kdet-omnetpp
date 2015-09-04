from jinja2 import Template, Environment
import json
import textwrap

def generate(context_file, experiment_label, dropProbability):
    
    context = json.load(context_file)
    context['expLabel'] = experiment_label
    context['dropProbability'] = dropProbability
    
    template = Template( textwrap.dedent("""
        [General]
        network = kdet
        experiment-label = {{ expLabel }}
        output-vector-file = ${resultdir}/${configname}-${runnumber}-{{ expLabel }}.vec
        output-scalar-file = ${resultdir}/${configname}-${runnumber}-{{ expLabel }}.sca
        sim-time-limit = 500s
        **.numNodes = {{ nHosts }}
        **.numProxies = {{ nProxies }}
        **.mobility.numHosts = 200
        **.nodeResults = "${resultdir}/node_{{ expLabel }}.csv"
        **.coreResults = "${resultdir}/core_{{ expLabel }}.csv"
        num-rngs = 3
        **.mobility.rng-0 = 1
        **.wlan[*].mac.rng-0 = 2
        #debug-on-errors = true
        **.interval = 20s

        tkenv-plugin-path = ../../../etc/plugins

        **.channelNumber = 0

        # mobility
        {% for i in range(nHosts) %}
        **.hosts[{{ i }}].mobility.constraintAreaMinX = {{ positions[i][0] }}m
        **.hosts[{{ i }}].mobility.constraintAreaMaxX = {{ positions[i][0] }}m
        **.hosts[{{ i }}].mobility.constraintAreaMinY = {{ positions[i][1] }}m
        **.hosts[{{ i }}].mobility.constraintAreaMaxY = {{ positions[i][1] }}m
        {% endfor %}
        {% for i in range(nProxies) %}
        **.proxies[{{ i }}].mobility.constraintAreaMinX = {{ positions[i+nHosts][0] }}m
        **.proxies[{{ i }}].mobility.constraintAreaMaxX = {{ positions[i+nHosts][0] }}m
        **.proxies[{{ i }}].mobility.constraintAreaMinY = {{ positions[i+nHosts][1] }}m
        **.proxies[{{ i }}].mobility.constraintAreaMaxY = {{ positions[i+nHosts][1] }}m
        {% endfor %}
        **.mobility.constraintAreaMinZ = 0m
        **.mobility.constraintAreaMaxZ = 0m
        **.mobilityType = "StationaryMobility"

        #### Malicious behavior
        {% for i in faulty %}
        **.hosts[{{ i }}].*.dropProbability = {{ dropProbability }}
        **.hosts[{{ i }}].trafGen.numPackets = 0
        **.hosts[{{ i }}].faulty= true
        {% endfor %}
        **.dropProbability = 0.0
        **.k = 1

        #### Traffic Generator
        **.trafGenType = "WCNTrafGen"

        **.hosts[*].trafGen.startTime = uniform(61s, 70s)
        **.hosts[*].trafGen.protocol = 258
        {% for i in range(nHosts) if i not in faulty %}
        **.hosts[{{ i }}].trafGen.destAddresses = "proxies[{{ selectedProxy[i]}}]"
        **.hosts[{{ i }}].trafGen.filename = "{{ selectedFile[i]}}"
        {% endfor  %}
        **.proxies[*].trafGen.protocol = 258

        ### Vector recording
        **.ta.*.vector-recording = true
        **.dropper.*.vector-recording = true
        **.flooding.*.vector-recording = true
        **.vector-recording = false
        # Routing
        **.routingProtocol = "OLSR"
        **.Tc_redundancy = 2    
        **.waitTime = 60s  
        **.maxDistance = 750m
        
        # Implementation
        **.monitorType = "LinkMonitor"
        **.detectorType = "LinkDetector"
        
        [Config interval]
        **.interval = ${interval=5,10, 20, 30, 60}s
        
        [Config sketchSize]
        **.sketchNumRows = ${rows=1, 8, 16, 32}
        **.sketchNumColumns = ${columns=8, 16, 32}
        
        [Config sketchType]
        **.sketchType = ${sketchType="FastCount", "FAGMS"}
        
        [Config k]
        **.k = ${k=1..4}
        
        [Config implementation]
        **.monitorType = ${implementation="CoreMonitor", "LinkMonitor"}
        **.detectorType = ${"CoreDetector", "LinkDetector" ! implementation }
    """))
    print template.render(context)

import argparse
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Dumps a *.ini file for a KDet experiment")
    parser.add_argument("context", help="Name of the json file with the experiment scenario", type=argparse.FileType('r'))
    parser.add_argument("experiment_label", help="Label of the experiment", type=str)
    parser.add_argument("dropProbability", 
        help="Drop probability of faulty  nodes", type=float)
    args = parser.parse_args()
    generate(args.context, args.experiment_label, args.dropProbability)

