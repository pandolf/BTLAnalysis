#include "TFile.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TH2D.h"
#include "TGraphErrors.h"

#include "../interface/BTLConf.h"
#include "../interface/BTLCommon.h"



TGraphErrors* getGraph( int sensorConf, const std::string& digiChSet );
TF1* fitGraph( TGraphErrors* graph, int xMin, int xMax );


int main() {

  BTLCommon::setStyle();

  std::vector<int> colors = BTLCommon::colors();

  TGraphErrors* gr_4a = getGraph( 4, "a" );
  TGraphErrors* gr_4b = getGraph( 4, "b" );
  TGraphErrors* gr_5a = getGraph( 5, "a" );
  TGraphErrors* gr_5b = getGraph( 5, "b" );

  TGraphErrors* gr_4 = new TGraphErrors(0);
  for( unsigned i=0; i<gr_4a->GetN(); ++i ) {
    double x, y, xerr, yerr;
    int i4 = gr_4->GetN();
    gr_4a->GetPoint( i, x, y );
    gr_4->SetPoint( i4, x, y );
    xerr = gr_4a->GetErrorX(i);
    yerr = gr_4a->GetErrorY(i);
    gr_4->SetPointError( i4, xerr, yerr );
    i4 = gr_4->GetN();
    gr_4b->GetPoint( i, x, y );
    gr_4->SetPoint( i4, x, y );
    xerr = gr_4b->GetErrorX(i);
    yerr = gr_4b->GetErrorY(i);
    gr_4->SetPointError( i4, xerr, yerr );
  }

  gr_4->SetMarkerSize(1.3);
  //gr_4a->SetMarkerSize(1.3);
  //gr_4b->SetMarkerSize(1.3);
  gr_5a->SetMarkerSize(1.3);
  gr_5b->SetMarkerSize(1.3);

  gr_4->SetMarkerStyle(20); // 4a and 4b use the same SiPMs
  //gr_4a->SetMarkerStyle(20); // 4a and 4b use the same SiPMs
  //gr_4b->SetMarkerStyle(20);
  gr_5a->SetMarkerStyle(21);
  gr_5b->SetMarkerStyle(22);

  gr_4->SetMarkerColor(colors[2]);
  //gr_4a->SetMarkerColor(colors[2]);
  //gr_4b->SetMarkerColor(colors[2]);
  gr_5a->SetMarkerColor(colors[1]);
  gr_5b->SetMarkerColor(colors[0]);

  gr_4->SetLineColor(colors[2]);
  //gr_4a->SetLineColor(colors[2]);
  //gr_4b->SetLineColor(colors[2]);
  gr_5a->SetLineColor(colors[1]);
  gr_5b->SetLineColor(colors[0]);


  float xMin = 0.;
  float xMax = 0.5;

  TF1* f1_4 = fitGraph( gr_4 , xMin, xMax );
  //fitGraph( gr_5a, xMin, xMax );
  //fitGraph( gr_5b, xMin, xMax );

  // i hate root and TF1 ranges
  TH1D* h1_f1_4 = new TH1D( "h1_f1_4", "", 1000, xMin, xMax );
  for( unsigned i=0; i<1000; ++i ) h1_f1_4->SetBinContent( i+1, f1_4->Eval( xMin+(i+0.5)*h1_f1_4->GetBinWidth(1)) );
  h1_f1_4->SetLineColor( f1_4->GetLineColor() );
  h1_f1_4->SetLineWidth(2);

  BTLConf conf_4( 4, "a" );
  BTLConf conf_5a( 5, "a" );
  BTLConf conf_5b( 5, "b" );
  
  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  TH2D* h2_axes = new TH2D( "axes", "", 10, xMin, xMax, 10, 0., 90. );
  h2_axes->SetXTitle( "MIP Peak [a.u.]" );
  h2_axes->SetYTitle( "Resolution [ps]" );
  h2_axes->Draw();

  gr_4->Draw("P same");
  h1_f1_4->Draw("L same");
  //gr_4a->Draw("P same");
  //gr_4b->Draw("P same");
  gr_5a->Draw("P same");
  gr_5b->Draw("P same");


  TLegend* legend = new TLegend( 0.2, 0.68, 0.5, 0.9 );
  legend->SetTextSize( 0.035 );
  legend->SetFillColor( 0 );
  legend->AddEntry( gr_4 , conf_4 .SiPMTypeText().c_str(), "P" );
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


TF1* fitGraph( TGraphErrors* graph, int xMin, int xMax ) {

  int color = graph->GetMarkerColor();

  TF1* f1_sqrt = new TF1( Form("f1_%s", graph->GetName()), "sqrt([0] + [1]/x))", 0.002, xMax );
  f1_sqrt->SetLineColor(color);
  f1_sqrt->SetLineWidth(2);

  graph->Fit( f1_sqrt, "R0" );

  f1_sqrt->SetRange( xMin, xMax );

  return f1_sqrt;

}
