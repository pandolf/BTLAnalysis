#ifndef BTLCommon_h
#define BTLCommon_h


#include "TPaveText.h"
#include "TCanvas.h"


class BTLCommon {

 public:

  BTLCommon();
  ~BTLCommon();

  static void setStyle();

  static void addLabels( TCanvas* c1 );

  static TPaveText* getLabelLeft ( const std::string& text="H4 Test Beam (September 2018)" );
  static TPaveText* getLabelRight( const std::string& text="60 GeV #pi^{+}" );

  static std::vector<int> colors();

};

#endif
