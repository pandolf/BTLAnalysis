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



struct TFuncStruct {

  TF1* tLeft;
  TF1* tRight;

};


TFuncStruct draw_vs_pos( BTLConf conf, TTree* tree, const std::string& yVar, const std::string& posvar, const std::string& axisName, int nBins, float varMin, float varMax, TF1* f1_tLeft=0, TF1* f1_tRight=0 );
void addPointToGraph( TGraphErrors* gr, const std::string& yVar, float x, float xerr, TF1* f1 );
TF1* fitLine( TGraphErrors* graph, float varMin, float varMax );
void drawHisto( BTLConf conf, TH1D* histo, const std::string& posCut );


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

//TF1* f1_tLeft  = 0;
//TF1* f1_tRight = 0;

  TFuncStruct lines = draw_vs_pos( conf, tree, "mean" , "x_hodo", "Hodoscope X [mm]", 30, -10., 10. );
  draw_vs_pos( conf, tree, "mean" , "x_hodo", "Hodoscope X [mm]", 30, -10., 10., lines.tLeft, lines.tRight ); 

  draw_vs_pos( conf, tree, "sigma", "x_hodo", "Hodoscope X [mm]", 10, -10., 10. ); 
  draw_vs_pos( conf, tree, "sigma", "x_hodo", "Hodoscope X [mm]", 10, -10., 10., lines.tLeft, lines.tRight ); 

  draw_vs_pos( conf, tree, "mean" , "y_hodo", "Hodoscope Y [mm]", 20, 0., 4. ); 
  draw_vs_pos( conf, tree, "sigma", "y_hodo", "Hodoscope Y [mm]",  8, 0., 4. ); 

  draw_vs_pos( conf, tree, "mean" , "y_hodo", "Hodoscope Y [mm]", 20, 0., 4., lines.tLeft, lines.tRight ); 
  draw_vs_pos( conf, tree, "sigma", "y_hodo", "Hodoscope Y [mm]",  8, 0., 4., lines.tLeft, lines.tRight ); 

  return 0;

}



TFuncStruct draw_vs_pos( BTLConf conf, TTree* tree, const std::string& yVar, const std::string& posvar, const std::string& axisName, int nBins, float varMin, float varMax, TF1* f1_tLeft, TF1* f1_tRight ) {

  TFuncStruct lines;

  std::string suffix = (f1_tLeft==0 && f1_tRight==0) ? "" : "_corr";

  float xMinT = (conf.digiChannelSet()=="a") ? 2.4001 : 3.6001;
  float xMaxT = (conf.digiChannelSet()=="a") ? 3.599 : 4.99;
  int  nBinsT = (int)(( xMaxT-xMinT )/0.0025);

  float binWidth_hodo = (varMax-varMin)/((float)nBins);

  std::string otherVar = (posvar=="x_hodo") ? "y_hodo" : "x_hodo";
  float otherVarMin    = (posvar=="x_hodo") ? 0. : -8.;
  float otherVarMax    = (posvar=="x_hodo") ? 5. :  8.;

  //std::vector< TH1D* > vh1_tAve_vs_pos;
  //std::vector< TH1D* > vh1_tLeft_vs_pos;
  //std::vector< TH1D* > vh1_tRight_vs_pos;

  TGraphErrors* gr_tAve   = new TGraphErrors(0);
  TGraphErrors* gr_tLeft  = new TGraphErrors(0);
  TGraphErrors* gr_tRight = new TGraphErrors(0);

  gr_tAve  ->SetName( Form("tAve_%s_vs_%s%s"  , yVar.c_str(), posvar.c_str(), suffix.c_str()) );
  gr_tLeft ->SetName( Form("tLeft_%s_vs_%s%s" , yVar.c_str(), posvar.c_str(), suffix.c_str()) );
  gr_tRight->SetName( Form("tRight_%s_vs_%s%s", yVar.c_str(), posvar.c_str(), suffix.c_str()) );

  gr_tAve  ->SetMarkerStyle(20);
  gr_tLeft ->SetMarkerStyle(20);
  gr_tRight->SetMarkerStyle(20);

  gr_tAve  ->SetMarkerSize(1.1);
  gr_tLeft ->SetMarkerSize(1.1);
  gr_tRight->SetMarkerSize(1.1);

  gr_tAve  ->SetMarkerColor(kBlack);
  gr_tLeft ->SetMarkerColor(46);
  gr_tRight->SetMarkerColor(38);

  gr_tAve  ->SetLineColor(kBlack);
  gr_tLeft ->SetLineColor(46);
  gr_tRight->SetLineColor(38);


  std::string plotDir( Form( "plots/%s/t_vs_pos_fits/", conf.get_confName().c_str()) );
  system( Form("mkdir -p %s", plotDir.c_str()) );


  for( int i=0; i<nBins; ++i ) {

    TH1D* h1_tAve   = new TH1D( Form( "tAve_%s_vs_%s_%d%s"  , yVar.c_str(), posvar.c_str(), i, suffix.c_str() ), "", nBinsT, xMinT, xMaxT );
    TH1D* h1_tLeft  = new TH1D( Form( "tLeft_%s_vs_%s_%d%s" , yVar.c_str(), posvar.c_str(), i, suffix.c_str() ), "", nBinsT, xMinT, xMaxT );
    TH1D* h1_tRight = new TH1D( Form( "tRight_%s_vs_%s_%d%s", yVar.c_str(), posvar.c_str(), i, suffix.c_str() ), "", nBinsT, xMinT, xMaxT );

    h1_tAve   ->SetXTitle( "t(ave) - t(MCP) [ns]" );
    h1_tLeft  ->SetXTitle( "t(left) - t(MCP) [ns]" );
    h1_tRight ->SetXTitle( "t(right) - t(MCP) [ns]" );

    float varMin_cut = varMin + (i  )*binWidth_hodo;
    float varMax_cut = varMin + (i+1)*binWidth_hodo;

    float x = 0.5*(varMin_cut+varMax_cut);
    float xerr = (varMax_cut-varMin_cut)/sqrt(12);

    std::string hodoCut( Form("%s>=%f && %s<%f && %s>=%f && %s<%f", posvar.c_str(), varMin_cut, posvar.c_str(), varMax_cut, otherVar.c_str(), otherVarMin, otherVar.c_str(), otherVarMax) );
    //std::cout << hodoCut << std::endl;
    float corrLeft  = (f1_tLeft ) ? ( f1_tLeft ->Eval( 0.5*(varMin+varMax) ) / f1_tLeft ->Eval(x) ) : 1.;
    float corrRight = (f1_tRight) ? ( f1_tRight->Eval( 0.5*(varMin+varMax) ) / f1_tRight->Eval(x) ) : 1.;

    tree->Project( h1_tAve  ->GetName(), Form("0.5*(%f*tLeft_corr+%f*tRight_corr)", corrLeft, corrRight), hodoCut.c_str() );
    tree->Project( h1_tLeft ->GetName(), Form("%f*tLeft_corr"                     , corrLeft           ), hodoCut.c_str() );
    tree->Project( h1_tRight->GetName(), Form("%f*tRight_corr"                    ,           corrRight), hodoCut.c_str() );

    //if( h1_tAve->GetEntries()<20 ) continue;

    TF1* f1_gaus_tAve   = BTLCommon::fitGaus( h1_tAve  , 2.1 );
    TF1* f1_gaus_tLeft  = BTLCommon::fitGaus( h1_tLeft , 2.1 );
    TF1* f1_gaus_tRight = BTLCommon::fitGaus( h1_tRight, 2.1 );

    drawHisto( conf, h1_tAve  , Form("%.2f < %s < %.2f mm", varMin_cut, posvar.c_str(), varMax_cut) );
    drawHisto( conf, h1_tRight, Form("%.2f < %s < %.2f mm", varMin_cut, posvar.c_str(), varMax_cut) );
    drawHisto( conf, h1_tLeft , Form("%.2f < %s < %.2f mm", varMin_cut, posvar.c_str(), varMax_cut) );

    addPointToGraph( gr_tAve  , yVar, x, xerr, f1_gaus_tAve   );
    addPointToGraph( gr_tLeft , yVar, x, xerr, f1_gaus_tLeft  );
    addPointToGraph( gr_tRight, yVar, x, xerr, f1_gaus_tRight );

    //vh1_tAve_vs_pos  .push_back( h1_tAve   );
    //vh1_tLeft_vs_pos .push_back( h1_tLeft  );
    //vh1_tRight_vs_pos.push_back( h1_tRight );
  
  } // for bins

  if( yVar=="mean" ) {
    fitLine( gr_tAve  , varMin, varMax ); // no need to save f1_tAve
    TF1* f1_line_tLeft  = fitLine( gr_tLeft , varMin, varMax );
    TF1* f1_line_tRight = fitLine( gr_tRight, varMin, varMax );
    lines.tLeft  = new TF1(*f1_line_tLeft  );
    lines.tRight = new TF1(*f1_line_tRight );
  }

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  // first mean

  float padding = (posvar=="x_hodo") ? 5. : 1.;
  float xMin = varMin-padding;
  float xMax = varMax+padding;
  float yMin         = ( yVar=="mean" ) ? xMinT : 0.;
  float yMax         = ( yVar=="mean" ) ? xMaxT : 90.;
  std::string yTitle = ( yVar=="mean" ) ? "t(i) - t(PTK) [ns]" : "Timing Resolution [ps]";

  TH2D* h2_axes = new TH2D( "axes", "", 10, xMin, xMax, 10, yMin, yMax );
  h2_axes->SetYTitle( yTitle.c_str() );
  h2_axes->SetXTitle( axisName.c_str() );
  h2_axes->Draw();

  if( yVar=="sigma" ) {
    TLine* line30 = new TLine( xMin, 30., xMax, 30. );
    line30->SetLineStyle(3);
    line30->SetLineColor(kBlack);
    line30->SetLineWidth(2);
    line30->Draw("same");
  }

  gr_tLeft ->Draw("p same"); 
  gr_tRight->Draw("p same"); 
  gr_tAve  ->Draw("p same"); 

  TLegend* legend = new TLegend( 0.75, 0.72, 0.9, 0.9 );
  legend->SetTextSize(0.035);
  legend->SetFillColor(0);
  legend->AddEntry( gr_tLeft , "t(Left)" , "P" );
  legend->AddEntry( gr_tRight, "t(Right)", "P" );
  legend->AddEntry( gr_tAve  , "t(Ave)"  , "P" );
  legend->Draw("same");

  TPaveText* label_conf = conf.get_labelConf(2);
  label_conf->Draw("same");
   
  BTLCommon::addLabels( c1, conf );

  c1->SaveAs( Form("plots/%s/t%s_vs_%s%s.pdf", conf.get_confName().c_str(), yVar.c_str(), posvar.c_str(), suffix.c_str()) );


  delete c1;
  delete h2_axes;

  return lines;

}


void addPointToGraph( TGraphErrors* gr, const std::string& yVar, float x, float xerr, TF1* f1_gaus ) {

  int nPoint = gr->GetN();

  if( yVar=="mean" ) {

    gr->SetPoint     ( nPoint,  x   , f1_gaus->GetParameter(1) );
    gr->SetPointError( nPoint,  xerr, f1_gaus->GetParError (1) );

  } else if( yVar=="sigma" ) {

    gr->SetPoint     ( nPoint,  x, BTLCommon::subtractResoPTK(f1_gaus->GetParameter(2))*1000. );
    gr->SetPointError( nPoint,  xerr, f1_gaus->GetParError(2)*1000. );

  } else {

    std::cout << "yVar can be only 'mean' or 'sigma'!!" << std::endl;

  }

}


TF1* fitLine( TGraphErrors* graph, float varMin, float varMax ) {

  TF1* f1 = new TF1( Form("line_%s", graph->GetName()), "[0] + [1]*x", varMin, varMax );
  f1->SetLineColor( graph->GetLineColor() );
  f1->SetParameter(0, 3.);
  f1->SetParameter(1, 0.);

  graph->Fit( f1->GetName(), "RQ+" );

  return f1;

}


void drawHisto( BTLConf conf, TH1D* histo, const std::string& posCut ) {

  std::string plotDir(Form("plots/%s/t_vs_pos_fits", conf.get_confName().c_str()));

  TCanvas* c1 = new TCanvas( Form("c1_%s", histo->GetName()), "", 600, 600 );
  c1->cd();

  histo->SetFillColor( 46 );
  histo->SetLineColor( 46 );
  histo->SetFillStyle( 3004 );
  histo->Draw();

  TF1* f1 = histo->GetFunction( Form("gaus_%s", histo->GetName()) );
  
  TPaveText* gausText = new TPaveText( 0.6, 0.7, 0.9, 0.9, "brNDC" );
  gausText->SetFillColor( 0 );
  gausText->SetTextColor( 46 );
  gausText->SetTextSize( 0.035 );
  gausText->AddText( posCut.c_str() );
  gausText->AddText( Form( "#mu = %.3f #pm %.3f ns"   , f1->GetParameter(1), f1->GetParError(1) ) );
  gausText->AddText( Form( "#sigma = %.1f #pm %.1f ps", f1->GetParameter(2)*1000., f1->GetParError(2)*1000. ) );
  gausText->Draw("same");

  TPaveText* labelConf = conf.get_labelConf(2);
  labelConf->Draw("same");

  BTLCommon::addLabels( c1, conf );

  c1->SaveAs( Form("%s/fit_%s.pdf", plotDir.c_str(), histo->GetName()) );

  delete c1;

}
  
