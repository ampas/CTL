# Compiler Info ('g++-4.0 --version')
# i686-apple-darwin9-g++-4.0.1 (GCC) 4.0.1 (Apple Inc. build 5488)

# Note that three paths need to be added
# NDK is the path to the NDK directory i.e. /Applications/Nuke6.3v8/Nuke6.3v8.app/Contents/MacOS
# CTLDIR is the path for the ctl directory i.e. /Users/oscar/src/ctl-1.4.2/ctl
# ILMBASE is the path to the Ilm Base directory i.e. /Users/oscar/aces-dev/apps/ctlrender/prereqs/ilmbase-1.0.2
# As a prerequisite, the source files must be compiled to create the appropriate libraries seen in the LIB macro

MYCXX ?= g++-4.0
LINK ?= g++-4.0
NDKDIR ?= /Path/to/NDK/directory
CTLDIR ?= /Path/to/CTL directory/directory
ILMBASE ?= /Path/to/Ilm Base/directory
INCLUDEDIRS ?= -I$(NDKDIR)/include -I$(CTLDIR)/IlmCtl -I$(CTLDIR)/IlmCtlSimd -I$(ILMBASE)/Half -I$(ILMBASE)/IlmThread -I$(ILMBASE)/config.windows
CXXFLAGS ?= -g -c -fPIC $(INCLUDEDIRS) -arch x86_64
LINKFLAGS ?= -L$(NDKDIR) -arch x86_64 -bundle
LIBS ?= -lDDImage -lIlmCtl -lIlmCtlMath -lIlmCtlSimd -lIlmThread -lHalf -lIex

all: NukeCtl.dylib

NukeCtl.os: NukeCtl.cpp
	$(MYCXX) $(CXXFLAGS) -o NukeCtl.os NukeCtl.cpp
NukeInputParameters.os: NukeInputParameters.cpp
	$(MYCXX) $(CXXFLAGS) -o NukeInputParameters.os NukeInputParameters.cpp
NukeTransform.os: NukeTransform.cpp
	$(MYCXX) $(CXXFLAGS) -o NukeTransform.os NukeTransform.cpp

NukeCtl.dylib: NukeCtl.os NukeInputParameters.os NukeTransform.os
	$(LINK) $(LINKFLAGS) -fPIC -shared $(LIBS) -o NukeCtl.dylib NukeCtl.os NukeInputParameters.os NukeTransform.os
clean:
	rm -rf NukeCtl.dylib NukeCtl.os NukeInputParameters.os NukeTransform.os
