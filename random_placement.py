###################### Random scenarios generation #############################
import random
import math
import json

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
    distances = [ math.sqrt((x-pos[0])**2 + (y-pos[1])**2) for pos in positions ]
    return min(distances)

def get_next_min_distance(positions, x, y, radius):
    distances = [ math.sqrt((x-pos[0])**2 + (y-pos[1])**2) for pos in positions ]
    distances_filtered = [ d for d in distances if d > radius ]
    if not distances_filtered:
        return float("inf")
    return min(distances_filtered)

def get_num_closer(positions, x, y, radius):
    distances = [ math.sqrt((x-pos[0])**2 + (y-pos[1])**2) for pos in positions ]
    result = 0
    for distance in distances:
        if distance < radius:
            result += 1
    return result

def get_max_neighbors(positions, radius):
    max_val = 0
    for i in range(len(positions)):
        max_val = max(max_val, get_num_neighbors(positions, i, radius))
    return max_val

def get_num_neighbors(positions, i, radius):
    result = 0
    (x0, y0) = positions[i]
    for x,y in positions:
        if math.sqrt((x-x0)**2 + (y-y0)**2) < radius:
            result += 1
    return result - 1

def get_random_position(positions, radius):
    (x_0, y_0) = choose_random(positions)
    d = random.uniform(radius/2, radius)
    angle = random.uniform(0, 2*math.pi)
    return (x_0 + d*math.cos(angle), y_0 + d * math.sin(angle))

def getNpositions(N, radius):
    positions = [(0,0)]
    for i in range(N-1):
        (x, y) = get_random_position(positions, radius)
        while ( get_min_distance(positions, x, y) < radius/5 or 
                #get_next_min_distance(positions, x, y, radius) < 1.25*radius or
                get_max_neighbors(positions + [(x,y)], radius) > 5):
            (x, y) = get_random_position(positions, radius)
        positions += [ (x,y) ]
    return positions

def getRandomFiles(N):
    import glob
    filenames = glob.glob("traffic/trafGen*.5")
    return random.sample(filenames, N)

def generateScenario(nHosts, nProxies, faultyProportion, dropProbability, filename):
    positions = getNpositions(nHosts+nProxies, 750)
    random.shuffle(positions)
    faulty = random.sample(range(nHosts), int(nHosts*faultyProportion))
    context = {
        'nHosts': nHosts,
        'nProxies': nProxies,
        'positions': positions,
        'faulty': faulty,
        'selectedProxy': [ random.choice(range(nProxies)) for i in range(nHosts) ],
        'selectedFile': getRandomFiles(nHosts),
        'dropProbability' : dropProbability
    }
    with open(filename, 'w') as context_json:
        json.dump(context, context_json)

import argparse
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Creates random scenarios and saves them into a json file")
    parser.add_argument("basename", help="Base of the filename to generate", 
        type=str)
    parser.add_argument("-N", help="Number of scenarios", default=1, type=int)
    parser.add_argument("nHosts", help="Number of hosts", type=int)
    parser.add_argument("nProxies", help="Number of proxies", type=int)
    parser.add_argument("faultProbability", help="Proportion of faulty hosts", 
        type=float)
    parser.add_argument("dropProbability", 
        help="Drop probability of faulty  nodes", type=float)
    args = parser.parse_args()
    for i in range(args.N):
        filename = args.basename + str(i) + ".json"
        generateScenario(args.nHosts, args.nProxies, args.faultProbability, 
                            args.dropProbability, filename)

