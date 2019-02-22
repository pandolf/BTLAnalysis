CC = g++
CFLAGS = -Wall -c -g -fpermissive 

#ROOFIT_INCLUDE := $(shell cd $(CMSSW_BASE); scram tool info roofitcore | grep INCLUDE= | sed 's|INCLUDE=||')
#ROOFIT_LIBDIR := $(shell cd $(CMSSW_BASE); scram tool info roofitcore | grep LIBDIR= | sed 's|LIBDIR=||')

INCLUDES = -I. -I.. -I$(ROOTSYS)/include #-I$(ROOFIT_INCLUDE)/ C-I$(CMSSW_BASE)/src -I$(CMSSW_BASE)/src/CommonTools  

ROOTSYS ?= ERROR_RootSysIsNotDefined

ROOTFLAG = `${ROOTSYS}/bin/root-config --cflags --libs`

EXTRALIBS := -L$(ROOTSYS)/lib -lHtml -lMathCore -lGenVector -lMinuit -lTreePlayer -lRooFitCore -lRooFit #-L$(ROOFIT_LIBDIR)/ 


makeTreesLite: makeTreesLite.cpp BTLConf.o BTLCrystal.o BTLCommon.o
	$(CC) -Wall $(INCLUDES) -o makeTreesLite makeTreesLite.cpp BTLConf.o BTLCrystal.o BTLCommon.o $(ROOTFLAG) $(EXTRALIBS)

calibrateTreeLite: calibrateTreeLite.cpp BTLCommon.o BTLConf.o BTLCrystal.o
	$(CC) -Wall $(INCLUDES) -o calibrateTreeLite calibrateTreeLite.cpp BTLCommon.o BTLConf.o BTLCrystal.o $(ROOTFLAG) $(EXTRALIBS)

drawResolution: drawResolution.cpp BTLCommon.o BTLConf.o BTLCrystal.o
	$(CC) -Wall $(INCLUDES) -o drawResolution drawResolution.cpp BTLCommon.o BTLConf.o BTLCrystal.o $(ROOTFLAG) $(EXTRALIBS)

drawResoScans: drawResoScans.cpp BTLCommon.o BTLConf.o BTLCrystal.o
	$(CC) -Wall $(INCLUDES) -o drawResoScans drawResoScans.cpp BTLCommon.o BTLConf.o BTLCrystal.o $(ROOTFLAG) $(EXTRALIBS)

drawReso_vs_pos: drawReso_vs_pos.cpp BTLCommon.o BTLConf.o BTLCrystal.o
	$(CC) -Wall $(INCLUDES) -o drawReso_vs_pos drawReso_vs_pos.cpp BTLCommon.o BTLConf.o BTLCrystal.o $(ROOTFLAG) $(EXTRALIBS)

compareResoAW: compareResoAW.cpp BTLCommon.o BTLConf.o
	$(CC) -Wall $(INCLUDES) -o compareResoAW compareResoAW.cpp BTLCommon.o BTLConf.o $(ROOTFLAG) $(EXTRALIBS)

drawReso_vs_amp: drawReso_vs_amp.cpp BTLCommon.o BTLConf.o BTLCrystal.o
	$(CC) -Wall $(INCLUDES) -o drawReso_vs_amp drawReso_vs_amp.cpp BTLCommon.o BTLConf.o BTLCrystal.o $(ROOTFLAG) $(EXTRALIBS)

drawAmp_vs_vOV: drawAmp_vs_vOV.cpp BTLCommon.o BTLConf.o BTLCrystal.o
	$(CC) -Wall $(INCLUDES) -o drawAmp_vs_vOV drawAmp_vs_vOV.cpp BTLCommon.o BTLConf.o BTLCrystal.o $(ROOTFLAG) $(EXTRALIBS)



analyzeLLP: analyzeLLP.cpp 
	$(CC) -Wall $(INCLUDES) -o analyzeLLP analyzeLLP.cpp $(ROOTFLAG) $(EXTRALIBS)



BTLCommon.o: ../src/BTLCommon.cc BTLConf.o
	$(CC) -Wall $(INCLUDES) -c ../src/BTLCommon.cc BTLConf.o $(ROOTFLAG) $(EXTRALIBS)

BTLConf.o: ../src/BTLConf.cc BTLCrystal.o
	$(CC) -Wall $(INCLUDES) -c ../src/BTLConf.cc BTLCrystal.o $(ROOTFLAG) $(EXTRALIBS)

BTLCrystal.o: ../src/BTLCrystal.cc
	$(CC) -Wall $(INCLUDES) -c ../src/BTLCrystal.cc $(ROOTFLAG) $(EXTRALIBS)



clean:
	rm *.o makeTreesLite calibrateTreeLite drawResolution drawResoScans
