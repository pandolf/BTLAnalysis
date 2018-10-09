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
#include "../interface/BTLConf.h"



void drawResolution( BTLConf conf, TTree* tree, const std::string& name, const std::string& selection );


int main( int argc, char* argv[] ) {

  
  if( argc==1 ) {

    std::cout << "USAGE: ./drawResolution [confName]" << std::endl;
    exit(1);

  }

  BTLCommon::setStyle();

  std::string confName( argv[1] );

  BTLConf conf(confName);

  TFile* file = TFile::Open( Form("treesLite/%s_corr.root", confName.c_str()) );
  TTree* tree = (TTree*)file->Get( "treeLite" );

  drawResolution( conf, tree, "", "" );
  drawResolution( conf, tree, "hodoOnBar" , "x_hodo>-9. && x_hodo<10." );
  drawResolution( conf, tree, "hodoCenter", "x_hodo>-5. && x_hodo<5." );

  return 0;

}


void drawResolution( BTLConf conf, TTree* tree, const std::string& name, const std::string& selection ) {

  std::string suffix(name);
  if( suffix!="" ) suffix = "_" + suffix;

  float xMin = (conf.digiChannelSet()=="a") ? 2.4001 : 3.6001;
  float xMax = (conf.digiChannelSet()=="a") ? 3.799 : 4.99;
  int nBins = (int)( xMax-xMin )/0.005;

  //TH1D* h1_reso       = new TH1D( Form("reso%s"      , suffix.c_str()), "", nBins, xMin, xMax );
  //TH1D* h1_reso_corr  = new TH1D( Form("reso_corr%s" , suffix.c_str()), "", nBins, xMin, xMax );
  //TH1D* h1_reso_corr2 = new TH1D( Form("reso_corr2%s", suffix.c_str()), "", nBins, xMin, xMax );

  TH1D* h1_reso       = new TH1D( "reso"      , "", nBins, xMin, xMax );
  TH1D* h1_reso_corr  = new TH1D( "reso_corr" , "", nBins, xMin, xMax );
  TH1D* h1_reso_corr2 = new TH1D( "reso_corr2", "", nBins, xMin, xMax );

  h1_reso->SetXTitle( "0.5 * ( t_{Left} + t_{Right} ) [ns]" );
  h1_reso->SetYTitle( "Entries" );

  h1_reso_corr->SetXTitle( "0.5 * ( t_{Left} + t_{Right} ) [ns]" );
  h1_reso_corr->SetYTitle( "Entries" );

  h1_reso_corr2->SetXTitle( "0.5 * ( t_{Left} + t_{Right} ) [ns]" );
  h1_reso_corr2->SetYTitle( "Entries" );

  h1_reso->SetLineWidth( 2 );
  h1_reso->SetLineColor( 38 );
  h1_reso->SetFillColor( 38 );
  h1_reso->SetFillStyle( 3004 );
  
  h1_reso_corr->SetLineWidth( 2 );
  h1_reso_corr->SetLineColor( 46 );
  h1_reso_corr->SetFillColor( 46 );
  h1_reso_corr->SetFillStyle( 3005 );
  
  h1_reso_corr2->SetLineWidth( 2 );
  h1_reso_corr2->SetLineColor( kGray+2 );
  //h1_reso_corr2->SetFillColor( kGray+2 );
  //h1_reso_corr2->SetFillStyle( 3006 );
  
  tree->Project( h1_reso      ->GetName(), "0.5*(tLeft      + tRight     )", selection.c_str());
  tree->Project( h1_reso_corr ->GetName(), "0.5*(tLeft_corr + tRight_corr)", selection.c_str());

  TBranch* br_tAveCorr = tree->FindBranch( "tAveCorr" );
  bool hodoCorr = (br_tAveCorr != 0 );
  if( hodoCorr ) tree->Project( h1_reso_corr2->GetName(), "tAveCorr", selection.c_str() );

  if( h1_reso->GetEntries()<1000 ) {
    h1_reso->Rebin(4);
    h1_reso_corr->Rebin(4);
    if( hodoCorr ) h1_reso_corr2->Rebin(4);
  } else if( h1_reso->GetEntries()<2000 ) {
    h1_reso->Rebin(2);
    h1_reso_corr->Rebin(2);
    if( hodoCorr ) h1_reso_corr2->Rebin(2);
  }


  TF1* f1_gaus       = BTLCommon::fitGaus( h1_reso      , 1.7 );
  TF1* f1_gaus_corr  = BTLCommon::fitGaus( h1_reso_corr , 2.1 );
  TF1* f1_gaus_corr2 = (hodoCorr) ? BTLCommon::fitGaus( h1_reso_corr2, 2.1 ) : 0;

  float sigma_eff_raw   = BTLCommon::getSigmaEff( h1_reso       );
  float sigma_eff_corr  = BTLCommon::getSigmaEff( h1_reso_corr  );
  float sigma_eff_corr2 = (hodoCorr) ? BTLCommon::getSigmaEff( h1_reso_corr2 ) : 0;


  TCanvas* c1 = new TCanvas( Form("c1_reso%s", suffix.c_str()), "", 600, 600 );
  c1->cd();

  //float xMin_axes = f1_gaus->GetParameter(1)-0.3;
  //float xMax_axes = f1_gaus->GetParameter(1)+10.0;

  TH2D* h2_axes = new TH2D( Form("axes%s", suffix.c_str()), "", 10, xMin, xMax, 10, 0., 1.1*h1_reso_corr->GetMaximum() );
  h2_axes->SetXTitle( "0.5 * ( t_{Left} + t_{Right} ) [ns]" );
  h2_axes->SetYTitle( "Entries" );
  h2_axes->Draw();

//  float peakPos = ( conf.digiConf()=="6a" ) ? 2.85 : 4.3;

  float xMin_text = ( f1_gaus_corr->GetParameter(1)>0.5*(xMin+xMax)) ? 0.2  : 0.6;
  float xMax_text = ( f1_gaus_corr->GetParameter(1)>0.5*(xMin+xMax)) ? 0.58 : 0.9;


  f1_gaus      ->SetLineColor( 38 );
  f1_gaus_corr ->SetLineColor( 46 );
  if( hodoCorr ) f1_gaus_corr2->SetLineColor( kGray+2 );

  f1_gaus      ->SetLineWidth( 2 );
  f1_gaus_corr ->SetLineWidth( 2 );
  if( hodoCorr ) f1_gaus_corr2->SetLineWidth( 2 );
  
  //h1_reso     ->Fit( f1_gaus     , "R" );
  //h1_reso_corr->Fit( f1_gaus_corr, "R" );

  TPaveText* text_raw = new TPaveText( xMin_text, 0.73, xMax_text, 0.88, "brNDC" );
  text_raw->SetTextSize(0.035);
  text_raw->SetFillColor(0);
  text_raw->SetTextColor( 38 );
  text_raw->AddText( "Raw Data" );
  text_raw->AddText( Form("#sigma_{eff} = %.1f ps", BTLCommon::subtractResoPTK(sigma_eff_raw)*1000.            ) );
  text_raw->AddText( Form("#sigma_{fit} = %.1f ps", BTLCommon::subtractResoPTK(f1_gaus->GetParameter(2))*1000. ) );
  text_raw->SetTextAlign(11);
  text_raw->Draw("same");

  TPaveText* text_corr = new TPaveText( xMin_text, 0.54, xMax_text, 0.69, "brNDC" );
  text_corr->SetTextSize(0.035);
  text_corr->SetFillColor(0);
  text_corr->SetTextColor( 46 );
  text_corr->AddText( "Amp. Walk Corr." );
  text_corr->AddText( Form("#sigma_{eff} = %.1f ps", BTLCommon::subtractResoPTK(sigma_eff_corr)*1000.                ) );
  text_corr->AddText( Form("#sigma_{fit} = %.1f ps", BTLCommon::subtractResoPTK(f1_gaus_corr->GetParameter(2))*1000. ) );
  text_corr->SetTextAlign(11);
  text_corr->Draw("same");

  h1_reso     ->Draw("same"); 
  f1_gaus     ->Draw("same"); 
  h1_reso_corr->Draw("same");
  f1_gaus_corr->Draw("same");

  
  TPaveText* text_conf = new TPaveText( xMin_text, 0.25, xMax_text, 0.35, "brNDC" );
  text_conf->SetTextSize(0.03);
  text_conf->SetTextFont(42);
  text_conf->SetFillColor(0);
  //text_conf->SetTextAlign(11);
  text_conf->AddText( Form("NINO thr = %.0f mV", conf.ninoThr()) );
  text_conf->AddText( Form("V(bias) = %.0f V", conf.vBias()) );
  //text_conf->Draw("same");
  

  // avoid white boxes over the data
  h1_reso      ->Draw("same"); 
  h1_reso_corr ->Draw("same");

  BTLCommon::addLabels( c1, conf );

  c1->SaveAs( Form("plots/%s/reso%s.eps", conf.get_confName().c_str(), suffix.c_str()) );
  c1->SaveAs( Form("plots/%s/reso%s.pdf", conf.get_confName().c_str(), suffix.c_str()) );
  c1->SaveAs( Form("plots/%s/reso%s.png", conf.get_confName().c_str(), suffix.c_str()) );



  if( hodoCorr ) {

    h1_reso      ->SetLineWidth(1); 
    h1_reso_corr ->SetLineWidth(1);

    TPaveText* text_corr2 = new TPaveText( xMin_text, 0.35, xMax_text, 0.5, "brNDC" );
    text_corr2->SetTextSize(0.035);
    text_corr2->SetFillColor(0);
    text_corr2->SetTextColor( kGray+2 );
    text_corr2->AddText( "AW + Hodo Corr." );
    text_corr2->AddText( Form("#sigma_{eff} = %.1f ps", BTLCommon::subtractResoPTK(sigma_eff_corr2)*1000.                ) );
    text_corr2->AddText( Form("#sigma_{fit} = %.1f ps", BTLCommon::subtractResoPTK(f1_gaus_corr2->GetParameter(2))*1000. ) );
    text_corr2->SetTextAlign(11);
    text_corr2->Draw("same");

    h1_reso_corr2->Draw("same");
    f1_gaus_corr2->Draw("same");

    c1->SaveAs( Form("plots/%s/reso%s_withHodo.eps", conf.get_confName().c_str(), suffix.c_str()) );
    c1->SaveAs( Form("plots/%s/reso%s_withHodo.pdf", conf.get_confName().c_str(), suffix.c_str()) );
    c1->SaveAs( Form("plots/%s/reso%s_withHodo.png", conf.get_confName().c_str(), suffix.c_str()) );

  }

  //TLegend* legend = new TLegend( 0.5, 0.75, 0.9, 0.9 );
  //legend->SetFillColor(0);
  //legend->SetTextSize( 0.035 );
  //legend->AddEntry( f1_gaus     , Form("Raw (#sigma = %.1f ps)", f1_gaus     ->GetParameter(2)*1000.), "L" );
  //legend->AddEntry( f1_gaus_corr, Form("Corr. (#sigma = %.1f ps)" , f1_gaus_corr->GetParameter(2)*1000.), "L" );
  //legend->Draw("same");

  c1->Clear();
  c1->SetLogy();

  TH2D* h2_axes_log = new TH2D( "axes_log", "", 10, xMin, xMax, 10, 0.1, 5.*h1_reso_corr->GetMaximum() );
  h2_axes_log->SetXTitle( "0.5 * ( t_{Left} + t_{Right} ) [ns]" );
  h2_axes_log->SetYTitle( "Entries" );
  h2_axes_log->Draw();

  text_raw ->Draw("same");
  text_corr->Draw("same");

  h1_reso      ->SetLineWidth(2); 
  h1_reso_corr ->SetLineWidth(2);

  h1_reso      ->Draw("same"); 
  h1_reso_corr ->Draw("same");

  f1_gaus      ->Draw("same"); 
  f1_gaus_corr ->Draw("same");

  BTLCommon::addLabels( c1, conf );

  c1->SaveAs( Form("plots/%s/reso%s_log.eps", conf.get_confName().c_str(), suffix.c_str()) );
  c1->SaveAs( Form("plots/%s/reso%s_log.pdf", conf.get_confName().c_str(), suffix.c_str()) );
  c1->SaveAs( Form("plots/%s/reso%s_log.png", conf.get_confName().c_str(), suffix.c_str()) );


  if( hodoCorr ) {

    h1_reso      ->SetLineWidth(1); 
    h1_reso_corr ->SetLineWidth(1);

    h1_reso_corr2->Draw("same");
    c1->SaveAs( Form("plots/%s/reso%s_withHodo_log.eps", conf.get_confName().c_str(), suffix.c_str()) );
    c1->SaveAs( Form("plots/%s/reso%s_withHodo_log.pdf", conf.get_confName().c_str(), suffix.c_str()) );
    c1->SaveAs( Form("plots/%s/reso%s_withHodo_log.png", conf.get_confName().c_str(), suffix.c_str()) );

  }

  TFile* outfile = TFile::Open( Form("plots/%s/resoFile%s.root", conf.get_confName().c_str(), suffix.c_str()), "RECREATE" );
  outfile->cd();

  h1_reso->Write();
  h1_reso_corr->Write();
  if( hodoCorr ) h1_reso_corr2->Write();

  //f1_gaus->Write();
  //f1_gaus_corr->Write();
  //if( hodoCorr ) f1_gaus_corr2->Write();

  outfile->Close();

  std::cout << "-> Saved reso info in: " << outfile->GetName() << std::endl;

  delete c1;
  delete h2_axes;
  delete h2_axes_log;

  delete h1_reso;
  delete h1_reso_corr;
  if( hodoCorr ) delete h1_reso_corr2;

}
