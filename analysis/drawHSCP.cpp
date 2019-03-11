#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TLine.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TPaveText.h"
#include "TLorentzVector.h"

#include "../interface/HSCPCommon.h"



float getSigmaMass( float sigma_t, float mass, float pt, float eta, TF1* f1_p0, TF1* f1_p1, TF1* f1_p2 );
float findPtMax( float mX, float sigma_t, float eta, TF1* f1_p0, TF1* f1_p1, TF1* f1_p2 );


int main( int argc, char* argv[] ) {


  HSCPCommon::setStyle();

  TFile* paramFile = TFile::Open( "paramFileHSCP.root" );

  TF1* f1_p0 = (TF1*)paramFile->Get("f1_p0");
  TF1* f1_p1 = (TF1*)paramFile->Get("f1_p1");
  TF1* f1_p2 = (TF1*)paramFile->Get("f1_p2");

  std::vector<float> etaValues;
  for( float eta=0.; eta<1.4; eta += 0.04 )
    etaValues.push_back(eta);





  std::vector<TGraph*> graphs;

  TGraph* gr_ptMax_vs_eta_30ps = new TGraph(0);
  TGraph* gr_ptMax_vs_eta_50ps = new TGraph(0);
  TGraph* gr_ptMax_vs_eta_70ps = new TGraph(0);

  gr_ptMax_vs_eta_30ps->SetName("gr_ptMax_vs_eta_30ps");
  gr_ptMax_vs_eta_50ps->SetName("gr_ptMax_vs_eta_50ps");
  gr_ptMax_vs_eta_70ps->SetName("gr_ptMax_vs_eta_70ps");

  // first point on x axis (for fill):
  gr_ptMax_vs_eta_30ps->SetPoint( 0, 0., 0. );
  gr_ptMax_vs_eta_50ps->SetPoint( 0, 0., 0. );
  gr_ptMax_vs_eta_70ps->SetPoint( 0, 0., 0. );


  float mass = 0.983;
  float mX = 500.;
  //float mass = 0.139;

  for( unsigned iEta=0; iEta<etaValues.size(); ++iEta ) {

    TGraph* gr_sigmam_vs_pt = new TGraph(0);
    gr_sigmam_vs_pt->SetName( Form("gr_sigmam_vs_pt_eta%d", iEta) );

    for( float pt=1.; pt<50.; pt+=0.5 )
      gr_sigmam_vs_pt->SetPoint( gr_sigmam_vs_pt->GetN(), pt, mass + 3.*getSigmaMass( 0.030, mass, pt, etaValues[iEta], f1_p0, f1_p1, f1_p2 ) );

    graphs.push_back( gr_sigmam_vs_pt );


    gr_ptMax_vs_eta_30ps->SetPoint( gr_ptMax_vs_eta_30ps->GetN(), etaValues[iEta], findPtMax( mX, 0.030, etaValues[iEta], f1_p0, f1_p1, f1_p2 ) );
    gr_ptMax_vs_eta_50ps->SetPoint( gr_ptMax_vs_eta_50ps->GetN(), etaValues[iEta], findPtMax( mX, 0.050, etaValues[iEta], f1_p0, f1_p1, f1_p2 ) );
    gr_ptMax_vs_eta_70ps->SetPoint( gr_ptMax_vs_eta_70ps->GetN(), etaValues[iEta], findPtMax( mX, 0.070, etaValues[iEta], f1_p0, f1_p1, f1_p2 ) );

  } // for ieta

  // add last point on x axis so that fill drawing works:
  gr_ptMax_vs_eta_30ps->SetPoint( gr_ptMax_vs_eta_30ps->GetN(), etaValues[etaValues.size()-1], 0. );
  gr_ptMax_vs_eta_50ps->SetPoint( gr_ptMax_vs_eta_50ps->GetN(), etaValues[etaValues.size()-1], 0. );
  gr_ptMax_vs_eta_70ps->SetPoint( gr_ptMax_vs_eta_70ps->GetN(), etaValues[etaValues.size()-1], 0. );


  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  TH2D* h2_axes = new TH2D( "axes", "", 10, 0., 50., 10, 0., 60. );
  h2_axes->SetXTitle( "Track p_{T} [GeV]" );
  h2_axes->SetYTitle( Form("#sigma(M) for M = %.0f GeV [GeV]", mass) );
  h2_axes->Draw();

  std::vector<int> colors = HSCPCommon::colors();

  TLegend* legend = new TLegend( 0.2, 0.6, 0.55, 0.9 );
  legend->SetFillColor(0);
  legend->SetTextSize(0.035);

  for( unsigned i=0; i<graphs.size(); ++i ) {

    graphs[i]->SetMarkerStyle(20);
    graphs[i]->SetMarkerSize(1.3);
    graphs[i]->SetMarkerColor(colors[i]);
    graphs[i]->Draw("P same");

    legend->AddEntry( graphs[i], Form("|#eta| = %.1f", etaValues[i]), "P" );

  }

  legend->Draw("same");

  gPad->RedrawAxis();

  c1->SaveAs("hscp_sigmam_vs_pt.pdf");

  delete h2_axes;


  c1->Clear();

  c1->cd();


  TH2D* h2_axes2 = new TH2D("axes2", "", 10, 0., 1.5, 10, 0., 190.);
  h2_axes2->SetXTitle( "|#eta|" );
  h2_axes2->SetYTitle( "Maximum p_{T} [GeV]" );
  h2_axes2->Draw();

  
  gr_ptMax_vs_eta_30ps->SetMarkerStyle(20);
  gr_ptMax_vs_eta_30ps->SetMarkerSize(1.5);
  gr_ptMax_vs_eta_30ps->SetMarkerColor(colors[0]);
  gr_ptMax_vs_eta_30ps->SetFillColor(colors[0]);
  
  gr_ptMax_vs_eta_50ps->SetMarkerStyle(20);
  gr_ptMax_vs_eta_50ps->SetMarkerSize(1.5);
  gr_ptMax_vs_eta_50ps->SetMarkerColor(colors[1]);
  gr_ptMax_vs_eta_50ps->SetFillColor(colors[1]);

  gr_ptMax_vs_eta_70ps->SetMarkerStyle(20);
  gr_ptMax_vs_eta_70ps->SetMarkerSize(1.5);
  gr_ptMax_vs_eta_70ps->SetMarkerColor(colors[2]);
  gr_ptMax_vs_eta_70ps->SetFillColor(colors[2]);

  //TLegend* legend2 = new TLegend( 0.2, 0.2, 0.5, 0.35 );
  //legend2->SetTextSize(0.035);
  //legend2->SetFillColor(0);
  //legend2->AddEntry( gr_ptMax_vs_eta_30ps, "#sigma_{t} = 30 ps", "P" );
  //legend2->AddEntry( gr_ptMax_vs_eta_50ps, "#sigma_{t} = 50 ps", "P" );
  //legend2->AddEntry( gr_ptMax_vs_eta_70ps, "#sigma_{t} = 70 ps", "P" );
  //legend2->Draw("same");

  gr_ptMax_vs_eta_30ps->Draw("F same" );
  gr_ptMax_vs_eta_50ps->Draw("F same" );
  gr_ptMax_vs_eta_70ps->Draw("F same" );

  TPaveText* label_70ps = new TPaveText( 0.05, 80., 0.35, 90. );
  label_70ps->SetFillColor(colors[2]);
  label_70ps->SetTextSize(0.035);
  label_70ps->AddText( "#sigma_{t} = 70 ps" );
  label_70ps->Draw("same");

  TPaveText* label_50ps = new TPaveText( 0.05, 100., 0.35, 110. );
  label_50ps->SetFillColor(colors[1]);
  label_50ps->SetTextSize(0.035);
  label_50ps->AddText( "#sigma_{t} = 50 ps" );
  label_50ps->Draw("same");

  TPaveText* label_30ps = new TPaveText( 0.05, 135., 0.35, 140. );
  label_30ps->SetFillColor(colors[0]);
  label_30ps->SetTextSize(0.035);
  label_30ps->AddText( "#sigma_{t} = 30 ps" );
  label_30ps->Draw("same");


  TPaveText* label_hscp = new TPaveText( 0.6, 0.8, 0.9, 0.9, "brNDC" );
  label_hscp->SetFillColor(0);
  label_hscp->SetTextSize(0.04);
  label_hscp->AddText( "HSCP M = 500 GeV");
  label_hscp->Draw("same");

  HSCPCommon::addLabels(c1);

  c1->SaveAs( "ptMax_vs_eta.pdf" );

  delete c1;


  return 0;

}


float getSigmaMass( float sigma_t, float mass, float pt, float eta, TF1* f1_p0, TF1* f1_p1, TF1* f1_p2 ) {

  TLorentzVector* p4 = new TLorentzVector();
  p4->SetPtEtaPhiM( pt, eta, 0., mass );

  TF1* f1_pathLength = new TF1( "pathLength", "[0] + [1]/x + [2]/x/x", 0.7, 100. ); // path length vs pt
  f1_pathLength->SetParameter( 0, f1_p0->Eval(eta) );
  f1_pathLength->SetParameter( 1, f1_p1->Eval(eta) );
  f1_pathLength->SetParameter( 2, f1_p2->Eval(eta) );

  float pathLength = f1_pathLength->Eval( pt );

  float beta = p4->P()/p4->Energy();

  float sigma_invBeta = sigma_t * 30./pathLength;
  float sigma_m = p4->P()*sigma_invBeta/( beta*sqrt( 1./(beta*beta) - 1. ) );

  return sigma_m;

}



float findPtMax( float mX, float sigma_t, float eta, TF1* f1_p0, TF1* f1_p1, TF1* f1_p2 ) {

  float ptMax = -1.;

  for( float pt=1.; pt<1000.; pt += 0.5 ) {

    float ul = 0.938 + 3.*getSigmaMass( sigma_t, 0.983, pt, eta, f1_p0, f1_p1, f1_p2 );

    if( ul >= mX ) {
      ptMax = pt;
      break;
    }

  } // for pt;

  return ptMax;

}
