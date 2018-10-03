CC = g++
CFLAGS = -Wall -c -g -fpermissive 

#ROOFIT_INCLUDE := $(shell cd $(CMSSW_BASE); scram tool info roofitcore | grep INCLUDE= | sed 's|INCLUDE=||')
#ROOFIT_LIBDIR := $(shell cd $(CMSSW_BASE); scram tool info roofitcore | grep LIBDIR= | sed 's|LIBDIR=||')

INCLUDES = -I. -I.. -I$(ROOTSYS)/include #-I$(ROOFIT_INCLUDE)/ C-I$(CMSSW_BASE)/src -I$(CMSSW_BASE)/src/CommonTools  

ROOTSYS ?= ERROR_RootSysIsNotDefined

ROOTFLAG = `${ROOTSYS}/bin/root-config --cflags --libs`

EXTRALIBS := -L$(ROOTSYS)/lib -lHtml -lMathCore -lGenVector -lMinuit -lTreePlayer -lRooFitCore -lRooFit #-L$(ROOFIT_LIBDIR)/ 


makeTreesLite: makeTreesLite.cpp 
	$(CC) -Wall $(INCLUDES) -o makeTreesLite makeTreesLite.cpp $(ROOTFLAG) $(EXTRALIBS)

calibrateTreeLite: calibrateTreeLite.cpp BTLCommon.o
	$(CC) -Wall $(INCLUDES) -o calibrateTreeLite calibrateTreeLite.cpp BTLCommon.o $(ROOTFLAG) $(EXTRALIBS)

drawResolution: drawResolution.cpp BTLCommon.o
	$(CC) -Wall $(INCLUDES) -o drawResolution drawResolution.cpp BTLCommon.o $(ROOTFLAG) $(EXTRALIBS)



BTLCommon.o: ../src/BTLCommon.cc
	$(CC) -Wall $(INCLUDES) -c ../src/BTLCommon.cc $(ROOTFLAG) $(EXTRALIBS)


clean:
	rm *.o makeTreesLite calibrateTreeLite drawResolution
