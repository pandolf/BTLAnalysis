#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TPaveText.h"
#include "TGraphErrors.h"

#include "../interface/HSCPCommon.h"


void addPointToGraph( TGraphErrors* graph, float par, float parerr, float x );
TF1* fitGraph( const std::string& outdir, const std::string& name, TGraphErrors* graph, float xMin, float xMax );


int main() {


  HSCPCommon::setStyle();

  TFile* file = TFile::Open( "hscpLite_DY_crab_yesMTD.root" );
  TTree* tree = (TTree*)file->Get("treeLite");


  std::vector<float> etaBins;
  for( float eta=1.55; eta<3.; eta+=0.05 )
    etaBins.push_back( eta );


  //etaBins.push_back(0.);
  //etaBins.push_back(0.1);
  //etaBins.push_back(0.2);
  //etaBins.push_back(0.3);
  //etaBins.push_back(0.4);
  //etaBins.push_back(0.5);
  //etaBins.push_back(0.6);
  //etaBins.push_back(0.7);
  //etaBins.push_back(0.8);
  //etaBins.push_back(0.9);
  //etaBins.push_back(1.0);
  //etaBins.push_back(1.1);
  //etaBins.push_back(1.2);
  //etaBins.push_back(1.3);
  //etaBins.push_back(1.4);

  float xMin = 0.7;
  float xMax = 4.7;

  std::string outdir( "fitsHSCP" );
  system( Form("mkdir -p %s", outdir.c_str()) );

  TGraphErrors* gr_p0 = new TGraphErrors(0);
  gr_p0->SetName("p0");

  TGraphErrors* gr_p1 = new TGraphErrors(0);
  gr_p1->SetName("p1");

  TGraphErrors* gr_p2 = new TGraphErrors(0);
  gr_p2->SetName("p2");


  for( unsigned iEta=0; iEta<etaBins.size()-1; ++iEta ) {

    if( iEta==8 ) continue; // for now

    TProfile* thisProfile = new TProfile( Form("prof_eta%d", iEta), "", 200, xMin, xMax );

    std::string selection(Form("abs(eta)>%f && abs(eta)<%f && pt < %f && pathLength>20.", etaBins[iEta], etaBins[iEta+1], xMax));
    tree->Project( thisProfile->GetName(), "pathLength:pt", selection.c_str(), "prof");

    float xMin_fit = 0.82;
    TF1* thisFunc = new TF1( Form("func_eta%d", iEta), "[0] + [1]/x + [2]/x/x",  xMin_fit, xMax );
    thisFunc->SetParameter( 0, thisProfile->GetBinContent(150) );
    thisFunc->SetLineColor(46);

    thisProfile->Fit( thisFunc, "QR" );

    TCanvas* c1 = new TCanvas( Form("c1_%d", iEta), "", 600, 600 );
    c1->cd();

    TH2D* h2_axes = new TH2D( Form("axes_eta%d", iEta), "", 10, xMin, xMax, 10, 0.9*thisFunc->Eval(0.8*xMax), 1.4*thisFunc->Eval(0.8*xMax) );
    h2_axes->SetXTitle( "Track p_{T} [GeV]" );
    h2_axes->SetYTitle( "Path Length [cm]" );
    h2_axes->Draw();

    thisProfile->SetMarkerStyle(20);
    thisProfile->SetMarkerColor(kBlack);
    thisProfile->Draw("P same");

    TPaveText* etaLabel = new TPaveText( 0.7, 0.8, 0.9, 0.9, "brNDC" );
    etaLabel->SetFillColor(0);
    etaLabel->SetTextSize(0.035);
    etaLabel->AddText( Form("%.1f < |#eta| < %.1f", etaBins[iEta], etaBins[iEta+1]) );
    etaLabel->Draw("same");

    gPad->RedrawAxis();

    c1->SaveAs( Form("%s/fit_eta%d.pdf", outdir.c_str(), iEta) );


    TH1D* h1_eta = new TH1D( Form("eta_eta%d", iEta), "", 200, 0., 2.5 );
    tree->Project( h1_eta->GetName(), "eta", selection.c_str() );

    addPointToGraph( gr_p0, thisFunc->GetParameter(0), thisFunc->GetParError(0), h1_eta->GetMean() );
    addPointToGraph( gr_p1, thisFunc->GetParameter(1), thisFunc->GetParError(1), h1_eta->GetMean() );
    addPointToGraph( gr_p2, thisFunc->GetParameter(2), thisFunc->GetParError(2), h1_eta->GetMean() );

    delete c1;
    delete h2_axes;
    delete h1_eta;
    delete thisProfile;

  } // for eta bins 

  TF1* f1_p0 = fitGraph( outdir, "f1_p0", gr_p0, etaBins[0], etaBins[etaBins.size()-1] );
  TF1* f1_p1 = fitGraph( outdir, "f1_p1", gr_p1, etaBins[0], etaBins[etaBins.size()-1] );
  TF1* f1_p2 = fitGraph( outdir, "f1_p2", gr_p2, etaBins[0], etaBins[etaBins.size()-1] );


  TFile* outfile = TFile::Open( "paramFileHSCP.root", "recreate" );
  outfile->cd();

  f1_p0->Write();
  f1_p1->Write();
  f1_p2->Write();

  outfile->Close();

  return 0;

}



void addPointToGraph( TGraphErrors* graph, float par, float parerr, float x ) {

  int n = graph->GetN();
  
  graph->SetPoint( n, x, par );
  graph->SetPointError( n, 0., parerr );

}


TF1* fitGraph( const std::string& outdir, const std::string& name, TGraphErrors* graph, float xMin, float xMax ) {

  TF1* f1 = new TF1( name.c_str(), "[0] + [1]*x + [2]*x*x", xMin, xMax );
  graph->Fit( f1, "QR" );

  TCanvas* c1 = new TCanvas( Form("c1_%s", name.c_str()), "", 600, 600 );
  c1->cd();

  graph->SetMarkerStyle(20);
  graph->SetMarkerColor(46);
  graph->SetMarkerSize(1.6);
  graph->Draw("APE");

  c1->SaveAs( Form("%s/fit_%s.pdf", outdir.c_str(), f1->GetName()) );

  delete c1;

  return f1;

}
