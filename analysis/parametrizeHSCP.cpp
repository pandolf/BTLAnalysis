#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TPaveText.h"

#include "../interface/HSCPCommon.h"




int main() {


  HSCPCommon::setStyle();

  TFile* file = TFile::Open( "hscpLite_DY_crab_yesMTD.root" );
  TTree* tree = (TTree*)file->Get("treeLite");


  std::vector<float> etaBins;
  etaBins.push_back(0.);
  etaBins.push_back(0.1);
  etaBins.push_back(0.2);
  etaBins.push_back(0.3);
  etaBins.push_back(0.4);
  etaBins.push_back(0.5);
  etaBins.push_back(0.6);
  etaBins.push_back(0.7);
  etaBins.push_back(0.8);
  etaBins.push_back(0.9);
  etaBins.push_back(1.0);
  etaBins.push_back(1.1);
  etaBins.push_back(1.2);
  etaBins.push_back(1.3);
  etaBins.push_back(1.4);

  float xMin = 0.7;
  float xMax = 4.7;

  std::string outdir( "fitsHSCP" );
  system( Form("mkdir -p %s", outdir.c_str()) );


  for( unsigned iEta=0; iEta<etaBins.size()-1; ++iEta ) {

    TProfile* thisProfile = new TProfile( Form("prof_eta%d", iEta), "", 200, xMin, xMax );

    tree->Project( thisProfile->GetName(), "pathLength:pt", Form("abs(eta)>%f && abs(eta)<%f && pt < %f && pathLength>20.", etaBins[iEta], etaBins[iEta+1], xMax), "prof");

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

  } // for eta bins 



  return 0;

}
