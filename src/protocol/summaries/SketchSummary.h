#ifndef SKETCHSUMMARY_H_
#define SKETCHSUMMARY_H_

#include "Summary.h"
#include "sketches/sketches.h"
typedef Sketch<uint32_t> NetworkSketch;

class SketchSummary {
public:
    static void setBaseSketch(cModule* module);
    static void setBaseSketch(NetworkSketch* sketch);
    static NetworkSketch* getBaseSketch();
    uint32_t getPacketHash(IPv4Datagram* pkt);
protected:
    static NetworkSketch* baseSketch;

};

#endif /* SKETCHSUMMARY_H_ */
