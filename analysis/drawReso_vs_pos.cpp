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



void draw_vs_pos( BTLConf conf, const std::string& awType, TTree* tree, const std::string& yVar, const std::string& suffixVar, const std::string& posvar, const std::string& axisName, int nBins ); //, TF1* f1_tLeft=0, TF1* f1_tRight=0 );
void addPointToGraph( TGraphErrors* gr, const std::string& yVar, float x, float xerr, TH1D* histo );
void addDelayToGraph( TGraphErrors* graph, float tDelay );
TF1* fitLine( TGraphErrors* graph, float varMin, float varMax );
void drawHisto( BTLConf conf, const std::string& awType, TH1D* histo, const std::string& posCut );


int main( int argc, char* argv[] ) {


  if( argc==1 ) {

    std::cout << "USAGE: ./drawReso_vs_pos [confName]" << std::endl;
    exit(1);

  }

  BTLCommon::setStyle();

  std::string confName( argv[1] );
  std::string awType = "aw1bins";

  if( argc>2 ) {

    int nBinsHodo = 1;
    bool centralAmpWalk = false;
    std::string argv2(argv[2]);
    if( argv2=="Center" || argv2=="center" || argv2=="central" || argv2=="Central" ) {
      nBinsHodo = 1;
      centralAmpWalk = true;
    } else {
      nBinsHodo = atoi(argv[2]);
    }

    if( centralAmpWalk ) {

      awType = "awCentral";
 
    } else {

      awType = std::string(Form("aw%dbins", nBinsHodo));

    }

  }


  BTLConf conf(confName);

  TFile* file = TFile::Open( Form("treesLite/%s_%s.root", confName.c_str(), awType.c_str()) );
  TTree* tree = (TTree*)file->Get( "treeLite" );

  system( Form("mkdir -p plots/%s/reso_vs_pos_%s", conf.get_confName().c_str(), awType.c_str()) );

  // vs X:
  draw_vs_pos( conf, awType, tree, "mean" , ""     , "x_hodo_corr", "Hodoscope X [mm]", 30 );
  draw_vs_pos( conf, awType, tree, "mean" , "_corr", "x_hodo_corr", "Hodoscope X [mm]", 30 );

  draw_vs_pos( conf, awType, tree, "sigma", ""     , "x_hodo_corr", "Hodoscope X [mm]", 20 );
  draw_vs_pos( conf, awType, tree, "sigma", "_corr", "x_hodo_corr", "Hodoscope X [mm]", 20 );

  // vs Y:
  draw_vs_pos( conf, awType, tree, "mean" , ""     , "y_hodo_corr", "Hodoscope Y [mm]", 30 );
  draw_vs_pos( conf, awType, tree, "mean" , "_corr", "y_hodo_corr", "Hodoscope Y [mm]", 30 );

  draw_vs_pos( conf, awType, tree, "sigma", ""     , "y_hodo_corr", "Hodoscope Y [mm]", 20 );
  draw_vs_pos( conf, awType, tree, "sigma", "_corr", "y_hodo_corr", "Hodoscope Y [mm]", 20 );


  return 0;

}



void draw_vs_pos( BTLConf conf, const std::string& awType, TTree* tree, const std::string& yVar, const std::string& suffixVar, const std::string& posvar, const std::string& axisName, int nBins ) { //, TF1* f1_tLeft, TF1* f1_tRight ) {


  bool isX = posvar=="x_hodo_corr";

  BTLCrystal crys = conf.crystal();

  float xEdgeLow  = crys.xLow()   - 5.;
  float xEdgeHigh = crys.xHigh()  + 5.;
  float yEdgeLow  = crys.yLow()   - 0.5;
  float yEdgeHigh = crys.yHigh()  + 0.5;

  float varMin = (isX) ? xEdgeLow  : yEdgeLow ;
  float varMax = (isX) ? xEdgeHigh : yEdgeHigh;

  std::string otherVar = (isX) ? "y_hodo_corr" : "x_hodo_corr";
  float otherVarMin    = (isX) ? crys.yLowFiducial()  : crys.xLowFiducial ();
  float otherVarMax    = (isX) ? crys.yHighFiducial() : crys.xHighFiducial();


  //std::string suffix = (f1_tLeft==0 && f1_tRight==0) ? "" : "_corr";
  std::string suffix = ""; // should maybe remove this if i dont intend to correct

  float xMinT = (conf.digiChSet()=="a") ? 2.1001 : 3.6001;
  float xMaxT = (conf.digiChSet()=="a") ? 3.299 : 4.99;
  int  nBinsT = (int)(( xMaxT-xMinT )/0.0025);

  float binWidth_hodo = (varMax-varMin)/((float)nBins);


  //std::vector< TH1D* > vh1_tAve_vs_pos;
  //std::vector< TH1D* > vh1_tLeft_vs_pos;
  //std::vector< TH1D* > vh1_tRight_vs_pos;

  TGraphErrors* gr_tAve   = new TGraphErrors(0);
  TGraphErrors* gr_tLeft  = new TGraphErrors(0);
  TGraphErrors* gr_tRight = new TGraphErrors(0);
  TGraphErrors* gr_tDiff  = new TGraphErrors(0);

  gr_tAve  ->SetName( Form("tAve_%s%s_vs_%s%s"  , yVar.c_str(), suffixVar.c_str(), posvar.c_str(), suffix.c_str()) );
  gr_tLeft ->SetName( Form("tLeft_%s%s_vs_%s%s" , yVar.c_str(), suffixVar.c_str(), posvar.c_str(), suffix.c_str()) );
  gr_tRight->SetName( Form("tRight_%s%s_vs_%s%s", yVar.c_str(), suffixVar.c_str(), posvar.c_str(), suffix.c_str()) );
  gr_tDiff ->SetName( Form("tDiff_%s%s_vs_%s%s" , yVar.c_str(), suffixVar.c_str(), posvar.c_str(), suffix.c_str()) );

  gr_tAve  ->SetMarkerStyle(20);
  gr_tLeft ->SetMarkerStyle(20);
  gr_tRight->SetMarkerStyle(20);
  gr_tDiff ->SetMarkerStyle(20);

  gr_tAve  ->SetMarkerSize(1.1);
  gr_tLeft ->SetMarkerSize(1.1);
  gr_tRight->SetMarkerSize(1.1);
  gr_tDiff ->SetMarkerSize(1.1);

  gr_tAve  ->SetMarkerColor(kBlack);
  gr_tLeft ->SetMarkerColor(38);
  gr_tRight->SetMarkerColor(46);
  gr_tDiff ->SetMarkerColor(kBlack);

  gr_tAve  ->SetLineColor(kBlack);
  gr_tLeft ->SetLineColor(38);
  gr_tRight->SetLineColor(46);
  gr_tDiff ->SetLineColor(kBlack);


  std::string plotDir( Form( "plots/%s/reso_vs_pos_%s/individualFits/", conf.get_confName().c_str(), awType.c_str()) );
  system( Form("mkdir -p %s", plotDir.c_str()) );


  for( int i=0; i<nBins; ++i ) {

    TH1D* h1_tAve   = new TH1D( Form( "tAve_%s%s_vs_%s_%d%s"  , yVar.c_str(), suffixVar.c_str(), posvar.c_str(), i, suffix.c_str() ), "", nBinsT, xMinT, xMaxT );
    TH1D* h1_tLeft  = new TH1D( Form( "tLeft_%s%s_vs_%s_%d%s" , yVar.c_str(), suffixVar.c_str(), posvar.c_str(), i, suffix.c_str() ), "", nBinsT, xMinT, xMaxT );
    TH1D* h1_tRight = new TH1D( Form( "tRight_%s%s_vs_%s_%d%s", yVar.c_str(), suffixVar.c_str(), posvar.c_str(), i, suffix.c_str() ), "", nBinsT, xMinT, xMaxT );
    TH1D* h1_tDiff  = new TH1D( Form( "tDiff_%s%s_vs_%s_%d%s" , yVar.c_str(), suffixVar.c_str(), posvar.c_str(), i, suffix.c_str() ), "", nBinsT, -1., +1. );

    h1_tAve   ->SetXTitle( "t(ave) - t(MCP) [ns]" );
    h1_tLeft  ->SetXTitle( "t(Left) - t(MCP) [ns]" );
    h1_tRight ->SetXTitle( "t(Right) - t(MCP) [ns]" );
    h1_tDiff  ->SetXTitle( "t(Right) - t(Left) [ns]" );

    float varMin_cut = varMin + (i  )*binWidth_hodo;
    float varMax_cut = varMin + (i+1)*binWidth_hodo;

    float x = 0.5*(varMin_cut+varMax_cut);
    float xerr = (varMax_cut-varMin_cut)/sqrt(12);

    std::string hodoCut( Form("%s>=%f && %s<%f && %s>=%f && %s<%f", posvar.c_str(), varMin_cut, posvar.c_str(), varMax_cut, otherVar.c_str(), otherVarMin, otherVar.c_str(), otherVarMax) );
    //std::cout << hodoCut << std::endl;
    //float corrLeft  = (f1_tLeft ) ? ( f1_tLeft ->Eval( 0.5*(varMin+varMax) ) / f1_tLeft ->Eval(x) ) : 1.;
    //float corrRight = (f1_tRight) ? ( f1_tRight->Eval( 0.5*(varMin+varMax) ) / f1_tRight->Eval(x) ) : 1.;

    //std::string corrLeft  = (f1_tLeft ) ? "
    //std::string corrRight = (f1_tRight) ? ( f1_tRight->Eval( 0.5*(varMin+varMax) ) / f1_tRight->Eval(x) ) : 1.;


    tree->Project( h1_tAve  ->GetName(), Form("0.5*(tLeft%s+tRight%s)", suffixVar.c_str(), suffixVar.c_str()), hodoCut.c_str() );

    // inverted!!!! this is because of a wrong definition in the ntuples. will need to fix at ntuple level SOON!
    tree->Project( h1_tLeft ->GetName(), Form("tRight%s" , suffixVar.c_str()), hodoCut.c_str() );
    tree->Project( h1_tRight->GetName(), Form("tLeft%s", suffixVar.c_str()), hodoCut.c_str() );
    //tree->Project( h1_tLeft ->GetName(), Form("tLeft%s" , suffixVar.c_str()), hodoCut.c_str() );
    //tree->Project( h1_tRight->GetName(), Form("tRight%s", suffixVar.c_str()), hodoCut.c_str() );

    tree->Project( h1_tDiff->GetName(), Form("tRight%s-tLeft%s", suffixVar.c_str(), suffixVar.c_str()), hodoCut.c_str() );
   
    BTLCommon::fitGaus( h1_tAve  , 2.1 );
    BTLCommon::fitGaus( h1_tLeft , 2.1 );
    BTLCommon::fitGaus( h1_tRight, 2.1 );
    BTLCommon::fitGaus( h1_tDiff , 2.1 );

    //if( h1_tAve->GetEntries()<20 ) continue;

    drawHisto( conf, awType, h1_tAve  , Form("%.2f < %s < %.2f mm", varMin_cut, posvar.c_str(), varMax_cut) );
    drawHisto( conf, awType, h1_tRight, Form("%.2f < %s < %.2f mm", varMin_cut, posvar.c_str(), varMax_cut) );
    drawHisto( conf, awType, h1_tLeft , Form("%.2f < %s < %.2f mm", varMin_cut, posvar.c_str(), varMax_cut) );
    drawHisto( conf, awType, h1_tDiff , Form("%.2f < %s < %.2f mm", varMin_cut, posvar.c_str(), varMax_cut) );

    
    addPointToGraph( gr_tAve  , yVar, x, xerr, h1_tAve   );
    addPointToGraph( gr_tLeft , yVar, x, xerr, h1_tLeft  );
    addPointToGraph( gr_tRight, yVar, x, xerr, h1_tRight );

    addPointToGraph( gr_tDiff , yVar, x, xerr, h1_tDiff );

    //vh1_tAve_vs_pos  .push_back( h1_tAve   );
    //vh1_tLeft_vs_pos .push_back( h1_tLeft  );
    //vh1_tRight_vs_pos.push_back( h1_tRight );
  
  } // for bins


  float padding = (posvar=="x_hodo_corr") ? 5. : 1.;
  float xMin = varMin-padding;
  float xMax = varMax+padding;


  if( yVar=="mean" ) {

    // want to add delay to tLeft so that tLeft=tRight for the center of the bar
    // step one is to fit tDiff and find delay at x=0:

    TF1* f1_lineDiff_tmp = new TF1( Form("line_tmp_%s", gr_tDiff->GetName()), "[0]+[1]*x", xMin, xMax );
    f1_lineDiff_tmp->SetLineWidth(2);
    f1_lineDiff_tmp->SetLineColor(46);
    f1_lineDiff_tmp->SetParameter(0, -0.5 );
    f1_lineDiff_tmp->SetParameter(1, 0.02 );
    gr_tDiff->Fit( f1_lineDiff_tmp, "RQ0" );

    float crysCenter = 0.5*(crys.xLow()+crys.xHigh());
    float tDelay = f1_lineDiff_tmp->Eval(crysCenter);

    // step two is to correct tLeft for the delay:
    addDelayToGraph( gr_tLeft, -tDelay );
    addDelayToGraph( gr_tDiff, -tDelay );
    addDelayToGraph( gr_tAve , -0.5*tDelay );

  } // if mean
 

  TCanvas* c1 = new TCanvas( Form("c1_%s", gr_tAve->GetName()), "", 600, 600 );
  c1->cd();

  // first mean

  float yMin         = ( yVar=="mean" ) ? xMinT : 0.;
  float yMax         = ( yVar=="mean" ) ? xMaxT : 90.;
  std::string yTitle = ( yVar=="mean" ) ? "t(i) - t(MCP) [ns]" : "Timing Resolution [ps]";

  TH2D* h2_axes = new TH2D( Form("axes_%s", gr_tAve->GetName()), "", 10, xMin, xMax, 10, yMin, yMax );
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

  //float barMin = (isX) ? crys.xLow()  : crys.yLow() ;
  //float barMax = (isX) ? crys.xHigh() : crys.yHigh();

  //TLine* line_barMin = new TLine( barMin, yMin, barMin, yMax );
  //line_barMin->SetLineColor( kGray+1 );
  //line_barMin->SetLineWidth( 3 );
  //line_barMin->SetLineStyle( 2 );
  //line_barMin->Draw("same");

  //TLine* line_barMax = new TLine( barMax, yMin, barMax, yMax );
  //line_barMax->SetLineColor( kGray+1 );
  //line_barMax->SetLineWidth( 3 );
  //line_barMax->SetLineStyle( 2 );
  //line_barMax->Draw("same");

  TLegend* legend = new TLegend( 0.75, 0.72, 0.9, 0.9 );
  legend->SetTextSize(0.035);
  //legend->SetFillColor(0);
  legend->AddEntry( gr_tLeft , "t(Left)" , "P" );
  legend->AddEntry( gr_tRight, "t(Right)", "P" );
  legend->AddEntry( gr_tAve  , "t(Ave)"  , "P" );
  legend->Draw("same");

  TPaveText* label_conf = conf.get_labelConf(2);
  label_conf->Draw("same");
   
  BTLCommon::addLabels( c1, conf );

  c1->SaveAs( Form("plots/%s/reso_vs_pos_%s/t%s%s_vs_%s%s.pdf", conf.get_confName().c_str(), awType.c_str(), yVar.c_str(), suffixVar.c_str(), posvar.c_str(), suffix.c_str()) );


  c1->Clear();

  c1->cd();

  TH2D* h2_axes2 = new TH2D( Form("axes2_%s", gr_tDiff->GetName()), "", 10, xMin, xMax, 10, -0.7, 0.7 );
  //h2_axes2->SetYTitle( "t(Right) - t(Left) [ns]" );
  h2_axes2->SetYTitle( "t(Left) - t(Right) [ns]" );
  h2_axes2->SetXTitle( axisName.c_str() );
  h2_axes2->Draw();

  TF1* f1_lineDiff = new TF1( Form("line_%s", gr_tDiff->GetName()), "[0]+[1]*x", xMin, xMax );
  f1_lineDiff->SetLineWidth(2);
  f1_lineDiff->SetLineColor(46);
  f1_lineDiff->SetParameter(0, -0.5 );
  f1_lineDiff->SetParameter(1, 0.02 );
  gr_tDiff->Fit( f1_lineDiff, "RQ0" );

  f1_lineDiff->Draw("L same");
  gr_tDiff->Draw("P same");

  TPaveText* label_slope = new TPaveText( 0.6, 0.2, 0.9, 0.25, "brNDC" );
  label_slope->SetTextSize(0.035);
  label_slope->SetFillColor(0);
  label_slope->SetTextColor(f1_lineDiff->GetLineColor() );
  label_slope->AddText( Form( "Slope = %.1f ps/mm", f1_lineDiff->GetParameter(1)*1000. ) );
  label_slope->Draw("same");

  label_conf->Draw("same");
  BTLCommon::addLabels( c1, conf );

  c1->SaveAs( Form("plots/%s/reso_vs_pos_%s/tDiff%s%s_vs_%s%s.pdf", conf.get_confName().c_str(), awType.c_str(), yVar.c_str(), suffixVar.c_str(), posvar.c_str(), suffix.c_str()) );

  delete c1;
  delete h2_axes;



  //return lines;

}


void addPointToGraph( TGraphErrors* gr, const std::string& yVar, float x, float xerr, TH1D* histo ) {

  TF1* f1_gaus = histo->GetFunction( Form("gaus_%s", histo->GetName()) );

  bool useFunc = ( f1_gaus!=0 && f1_gaus->GetParError(1)>0. && histo->GetEntries()>10 && (f1_gaus->GetParError(1)/f1_gaus->GetParameter(1)) < 0.2 );

  int nPoint = gr->GetN();

  if( yVar=="mean" ) {

    if( useFunc ) {
      gr->SetPoint     ( nPoint,  x   , f1_gaus->GetParameter(1) );
      gr->SetPointError( nPoint,  xerr, f1_gaus->GetParError (1) );
    } else {
      if( histo->GetEntries()>1 ) {
        gr->SetPoint     ( nPoint,  x   , histo->GetMean() );
        gr->SetPointError( nPoint,  xerr, histo->GetMeanError() );
      }
    }

  } else if( yVar=="sigma" ) {

    if( useFunc ) {
      gr->SetPoint     ( nPoint,  x, BTLCommon::subtractResoPTK(f1_gaus->GetParameter(2))*1000. );
      gr->SetPointError( nPoint,  xerr, f1_gaus->GetParError(2)*1000. );
    } else { // prefer not to do anything here
      gr->SetPoint     ( nPoint,  x, BTLCommon::subtractResoPTK(f1_gaus->GetParameter(2))*1000. );
      gr->SetPointError( nPoint,  xerr, f1_gaus->GetParError(2)*1000. );
    }

  } else {

    std::cout << "yVar can be only 'mean' or 'sigma'!!" << std::endl;

  }

}


void addDelayToGraph( TGraphErrors* graph, float tDelay ) {

  for( unsigned iPoint=0; iPoint<graph->GetN(); ++iPoint ) {

    double x, old_y;
    graph->GetPoint( iPoint, x, old_y );
    graph->SetPoint( iPoint, x, old_y+tDelay );

  } // for points

}


TF1* fitLine( TGraphErrors* graph, float varMin, float varMax ) {

  TF1* f1 = new TF1( Form("line_%s", graph->GetName()), "[0] + [1]*x", varMin, varMax );
  f1->SetLineColor( graph->GetLineColor() );
  f1->SetParameter(0, 3.);
  f1->SetParameter(1, 0.);

  graph->Fit( f1->GetName(), "RQ+" );

  return f1;

}


void drawHisto( BTLConf conf, const std::string& awType, TH1D* histo, const std::string& posCut ) {

  std::string plotDir(Form("plots/%s/reso_vs_pos_%s/individualFits", conf.get_confName().c_str(), awType.c_str()));

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
  
