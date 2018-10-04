#ifndef BTLCommon_h
#define BTLCommon_h


#include "TPaveText.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TH1D.h"


class BTLCommon {

 public:

  BTLCommon();
  ~BTLCommon();


  static TF1* fitGaus( TH1D* histo, float nSigma=2.5, bool addFunc=true );

  static float getSigmaEff( TH1D* histo );

  static void setStyle();

  static void addLabels( TCanvas* c1 );

  static TPaveText* getLabelLeft ( const std::string& text="LYSO:Ce 3x3x50 mm^{3} - HPK 3x3 mm^{2} (15 #mum)" );
  static TPaveText* getLabelRight( const std::string& text="LYSO bars" );

  static std::vector<int> colors();

};

#endif
