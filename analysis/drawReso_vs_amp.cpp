#include "TFile.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TH2D.h"
#include "TGraphErrors.h"

#include "../interface/BTLConf.h"
#include "../interface/BTLCommon.h"



TGraphErrors* getGraph( int sensorConf, const std::string& digiChSet );

int main() {

  BTLCommon::setStyle();

  std::vector<int> colors = BTLCommon::colors();

  TGraphErrors* gr_4a = getGraph( 4, "a" );
  TGraphErrors* gr_4b = getGraph( 4, "b" );
  TGraphErrors* gr_5a = getGraph( 5, "a" );
  TGraphErrors* gr_5b = getGraph( 5, "b" );

  gr_4a->SetMarkerSize(1.3);
  gr_4b->SetMarkerSize(1.3);
  gr_5a->SetMarkerSize(1.3);
  gr_5b->SetMarkerSize(1.3);

  gr_4a->SetMarkerStyle(20); // 4a and 4b use the same SiPMs
  gr_4b->SetMarkerStyle(20);
  gr_5a->SetMarkerStyle(21);
  gr_5b->SetMarkerStyle(22);

  gr_4a->SetMarkerColor(colors[2]);
  gr_4b->SetMarkerColor(colors[2]);
  gr_5a->SetMarkerColor(colors[1]);
  gr_5b->SetMarkerColor(colors[0]);

  gr_4a->SetLineColor(colors[2]);
  gr_4b->SetLineColor(colors[2]);
  gr_5a->SetLineColor(colors[1]);
  gr_5b->SetLineColor(colors[0]);

  BTLConf conf_4a( 4, "a" );
  BTLConf conf_4b( 4, "b" );
  BTLConf conf_5a( 5, "a" );
  BTLConf conf_5b( 5, "b" );
  
  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  TH2D* h2_axes = new TH2D( "axes", "", 10, 0., 0.5, 10, 0., 90. );
  h2_axes->SetXTitle( "MIP Peak [a.u.]" );
  h2_axes->SetYTitle( "Resolution [ps]" );
  h2_axes->Draw();

  gr_4a->Draw("P same");
  gr_4b->Draw("P same");
  gr_5a->Draw("P same");
  gr_5b->Draw("P same");


  TLegend* legend = new TLegend( 0.2, 0.68, 0.5, 0.9 );
  legend->SetTextSize( 0.035 );
  legend->SetFillColor( 0 );
  legend->AddEntry( gr_4a, conf_4a.SiPMTypeText().c_str(), "P" );
  legend->AddEntry( gr_5a, conf_5a.SiPMTypeText().c_str(), "P" );
  legend->AddEntry( gr_5b, conf_5b.SiPMTypeText().c_str(), "P" );
  legend->Draw("same");

  TPaveText* label_nino = new TPaveText( 0.6, 0.2, 0.9, 0.26, "brNDC" );
  label_nino->SetFillColor(0);
  label_nino->SetTextSize(0.035);
  label_nino->SetTextColor(kGray+2);
  label_nino->AddText( "NINO thr = 100 mV");
  label_nino->Draw("same");

  gPad->RedrawAxis();

  c1->SaveAs("plots/reso_vs_amp.pdf"); 
  
  delete c1;
  delete h2_axes;

  return 0;

}



TGraphErrors* getGraph( int sensorConf, const std::string& digiChSet ) {

  TFile* file = TFile::Open( Form("plots/resoSummaryFile_%d%s.root", sensorConf, digiChSet.c_str()) );
  TGraphErrors* graph = (TGraphErrors*)file->Get( Form("reso_vs_amp_%d%s_aw4bins_hodoOnBar", sensorConf, digiChSet.c_str()) );

  return graph;

}
