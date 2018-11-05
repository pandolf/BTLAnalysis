#include "TFile.h"
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


  TFile* file_aw1 = TFile::Open( Form("plots/%s/resoFile_aw1bins_hodoOnBar.root"      , conf.get_confName().c_str()) );
  TFile* file_aw4 = TFile::Open( Form("plots/%s/resoFile_aw4bins_hodoOnBar.root"      , conf.get_confName().c_str()) );
  TFile* file_awC = TFile::Open( Form("plots/%s/resoFile_awCentral_hodoOnBar.root", conf.get_confName().c_str()) );

  TH1D* h1_reso_aw1 = (TH1D*)file_aw1->Get( "reso_corr" );
  TH1D* h1_reso_aw4 = (TH1D*)file_aw4->Get( "reso_corr" );
  TH1D* h1_reso_awC = (TH1D*)file_awC->Get( "reso_corr" );

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  h1_reso_aw1->SetLineColor( 38 );
  h1_reso_aw4->SetLineColor( 46 );
  h1_reso_awC->SetLineColor( kGray+2 );

  h1_reso_aw1->SetLineWidth(2);
  h1_reso_aw4->SetLineWidth(2);
  h1_reso_awC->SetLineWidth(2);

  h1_reso_aw4->Draw("same");
  h1_reso_aw1->Draw("same");
  h1_reso_awC->Draw("same");

  BTLCommon::addLabels( c1, conf );

  c1->SaveAs( Form( "plots/%s/resoCompareAW.pdf", conf.get_confName().c_str() ) );

  return 0;

}
