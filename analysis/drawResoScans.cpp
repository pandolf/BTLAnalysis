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




std::pair< TGraphErrors*, TGraphErrors* >  getScan( const std::string& var, float value );
void drawScan( const std::string& name, std::vector< std::pair< TGraphErrors*, TGraphErrors* > > scans, float xMin, float xMax, const std::string& axisName, const std::string& legendTitle );


int main( int argc, char* argv[] ) {

  
  BTLCommon::setStyle();


  // Vbias scan

  std::vector< std::pair<TGraphErrors*,TGraphErrors*> > scans_vBias;
  scans_vBias.push_back( getScan("ninoThr",  40) );
  scans_vBias.push_back( getScan("ninoThr",  60) );
  scans_vBias.push_back( getScan("ninoThr", 100) );
  scans_vBias.push_back( getScan("ninoThr", 200) );
  scans_vBias.push_back( getScan("ninoThr", 500) );

  drawScan( "vBias", scans_vBias, 67., 77.99, "V(bias) [V]", "NINO threshold" );


  // NINO scan

  std::vector< std::pair<TGraphErrors*,TGraphErrors*> > scans_nino;
  scans_nino.push_back( getScan("vBias",  69) );
  scans_nino.push_back( getScan("vBias",  70) );
  scans_nino.push_back( getScan("vBias",  72) );


  drawScan( "ninoThr", scans_nino, 0., 580., "NINO threshold [mV]", "V(bias)" );

  return 0;

}





std::pair<TGraphErrors*,TGraphErrors*> getScan( const std::string& var, float value ) {


  TGraphErrors* graph = new TGraphErrors(0);
  if( var=="ninoThr" )
    graph->SetName( Form( "%.0f mV", value ) );
  else
    graph->SetName( Form( "%.0f V", value ) );

  TGraphErrors* graph_sigmaEff = new TGraphErrors(0);
  graph_sigmaEff->SetName(Form("gr_%s_sigmaEff", var.c_str()));


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

    if( resoFile!=0 ) {

      TH1D* h1_reso = (TH1D*)resoFile->Get("reso_corr");

      if( h1_reso==0 ) continue;

      TF1* f1_gaus = h1_reso->GetFunction( Form("gaus_%s", h1_reso->GetName()) );

      if( f1_gaus==0 ) continue;

      float y = f1_gaus->GetParameter(2)*1000.;
      float y_err = f1_gaus->GetParError(2)*1000.;

      int iPoint = graph->GetN();
      graph->SetPoint( iPoint, x_values[i], BTLCommon::subtractResoPTK(y) );
      graph->SetPointError( iPoint, 0., y_err );

      graph_sigmaEff->SetPoint( iPoint, x_values[i], BTLCommon::subtractResoPTK(BTLCommon::getSigmaEff(h1_reso)*1000.) );

    }

  } // for configs

  std::pair< TGraphErrors*, TGraphErrors* > gr_pair;
  gr_pair.first  = graph; 
  gr_pair.second = graph_sigmaEff; 

  return gr_pair;

}


void drawScan( const std::string& name, std::vector< std::pair<TGraphErrors*,TGraphErrors*> > scans, float xMin, float xMax, const std::string& axisName, const std::string& legendTitle ) {

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  TH2D* h2_axes = new TH2D( Form("axes_%s", name.c_str()), "", 10, xMin, xMax, 10, 0., 95. );
  h2_axes->SetXTitle( axisName.c_str() );
  h2_axes->SetYTitle( "Time Resolution [ps]" );
  h2_axes->Draw();

  TLine* line30 = new TLine( xMin, 30., xMax, 30. );
  line30->SetLineStyle(3);
  line30->SetLineColor(kBlack);
  line30->SetLineWidth(2);
  line30->Draw("same");

  TLegend* legend = new TLegend( 0.63, 0.9 - 0.08*scans.size(), 0.9, 0.9, legendTitle.c_str() );
  legend->SetFillColor(0);
  legend->SetTextSize(0.035);

  std::vector<int> colors = BTLCommon::colors();

  // first round for sigmaEff (behind):
  for( unsigned i=0; i<scans.size(); ++i ) {

    scans[i].second->SetLineColor( colors[i] );
    scans[i].second->SetLineWidth(2);
    scans[i].second->SetLineStyle(2);
    scans[i].second->Draw("Lsame");

  }

  // second round to draw reso graphs:
  for( unsigned i=0; i<scans.size(); ++i ) {

    legend->AddEntry( scans[i].first, scans[i].first->GetName(), "LP" );
    // then reso graph:
    scans[i].first->SetLineColor( colors[i] );
    scans[i].first->SetLineWidth(2);
    scans[i].first->SetMarkerStyle(20+i);
    scans[i].first->SetMarkerSize(1.3);
    scans[i].first->SetMarkerColor(colors[i]);
    scans[i].first->Draw("PLsame");

  }

  legend->Draw("same");

  TGraph* gr_dummyLine = new TGraph(0);
  gr_dummyLine->SetLineColor( kGray+1 );
  gr_dummyLine->SetLineWidth( 2 );
  gr_dummyLine->SetLineStyle( 1 );

  TGraph* gr_dummyLine2 = new TGraph(0);
  gr_dummyLine2->SetLineColor( kGray+1 );
  gr_dummyLine2->SetLineWidth( 2 );
  gr_dummyLine2->SetLineStyle( 2 );

  TLegend* legend2 = new TLegend( 0.63, 0.2, 0.9, 0.33 );
  legend2->SetFillColor(0);
  legend2->SetTextSize(0.038);
  legend2->AddEntry( gr_dummyLine2, "#sigma_{eff}", "L" );
  legend2->AddEntry( gr_dummyLine , "#sigma_{fit}", "L" );
  legend2->Draw("same");


  BTLCommon::addLabels( c1 );

  c1->SaveAs( Form("plots/scan_%s.pdf", name.c_str()) );

  delete c1;
  delete h2_axes;

}
