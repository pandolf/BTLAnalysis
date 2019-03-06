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



float getSigmaMass( float mass, float pt, float eta, TF1* f1_p0, TF1* f1_p1, TF1* f1_p2 );


int main( int argc, char* argv[] ) {


  HSCPCommon::setStyle();

  TFile* paramFile = TFile::Open( "paramFileHSCP.root" );

  TF1* f1_p0 = (TF1*)paramFile->Get("f1_p0");
  TF1* f1_p1 = (TF1*)paramFile->Get("f1_p1");
  TF1* f1_p2 = (TF1*)paramFile->Get("f1_p2");

  std::vector<float> etaValues;
  etaValues.push_back(0.);
  etaValues.push_back(0.3);
  etaValues.push_back(0.6);
  etaValues.push_back(1.0);
  etaValues.push_back(1.3);


  std::vector<TGraph*> graphs;

  //float mass = 0.983;
  float mass = 100.;

  for( unsigned iEta=0; iEta<etaValues.size(); ++iEta ) {

    TGraph* gr_sigmam_vs_pt = new TGraph(0);
    gr_sigmam_vs_pt->SetName( Form("gr_sigmam_vs_pt_eta%d", iEta) );

    for( float pt=1.; pt<50.; pt+=0.5 )
      gr_sigmam_vs_pt->SetPoint( gr_sigmam_vs_pt->GetN(), pt, getSigmaMass( mass, pt, etaValues[iEta], f1_p0, f1_p1, f1_p2 ) );

    graphs.push_back( gr_sigmam_vs_pt );

  } // for ieta

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
    graphs[i]->Draw("p same");

    legend->AddEntry( graphs[i], Form("|#eta| = %.1f", etaValues[i]), "P" );

  }

  legend->Draw("same");

  gPad->RedrawAxis();

  c1->SaveAs("hscp_sigmam_vs_pt.pdf");

  delete c1;
  delete h2_axes;


TFile* outfile = TFile::Open("prova.root", "recreate");
outfile->cd();
  for( unsigned i=0; i<graphs.size(); ++i ) 
    graphs[i]->Write();
outfile->Close();

  return 0;

}


float getSigmaMass( float mass, float pt, float eta, TF1* f1_p0, TF1* f1_p1, TF1* f1_p2 ) {

  TLorentzVector* p4 = new TLorentzVector();
  p4->SetPtEtaPhiM( pt, eta, 0., mass );

  TF1* f1_pathLength = new TF1( "pathLength", "[0] + [1]/x + [2]/x/x", 0.7, 100. ); // path length vs pt
  f1_pathLength->SetParameter( 0, f1_p0->Eval(eta) );
  f1_pathLength->SetParameter( 1, f1_p1->Eval(eta) );
  f1_pathLength->SetParameter( 2, f1_p2->Eval(eta) );

  float pathLength = f1_pathLength->Eval( pt );

  float beta = p4->P()/p4->Energy();

  float sigma_invBeta = 0.035 * 30./pathLength;
  float sigma_m = p4->P()*sigma_invBeta/( beta*sqrt( 1./(beta*beta) - 1. ) );

  return sigma_m;

}
