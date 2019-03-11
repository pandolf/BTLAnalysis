#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TPaveText.h"
#include "TLegend.h"

#include "../interface/HSCPCommon.h"


void drawClosure( const std::string& outdir, TTree* tree, TFile* file, const std::string& name, float ptMin, float ptMax, float eta1, float eta2, float eta3, float eta4 );
TProfile* projectEtaSlice( TTree* tree, const std::string& name, float ptMin, float ptMax, float eta );
TF1* getFunctionAtEta( const std::string& name, float eta, TF1* f1_p0, TF1* f1_p1, TF1* f1_p2, float ptMin, float ptMax);


int main() {


  HSCPCommon::setStyle();

  TFile* file = TFile::Open( "hscpLite_DY_crab_yesMTD.root" );
  TTree* tree = (TTree*)file->Get("treeLite");

  std::string outdir( "fitsHSCP" );
  system( Form("mkdir -p %s", outdir.c_str()) );

  TFile* paramFile = TFile::Open( "paramFileHSCP.root" );

  drawClosure( outdir, tree, paramFile, "eb", 1., 4.7, 0., 0.4, 0.8, 1.2 );
  drawClosure( outdir, tree, paramFile, "ee", 1., 4.7, 1.6, 1.9, 2.1, 2.3 );


  return 0;

}





void drawClosure( const std::string& outdir, TTree* tree, TFile* file, const std::string& name, float ptMin, float ptMax, float eta1, float eta2, float eta3, float eta4 ) {

  bool isEB = fabs(eta1)<1.4;

  std::string name_f1_p0 = (isEB) ? "f1_eb_p0" : "f1_ee_p0";
  std::string name_f1_p1 = (isEB) ? "f1_eb_p1" : "f1_ee_p1";
  std::string name_f1_p2 = (isEB) ? "f1_eb_p2" : "f1_ee_p2";

  TF1* f1_p0 = (TF1*)file->Get( name_f1_p0.c_str() );
  TF1* f1_p1 = (TF1*)file->Get( name_f1_p1.c_str() );
  TF1* f1_p2 = (TF1*)file->Get( name_f1_p2.c_str() );

  std::vector<int> colors = HSCPCommon::colors();

  TProfile* hp_eta1 = projectEtaSlice( tree, Form("%s_eta1", name.c_str()), ptMin, ptMax, eta1 );
  TProfile* hp_eta2 = projectEtaSlice( tree, Form("%s_eta2", name.c_str()), ptMin, ptMax, eta2 );
  TProfile* hp_eta3 = projectEtaSlice( tree, Form("%s_eta3", name.c_str()), ptMin, ptMax, eta3 );
  TProfile* hp_eta4 = projectEtaSlice( tree, Form("%s_eta4", name.c_str()), ptMin, ptMax, eta4 );

  hp_eta1->SetMarkerStyle(24);
  hp_eta1->SetMarkerSize(1.3);
  hp_eta1->SetMarkerColor(colors[0]);
  hp_eta1->SetLineColor(colors[0]);

  hp_eta2->SetMarkerStyle(24);
  hp_eta2->SetMarkerSize(1.3);
  hp_eta2->SetMarkerColor(colors[1]);
  hp_eta2->SetLineColor(colors[1]);

  hp_eta3->SetMarkerStyle(24);
  hp_eta3->SetMarkerSize(1.3);
  hp_eta3->SetMarkerColor(colors[2]);
  hp_eta3->SetLineColor(colors[2]);

  hp_eta4->SetMarkerStyle(24);
  hp_eta4->SetMarkerSize(1.3);
  hp_eta4->SetMarkerColor(colors[3]);
  hp_eta4->SetLineColor(colors[3]);

  TF1* f1_eta1 = getFunctionAtEta( "eta1", eta1, f1_p0, f1_p1, f1_p2, ptMin, ptMax);
  TF1* f1_eta2 = getFunctionAtEta( "eta2", eta2, f1_p0, f1_p1, f1_p2, ptMin, ptMax);
  TF1* f1_eta3 = getFunctionAtEta( "eta3", eta3, f1_p0, f1_p1, f1_p2, ptMin, ptMax);
  TF1* f1_eta4 = getFunctionAtEta( "eta4", eta4, f1_p0, f1_p1, f1_p2, ptMin, ptMax);

  f1_eta1->SetLineColor( colors[0] );
  f1_eta2->SetLineColor( colors[1] );
  f1_eta3->SetLineColor( colors[2] );
  f1_eta4->SetLineColor( colors[3] );

  TCanvas* c1 = new TCanvas( Form("c1_closure_%s", name.c_str()), "", 600, 600 );
  c1->cd();

  TH2D* h2_axes = new TH2D( Form("axes_closure_%s", name.c_str()), "", 10, 0., ptMax, 10, 0.9*f1_eta1->Eval(0.7*ptMax), 1.2*f1_eta4->Eval(0.7*ptMax) );
  h2_axes->SetXTitle( "p_{T} [GeV]" );
  h2_axes->SetYTitle( "Track Length [cm]" );
  h2_axes->Draw();

  hp_eta1->Draw("p same" );
  f1_eta1->Draw("L same" );

  hp_eta2->Draw("p same" );
  f1_eta2->Draw("L same" );

  hp_eta3->Draw("p same" );
  f1_eta3->Draw("L same" );

  hp_eta4->Draw("p same" );
  f1_eta4->Draw("L same" );

  TPaveText* label_eta1 = new TPaveText( 0.2*ptMin, f1_eta1->Eval(0.9*ptMax)*1.00, 0.9*ptMin, f1_eta1->Eval(0.9*ptMax)*1.02 );
  label_eta1->SetTextSize(0.035);
  label_eta1->SetFillColor(0);
  label_eta1->SetTextColor(f1_eta1->GetLineColor());
  label_eta1->AddText( Form("|#eta| = %.1f", eta1) );
  label_eta1->Draw("same");

  TPaveText* label_eta2 = new TPaveText( 0.2*ptMin, f1_eta2->Eval(0.9*ptMax)*1.05, 0.9*ptMin, f1_eta2->Eval(0.9*ptMax)*1.07 );
  label_eta2->SetTextSize(0.035);
  label_eta2->SetFillColor(0);
  label_eta2->SetTextColor(f1_eta2->GetLineColor());
  label_eta2->AddText( Form("|#eta| = %.1f", eta2) );
  label_eta2->Draw("same");

  TPaveText* label_eta3 = new TPaveText( 0.2*ptMin, f1_eta3->Eval(0.9*ptMax)*1.02, 0.9*ptMin, f1_eta3->Eval(0.9*ptMax)*1.05 );
  label_eta3->SetTextSize(0.035);
  label_eta3->SetFillColor(0);
  label_eta3->SetTextColor(f1_eta3->GetLineColor());
  label_eta3->AddText( Form("|#eta| = %.1f", eta3) );
  label_eta3->Draw("same");

  TPaveText* label_eta4 = new TPaveText( 0.2*ptMin, f1_eta4->Eval(0.9*ptMax)*1.02, 0.9*ptMin, f1_eta4->Eval(0.9*ptMax)*1.05 );
  label_eta4->SetTextSize(0.035);
  label_eta4->SetFillColor(0);
  label_eta4->SetTextColor(f1_eta4->GetLineColor());
  label_eta4->AddText( Form("|#eta| = %.1f", eta4) );
  label_eta4->Draw("same");


//TLegend* legend = new TLegend( 0.55, 0.6, 0.9, 0.9 );
//legend->SetFillColor(0);
//legend->SetTextSize( 0.035 );
//legend->AddEntry( f1_eta1, Form("|#eta| = %.1f", eta1), "L" );
//legend->AddEntry( f1_eta2, Form("|#eta| = %.1f", eta2), "L" );
//legend->AddEntry( f1_eta3, Form("|#eta| = %.1f", eta3), "L" );
//legend->AddEntry( f1_eta4, Form("|#eta| = %.1f", eta4), "L" );
//legend->Draw("same");

  HSCPCommon::addLabels( c1 );

  c1->SaveAs( Form("%s/closure_%s.pdf", outdir.c_str(), name.c_str()) );

  delete c1;
  delete h2_axes;

}



TProfile* projectEtaSlice( TTree* tree, const std::string& name, float ptMin, float ptMax, float eta ) {

  TProfile* hp_eta = new TProfile( Form("prof_%s", name.c_str()), "", 100, ptMin, ptMax );
  tree->Project( hp_eta->GetName(), "pathLength:pt", Form("pt > %f && pt < %f && abs(eta)> %f-0.01 && abs(eta)< %f+0.01 && pathLength>20.", ptMin, ptMax, eta, eta), "prof" );

  return hp_eta;

}


TF1* getFunctionAtEta( const std::string& name, float eta, TF1* f1_p0, TF1* f1_p1, TF1* f1_p2, float ptMin, float ptMax) {

  TF1* f1 = new TF1( Form("closure_%s", name.c_str()), "[0] + [1]/x + [2]/x/x", ptMin, ptMax );
  f1->SetParameter( 0, f1_p0->Eval(eta) );
  f1->SetParameter( 1, f1_p1->Eval(eta) );
  f1->SetParameter( 2, f1_p2->Eval(eta) );

  return f1;

}

