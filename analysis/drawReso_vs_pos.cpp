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



void drawReso_vs_pos( BTLConf conf, TTree* tree, const std::string& posvar, const std::string& axisName, int nBins, float varMin, float varMax );
void addPointToGraph( TGraphErrors* gr_mean, TGraphErrors* gr_sigma, float x, TF1* f1 );
TF1* fitLine( TGraphErrors* graph );


int main( int argc, char* argv[] ) {


  if( argc==1 ) {

    std::cout << "USAGE: ./drawReso_vs_pos [confName]" << std::endl;
    exit(1);

  }

  BTLCommon::setStyle();

  std::string confName( argv[1] );

  BTLConf conf(confName);

  TFile* file = TFile::Open( Form("treesLite/%s_corr.root", confName.c_str()) );
  TTree* tree = (TTree*)file->Get( "treeLite" );

  drawReso_vs_pos( conf, tree, "x_hodo", "Hodoscope X [mm]", 20, -10., 10. ); 

  return 0;

}



void drawReso_vs_pos( BTLConf conf, TTree* tree, const std::string& posvar, const std::string& axisName, int nBins, float varMin, float varMax ) {

  float xMinT = (conf.digiChannelSet()=="a") ? 2.4001 : 3.6001;
  float xMaxT = (conf.digiChannelSet()=="a") ? 3.599 : 4.99;
  int  nBinsT = (int)(( xMaxT-xMinT )/0.0025);

  float binWidth_hodo = (varMax-varMin)/((float)nBins);

  std::string otherVar = (posvar=="x_hodo") ? "y_hodo" : "x_hodo";
  float otherVarMin    = (posvar=="x_hodo") ? 0. : -10.;
  float otherVarMax    = (posvar=="x_hodo") ? 5. :  10.;

  //std::vector< TH1D* > vh1_tAve_vs_pos;
  //std::vector< TH1D* > vh1_tLeft_vs_pos;
  //std::vector< TH1D* > vh1_tRight_vs_pos;

  TGraphErrors* gr_tAve_mean   = new TGraphErrors(0);
  TGraphErrors* gr_tLeft_mean  = new TGraphErrors(0);
  TGraphErrors* gr_tRight_mean = new TGraphErrors(0);

  gr_tAve_mean  ->SetName( Form("tAve_mean_%s"  , posvar.c_str()) );
  gr_tLeft_mean ->SetName( Form("tLeft_mean_%s" , posvar.c_str()) );
  gr_tRight_mean->SetName( Form("tRight_mean_%s", posvar.c_str()) );

  gr_tAve_mean  ->SetMarkerStyle(20);
  gr_tLeft_mean ->SetMarkerStyle(20);
  gr_tRight_mean->SetMarkerStyle(20);

  gr_tAve_mean  ->SetMarkerSize(1.1);
  gr_tLeft_mean ->SetMarkerSize(1.1);
  gr_tRight_mean->SetMarkerSize(1.1);

  gr_tAve_mean  ->SetMarkerColor(kBlack);
  gr_tLeft_mean ->SetMarkerColor(46);
  gr_tRight_mean->SetMarkerColor(38);

  gr_tAve_mean  ->SetLineColor(kBlack);
  gr_tLeft_mean ->SetLineColor(46);
  gr_tRight_mean->SetLineColor(38);

  TGraphErrors* gr_tAve_sigma   = new TGraphErrors(0);
  TGraphErrors* gr_tLeft_sigma  = new TGraphErrors(0);
  TGraphErrors* gr_tRight_sigma = new TGraphErrors(0);

  gr_tAve_sigma  ->SetName( Form("tAve_sigma_%s"  , posvar.c_str()) );
  gr_tLeft_sigma ->SetName( Form("tLeft_sigma_%s" , posvar.c_str()) );
  gr_tRight_sigma->SetName( Form("tRight_sigma_%s", posvar.c_str()) );

  gr_tAve_sigma  ->SetMarkerStyle(20);
  gr_tLeft_sigma ->SetMarkerStyle(20);
  gr_tRight_sigma->SetMarkerStyle(20);

  gr_tAve_sigma  ->SetMarkerSize(1.1);
  gr_tLeft_sigma ->SetMarkerSize(1.1);
  gr_tRight_sigma->SetMarkerSize(1.1);

  gr_tAve_sigma  ->SetMarkerColor(kBlack);
  gr_tLeft_sigma ->SetMarkerColor(46);
  gr_tRight_sigma->SetMarkerColor(38);

  gr_tAve_sigma  ->SetLineColor(kBlack);
  gr_tLeft_sigma ->SetLineColor(46);
  gr_tRight_sigma->SetLineColor(38);

  for( int i=0; i<nBins; ++i ) {

    TH1D* h1_tAve   = new TH1D( Form( "tAve_vs_%s_%d"  , posvar.c_str(), i ), "", nBinsT, xMinT, xMaxT );
    TH1D* h1_tLeft  = new TH1D( Form( "tLeft_vs_%s_%d" , posvar.c_str(), i ), "", nBinsT, xMinT, xMaxT );
    TH1D* h1_tRight = new TH1D( Form( "tRight_vs_%s_%d", posvar.c_str(), i ), "", nBinsT, xMinT, xMaxT );

    float varMin_cut = varMin + (i  )*binWidth_hodo;
    float varMax_cut = varMin + (i+1)*binWidth_hodo;

    std::string hodoCut( Form("%s>=%f && %s<%f && %s>=%f && %s<%f", posvar.c_str(), varMin_cut, posvar.c_str(), varMax_cut, otherVar.c_str(), otherVarMin, otherVar.c_str(), otherVarMax) );

    tree->Project( h1_tAve  ->GetName(), "0.5*(tLeft_corr+tRight_corr)", hodoCut.c_str() );
    tree->Project( h1_tLeft ->GetName(), "tLeft_corr"             , hodoCut.c_str() );
    tree->Project( h1_tRight->GetName(), "tRight_corr"            , hodoCut.c_str() );

    TF1* f1_gaus_tAve   = BTLCommon::fitGaus( h1_tAve  , 2.1 );
    TF1* f1_gaus_tLeft  = BTLCommon::fitGaus( h1_tLeft , 2.1 );
    TF1* f1_gaus_tRight = BTLCommon::fitGaus( h1_tRight, 2.1 );

    addPointToGraph( gr_tAve_mean  , gr_tAve_sigma  ,  0.5*(varMin_cut+varMax_cut), f1_gaus_tAve   );
    addPointToGraph( gr_tLeft_mean , gr_tLeft_sigma ,  0.5*(varMin_cut+varMax_cut), f1_gaus_tLeft  );
    addPointToGraph( gr_tRight_mean, gr_tRight_sigma,  0.5*(varMin_cut+varMax_cut), f1_gaus_tRight );

    //vh1_tAve_vs_pos  .push_back( h1_tAve   );
    //vh1_tLeft_vs_pos .push_back( h1_tLeft  );
    //vh1_tRight_vs_pos.push_back( h1_tRight );
  
  } // for bins

  fitLine( gr_tAve_mean   );
  fitLine( gr_tLeft_mean  );
  fitLine( gr_tRight_mean );

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  TH2D* h2_axes = new TH2D( "axes", "", 10, varMin-5., varMax+5., 10, xMinT, xMaxT );
  h2_axes->SetYTitle( "t(i) - t(PTK) [ns]" );
  h2_axes->SetXTitle( axisName.c_str() );
  h2_axes->Draw();

  TLegend* legend = new TLegend( 0.75, 0.72, 0.9, 0.9 );
  legend->SetTextSize(0.035);
  legend->SetFillColor(0);
  legend->AddEntry( gr_tLeft_mean , "t(Left)" , "P" );
  legend->AddEntry( gr_tRight_mean, "t(Right)", "P" );
  legend->AddEntry( gr_tAve_mean  , "t(Ave)"  , "P" );
  legend->Draw("same");

  TPaveText* label_conf = conf.get_labelConf(2);
  label_conf->Draw("same");
   
  gr_tLeft_mean ->Draw("p same"); 
  gr_tRight_mean->Draw("p same"); 
  gr_tAve_mean  ->Draw("p same"); 

  BTLCommon::addLabels( c1, conf );

  c1->SaveAs( Form("plots/%s/t_vs_%s.pdf", conf.get_confName().c_str(), posvar.c_str()) );

  delete c1;
  delete h2_axes;

}


void addPointToGraph( TGraphErrors* gr_mean, TGraphErrors* gr_sigma, float x, TF1* f1 ) {

  int nPoint = gr_mean->GetN();

  gr_mean ->SetPoint     ( nPoint,  x, f1->GetParameter(1) );
  gr_mean ->SetPointError( nPoint, 0., f1->GetParError (1) );

  gr_sigma->SetPoint     ( nPoint,  x, f1->GetParameter(2) );
  gr_sigma->SetPointError( nPoint, 0., f1->GetParError (2) );

}


TF1* fitLine( TGraphErrors* graph ) {

  TF1* f1 = new TF1( Form("line_%s", graph->GetName()), "[0] + [1]*x" );
  f1->SetLineColor( graph->GetLineColor() );

  graph->Fit( f1->GetName(), "Q+" );

  return f1;

}
