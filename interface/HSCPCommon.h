#ifndef HSCPCommon_h
#define HSCPCommon_h


#include "TPaveText.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TH1D.h"



class HSCPCommon {

 public:

  HSCPCommon();
  ~HSCPCommon();

  static void setStyle();

  static std::vector<int> colors();

  static std::vector<float> etaBins();
  static std::vector<float> pBins();

  static float barrelEnd()   { return 1.4442; };
  static float endcapStart() { return 1.566 ; };

  static void addLabels( TCanvas* c1, std::string left_text="CMS", std::string right_text="14 TeV", int iPosX=0, TString extraText="Phase-2 Simulation Preliminary" );

  static TPaveText* getLabelRight();
  static TPaveText* getLabelLeft();

 private:

  static void symmetrize( std::vector<float> &etaBins );

};

#endif
