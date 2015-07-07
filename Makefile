#
# OMNeT++/OMNEST Makefile for kdet
#
# This file was generated with the command:
#  opp_makemake -f --deep -O out -Xsketches/build -I/usr/include/python2.7 -I/usr/include/x86_64-linux-gnu/python2.7 -I../inet/src/linklayer/ieee80211/radio -I../inet/src/applications/generic -I../inet/src/networklayer/routing/aodv -I../inet/src/networklayer/common -I../inet/src/networklayer/icmpv6 -I../inet/src -I../inet/src/world/obstacles -I../inet/src/networklayer/xmipv6 -I../inet/src/networklayer/contract -I../inet/src/networklayer/autorouting/ipv4 -I../inet/src/util -I../inet/src/transport/contract -I../inet/src/linklayer/common -I../inet/src/status -I../inet/src/linklayer/radio/propagation -I../inet/src/linklayer/ieee80211/radio/errormodel -I../inet/src/linklayer/radio -I../inet/src/util/headerserializers/tcp -I../inet/src/networklayer/ipv4 -I../inet/src/mobility/contract -I../inet/src/util/headerserializers/ipv4 -I../inet/src/base -I../inet/src/util/headerserializers -I../inet/src/world/radio -I../inet/src/linklayer/ieee80211/mac -I../inet/src/networklayer/ipv6 -I../inet/src/transport/sctp -I../inet/src/util/headerserializers/udp -I../inet/src/networklayer/ipv6tunneling -I../inet/src/applications/pingapp -I../inet/src/battery/models -I../inet/src/util/headerserializers/ipv6 -I../inet/src/util/headerserializers/sctp -I../inet/src/linklayer/contract -I../inet/src/transport/tcp_common -I../inet/src/networklayer/arp -I../inet/src/transport/udp -L/usr/lib/x86_64-linux-gnu -L../inet/out/$$\(CONFIGNAME\)/src -lcrypto -lssl -linet -DINET_IMPORT -KINET_PROJ=../inet
#

# Name of target to be created (-o option)
TARGET = kdet$(EXE_SUFFIX)

# User interface (uncomment one) (-u option)
USERIF_LIBS = $(ALL_ENV_LIBS) # that is, $(TKENV_LIBS) $(CMDENV_LIBS)
#USERIF_LIBS = $(CMDENV_LIBS)
#USERIF_LIBS = $(TKENV_LIBS)

# C++ include paths (with -I)
INCLUDE_PATH = \
    -I/usr/include/python2.7 \
    -I/usr/include/x86_64-linux-gnu/python2.7 \
    -I../inet/src/linklayer/ieee80211/radio \
    -I../inet/src/applications/generic \
    -I../inet/src/networklayer/routing/aodv \
    -I../inet/src/networklayer/common \
    -I../inet/src/networklayer/icmpv6 \
    -I../inet/src \
    -I../inet/src/world/obstacles \
    -I../inet/src/networklayer/xmipv6 \
    -I../inet/src/networklayer/contract \
    -I../inet/src/networklayer/autorouting/ipv4 \
    -I../inet/src/util \
    -I../inet/src/transport/contract \
    -I../inet/src/linklayer/common \
    -I../inet/src/status \
    -I../inet/src/linklayer/radio/propagation \
    -I../inet/src/linklayer/ieee80211/radio/errormodel \
    -I../inet/src/linklayer/radio \
    -I../inet/src/util/headerserializers/tcp \
    -I../inet/src/networklayer/ipv4 \
    -I../inet/src/mobility/contract \
    -I../inet/src/util/headerserializers/ipv4 \
    -I../inet/src/base \
    -I../inet/src/util/headerserializers \
    -I../inet/src/world/radio \
    -I../inet/src/linklayer/ieee80211/mac \
    -I../inet/src/networklayer/ipv6 \
    -I../inet/src/transport/sctp \
    -I../inet/src/util/headerserializers/udp \
    -I../inet/src/networklayer/ipv6tunneling \
    -I../inet/src/applications/pingapp \
    -I../inet/src/battery/models \
    -I../inet/src/util/headerserializers/ipv6 \
    -I../inet/src/util/headerserializers/sctp \
    -I../inet/src/linklayer/contract \
    -I../inet/src/transport/tcp_common \
    -I../inet/src/networklayer/arp \
    -I../inet/src/transport/udp \
    -I. \
    -Iresults \
    -Isketches \
    -Isketches/ttmath

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS = -L/usr/lib/x86_64-linux-gnu -L../inet/out/$(CONFIGNAME)/src  -lcrypto -lssl -linet
LIBS += -Wl,-rpath,`abspath /usr/lib/x86_64-linux-gnu` -Wl,-rpath,`abspath ../inet/out/$(CONFIGNAME)/src`

# Output directory
PROJECT_OUTPUT_DIR = out
PROJECTRELATIVE_PATH =
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cpp and .msg files
OBJS = \
    $O/Flooder.o \
    $O/Report.o \
    $O/Dropper.o \
    $O/ReliableFlooding.o \
    $O/SketchMonitor.o \
    $O/SketchSummary.o \
    $O/Detector.o \
    $O/Clock.o \
    $O/WCNTrafGen.o \
    $O/GraphServer.o \
    $O/TopologyAnnouncer.o \
    $O/TrustedAuthority.o \
    $O/FakeFlooding.o \
    $O/sketches/random.o \
    $O/sketches/hash.o \
    $O/sketches/sketches.o \
    $O/ReportAck_m.o \
    $O/Report_m.o \
    $O/NeighborsAnnouncement_m.o \
    $O/AppMsg_m.o \
    $O/DropperReport_m.o \
    $O/CoresUpdate_m.o \
    $O/CoreEvaluation_m.o \
    $O/ReportIdMsg_m.o

# Message files
MSGFILES = \
    ReportAck.msg \
    Report.msg \
    NeighborsAnnouncement.msg \
    AppMsg.msg \
    DropperReport.msg \
    CoresUpdate.msg \
    CoreEvaluation.msg \
    ReportIdMsg.msg

# Other makefile variables (-K)
INET_PROJ=../inet

#------------------------------------------------------------------------------

# Pull in OMNeT++ configuration (Makefile.inc or configuser.vc)

ifneq ("$(OMNETPP_CONFIGFILE)","")
CONFIGFILE = $(OMNETPP_CONFIGFILE)
else
ifneq ("$(OMNETPP_ROOT)","")
CONFIGFILE = $(OMNETPP_ROOT)/Makefile.inc
else
CONFIGFILE = $(shell opp_configfilepath)
endif
endif

ifeq ("$(wildcard $(CONFIGFILE))","")
$(error Config file '$(CONFIGFILE)' does not exist -- add the OMNeT++ bin directory to the path so that opp_configfilepath can be found, or set the OMNETPP_CONFIGFILE variable to point to Makefile.inc)
endif

include $(CONFIGFILE)

# Simulation kernel and user interface libraries
OMNETPP_LIB_SUBDIR = $(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)
OMNETPP_LIBS = -L"$(OMNETPP_LIB_SUBDIR)" -L"$(OMNETPP_LIB_DIR)" -loppmain$D $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS)

COPTS = $(CFLAGS) -DINET_IMPORT $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
MSGCOPTS = $(INCLUDE_PATH)

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE and have object
# files depend on it (except when "make depend" was called)
COPTS_FILE = $O/.last-copts
ifneq ($(MAKECMDGOALS),depend)
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
$(shell $(MKPATH) "$O" && echo "$(COPTS)" >$(COPTS_FILE))
endif
endif

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
# >>>
# <<<
#------------------------------------------------------------------------------

# Main target
all: $O/$(TARGET)
	$(Q)$(LN) $O/$(TARGET) .

$O/$(TARGET): $(OBJS)  $(wildcard $(EXTRA_OBJS)) Makefile
	@$(MKPATH) $O
	@echo Creating executable: $@
	$(Q)$(CXX) $(LDFLAGS) -o $O/$(TARGET)  $(OBJS) $(EXTRA_OBJS) $(AS_NEEDED_OFF) $(WHOLE_ARCHIVE_ON) $(LIBS) $(WHOLE_ARCHIVE_OFF) $(OMNETPP_LIBS)

.PHONY: all clean cleanall depend msgheaders

.SUFFIXES: .cpp

$O/%.o: %.cpp $(COPTS_FILE)
	@$(MKPATH) $(dir $@)
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) -o $@ $<

%_m.cpp %_m.h: %.msg
	$(qecho) MSGC: $<
	$(Q)$(MSGC) -s _m.cpp $(MSGCOPTS) $?

msgheaders: $(MSGFILES:.msg=_m.h)

clean:
	$(qecho) Cleaning...
	$(Q)-rm -rf $O
	$(Q)-rm -f kdet kdet.exe libkdet.so libkdet.a libkdet.dll libkdet.dylib
	$(Q)-rm -f ./*_m.cpp ./*_m.h
	$(Q)-rm -f results/*_m.cpp results/*_m.h
	$(Q)-rm -f sketches/*_m.cpp sketches/*_m.h
	$(Q)-rm -f sketches/ttmath/*_m.cpp sketches/ttmath/*_m.h

cleanall: clean
	$(Q)-rm -rf $(PROJECT_OUTPUT_DIR)

depend:
	$(qecho) Creating dependencies...
	$(Q)$(MAKEDEPEND) $(INCLUDE_PATH) -f Makefile -P\$$O/ -- $(MSG_CC_FILES)  ./*.cpp results/*.cpp sketches/*.cpp sketches/ttmath/*.cpp

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/Clock.o: Clock.cpp \
	Clock.h
$O/Detector.o: Detector.cpp \
	$(INET_PROJ)/src/base/Compat.h \
	$(INET_PROJ)/src/base/INETDefs.h \
	$(INET_PROJ)/src/networklayer/contract/IPProtocolId_m.h \
	$(INET_PROJ)/src/networklayer/contract/IPv4Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPv6Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPvXAddress.h \
	$(INET_PROJ)/src/networklayer/contract/IPvXAddressResolver.h \
	$(INET_PROJ)/src/networklayer/ipv4/IPv4Datagram.h \
	$(INET_PROJ)/src/networklayer/ipv4/IPv4Datagram_m.h \
	CoreEvaluation_m.h \
	CoresUpdate_m.h \
	Detector.h \
	LinkSummary.h \
	Report.h \
	Report_m.h \
	kdet_defs.h
$O/Dropper.o: Dropper.cpp \
	$(INET_PROJ)/src/base/AbstractQueue.h \
	$(INET_PROJ)/src/base/Compat.h \
	$(INET_PROJ)/src/base/ILifecycle.h \
	$(INET_PROJ)/src/base/INETDefs.h \
	$(INET_PROJ)/src/base/ModuleAccess.h \
	$(INET_PROJ)/src/base/NotifierConsts.h \
	$(INET_PROJ)/src/base/ProtocolMap.h \
	$(INET_PROJ)/src/base/QueueBase.h \
	$(INET_PROJ)/src/base/ReassemblyBuffer.h \
	$(INET_PROJ)/src/linklayer/contract/MACAddress.h \
	$(INET_PROJ)/src/networklayer/arp/IARPCache.h \
	$(INET_PROJ)/src/networklayer/common/INetfilter.h \
	$(INET_PROJ)/src/networklayer/common/InterfaceEntry.h \
	$(INET_PROJ)/src/networklayer/common/InterfaceToken.h \
	$(INET_PROJ)/src/networklayer/contract/IPProtocolId_m.h \
	$(INET_PROJ)/src/networklayer/contract/IPv4Address.h \
	$(INET_PROJ)/src/networklayer/ipv4/ICMP.h \
	$(INET_PROJ)/src/networklayer/ipv4/ICMPAccess.h \
	$(INET_PROJ)/src/networklayer/ipv4/ICMPMessage.h \
	$(INET_PROJ)/src/networklayer/ipv4/ICMPMessage_m.h \
	$(INET_PROJ)/src/networklayer/ipv4/IPv4.h \
	$(INET_PROJ)/src/networklayer/ipv4/IPv4Datagram.h \
	$(INET_PROJ)/src/networklayer/ipv4/IPv4Datagram_m.h \
	$(INET_PROJ)/src/networklayer/ipv4/IPv4FragBuf.h \
	$(INET_PROJ)/src/networklayer/ipv4/IPv4Route.h \
	$(INET_PROJ)/src/networklayer/ipv4/IRoutingTable.h \
	$(INET_PROJ)/src/networklayer/ipv4/RoutingTableAccess.h \
	Dropper.h \
	DropperReport_m.h \
	kdet_defs.h
$O/FakeFlooding.o: FakeFlooding.cpp \
	$(INET_PROJ)/src/base/Compat.h \
	$(INET_PROJ)/src/base/ILifecycle.h \
	$(INET_PROJ)/src/base/INETDefs.h \
	$(INET_PROJ)/src/base/INotifiable.h \
	$(INET_PROJ)/src/base/ModuleAccess.h \
	$(INET_PROJ)/src/base/NotificationBoard.h \
	$(INET_PROJ)/src/base/NotifierConsts.h \
	$(INET_PROJ)/src/linklayer/contract/Ieee802Ctrl_m.h \
	$(INET_PROJ)/src/linklayer/contract/MACAddress.h \
	$(INET_PROJ)/src/networklayer/arp/ARP.h \
	$(INET_PROJ)/src/networklayer/arp/IARPCache.h \
	$(INET_PROJ)/src/networklayer/contract/IPProtocolId_m.h \
	$(INET_PROJ)/src/networklayer/contract/IPSocket.h \
	$(INET_PROJ)/src/networklayer/contract/IPv4Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPv4ControlInfo.h \
	$(INET_PROJ)/src/networklayer/contract/IPv4ControlInfo_m.h \
	$(INET_PROJ)/src/networklayer/contract/IPv6Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPvXAddress.h \
	$(INET_PROJ)/src/networklayer/contract/IPvXAddressResolver.h \
	$(INET_PROJ)/src/networklayer/ipv4/IPv4Datagram.h \
	$(INET_PROJ)/src/networklayer/ipv4/IPv4Datagram_m.h \
	CoresUpdate_m.h \
	FakeFlooding.h \
	GraphServer.h \
	LinkSummary.h \
	NeighborsAnnouncement_m.h \
	Report.h \
	ReportAck_m.h \
	ReportIdMsg_m.h \
	Report_m.h \
	kdet_defs.h
$O/Flooder.o: Flooder.cpp \
	Flooder.h
$O/GraphServer.o: GraphServer.cpp \
	$(INET_PROJ)/src/base/Compat.h \
	$(INET_PROJ)/src/base/INETDefs.h \
	$(INET_PROJ)/src/networklayer/contract/IPv4Address.h \
	CoresUpdate_m.h \
	GraphServer.h \
	NeighborsAnnouncement_m.h
$O/ReliableFlooding.o: ReliableFlooding.cpp \
	$(INET_PROJ)/src/base/Compat.h \
	$(INET_PROJ)/src/base/ILifecycle.h \
	$(INET_PROJ)/src/base/INETDefs.h \
	$(INET_PROJ)/src/base/INotifiable.h \
	$(INET_PROJ)/src/base/ModuleAccess.h \
	$(INET_PROJ)/src/base/NotificationBoard.h \
	$(INET_PROJ)/src/base/NotifierConsts.h \
	$(INET_PROJ)/src/linklayer/contract/Ieee802Ctrl_m.h \
	$(INET_PROJ)/src/linklayer/contract/MACAddress.h \
	$(INET_PROJ)/src/networklayer/arp/ARP.h \
	$(INET_PROJ)/src/networklayer/arp/IARPCache.h \
	$(INET_PROJ)/src/networklayer/contract/IPProtocolId_m.h \
	$(INET_PROJ)/src/networklayer/contract/IPSocket.h \
	$(INET_PROJ)/src/networklayer/contract/IPv4Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPv4ControlInfo.h \
	$(INET_PROJ)/src/networklayer/contract/IPv4ControlInfo_m.h \
	$(INET_PROJ)/src/networklayer/contract/IPv6Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPvXAddress.h \
	$(INET_PROJ)/src/networklayer/contract/IPvXAddressResolver.h \
	$(INET_PROJ)/src/networklayer/ipv4/IPv4Datagram.h \
	$(INET_PROJ)/src/networklayer/ipv4/IPv4Datagram_m.h \
	CoresUpdate_m.h \
	GraphServer.h \
	LinkSummary.h \
	NeighborsAnnouncement_m.h \
	ReliableFlooding.h \
	Report.h \
	ReportAck_m.h \
	ReportIdMsg_m.h \
	Report_m.h \
	kdet_defs.h
$O/Report.o: Report.cpp \
	$(INET_PROJ)/src/base/Compat.h \
	$(INET_PROJ)/src/base/INETDefs.h \
	$(INET_PROJ)/src/networklayer/contract/IPProtocolId_m.h \
	$(INET_PROJ)/src/networklayer/contract/IPv4Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPv6Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPvXAddress.h \
	$(INET_PROJ)/src/networklayer/ipv4/IPv4Datagram.h \
	$(INET_PROJ)/src/networklayer/ipv4/IPv4Datagram_m.h \
	LinkSummary.h \
	Report.h \
	Report_m.h \
	kdet_defs.h
$O/SketchMonitor.o: SketchMonitor.cpp \
	$(INET_PROJ)/src/base/AbstractQueue.h \
	$(INET_PROJ)/src/base/ByteArray.h \
	$(INET_PROJ)/src/base/ByteArray_m.h \
	$(INET_PROJ)/src/base/Compat.h \
	$(INET_PROJ)/src/base/ILifecycle.h \
	$(INET_PROJ)/src/base/INETDefs.h \
	$(INET_PROJ)/src/base/INotifiable.h \
	$(INET_PROJ)/src/base/ModuleAccess.h \
	$(INET_PROJ)/src/base/NotificationBoard.h \
	$(INET_PROJ)/src/base/NotifierConsts.h \
	$(INET_PROJ)/src/base/ProtocolMap.h \
	$(INET_PROJ)/src/base/QueueBase.h \
	$(INET_PROJ)/src/base/ReassemblyBuffer.h \
	$(INET_PROJ)/src/linklayer/contract/Ieee802Ctrl_m.h \
	$(INET_PROJ)/src/linklayer/contract/MACAddress.h \
	$(INET_PROJ)/src/networklayer/arp/ARP.h \
	$(INET_PROJ)/src/networklayer/arp/IARPCache.h \
	$(INET_PROJ)/src/networklayer/common/INetfilter.h \
	$(INET_PROJ)/src/networklayer/common/InterfaceEntry.h \
	$(INET_PROJ)/src/networklayer/common/InterfaceToken.h \
	$(INET_PROJ)/src/networklayer/contract/IPProtocolId_m.h \
	$(INET_PROJ)/src/networklayer/contract/IPv4Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPv6Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPvXAddress.h \
	$(INET_PROJ)/src/networklayer/contract/IPvXAddressResolver.h \
	$(INET_PROJ)/src/networklayer/ipv4/ICMP.h \
	$(INET_PROJ)/src/networklayer/ipv4/ICMPAccess.h \
	$(INET_PROJ)/src/networklayer/ipv4/ICMPMessage.h \
	$(INET_PROJ)/src/networklayer/ipv4/ICMPMessage_m.h \
	$(INET_PROJ)/src/networklayer/ipv4/IPv4.h \
	$(INET_PROJ)/src/networklayer/ipv4/IPv4Datagram.h \
	$(INET_PROJ)/src/networklayer/ipv4/IPv4Datagram_m.h \
	$(INET_PROJ)/src/networklayer/ipv4/IPv4FragBuf.h \
	$(INET_PROJ)/src/networklayer/ipv4/IPv4Route.h \
	$(INET_PROJ)/src/networklayer/ipv4/IRoutingTable.h \
	$(INET_PROJ)/src/networklayer/ipv4/RoutingTableAccess.h \
	$(INET_PROJ)/src/transport/tcp_common/TCPSegment.h \
	$(INET_PROJ)/src/transport/tcp_common/TCPSegment_m.h \
	LinkSummary.h \
	Report.h \
	Report_m.h \
	SketchMonitor.h \
	SketchSummary.h \
	kdet_defs.h \
	sketches/cauchy.h \
	sketches/hash.h \
	sketches/mersenne.h \
	sketches/sketches.h \
	sketches/ttmath/ttmath.h \
	sketches/ttmath/ttmathbig.h \
	sketches/ttmath/ttmathint.h \
	sketches/ttmath/ttmathmisc.h \
	sketches/ttmath/ttmathobjects.h \
	sketches/ttmath/ttmathparser.h \
	sketches/ttmath/ttmaththreads.h \
	sketches/ttmath/ttmathtypes.h \
	sketches/ttmath/ttmathuint.h \
	sketches/ttmath/ttmathuint_noasm.h \
	sketches/ttmath/ttmathuint_x86.h \
	sketches/ttmath/ttmathuint_x86_64.h \
	sketches/xis.h
$O/SketchSummary.o: SketchSummary.cpp \
	$(INET_PROJ)/src/base/Compat.h \
	$(INET_PROJ)/src/base/INETDefs.h \
	$(INET_PROJ)/src/networklayer/contract/IPProtocolId_m.h \
	$(INET_PROJ)/src/networklayer/contract/IPv4Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPv6Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPvXAddress.h \
	$(INET_PROJ)/src/networklayer/ipv4/IPv4Datagram.h \
	$(INET_PROJ)/src/networklayer/ipv4/IPv4Datagram_m.h \
	$(INET_PROJ)/src/util/headerserializers/ipv4/IPv4Serializer.h \
	LinkSummary.h \
	SketchSummary.h \
	sketches/cauchy.h \
	sketches/hash.h \
	sketches/mersenne.h \
	sketches/sketches.h \
	sketches/ttmath/ttmath.h \
	sketches/ttmath/ttmathbig.h \
	sketches/ttmath/ttmathint.h \
	sketches/ttmath/ttmathmisc.h \
	sketches/ttmath/ttmathobjects.h \
	sketches/ttmath/ttmathparser.h \
	sketches/ttmath/ttmaththreads.h \
	sketches/ttmath/ttmathtypes.h \
	sketches/ttmath/ttmathuint.h \
	sketches/ttmath/ttmathuint_noasm.h \
	sketches/ttmath/ttmathuint_x86.h \
	sketches/ttmath/ttmathuint_x86_64.h \
	sketches/xis.h
$O/TopologyAnnouncer.o: TopologyAnnouncer.cpp \
	$(INET_PROJ)/src/base/Compat.h \
	$(INET_PROJ)/src/base/INETDefs.h \
	$(INET_PROJ)/src/base/ModuleAccess.h \
	$(INET_PROJ)/src/networklayer/contract/IPv4Address.h \
	$(INET_PROJ)/src/networklayer/ipv4/IPv4Route.h \
	$(INET_PROJ)/src/networklayer/ipv4/IRoutingTable.h \
	$(INET_PROJ)/src/networklayer/ipv4/RoutingTableAccess.h \
	NeighborsAnnouncement_m.h \
	TopologyAnnouncer.h
$O/TrustedAuthority.o: TrustedAuthority.cpp \
	$(INET_PROJ)/src/base/Compat.h \
	$(INET_PROJ)/src/base/INETDefs.h \
	$(INET_PROJ)/src/networklayer/contract/IPProtocolId_m.h \
	$(INET_PROJ)/src/networklayer/contract/IPv4Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPv6Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPvXAddress.h \
	$(INET_PROJ)/src/networklayer/ipv4/IPv4Datagram.h \
	$(INET_PROJ)/src/networklayer/ipv4/IPv4Datagram_m.h \
	CoreEvaluation_m.h \
	CoresUpdate_m.h \
	DropperReport_m.h \
	GraphServer.h \
	LinkSummary.h \
	NeighborsAnnouncement_m.h \
	TrustedAuthority.h
$O/WCNTrafGen.o: WCNTrafGen.cpp \
	$(INET_PROJ)/src/applications/generic/IPvXTrafGen.h \
	$(INET_PROJ)/src/applications/generic/IPvXTrafSink.h \
	$(INET_PROJ)/src/base/Compat.h \
	$(INET_PROJ)/src/base/ILifecycle.h \
	$(INET_PROJ)/src/base/INETDefs.h \
	$(INET_PROJ)/src/linklayer/contract/MACAddress.h \
	$(INET_PROJ)/src/networklayer/contract/IPProtocolId_m.h \
	$(INET_PROJ)/src/networklayer/contract/IPSocket.h \
	$(INET_PROJ)/src/networklayer/contract/IPv4Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPv4ControlInfo.h \
	$(INET_PROJ)/src/networklayer/contract/IPv4ControlInfo_m.h \
	$(INET_PROJ)/src/networklayer/contract/IPv6Address.h \
	$(INET_PROJ)/src/networklayer/contract/IPvXAddress.h \
	$(INET_PROJ)/src/networklayer/contract/IPvXAddressResolver.h \
	$(INET_PROJ)/src/status/NodeStatus.h \
	AppMsg_m.h \
	WCNTrafGen.h
$O/sketches/hash.o: sketches/hash.cpp \
	sketches/cauchy.h \
	sketches/hash.h \
	sketches/mersenne.h \
	sketches/ttmath/ttmath.h \
	sketches/ttmath/ttmathbig.h \
	sketches/ttmath/ttmathint.h \
	sketches/ttmath/ttmathmisc.h \
	sketches/ttmath/ttmathobjects.h \
	sketches/ttmath/ttmathparser.h \
	sketches/ttmath/ttmaththreads.h \
	sketches/ttmath/ttmathtypes.h \
	sketches/ttmath/ttmathuint.h \
	sketches/ttmath/ttmathuint_noasm.h \
	sketches/ttmath/ttmathuint_x86.h \
	sketches/ttmath/ttmathuint_x86_64.h
$O/sketches/random.o: sketches/random.cpp \
	sketches/random.h \
	sketches/ttmath/ttmath.h \
	sketches/ttmath/ttmathbig.h \
	sketches/ttmath/ttmathint.h \
	sketches/ttmath/ttmathmisc.h \
	sketches/ttmath/ttmathobjects.h \
	sketches/ttmath/ttmathparser.h \
	sketches/ttmath/ttmaththreads.h \
	sketches/ttmath/ttmathtypes.h \
	sketches/ttmath/ttmathuint.h \
	sketches/ttmath/ttmathuint_noasm.h \
	sketches/ttmath/ttmathuint_x86.h \
	sketches/ttmath/ttmathuint_x86_64.h
$O/sketches/sketches.o: sketches/sketches.cpp \
	sketches/cauchy.h \
	sketches/hash.h \
	sketches/mersenne.h \
	sketches/sketches.h \
	sketches/ttmath/ttmath.h \
	sketches/ttmath/ttmathbig.h \
	sketches/ttmath/ttmathint.h \
	sketches/ttmath/ttmathmisc.h \
	sketches/ttmath/ttmathobjects.h \
	sketches/ttmath/ttmathparser.h \
	sketches/ttmath/ttmaththreads.h \
	sketches/ttmath/ttmathtypes.h \
	sketches/ttmath/ttmathuint.h \
	sketches/ttmath/ttmathuint_noasm.h \
	sketches/ttmath/ttmathuint_x86.h \
	sketches/ttmath/ttmathuint_x86_64.h \
	sketches/xis.h

