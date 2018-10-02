#include "../interface/BTLCommon.h"

#include <iostream>
#include <math.h>
 
#include "TStyle.h"
#include "TColor.h"



TF1* BTLCommon::fitGaus( TH1D* histo, float nSigma ) {

  float mean_histo = histo->GetMean();
  float rms_histo  = histo->GetRMS();

  TF1* f1_gaus = new TF1( Form("gaus_%s", histo->GetName()), "gaus", mean_histo-rms_histo, mean_histo+rms_histo );
  f1_gaus->SetLineColor( 46 );
  
  histo->Fit( f1_gaus->GetName(), "RQ0" );

  float xMin_fit = f1_gaus->GetParameter(1) - nSigma*f1_gaus->GetParameter(2);
  float xMax_fit = f1_gaus->GetParameter(1) + nSigma*f1_gaus->GetParameter(2);

  f1_gaus->SetRange( xMin_fit, xMax_fit );


  int n_iter = 5;

  for( int i=0; i<n_iter; ++i ) { // iterative fit

    if( i==n_iter-1 )
      histo->Fit( f1_gaus->GetName(), "RQ+" );
    else {
      histo->Fit( f1_gaus->GetName(), "RQ0" );
      xMin_fit = f1_gaus->GetParameter(1) - nSigma*f1_gaus->GetParameter(2);
      xMax_fit = f1_gaus->GetParameter(1) + nSigma*f1_gaus->GetParameter(2);
      f1_gaus->SetRange( xMin_fit, xMax_fit );
    }

  } // for iter


  return f1_gaus;

}




void BTLCommon::addLabels( TCanvas* c1 ) {

  TPaveText* labelLeft  = BTLCommon::getLabelLeft ();
  TPaveText* labelRight = BTLCommon::getLabelRight();

  c1->cd();

  labelRight->Draw("same");
  labelLeft ->Draw("same");

  gPad->RedrawAxis();

}


TPaveText* BTLCommon::getLabelRight( const std::string& text ) {

  TPaveText* label_top = new TPaveText(0.4,0.953,0.975,0.975, "brNDC");
  label_top->SetBorderSize(0);
  label_top->SetFillColor(kWhite);
  label_top->SetTextSize(0.038);
  label_top->SetTextAlign(31); // align right                                                                                                                                        
  label_top->SetTextFont(42);
  label_top->AddText(text.c_str());

  return label_top;

}


TPaveText* BTLCommon::getLabelLeft( const std::string& text ) {

  TPaveText* label_left = new TPaveText(0.143,0.96,0.27,0.965, "brNDC");
  label_left->SetBorderSize(0);
  label_left->SetFillColor(kWhite);
  label_left->SetTextSize(0.042);
  label_left->SetTextAlign(11); // align left
  label_left->SetTextFont(42);
  label_left->AddText( text.c_str() );

  return label_left;

}







std::vector<int> BTLCommon::colors() {

  std::vector<int> colors;
  
  colors.push_back( 38 );
  colors.push_back( 46 );
  colors.push_back( 29 );
  colors.push_back( kGray+1 );

  return colors;

}


void BTLCommon::setStyle() {

  // set the TStyle
  TStyle* style = new TStyle("DrawBaseStyle", "");
  style->SetCanvasColor(0);
  style->SetPadColor(0);
  style->SetFrameFillColor(0);
  style->SetStatColor(0);
  style->SetOptStat(0);
  style->SetOptFit(0);
  style->SetTitleFillColor(0);
  style->SetCanvasBorderMode(0);
  style->SetPadBorderMode(0);
  style->SetFrameBorderMode(0);
  style->SetPadBottomMargin(0.12);
  style->SetPadLeftMargin(0.12);
  style->cd();
  // For the canvas:
  style->SetCanvasBorderMode(0);
  style->SetCanvasColor(kWhite);
  style->SetCanvasDefH(600); //Height of canvas
  style->SetCanvasDefW(600); //Width of canvas
  style->SetCanvasDefX(0); //POsition on screen
  style->SetCanvasDefY(0);
  // For the Pad:
  style->SetPadBorderMode(0);
  style->SetPadColor(kWhite);
  style->SetPadGridX(false);
  style->SetPadGridY(false);
  style->SetGridColor(0);
  style->SetGridStyle(3);
  style->SetGridWidth(1);
  // For the frame:
  style->SetFrameBorderMode(0);
  style->SetFrameBorderSize(1);
  style->SetFrameFillColor(0);
  style->SetFrameFillStyle(0);
  style->SetFrameLineColor(1);
  style->SetFrameLineStyle(1);
  style->SetFrameLineWidth(1);
  // Margins:
  style->SetPadTopMargin(0.05);
  style->SetPadBottomMargin(0.15);//0.13);
  style->SetPadLeftMargin(0.15);//0.16);
  style->SetPadRightMargin(0.05);//0.02);
  // For the Global title:
  style->SetOptTitle(0);
  style->SetTitleFont(42);
  style->SetTitleColor(1);
  style->SetTitleTextColor(1);
  style->SetTitleFillColor(10);
  style->SetTitleFontSize(0.05);
  // For the axis titles:
  style->SetTitleColor(1, "XYZ");
  style->SetTitleFont(42, "XYZ");
  style->SetTitleSize(0.05, "XYZ");
  style->SetTitleXOffset(1.15);//0.9);
  style->SetTitleYOffset(1.5); // => 1.15 if exponents
  // For the axis labels:
  style->SetLabelColor(1, "XYZ");
  style->SetLabelFont(42, "XYZ");
  style->SetLabelOffset(0.007, "XYZ");
  style->SetLabelSize(0.045, "XYZ");
  // For the axis:
  style->SetAxisColor(1, "XYZ");
  style->SetStripDecimals(kTRUE);
  style->SetTickLength(0.03, "XYZ");
  style->SetNdivisions(510, "XYZ");
  style->SetPadTickX(1); // To get tick marks on the opposite side of the frame
  style->SetPadTickY(1);
  // for histograms:
  style->SetHistLineColor(1);
  // for the pallete
  Double_t stops[5] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
  Double_t red  [5] = { 0.00, 0.00, 0.87, 1.00, 0.51 };
  Double_t green[5] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
  Double_t blue [5] = { 0.51, 1.00, 0.12, 0.00, 0.00 };
  TColor::CreateGradientColorTable(5, stops, red, green, blue, 100);
  style->SetNumberContours(100);

  style->cd();

}

