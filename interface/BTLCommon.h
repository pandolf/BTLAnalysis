#ifndef BTLCommon_h
#define BTLCommon_h


#include "TPaveText.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TH1D.h"

#include "../interface/BTLConf.h"


class BTLCommon {

 public:

  BTLCommon();
  ~BTLCommon();

  static void setStyle();

  static std::vector<int> colors();

  static TF1* fitGaus( TH1D* histo, float nSigma=2.5, bool addFunc=true );

  static float getSigmaEff( TH1D* histo );

  static float subtractResoPTK( float reso_in_ps );

  static void addLabels( TCanvas* c1, BTLConf conf );

  static TPaveText* getLabelLeft ( BTLConf conf );
  static TPaveText* getLabelRight( const std::string& text="LYSO bars" );

  static std::vector<float> get_ninoThresholds( BTLConf conf );
  static std::vector<float> get_vBiasThresholds( BTLConf conf );

};

#endif
