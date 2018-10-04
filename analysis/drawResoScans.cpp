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




TGraphErrors* getScan( const std::string& var, float value );


int main( int argc, char* argv[] ) {

  
  BTLCommon::setStyle();

  //BTLConf conf_4_6_100_68("Conf_4_6_100_68");
  //BTLConf conf_4_6_100_69("Conf_4_6_100_69");
  //BTLConf conf_4_6_100_70("Conf_4_6_100_70");
  //BTLConf conf_4_6_100_72("Conf_4_6_100_72");
  //BTLConf conf_4_6_200_69("Conf_4_6_200_69");
  //BTLConf conf_4_6_200_70("Conf_4_6_200_70");
  //BTLConf conf_4_6_200_72("Conf_4_6_200_72");
  //BTLConf conf_4_6_40_69 ("Conf_4_6_40_69");
  //BTLConf conf_4_6_40_70 ("Conf_4_6_40_70");
  //BTLConf conf_4_6_40_72 ("Conf_4_6_40_72");
  //BTLConf conf_4_6_500_69("Conf_4_6_500_69");
  //BTLConf conf_4_6_500_70("Conf_4_6_500_70");
  //BTLConf conf_4_6_500_72("Conf_4_6_500_72");
  //BTLConf conf_4_6_60_69 ("Conf_4_6_60_69");
  //BTLConf conf_4_6_60_70 ("Conf_4_6_60_70");
  //BTLConf conf_4_6_60_72 ("Conf_4_6_60_72");

  //std::vector<BTLConf> nino100;
  //nino100.push_back( conf_4_6_100_68 );
  //nino100.push_back( conf_4_6_100_69 );
  //nino100.push_back( conf_4_6_100_70 );
  //nino100.push_back( conf_4_6_100_72 );

  //std::vector<BTLConf> nino200;
  //nino200.push_back( conf_4_6_200_69 );
  //nino200.push_back( conf_4_6_200_70 );
  //nino200.push_back( conf_4_6_200_72 );

  //std::vector<BTLConf> nino40;
  //nino40.push_back( conf_4_6_40_69 );
  //nino40.push_back( conf_4_6_40_70 );
  //nino40.push_back( conf_4_6_40_72 );

  //std::vector<BTLConf> nino40;
  //nino40.push_back( conf_4_6_40_69 );
  //nino40.push_back( conf_4_6_40_70 );
  //nino40.push_back( conf_4_6_40_72 );

  std::vector<TGraphErrors*> scans_vBias;
  scans_vBias.push_back( getScan("ninoThr", 100) );
  scans_vBias.push_back( getScan("ninoThr", 200) );

  return 0;

}


TGraphErrors* getScan( const std::string& var, float value ) {


  TGraphErrors* graph = new TGraphErrors(0);
  if( var=="ninoThr" )
    graph->SetName( Form( "NINO thr = %.0f mV", value ) );
  else
    graph->SetName( Form( "Vbias = %.0f V", value ) );


  std::vector<float> x_values;
  if( var=="ninoThr" ) { // then scan vBias
    x_values.push_back( 68. );
    x_values.push_back( 69. );
    x_values.push_back( 70. );
    //x_values.push_back( 71. );
    x_values.push_back( 72. );
  } else if( var=="vBias" ) { // then scan NINOthr
    x_values.push_back( 40. );
    x_values.push_back( 60. );
    x_values.push_back( 100. );
    x_values.push_back( 200. );
    x_values.push_back( 500. );
  }


  for( unsigned i=0; i<x_values.size(); ++i ) {

    BTLConf conf( 4, 6 );
    if( var=="ninoThr" ) {
      conf.set_ninoThr( value );
      conf.set_vBias( x_values[i] );
    } else if( var=="vBias" ) {
      conf.set_ninoThr( x_values[i] );
      conf.set_vBias( value );
    }

    TFile* resoFile = conf.get_resoFile();

    float y = -1.;
    float y_err = -1.;

    if( resoFile!=0 ) {

      TH1D* h1_reso = (TH1D*)resoFile->Get("reso_corr");

      TF1* f1_gaus = h1_reso->GetFunction( Form("gaus_%s", h1_reso->GetName()) );

      y = f1_gaus->GetParameter(2);
      y_err = f1_gaus->GetParError(2);

      int iPoint = graph->GetN();
      graph->SetPoint( iPoint, x_values[i], y );
      graph->SetPointError( iPoint, 0., y_err );

    }

  } // for configs

  return graph;
}
