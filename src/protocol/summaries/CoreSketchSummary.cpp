#include <CoreSketchSummary.h>

CoreSketchSummary::CoreSketchSummary(const IPv4Address reporter_var,
        const std::set<IPv4Address>& core_var) {
    sketchIn = SketchSummary::getBaseSketch();
    sketchOut = SketchSummary::getBaseSketch();
    reporter = reporter_var;
    core = core_var;
    boundaryReportsIncluded.insert(reporter);
}

CoreSketchSummary::CoreSketchSummary(const CoreSketchSummary& other){
    sketchIn = other.sketchIn->copy();
    sketchOut = other.sketchOut->copy();
    reporter = other.reporter;
    core = other.core;
    boundaryReportsIncluded = other.boundaryReportsIncluded;

}
CoreSketchSummary& CoreSketchSummary::operator=(const CoreSketchSummary& other){
    NetworkSketch* tmp = other.sketchIn->copy();
    delete sketchIn;
    sketchIn = tmp;
    tmp = other.sketchOut->copy();
    delete sketchOut;
    sketchOut = tmp;
    reporter = other.reporter;
    core = other.core;
    boundaryReportsIncluded = other.boundaryReportsIncluded;
    return *this;
}

CoreSketchSummary::~CoreSketchSummary() {
    delete sketchIn;
    delete sketchOut;
}

void CoreSketchSummary::updateSummaryPreRouting(IPv4Datagram* pkt) {
    if (core.count(pkt->getSrcAddress()) == 0) {
        try {
            uint32_t pktHash = getPacketHash(pkt);
            sketchOut->update(pktHash, 1);
        } catch (cRuntimeError e) {
            // Is not a Serializable packet
            std::cout << "No serializable" << endl;
        }
    }
}

void CoreSketchSummary::updateSummaryPostRouting(IPv4Datagram* pkt) {
    if (core.count(pkt->getDestAddress()) == 0) {
        try {
            uint32_t pktHash = getPacketHash(pkt);
            sketchIn->update(pktHash, 1);
        } catch (cRuntimeError e) {
            // Is not a Serializable packet
            std::cout << "No serializable" << endl;
        }
    }
}

void CoreSketchSummary::clear() {
    sketchIn->clear();
    sketchOut->clear();
}

Summary* CoreSketchSummary::copy() const {
    CoreSketchSummary* summary = new CoreSketchSummary(reporter, core);
    (*summary->sketchIn) += (*sketchIn);
    (*summary->sketchOut) += (*sketchOut);
    summary->boundaryReportsIncluded = boundaryReportsIncluded;
    return summary;
}

void CoreSketchSummary::add(Summary* otherPtr) {
    CoreSketchSummary* other = dynamic_cast<CoreSketchSummary*>(otherPtr);
    if (other != NULL) {
        (*sketchIn) += (*other->sketchIn);
        (*sketchOut) += (*other->sketchOut);
        boundaryReportsIncluded.insert(other->getReporter());
    }
}

double CoreSketchSummary::estimateDrop(std::set<IPv4Address> core) {
    NetworkSketch* sketch = sketchIn->copy();
    (*sketch) -= (*sketchOut);
    double dropped = sketch->second_moment();
    delete sketch;
    return dropped;
}

double CoreSketchSummary::estimateIn(std::set<IPv4Address> core) {
    return sketchIn->second_moment();
}

double CoreSketchSummary::estimateOut(std::set<IPv4Address> core) {
    return sketchOut->second_moment();
}

double CoreSketchSummary::getBytes() {
    return sketchIn->get_bytes() + sketchOut->get_bytes();
}

double CoreSketchSummary::getOptimizedBytes() {
    return sketchIn->get_optimized_bytes() + sketchOut->get_optimized_bytes();
}
