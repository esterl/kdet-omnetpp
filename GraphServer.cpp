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

#include "GraphServer.h"

Define_Module(GraphServer);

template<typename iterator>
std::set<IPv4Address> indexToIP(iterator begin, iterator end,
        std::map<int, std::string>& dictionary) {
    std::set<IPv4Address> result;
    for (iterator it = begin; it != end; it++) {
        result.insert(IPv4Address(dictionary[*it].c_str()));
    }
    return result;
}

std::set<int> GraphServer::getNeighbors(int node) {
    return networkGraph[node];
}

std::set<IPv4Address> GraphServer::getNeighbors(IPv4Address addr) {
    int index = name_to_index[addr.str()];
    std::set<int> neighbors = getNeighbors(index);
    return indexToIP(neighbors.begin(), neighbors.end(), index_to_name);
}

std::set<int> GraphServer::getNeighborhood(std::set<int> nodes) {
    std::set<int> neighborhood;
    for (auto it = nodes.begin(); it != nodes.end(); it++) {
        std::set<int> node_neighborhood = getNeighbors(*it);
        neighborhood.insert(node_neighborhood.begin(), node_neighborhood.end());
    }
    for (auto it = nodes.begin(); it != nodes.end(); it++) {
        neighborhood.erase(*it);
    }
    return neighborhood;
}

void GraphServer::initialize() {
    k = par("k");
}

void GraphServer::handleMessage(cMessage *msg) {
    NeighborsAnnouncement* announcement =
            check_and_cast<NeighborsAnnouncement*>(msg);
// Update connections on graph:
    updateGraph(announcement);
// Send update to nodes
    sendUpdate();
// Delete message:
    delete msg;
}

int GraphServer::getVertixIndex(IPv4Address node) {
    unsigned index;
// Is the node in the map?
    if (name_to_index.find(node.str()) == name_to_index.end()) {
        //Add a vertex to the graph
        index = networkGraph.size();
        networkGraph.push_back(std::set<int>());
        index_to_name[index] = node.str();
        name_to_index[node.str()] = index;
    } else {
        index = name_to_index[node.str()];
    }
    return index;
}

void GraphServer::updateGraph(NeighborsAnnouncement* msg) {
    IPv4Address node = msg->getNode();

// Printers
    if (node == IPv4Address("10.0.0.3")) {
        std::cout << "Neighbors for 3 (" << msg->getSeqNumber() << ") :";
        for (auto it = msg->getNeighbors().begin();
                it != msg->getNeighbors().end(); it++) {
            std::cout << (*it) << " ";
        }
        std::cout << endl;
    }

    unsigned from = getVertixIndex(node);
    index_to_gate[from] = msg->getArrivalGate()->getIndex();
// Update edges [Only adds neighbors]:
    for (auto it = msg->getNeighbors().begin(); it != msg->getNeighbors().end();
            it++) {
        unsigned to = getVertixIndex(*it);
        networkGraph[from].insert(to);
    }
    updateCores();
}

std::vector<std::vector<int>> GraphServer::getCores(int node) {
    std::set<int> usedNodes, neighbors;
    std::vector<std::vector<int>> cores;
    std::vector<int> partialCore;
    usedNodes.insert(node);
    for (int i = 1; i <= k; i++) {
        neighbors = getNeighbors(node);
        std::vector<std::vector<int>> cores_i = getCoresRecursive(i,
                partialCore, neighbors, usedNodes);
        cores.insert(cores.end(), cores_i.begin(), cores_i.end());
    }
    return cores;
}

template<typename iterator>
void printIter(iterator begin, iterator end) {
    for (iterator it = begin; it != end; it++) {
        std::cout << (*it) << " ";
    }
    std::cout << endl;
}
std::vector<std::vector<int>> GraphServer::getCoresRecursive(unsigned k,
        std::vector<int> partialCore, std::set<int> &possibleNodes,
        std::set<int> usedNodes) {
    std::vector<std::vector<int>> result;
// Is partialCore already complete?
    if (partialCore.size() == k) {
        result.push_back(partialCore);
        return result;
    }

// Choose every node from the set of possible nodes:
    for (auto it = possibleNodes.begin(); it != possibleNodes.end(); it++) {
        partialCore.push_back(*it);
        // Update used nodes:
        usedNodes.insert(*it);
        // Update list of possible nodes:
        std::set<int> newPossible = std::set<int>(possibleNodes);
        std::set<int> neighbors = getNeighbors(*it);
        newPossible.insert(neighbors.begin(), neighbors.end());
        for (auto aux = usedNodes.begin(); aux != usedNodes.end(); aux++) {
            newPossible.erase(*aux);
        }
        //newPossible.erase(usedNodes.begin(), usedNodes.end());
        std::vector<std::vector<int>> partialResult = getCoresRecursive(k,
                partialCore, newPossible, usedNodes);
        result.insert(result.end(), partialResult.begin(), partialResult.end());
        // Remove from partialCore
        partialCore.pop_back();
    }

    return (result);
}

std::vector<int> GraphServer::getBoundary(std::vector<int> core) {
    std::set<int> neighborhood;
    for (auto it = core.begin(); it != core.end(); it++) {
        std::set<int> node_neighborhood = getNeighbors(*it);
        neighborhood.insert(node_neighborhood.begin(), node_neighborhood.end());
    }
    for (auto it = core.begin(); it != core.end(); it++) {
        neighborhood.erase(*it);
    }
    return std::vector<int>(neighborhood.begin(), neighborhood.end());
}

// TODO empty igraph_vectors
void GraphServer::updateCores() {
    cores.clear();
    boundaries.clear();
    for (int i = 0; i < networkGraph.size(); i++) {
        cores.push_back(getCores(i));
        std::vector<std::vector<int>> nodeBoundaries;
        for (auto it = cores[i].begin(); it != cores[i].end(); it++) {
            nodeBoundaries.push_back(getBoundary(*it));
        }
        boundaries.push_back(nodeBoundaries);
    }
}

IPSetList GraphServer::indexToIPList(intListList cores) {
    IPSetList list;
    for (auto core = cores.begin(); core != cores.end(); core++) {
        list.push_back(
                indexToIP(core->begin(), core->end(), index_to_name));
    }
    return list;
}

void GraphServer::sendUpdate() {
    for (auto it = index_to_gate.begin(); it != index_to_gate.end(); it++) {
        int i = it->first;
        // Build CoresUpdate
        CoresUpdate* update = new CoresUpdate();
        update->setCores(indexToIPList(cores[i]));
        update->setBoundaries(indexToIPList(boundaries[i]));
        send(update, "out", it->second);
    }
}
