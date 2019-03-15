#include "../interface/HSCPCommon.h"

#include <iostream>
#include <math.h>
 
#include "TStyle.h"
#include "TColor.h"
#include "TAxis.h"
#include "TPaletteAxis.h"
#include "TH2.h"
#include "TLatex.h"



//void HSCPCommon::addLabels( TCanvas* c1 ) {
//
//  TPaveText* labelLeft  = HSCPCommon::getLabelLeft();
//  TPaveText* labelRight = HSCPCommon::getLabelRight();
//
//  c1->cd();
//
//  labelRight->Draw("same");
//  labelLeft ->Draw("same");
//
//  gPad->RedrawAxis();
//
//}
 

void HSCPCommon::addLabels( TCanvas* pad, std::string left_text, std::string right_text, int iPosX, TString extraText ) {
  //
  // Global variables
  //

  TString leftText     = left_text;
  float leftTextFont   = 62;  // default is helvetic-bold

  float extraTextFont = 52;  // default is helvetica-italics

  // text sizes and text offsets with respect to the top frame
  // in unit of the top margin size
  float rightTextSize     = 0.6;
  float rightTextOffset   = 0.2;
  float leftTextSize      = 0.75;
  float leftTextOffset    = 0.1;  // only used in outOfFrame version

  float relPosX    = 0.045;
  float relPosY    = 0.035;
  float relExtraDX = 1.1;
  float relExtraDY = 1.2;

  // ratio of "CMS" and extra text size
  float extraOverCmsTextSize  = 0.76;

  bool drawLogo      = false;

  pad->SetBottomMargin(0.13);
  pad->SetLeftMargin(0.17);
  pad->SetTopMargin(0.08);
  pad->SetRightMargin(0.05);
    
  int alignY_=3;
  int alignX_=2;
  if( iPosX/10==0 ) alignX_=1;
  if( iPosX==0    ) alignX_=1;
  if( iPosX==0    ) alignY_=1;
  if( iPosX/10==1 ) alignX_=1;
  if( iPosX/10==2 ) alignX_=2;
  if( iPosX/10==3 ) alignX_=3;
  //if( iPosX == 0  ) relPosX = 0.12;
  int align_ = 10*alignX_ + alignY_;

  float H = pad->GetWh();
  float W = pad->GetWw();
  float l = pad->GetLeftMargin();
  float t = pad->GetTopMargin();
  float r = pad->GetRightMargin();
  float b = pad->GetBottomMargin();
  //  float e = 0.025;

  pad->cd();
 
  TString rightText = right_text;
   
  TLatex latex;
  latex.SetNDC();
  latex.SetTextAngle(0);
  latex.SetTextColor(kBlack);    

  float extraTextSize = extraOverCmsTextSize*leftTextSize;

  latex.SetTextFont(42);
  latex.SetTextAlign(31); 
  latex.SetTextSize(rightTextSize*t);    
  latex.DrawLatex(1-r,1-t+rightTextOffset*t,rightText);

  if(iPosX==0)
    leftText += "#scale[0.76]{#bf{#it{ "+extraText+"}}}";
  latex.SetTextFont(leftTextFont);
  latex.SetTextAlign(11); 
  latex.SetTextSize(leftTextSize*t);    
  latex.DrawLatex(l,1-t+rightTextOffset*t,leftText);
  
  pad->cd();

  float posX_=0;
  if( iPosX%10<=1 )
    {
      posX_ =   l + relPosX*(1-l-r);
    }
  else if( iPosX%10==2 )
    {
      posX_ =  l + 0.5*(1-l-r);
    }
  else if( iPosX%10==3 )
    {
      posX_ =  1-r - relPosX*(1-l-r);
    }
  float posY_ = 1-t - relPosY*(1-t-b);
  if(extraText != "" && iPosX != 0)
    {
      latex.SetTextFont(extraTextFont);
      latex.SetTextSize(extraTextSize*t);
      latex.SetTextAlign(align_);
      latex.DrawLatex(posX_, posY_, extraText);      
    }

  pad->Update();

  gPad->RedrawAxis();

}



TPaveText* HSCPCommon::getLabelLeft() {

  TPaveText* label_left = new TPaveText(0.154,0.953,0.275,0.955, "brNDC");
  label_left->SetBorderSize(0);
  label_left->SetFillColor(kWhite);
  label_left->SetTextSize(0.04);
  label_left->SetTextAlign(11); // align left
  label_left->SetTextFont(42);
  label_left->AddText( "CMS Phase-2 Simulation");

  return label_left;

}



TPaveText* HSCPCommon::getLabelRight() {

  TPaveText* label_top = new TPaveText(0.4,0.953,0.975,0.955, "brNDC");
  label_top->SetBorderSize(0);
  label_top->SetFillColor(kWhite);
  label_top->SetTextSize(0.038);
  label_top->SetTextAlign(31); // align right                                                                                                                                        
  label_top->SetTextFont(42);
  label_top->AddText("#sqrt{s} = 14 TeV");

  return label_top;

}



std::vector<float> HSCPCommon::etaBins() {

  std::vector<float> etaBins;
  etaBins.push_back( 0. );
  etaBins.push_back( 0.2 );
  etaBins.push_back( 0.5 );
  etaBins.push_back( 0.8 );
  etaBins.push_back( 1.1 );
  etaBins.push_back( HSCPCommon::barrelEnd()   );
  etaBins.push_back( 1.9 );
  etaBins.push_back( 2.4 );

  return etaBins;

}



std::vector<float> HSCPCommon::pBins() {

  std::vector<float> pBins;
  pBins.push_back( 0.7 );
  pBins.push_back( 1.5 );
  pBins.push_back( 3.0 );
  pBins.push_back( 5.5 );
  pBins.push_back( 10. );
  pBins.push_back( 20. );
  pBins.push_back( 50. );
  pBins.push_back( 999 );

  return pBins;

}



void HSCPCommon::symmetrize( std::vector<float> &etaBins ) {

  std::vector<float> negativeVec;

  for( unsigned i=0; i<etaBins.size(); ++i )
    if( etaBins[i]>0. ) negativeVec.push_back( -etaBins[i] );

  std::vector<float>::iterator it;
  for( unsigned i=0; i<negativeVec.size(); ++i ) {
    it = etaBins.begin();
    it = etaBins.insert ( it , negativeVec[i] );
  }

}



std::vector<int> HSCPCommon::colors() {

  std::vector<int> colors;
  
  colors.push_back( 29 );
  colors.push_back( 38 );
  colors.push_back( 46 );
  colors.push_back( 42 );
  //colors.push_back( kGray+1 );
  colors.push_back( kBlack );
  colors.push_back( kGreen );

  return colors;

}


void HSCPCommon::setStyle() {

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
  style->SetPadTopMargin(0.06);
  style->SetPadBottomMargin(0.14);//0.13);
  style->SetPadLeftMargin(0.16);//0.16);
  style->SetPadRightMargin(0.04);//0.02);
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

