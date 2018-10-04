#include <iostream>

#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TLegend.h"
#include "TPaveText.h"
#include "TF1.h"
#include "TGraphErrors.h"

#include "../interface/BTLCommon.h"
#include "../interface/BTLConf.h"




TGraphErrors* getScan( const std::string& name, const std::string& scanType, std::vector<BTLConf> v_conf );


int main( int argc, char* argv[] ) {

  
  BTLCommon::setStyle();

  BTLConf conf_4_6_100_68("Conf_4_6_100_68");
  BTLConf conf_4_6_100_69("Conf_4_6_100_69");
  BTLConf conf_4_6_100_70("Conf_4_6_100_70");
  BTLConf conf_4_6_100_72("Conf_4_6_100_72");
  BTLConf conf_4_6_200_69("Conf_4_6_200_69");
  BTLConf conf_4_6_200_70("Conf_4_6_200_70");
  BTLConf conf_4_6_200_72("Conf_4_6_200_72");
  BTLConf conf_4_6_40_69 ("Conf_4_6_40_69");
  BTLConf conf_4_6_40_70 ("Conf_4_6_40_70");
  BTLConf conf_4_6_40_72 ("Conf_4_6_40_72");
  BTLConf conf_4_6_500_69("Conf_4_6_500_69");
  BTLConf conf_4_6_500_70("Conf_4_6_500_70");
  BTLConf conf_4_6_500_72("Conf_4_6_500_72");
  BTLConf conf_4_6_60_69 ("Conf_4_6_60_69");
  BTLConf conf_4_6_60_70 ("Conf_4_6_60_70");
  BTLConf conf_4_6_60_72 ("Conf_4_6_60_72");

  std::vector<BTLConf> nino100;
  nino100.push_back( conf_4_6_100_68 );
  nino100.push_back( conf_4_6_100_69 );
  nino100.push_back( conf_4_6_100_70 );
  nino100.push_back( conf_4_6_100_72 );

  std::vector<TGraphErrors*> scans_vBias;
  scans_vBias.push_back( getScan("NINO thr = 100 mV", "vBias", nino100) );

  return 0;

}


TGraphErrors* getScan( const std::string& name, const std::string& scanType, std::vector<BTLConf> v_conf ) {

  TGraphErrors* graph = new TGraphErrors(0);
  graph->SetName( name.c_str() );

  for( unsigned i=0; i<v_conf.size(); ++i ) {

    TFile* resoFile = v_conf[i].get_resoFile();

    float x = (scanType=="ninoThr") ? v_conf[i].vBias() : v_conf[i].ninoThr();

    float y = -1.;
    float y_err = -1.;

    if( resoFile!=0 ) {

      TH1D* h1_reso = (TH1D*)resoFile->Get("reso_corr");

      TF1* f1_gaus = h1_reso->GetFunction( Form("gaus_%s", h1_reso->GetName()) );

      y = f1_gaus->GetParameter(2);
      y_err = f1_gaus->GetParError(2);

      int iPoint = graph->GetN();
      graph->SetPoint( iPoint, x, y );
      graph->SetPointError( iPoint, 0., y_err );

    }

  } // for configs

  return graph;
}
