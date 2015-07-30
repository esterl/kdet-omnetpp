#ifndef CORESKETCHSUMMARY_H_
#define CORESKETCHSUMMARY_H_

#include "Summary.h"
#include "SketchSummary.h"

class CoreSketchSummary: public Summary, public SketchSummary {
public:
    CoreSketchSummary(const IPv4Address reporter, const std::set<IPv4Address>& core);
    CoreSketchSummary(const CoreSketchSummary& other);
    CoreSketchSummary& operator=(const CoreSketchSummary& other);
    virtual ~CoreSketchSummary();
    virtual void updateSummaryPreRouting(IPv4Datagram* pkt);
    virtual void updateSummaryPostRouting(IPv4Datagram* pkt);
    virtual void clear();
    virtual Summary* copy() const;
    virtual void add(Summary* otherPtr);
    virtual double estimateDrop(std::set<IPv4Address> core);
    virtual double estimateIn(std::set<IPv4Address> core);
    virtual double estimateOut(std::set<IPv4Address> core);
    virtual double getBytes();
    virtual double getOptimizedBytes();
    virtual std::set<IPv4Address> getCore() const {return core;};

protected:
    std::set<IPv4Address> core;
    std::set<IPv4Address> boundaryReportsIncluded;
    NetworkSketch *sketchIn, *sketchOut;

};

#endif /* CORESKETCHSUMMARY_H_ */
