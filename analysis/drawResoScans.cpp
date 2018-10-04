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
void drawScan( const std::string& name, std::vector<TGraphErrors*> scans, float xMin, float xMax, const std::string& axisName, const std::string& legendTitle );


int main( int argc, char* argv[] ) {

  
  BTLCommon::setStyle();


  // Vbias scan

  std::vector<TGraphErrors*> scans_vBias;
  scans_vBias.push_back( getScan("ninoThr",  40) );
  scans_vBias.push_back( getScan("ninoThr",  60) );
  scans_vBias.push_back( getScan("ninoThr", 100) );
  scans_vBias.push_back( getScan("ninoThr", 200) );
  scans_vBias.push_back( getScan("ninoThr", 500) );

  drawScan( "vBias", scans_vBias, 67., 78., "V(bias) [V]", "NINO threshold" );


  // NINO scan

  std::vector<TGraphErrors*> scans_nino;
  scans_nino.push_back( getScan("vBias",  69) );
  scans_nino.push_back( getScan("vBias",  70) );
  scans_nino.push_back( getScan("vBias",  72) );


  drawScan( "ninoThr", scans_nino, 0., 650., "NINO threshold [mV]", "V(bias)" );

  return 0;

}





TGraphErrors* getScan( const std::string& var, float value ) {


  TGraphErrors* graph = new TGraphErrors(0);
  if( var=="ninoThr" )
    graph->SetName( Form( "%.0f mV", value ) );
  else
    graph->SetName( Form( "%.0f V", value ) );


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

      if( h1_reso==0 ) continue;

      TF1* f1_gaus = h1_reso->GetFunction( Form("gaus_%s", h1_reso->GetName()) );

      if( f1_gaus==0 ) continue;

      y = f1_gaus->GetParameter(2)*1000.;
      y_err = f1_gaus->GetParError(2)*1000.;

      int iPoint = graph->GetN();
      graph->SetPoint( iPoint, x_values[i], BTLCommon::subtractResoPTK(y) );
      graph->SetPointError( iPoint, 0., y_err );

    }

  } // for configs

  return graph;
}


void drawScan( const std::string& name, std::vector<TGraphErrors*> scans, float xMin, float xMax, const std::string& axisName, const std::string& legendTitle ) {

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  TH2D* h2_axes = new TH2D( Form("axes_%s", name.c_str()), "", 10, xMin, xMax, 10, 0., 95. );
  h2_axes->SetXTitle( axisName.c_str() );
  h2_axes->SetYTitle( "Time Resolution [ps]" );
  h2_axes->Draw();

  TLegend* legend = new TLegend( 0.63, 0.9 - 0.08*scans.size(), 0.9, 0.9, legendTitle.c_str() );
  legend->SetFillColor(0);
  legend->SetTextSize(0.035);

  std::vector<int> colors = BTLCommon::colors();
  for( unsigned i=0; i<scans.size(); ++i ) {
    legend->AddEntry( scans[i], scans[i]->GetName(), "LP" );
    scans[i]->SetLineColor( colors[i] );
    scans[i]->SetLineWidth(2);
    scans[i]->SetMarkerStyle(20+i);
    scans[i]->SetMarkerSize(1.3);
    scans[i]->SetMarkerColor(colors[i]);
    scans[i]->Draw("PLsame");
  }

  legend->Draw("same");

  BTLCommon::addLabels( c1 );

  c1->SaveAs( Form("plots/scan_%s.pdf", name.c_str()) );

  delete c1;
  delete h2_axes;

}
