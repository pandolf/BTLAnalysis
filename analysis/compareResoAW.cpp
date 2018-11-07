#include "TFile.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TLegend.h"

#include "../interface/BTLCommon.h"
#include "../interface/BTLConf.h"




int main( int argc, char* argv[] ) {

  if( argc==1 ) {

    std::cout << "USAGE: ./drawResolution [confName]" << std::endl;
    exit(1);

  }


  BTLCommon::setStyle();

  std::string confName( argv[1] );
  BTLConf conf(confName);


  TFile* file_aw1 = TFile::Open( Form("plots/%s/resoFile_aw1bins_hodoOnBar.root", conf.get_confName().c_str()) );
  TFile* file_aw4 = TFile::Open( Form("plots/%s/resoFile_aw4bins_hodoOnBar.root", conf.get_confName().c_str()) );
  TFile* file_aw2 = TFile::Open( Form("plots/%s/resoFile_aw2bins_hodoOnBar.root", conf.get_confName().c_str()) );

  TH1D* h1_reso_aw1 = (TH1D*)file_aw1->Get( "reso_corr" );
  TH1D* h1_reso_aw2 = (TH1D*)file_aw2->Get( "reso_corr" );
  TH1D* h1_reso_aw4 = (TH1D*)file_aw4->Get( "reso_corr" );

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  h1_reso_aw2->SetLineColor( 38 );
  h1_reso_aw4->SetLineColor( 46 );
  h1_reso_aw1->SetLineColor( kGray+2 );

  h1_reso_aw2->SetFillColor( 38 );
  h1_reso_aw4->SetFillColor( 46 );
  h1_reso_aw1->SetFillColor( kGray+2 );

  h1_reso_aw2->SetFillStyle(3004);
  h1_reso_aw4->SetFillStyle(3005);
  h1_reso_aw1->SetFillStyle(3006);

  TF1* f1_reso_aw2 = h1_reso_aw2->GetFunction( Form( "gaus_%s", h1_reso_aw2->GetName() ) );
  TF1* f1_reso_aw4 = h1_reso_aw4->GetFunction( Form( "gaus_%s", h1_reso_aw4->GetName() ) );
  TF1* f1_reso_aw1 = h1_reso_aw1->GetFunction( Form( "gaus_%s", h1_reso_aw1->GetName() ) );

  f1_reso_aw2->SetLineColor( 38 );
  f1_reso_aw4->SetLineColor( 46 );
  f1_reso_aw1->SetLineColor( kGray+2 );

  h1_reso_aw2->SetLineWidth(2);
  h1_reso_aw4->SetLineWidth(2);
  h1_reso_aw1->SetLineWidth(2);

  float xMin = h1_reso_aw2->GetXaxis()->GetXmin();
  float xMax = h1_reso_aw2->GetXaxis()->GetXmax();
  float yMax = h1_reso_aw4->GetMaximum()*1.1;

  TH2D* h2_axes = new TH2D( "axes", "", 10, xMin, xMax, 10, 0., yMax );
  h2_axes->SetXTitle( "t(ave) - t(MCP) [ns]" );
  h2_axes->SetYTitle( "Entries" );
  h2_axes->Draw();

  h1_reso_aw1->Draw("same");
  f1_reso_aw1->Draw("same");
  h1_reso_aw2->Draw("same");
  f1_reso_aw2->Draw("same");
  h1_reso_aw4->Draw("same");
  f1_reso_aw4->Draw("same");

  bool drawLeft = f1_reso_aw4->GetParameter(1)>0.5*(xMin+xMax);

  float xMin_text = ( drawLeft ) ? 0.2  : 0.6;
  float xMax_text = ( drawLeft ) ? 0.58 : 0.9;


  TPaveText* text_aw1 = new TPaveText( xMin_text, 0.73, xMax_text, 0.88, "brNDC" );
  text_aw1->SetTextSize(0.035);
  text_aw1->SetFillColor(0);
  text_aw1->SetTextColor( kGray+2 );
  text_aw1->AddText( "Single Amp. Walk." );
  text_aw1->AddText( Form("#sigma_{eff} = %.1f ps", BTLCommon::subtractResoPTK(BTLCommon::getSigmaEff(h1_reso_aw1))*1000.) );
  text_aw1->AddText( Form("#sigma_{fit} = %.1f#pm%.1f ps", BTLCommon::subtractResoPTK(f1_reso_aw1->GetParameter(2))*1000., f1_reso_aw1->GetParError(2)*1000. ) );
  text_aw1->SetTextAlign(11);
  text_aw1->Draw("same");

  TPaveText* text_aw2 = new TPaveText( xMin_text, 0.54, xMax_text, 0.69, "brNDC" );
  text_aw2->SetTextSize(0.035);
  text_aw2->SetFillColor(0);
  text_aw2->SetTextColor( 38 );
  text_aw2->AddText( "2-bin Amp. Walk." );
  text_aw2->AddText( Form("#sigma_{eff} = %.1f ps", BTLCommon::subtractResoPTK(BTLCommon::getSigmaEff(h1_reso_aw2))*1000.) );
  text_aw2->AddText( Form("#sigma_{fit} = %.1f#pm%.1f ps", BTLCommon::subtractResoPTK(f1_reso_aw2->GetParameter(2))*1000., f1_reso_aw2->GetParError(2)*1000. ) );
  text_aw2->SetTextAlign(11);
  text_aw2->Draw("same");

  TPaveText* text_aw4 = new TPaveText( xMin_text, 0.35, xMax_text, 0.5, "brNDC" );
  text_aw4->SetTextSize(0.035);
  text_aw4->SetFillColor(0);
  text_aw4->SetTextColor( 46 );
  text_aw4->AddText( "4-bin Amp. Walk." );
  text_aw4->AddText( Form("#sigma_{eff} = %.1f ps", BTLCommon::subtractResoPTK(BTLCommon::getSigmaEff(h1_reso_aw4))*1000.) );
  text_aw4->AddText( Form("#sigma_{fit} = %.1f#pm%.1f ps", BTLCommon::subtractResoPTK(f1_reso_aw4->GetParameter(2))*1000., f1_reso_aw4->GetParError(2)*1000. ) );
  text_aw4->SetTextAlign(11);
  text_aw4->Draw("same");


  TPaveText* labelConf = conf.get_labelConf(4);
  labelConf->Draw("same");

  BTLCommon::addLabels( c1, conf );

  c1->SaveAs( Form( "plots/%s/resoCompareAW.pdf", conf.get_confName().c_str() ) );

  return 0;

}
