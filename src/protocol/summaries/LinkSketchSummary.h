#ifndef LINKSKETCHSUMMARY_H_
#define LINKSKETCHSUMMARY_H_

#include "LinkSummary.h"
#include "SketchSummary.h"
#include "sketches/sketches.h"
typedef Sketch<uint32_t> NetworkSketch;
typedef std::map<uint32_t, NetworkSketch*> SketchHash;

class LinkSketchSummary: public LinkSummary, public SketchSummary {
public:
    LinkSketchSummary(IPv4Address reporter, IPv4Address link =
            IPv4Address::UNSPECIFIED_ADDRESS);
    LinkSketchSummary(const LinkSketchSummary& other);
    LinkSketchSummary& operator=(const LinkSketchSummary& other);
    virtual ~LinkSketchSummary();
    virtual void updateSummaryPreRouting(IPv4Datagram* pkt);
    virtual void updateSummaryPostRouting(IPv4Datagram* pkt);
    virtual void clear();
    virtual Summary* copy() const;
    virtual void add(Summary* otherPtr);
    virtual double estimateDrop(std::set<IPv4Address> core);
    virtual double estimateIn(std::set<IPv4Address> core);
    virtual double estimateOut(std::set<IPv4Address> core);
    virtual double getBytes();
    virtual void optimizeSummary(std::set<IPv4Address> coreNodes);
    static void setBaseSketch(cModule* module);
    static void setBaseSketch(NetworkSketch* sketch);
    static NetworkSketch* getBaseSketch();
protected:
    void updateSketch(NetworkSketch* sketch, IPv4Datagram* pkt);
    static NetworkSketch* baseSketch;
    IPv4Address neighbor;
    NetworkSketch* from;
    NetworkSketch* to;
    SketchHash src;
    SketchHash dst;
};

#endif /* LINKSKETCHSUMMARY_H_ */
