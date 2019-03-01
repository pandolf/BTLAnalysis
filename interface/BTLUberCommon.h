#ifndef BTLUberCommon_h
#define BTLUberCommon_h


#include "TPaveText.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TH1D.h"



class BTLUberCommon {

 public:

  BTLUberCommon();
  ~BTLUberCommon();


  static void setStyle();

  static std::vector<int> colors();

  static std::vector<float> etaBins();

  static float barrelEnd()   { return 1.4442; };
  static float endcapStart() { return 1.566 ; };


 private:

  static void symmetrize( std::vector<float> &etaBins );

};

#endif
