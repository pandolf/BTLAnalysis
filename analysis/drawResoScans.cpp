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




void drawScans( BTLConf conf, const std::string& name="" );
std::pair< TGraphErrors*, TGraphErrors* >  getScan( const std::string& digiConf, const std::string& var, float value, const std::string& name );
void drawScan( BTLConf conf, const std::string& scanName, std::vector< std::pair< TGraphErrors*, TGraphErrors* > > scans, float xMin, float xMax, const std::string& axisName, const std::string& legendTitle, const std::string& name );


int main( int argc, char* argv[] ) {


  std::string digiConf = "6a";

  if( argc>1 ) {
    digiConf = std::string(argv[1]);
  }

  
  BTLCommon::setStyle();

  BTLConf conf( 4, digiConf );

  drawScans( conf, "" );
  drawScans( conf, "hodoOnBar" );
  drawScans( conf, "hodoCenter" );

  return 0;

}


void drawScans( BTLConf conf, const std::string& name ) {

  // Vbias scan

  std::vector< std::pair<TGraphErrors*,TGraphErrors*> > scans_vBias;
  scans_vBias.push_back( getScan(conf.digiConf(), "ninoThr",  40, name) );
  scans_vBias.push_back( getScan(conf.digiConf(), "ninoThr",  60, name) );
  scans_vBias.push_back( getScan(conf.digiConf(), "ninoThr", 100, name) );
  scans_vBias.push_back( getScan(conf.digiConf(), "ninoThr", 200, name) );
  scans_vBias.push_back( getScan(conf.digiConf(), "ninoThr", 500, name) );

  drawScan( conf, "vBias", scans_vBias, 67., 77.99, "V(bias) [V]", "NINO threshold", name );

  for( unsigned i=0; i<scans_vBias.size(); ++i ) {
    delete scans_vBias[i].first;
    delete scans_vBias[i].second;
  }


  // NINO scan

  std::vector< std::pair<TGraphErrors*,TGraphErrors*> > scans_nino;
  scans_nino.push_back( getScan(conf.digiConf(), "vBias",  69, name) );
  scans_nino.push_back( getScan(conf.digiConf(), "vBias",  70, name) );
  scans_nino.push_back( getScan(conf.digiConf(), "vBias",  72, name) );

  drawScan( conf, "ninoThr", scans_nino, 0., 580., "NINO threshold [mV]", "V(bias)", name );

  for( unsigned i=0; i<scans_nino.size(); ++i ) {
    delete scans_nino[i].first;
    delete scans_nino[i].second;
  }

}




std::pair<TGraphErrors*,TGraphErrors*> getScan( const std::string& digiConf, const std::string& var, float value, const std::string& name ) {


  std::string suffix(name);
  if( suffix != "" ) suffix = "_" + suffix;


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

    BTLConf conf( 4, digiConf );
    if( var=="ninoThr" ) {
      conf.set_ninoThr( value );
      conf.set_vBias( x_values[i] );
    } else if( var=="vBias" ) {
      conf.set_ninoThr( x_values[i] );
      conf.set_vBias( value );
    }

    TFile* resoFile = conf.get_resoFile(name);

    if( resoFile!=0 ) {

      TH1D* h1_reso = (TH1D*)resoFile->Get("reso_corr");

      if( h1_reso==0 ) continue;

      TF1* f1_gaus = h1_reso->GetFunction( Form("gaus_%s", h1_reso->GetName()) );

      if( f1_gaus==0 ) continue;

      float y = f1_gaus->GetParameter(2);
      float y_err = f1_gaus->GetParError(2);

      int iPoint = graph->GetN();
      graph->SetPoint( iPoint, x_values[i], BTLCommon::subtractResoPTK(y)*1000. );
      graph->SetPointError( iPoint, 0., y_err );

      graph_sigmaEff->SetPoint( iPoint, x_values[i], BTLCommon::subtractResoPTK(BTLCommon::getSigmaEff(h1_reso))*1000. );

    }

  } // for configs

  std::pair< TGraphErrors*, TGraphErrors* > gr_pair;
  gr_pair.first  = graph; 
  gr_pair.second = graph_sigmaEff; 

  return gr_pair;

}


void drawScan( BTLConf conf, const std::string& scanName, std::vector< std::pair<TGraphErrors*,TGraphErrors*> > scans, float xMin, float xMax, const std::string& axisName, const std::string& legendTitle, const std::string& name ) {

  std::string suffix(name);
  if( suffix != "" ) suffix = "_" + suffix;

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  TH2D* h2_axes = new TH2D( Form("axes_%s", scanName.c_str()), "", 10, xMin, xMax, 10, 0., 95. );
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


  BTLCommon::addLabels( c1, conf );

  c1->SaveAs( Form("plots/scan_%s_%s%s.pdf", scanName.c_str(), conf.digiConf().c_str(), suffix.c_str()) );

  delete c1;
  delete h2_axes;

}
