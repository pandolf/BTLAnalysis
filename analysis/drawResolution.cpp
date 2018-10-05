#include <iostream>

#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TLegend.h"
#include "TPaveText.h"
#include "TF1.h"

#include "../interface/BTLCommon.h"





int main( int argc, char* argv[] ) {

  
  if( argc==1 ) {

    std::cout << "USAGE: ./drawResolution [confName]" << std::endl;
    exit(1);

  }

  BTLCommon::setStyle();

  std::string confName( argv[1] );


  TFile* file = TFile::Open( Form("treesLite/%s_AW.root", confName.c_str()) );
  TTree* tree = (TTree*)file->Get( "treeLite" );

  float xMin = 2.4;
  float xMax = 3.5;

  TH1D* h1_reso      = new TH1D( "reso"     , "", 100, xMin, xMax );
  TH1D* h1_reso_corr = new TH1D( "reso_corr", "", 100, xMin, xMax );

  h1_reso->SetXTitle( "0.5 * ( t_{Left} + t_{Right} ) [ns]" );
  h1_reso->SetYTitle( "Entries" );

  h1_reso_corr->SetXTitle( "0.5 * ( t_{Left} + t_{Right} ) [ns]" );
  h1_reso_corr->SetYTitle( "Entries" );

  //h1_reso->SetLineWidth( 2 );
  h1_reso->SetLineColor( 38 );
  h1_reso->SetFillColor( 38 );
  h1_reso->SetFillStyle( 3004 );
  
  //h1_reso_corr->SetLineWidth( 2 );
  h1_reso_corr->SetLineColor( 46 );
  h1_reso_corr->SetFillColor( 46 );
  h1_reso_corr->SetFillStyle( 3005 );
  
  tree->Project( h1_reso     ->GetName(), "0.5*(tLeft      + tRight     )", "" );
  tree->Project( h1_reso_corr->GetName(), "0.5*(tLeft_corr + tRight_corr)", "" );

  TF1* f1_gaus      = BTLCommon::fitGaus( h1_reso     , 1.7 );
  TF1* f1_gaus_corr = BTLCommon::fitGaus( h1_reso_corr, 2.1 );

  float sigma_eff_raw  = BTLCommon::getSigmaEff( h1_reso      );
  float sigma_eff_corr = BTLCommon::getSigmaEff( h1_reso_corr );


  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  //float xMin_axes = f1_gaus->GetParameter(1)-0.3;
  //float xMax_axes = f1_gaus->GetParameter(1)+10.0;

  TH2D* h2_axes = new TH2D( "axes", "", 10, xMin, xMax, 10, 0., 1.3*h1_reso_corr->GetMaximum() );
  h2_axes->SetXTitle( "0.5 * ( t_{Left} + t_{Right} ) [ns]" );
  h2_axes->SetYTitle( "Entries" );
  h2_axes->Draw();

  float xMin_text = ( f1_gaus_corr->GetParameter(1)>2.8) ? 0.2 : 0.52;
  float xMax_text = ( f1_gaus_corr->GetParameter(1)>2.8) ? 0.58 : 0.9;


  f1_gaus     ->SetLineColor( 38 );
  f1_gaus_corr->SetLineColor( 46 );

  f1_gaus     ->SetLineWidth( 2 );
  f1_gaus_corr->SetLineWidth( 2 );
  
  h1_reso     ->Fit( f1_gaus     , "R" );
  h1_reso_corr->Fit( f1_gaus_corr, "R" );

  h1_reso     ->Draw("same"); 
  h1_reso_corr->Draw("same");

  TPaveText* text_raw = new TPaveText( xMin_text, 0.73, xMax_text, 0.88, "brNDC" );
  text_raw->SetTextSize(0.035);
  text_raw->SetFillColor(0);
  text_raw->SetTextColor( 38 );
  text_raw->AddText( "Raw Data" );
  text_raw->AddText( Form("#sigma_{eff} = %.1f ps", BTLCommon::subtractResoPTK(sigma_eff_raw*1000.)            ) );
  text_raw->AddText( Form("#sigma_{fit} = %.1f ps", BTLCommon::subtractResoPTK(f1_gaus->GetParameter(2)*1000.) ) );
  text_raw->SetTextAlign(11);
  text_raw->Draw("same");

  TPaveText* text_corr = new TPaveText( xMin_text, 0.54, xMax_text, 0.69, "brNDC" );
  text_corr->SetTextSize(0.035);
  text_corr->SetFillColor(0);
  text_corr->SetTextColor( 46 );
  text_corr->AddText( "Amp. Walk Corr." );
  text_corr->AddText( Form("#sigma_{eff} = %.1f ps", BTLCommon::subtractResoPTK(sigma_eff_corr*1000.)                ) );
  text_corr->AddText( Form("#sigma_{fit} = %.1f ps", BTLCommon::subtractResoPTK(f1_gaus_corr->GetParameter(2)*1000.) ) );
  text_corr->SetTextAlign(11);
  text_corr->Draw("same");

  // avoid white boxes over the data
  h1_reso     ->Draw("same"); 
  h1_reso_corr->Draw("same");



  //TLegend* legend = new TLegend( 0.5, 0.75, 0.9, 0.9 );
  //legend->SetFillColor(0);
  //legend->SetTextSize( 0.035 );
  //legend->AddEntry( f1_gaus     , Form("Raw (#sigma = %.1f ps)", f1_gaus     ->GetParameter(2)*1000.), "L" );
  //legend->AddEntry( f1_gaus_corr, Form("Corr. (#sigma = %.1f ps)" , f1_gaus_corr->GetParameter(2)*1000.), "L" );
  //legend->Draw("same");

  BTLCommon::addLabels( c1 );

  c1->SaveAs( Form("plots/%s/reso.eps", confName.c_str()) );
  c1->SaveAs( Form("plots/%s/reso.pdf", confName.c_str()) );
  c1->SaveAs( Form("plots/%s/reso.png", confName.c_str()) );

  c1->Clear();
  c1->SetLogy();

  TH2D* h2_axes_log = new TH2D( "axes_log", "", 10, xMin, xMax, 10, 0.1, 5.*h1_reso_corr->GetMaximum() );
  h2_axes_log->SetXTitle( "0.5 * ( t_{Left} + t_{Right} ) [ns]" );
  h2_axes_log->SetYTitle( "Entries" );
  h2_axes_log->Draw();

  h1_reso     ->Draw("same"); 
  h1_reso_corr->Draw("same");

  //legend->Draw("same");
  text_raw->Draw("same");

  BTLCommon::addLabels( c1 );

  c1->SaveAs( Form("plots/%s/reso_log.eps", confName.c_str()) );
  c1->SaveAs( Form("plots/%s/reso_log.pdf", confName.c_str()) );
  c1->SaveAs( Form("plots/%s/reso_log.png", confName.c_str()) );


  TFile* outfile = TFile::Open( Form("plots/%s/resoFile.root", confName.c_str()), "RECREATE" );
  outfile->cd();

  h1_reso->Write();
  h1_reso_corr->Write();

  outfile->Close();

  std::cout << "-> Saved reso info in: " << outfile->GetName() << std::endl;

  delete c1;

  return 0;

}
