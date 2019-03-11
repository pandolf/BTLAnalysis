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
TH1D* removeErrorZeroBins( TProfile* thisProfile );
void saveParamToFile( TFile* file, const std::string& outdir, TTree* tree, const std::string& name, const std::vector<float> etaBins );


int main() {


  HSCPCommon::setStyle();

  TFile* file = TFile::Open( "hscpLite_DY_crab_yesMTD.root" );
  TTree* tree = (TTree*)file->Get("treeLite");

  std::string outdir( "fitsHSCP" );
  system( Form("mkdir -p %s", outdir.c_str()) );

  TFile* paramFile = TFile::Open( "paramFileHSCP.root", "recreate" );

  std::vector<float> etaBinsEB;
  for( float eta=0.; eta<1.4; eta+=0.05 )
    etaBinsEB.push_back( eta );

  saveParamToFile( paramFile, outdir, tree, "eb", etaBinsEB );

  std::vector<float> etaBinsEE;
  for( float eta=1.55; eta<2.95; eta+=0.05 )
    etaBinsEE.push_back( eta );

  saveParamToFile( paramFile, outdir, tree, "ee", etaBinsEE );

  paramFile->Close();

  return 0;

}



void saveParamToFile( TFile* file, const std::string& outdir, TTree* tree, const std::string& name, const std::vector<float> etaBins ) {

  bool isEB = (fabs(etaBins[0]) < 1.4);

  float ptMin = 0.7;
  float ptMax = 4.7;

  TGraphErrors* gr_p0 = new TGraphErrors(0);
  gr_p0->SetName(Form("p0_%s", name.c_str()));

  TGraphErrors* gr_p1 = new TGraphErrors(0);
  gr_p1->SetName(Form("p1_%s", name.c_str()));

  TGraphErrors* gr_p2 = new TGraphErrors(0);
  gr_p2->SetName(Form("p2_%s", name.c_str()));


  for( unsigned iEta=0; iEta<etaBins.size()-1; ++iEta ) {

    TProfile* thisProfile = new TProfile( Form("prof_%s_eta%d", name.c_str(), iEta), "", 200, ptMin, ptMax );

    std::string selection(Form("abs(eta)>%f && abs(eta)<%f && pt < %f && pathLength>20.", etaBins[iEta], etaBins[iEta+1], ptMax));
    tree->Project( thisProfile->GetName(), "pathLength:pt", selection.c_str(), "prof");

    TH1D* cleanProfile = removeErrorZeroBins( thisProfile );

    float ptMin_fit = (isEB) ? 0.82 : 1.;
    TF1* thisFunc = new TF1( Form("func_%s_eta%d", name.c_str(), iEta), "[0] + [1]/x + [2]/x/x",  ptMin_fit, ptMax );
    thisFunc->SetParameter( 0, cleanProfile->GetBinContent(150) );
    thisFunc->SetLineColor(46);

    cleanProfile->Fit( thisFunc, "QR" );

    TCanvas* c1 = new TCanvas( Form("c1_%s_%d", name.c_str(), iEta), "", 600, 600 );
    c1->cd();

    float scaleYmax = (isEB) ? 1.4 : 1.05;
    TH2D* h2_axes = new TH2D( Form("axes_%s_eta%d", name.c_str(), iEta), "", 10, ptMin, ptMax, 10, 0.90*thisFunc->Eval(0.8*ptMax), scaleYmax*thisFunc->Eval(0.8*ptMax) );
    h2_axes->SetXTitle( "Track p_{T} [GeV]" );
    h2_axes->SetYTitle( "Path Length [cm]" );
    h2_axes->Draw();

    cleanProfile->SetMarkerStyle(20);
    cleanProfile->SetMarkerColor(kBlack);
    cleanProfile->Draw("P same");

    TPaveText* etaLabel = new TPaveText( 0.7, 0.8, 0.9, 0.9, "brNDC" );
    etaLabel->SetFillColor(0);
    etaLabel->SetTextSize(0.035);
    etaLabel->AddText( Form("%.2f < |#eta| < %.2f", etaBins[iEta], etaBins[iEta+1]) );
    etaLabel->Draw("same");

    gPad->RedrawAxis();

    c1->SaveAs( Form("%s/fit_%s_eta%d.pdf", outdir.c_str(), name.c_str(), iEta) );


    TH1D* h1_eta = new TH1D( Form("eta_%s_eta%d", name.c_str(), iEta), "", 200, 0., 3. );
    tree->Project( h1_eta->GetName(), "abs(eta)", selection.c_str() );

    addPointToGraph( gr_p0, thisFunc->GetParameter(0), thisFunc->GetParError(0), h1_eta->GetMean() );
    addPointToGraph( gr_p1, thisFunc->GetParameter(1), thisFunc->GetParError(1), h1_eta->GetMean() );
    addPointToGraph( gr_p2, thisFunc->GetParameter(2), thisFunc->GetParError(2), h1_eta->GetMean() );

    delete c1;
    delete h2_axes;
    delete h1_eta;
    delete thisProfile;
    delete cleanProfile;

  } // for eta bins 

  TF1* f1_p0 = fitGraph( outdir, Form("f1_%s_p0", name.c_str()), gr_p0, etaBins[0], etaBins[etaBins.size()-1] );
  TF1* f1_p1 = fitGraph( outdir, Form("f1_%s_p1", name.c_str()), gr_p1, etaBins[0], etaBins[etaBins.size()-1] );
  TF1* f1_p2 = fitGraph( outdir, Form("f1_%s_p2", name.c_str()), gr_p2, etaBins[0], etaBins[etaBins.size()-1] );

  file->cd();

  f1_p0->Write();
  f1_p1->Write();
  f1_p2->Write();

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


TH1D* removeErrorZeroBins( TProfile* thisProfile ) {

  TH1D* newProfile = new TH1D( Form("nozero_%s", thisProfile->GetName()), "", thisProfile->GetNbinsX(), thisProfile->GetXaxis()->GetXmin(), thisProfile->GetXaxis()->GetXmax() );

  for( unsigned i=1; i<thisProfile->GetNbinsX()+1; i++ ) {

    if( i>1 && (thisProfile->GetBinError(i) > 0.1*thisProfile->GetBinError(i-1)) ) {  // for stability

      newProfile->SetBinContent( i, thisProfile->GetBinContent(i) );
      newProfile->SetBinError  ( i, thisProfile->GetBinError(i)   );

    } // if

  } // for bins

  return newProfile;

}


