import random
import math
import sys
from jinja2 import Template
import textwrap

def choose_far_away(positions):
    x = [ pos[0] for pos in positions ]
    y = [ pos[1] for pos in positions ]
    max_x = max(x)
    min_x = min(x)
    max_y = max(y)
    min_y = min(y)
    extremes_x = [i for i, j in enumerate(x) if j == max_x or j == min_x ]
    extremes_y = [i for i, j in enumerate(x) if j == max_y or j == min_y ]
    extremes = list(set(extremes_x + extremes_y))
    return positions[random.choice(extremes)]

def choose_random(positions):
    return random.choice(positions)

def get_min_distance(positions, x, y):
    distances = [ math.sqrt((x - pos[0])**2 + (y - pos[1])**2) for pos in positions ]
    return min(distances)

def get_num_closer(positions, x, y, radius):
    distances = [ math.sqrt((x - pos[0])**2 + (y - pos[1])**2) for pos in positions ]
    result = 0
    for distance in distances:
        if distance < radius:
            result += 1
    return result

def get_random_position(positions, radius):
    (x_0, y_0) = choose_random(positions)
    d = random.uniform(radius/2, radius)
    angle = random.uniform(0, 2*math.pi)
    return (x_0 + d*math.cos(angle), y_0 + d * math.sin(angle))

def getNpositions(N, radius):
    positions = [(0,0)]
    for i in range(N-1):
        (x, y) = get_random_position(positions, radius)
        while get_min_distance(positions, x, y) < radius/5 and get_num_closer(positions, x, y, radius) > 4:
            (x, y) = get_random_position(positions, radius)
        positions += [ (x,y) ]
    return positions


def generate():
    template = Template( textwrap.dedent("""
        [General]
        network = kdet
        experiment-label = {{ expLabel }}
        output-vector-file = ${resultdir}/${configname}-${runnumber}-{{ expLabel }}.vec
        output-scalar-file = ${resultdir}/${configname}-${runnumber}-{{ expLabel }}.sca
        sim-time-limit = 65s
        **.numNodes = {{ nHosts }}
        **.numProxies = {{ nProxies }}
        **.mobility.numHosts = 100
        **.resultsFile = "results/TA{{ expLabel }}.csv"
        num-rngs = 3
        **.mobility.rng-0 = 1
        **.wlan[*].mac.rng-0 = 2
        #debug-on-errors = true

        tkenv-plugin-path = ../../../etc/plugins

        **.channelNumber = 0

        # channel physical parameters
        *.channelControl.carrierFrequency = 2.4GHz
        *.channelControl.pMax = 2.0mW
        *.channelControl.sat = -110dBm
        *.channelControl.alpha = 2
        *.channelControl.numChannels = 1

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

        # nic settings
        **.wlan[*].bitrate = 2Mbps
        **.wlan[*].mgmt.frameCapacity = 10
        **.wlan[*].mac.address = "auto"
        **.wlan[*].mac.maxQueueSize = 14
        **.wlan[*].mac.rtsThresholdBytes = 3000B
        **.wlan[*].mac.retryLimit = 7
        **.wlan[*].mac.cwMinData = 7
        **.wlan[*].mac.cwMinMulticast = 31
        **.wlan[*].radio.transmitterPower = 2mW
        **.wlan[*].radio.thermalNoise = -110dBm
        **.wlan[*].radio.sensitivity = -85dBm
        **.wlan[*].radio.pathLossAlpha = 2
        **.wlan[*].radio.snirThreshold = 4dB

        #### Malicious behavior
        {% for i in faulty %}
        **.hosts[{{ i }}].*.dropProbability = {{ dropFaulty }}
        **.hosts[{{ i }}].trafGen.numPackets = 0
        **.hosts[{{ i }}].faulty= true
        {% endfor %}
        **.dropProbability = 0.0
        **.k = {{ k }}

        #### Traffic Generator
        **.hosts[*].trafGenType = "IPvXTrafGen"
        **.proxies[*].trafGenType = "IPvXTrafSink"

        **.hosts[*].trafGen.startTime = uniform(20s,100s)
        **.hosts[*].trafGen.sendInterval = 100ms
        **.hosts[*].trafGen.numPackets = 100
        **.hosts[*].trafGen.protocol = 258
        **.hosts[*].trafGen.packetLength = 800B
        **.hosts[*].trafGen.destAddresses = "proxies[*]"
        **.proxies[*].trafGen.protocol = 258

        ### Vector recording
        **.ta.*.vector-recording = true
        **.flooding.*.vector-recording = true
        **.vector-recording = false
        # Routing
        **.routingProtocol = "OLSR"
        **.Tc_redundancy = 2    
        **.waitTime = 20s  
    """))
    nHosts = 20
    nProxies = 6
    positions = getNpositions(nHosts+nProxies, 200)
    random.shuffle(positions)
    faulty = random.sample(range(nHosts), int(nHosts*float(sys.argv[4])))
    context = {
        'expLabel': sys.argv[1],
        'nHosts': nHosts,
        'nProxies': nProxies,
        'positions': positions,
        'dropFaulty': float(sys.argv[2]),
        'k': int(sys.argv[3]),
        'faulty': faulty,
    }
    print template.render(context)


generate()
