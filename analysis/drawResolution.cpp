#include <iostream>

#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TLegend.h"
#include "TPaveText.h"
#include "TF1.h"
#include "TGraphErrors.h"

#include "../interface/BTLCommon.h"
#include "../interface/BTLConf.h"




void drawResolution( BTLConf conf, const std::string& awType, TTree* tree, const std::string& name, const std::string& selection );
TH1D* getResolutionHisto( BTLConf conf, const std::string& name, TTree* tree, const std::string& varName, const std::string& selection );
void drawResolutionSmear( BTLConf conf, const std::string& awType, TTree* tree, const std::string& name, const std::string& selection );


int main( int argc, char* argv[] ) {

  
  if( argc==1 ) {

    std::cout << "USAGE: ./drawResolution [confName] [awType=4]" << std::endl;
    exit(1);

  }


  BTLCommon::setStyle();

  std::string confName( argv[1] );
  BTLConf conf(confName);

  std::string awType("aw4bins");

  if( argc>2 ) {

    int nBinsHodo = 1;
    bool centralAmpWalk = false;
    std::string argv2(argv[2]);
    if( argv2=="Center" || argv2=="center" || argv2=="central" || argv2=="Central" ) {
      nBinsHodo = 1;
      centralAmpWalk = true;
    } else {
      nBinsHodo = atoi(argv[2]);
    }

    if( centralAmpWalk ) {

      awType = "awCentral";
 
    } else {

      awType = std::string(Form("aw%dbins", nBinsHodo));

    }

  }


  std::cout << "-> Starting: " << conf.get_confName() << std::endl;

  std::string fileName(Form("treesLite/%s_%s.root", confName.c_str(), awType.c_str()));

  TFile* file = TFile::Open( fileName.c_str() );
  std::cout << "-> Opened: " << fileName << std::endl;
  TTree* tree = (TTree*)file->Get( "treeLite" );

  drawResolution( conf, awType, tree, "", "" );
  drawResolution( conf, awType, tree, "hodoOnBar"   , "hodoOnBar" );
  drawResolution( conf, awType, tree, "hodoFiducial", "hodoFiducial" );

  drawResolutionSmear( conf, awType, tree, "hodoOnBar", "hodoOnBar" );

  return 0;

}


void drawResolution( BTLConf conf, const std::string& awType, TTree* tree, const std::string& name, const std::string& selection ) {

  std::string suffix = "_" + awType;
  if( name!="" ) suffix = suffix + "_" + name;

  //int nBins = (int)( xMax-xMin )/0.0025;

  //TH1D* h1_reso       = new TH1D( Form("reso%s"      , suffix.c_str()), "", nBins, xMin, xMax );
  //TH1D* h1_reso_corr  = new TH1D( Form("reso_corr%s" , suffix.c_str()), "", nBins, xMin, xMax );
  //TH1D* h1_reso_corr2 = new TH1D( Form("reso_corr2%s", suffix.c_str()), "", nBins, xMin, xMax );

  TH1D* h1_reso       = getResolutionHisto( conf, "reso"     , tree, "0.5*(tLeft      + tRight     )", selection );
  TH1D* h1_reso_corr  = getResolutionHisto( conf, "reso_corr", tree, "0.5*(tLeft_corr + tRight_corr)", selection );


  std::string axisName = "t(ave) - t(MCP) [ns]";

  h1_reso->SetXTitle( axisName.c_str() );
  h1_reso->SetYTitle( "Entries" );

  h1_reso_corr->SetXTitle( axisName.c_str() );
  h1_reso_corr->SetYTitle( "Entries" );

  h1_reso->SetLineWidth( 2 );
  h1_reso->SetLineColor( 38 );
  h1_reso->SetFillColor( 38 );
  h1_reso->SetFillStyle( 0 );
  //h1_reso->SetFillStyle( 3004 );
  
  h1_reso_corr->SetLineWidth( 2 );
  h1_reso_corr->SetLineColor( 46 );
  h1_reso_corr->SetFillColor( 46 );
  h1_reso_corr->SetFillStyle( 0 );
  //h1_reso_corr->SetFillStyle( 3005 );
  
  //h1_reso_corr2->SetFillColor( kGray+2 );
  //h1_reso_corr2->SetFillStyle( 3006 );
  
  //if( h1_reso->GetEntries()<1000 ) {
  //  h1_reso->Rebin(2);
  //  h1_reso_corr->Rebin(2);
  //  if( hodoCorr ) h1_reso_corr2->Rebin(2);
  ////} else if( h1_reso->GetEntries()<2000 ) {
  ////  h1_reso->Rebin(2);
  ////  h1_reso_corr->Rebin(2);
  ////  if( hodoCorr ) h1_reso_corr2->Rebin(2);
  //}


  TH1D* h1_reso_corr2 = 0;

  TBranch* br_tAveCorr = tree->FindBranch( "tAveCorr" );

  bool hodoCorr = (br_tAveCorr != 0 );

  if( hodoCorr ) {

    h1_reso_corr2 = getResolutionHisto( conf, "reso_corr2", tree, "tAveCorr", selection );

    h1_reso_corr2->SetXTitle( axisName.c_str() );
    h1_reso_corr2->SetYTitle( "Entries" );

    h1_reso_corr2->SetLineWidth( 2 );
    h1_reso_corr2->SetLineColor( kGray+2 );

  } // if hodo corr


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

  float xMin = h1_reso->GetXaxis()->GetXmin();
  float xMax = h1_reso->GetXaxis()->GetXmax();

  TH2D* h2_axes = new TH2D( Form("axes%s", suffix.c_str()), "", 10, xMin, xMax, 10, 0., 1.1*h1_reso_corr->GetMaximum() );
  h2_axes->SetXTitle( axisName.c_str() );
  h2_axes->SetYTitle( "Entries" );
  h2_axes->Draw();

//  float peakPos = ( conf.digiConf()=="6a" ) ? 2.85 : 4.3;

  bool drawLeft = f1_gaus_corr->GetParameter(1)>0.5*(xMin+xMax);

  float xMin_text = ( drawLeft ) ? 0.2  : 0.6;
  float xMax_text = ( drawLeft ) ? 0.58 : 0.9;


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
  text_raw->AddText( Form("#sigma_{fit} = %.1f#pm%.1f ps", BTLCommon::subtractResoPTK(f1_gaus->GetParameter(2))*1000., f1_gaus->GetParError(2)*1000. ) );
  text_raw->SetTextAlign(11);
  text_raw->Draw("same");

  TPaveText* text_corr = new TPaveText( xMin_text, 0.54, xMax_text, 0.69, "brNDC" );
  text_corr->SetTextSize(0.035);
  text_corr->SetFillColor(0);
  text_corr->SetTextColor( 46 );
  text_corr->AddText( "Amp. Walk Corr." );
  text_corr->AddText( Form("#sigma_{eff} = %.1f ps", BTLCommon::subtractResoPTK(sigma_eff_corr)*1000.                ) );
  text_corr->AddText( Form("#sigma_{fit} = %.1f#pm%.1f ps", BTLCommon::subtractResoPTK(f1_gaus_corr->GetParameter(2))*1000., f1_gaus_corr->GetParError(2)*1000. ) );
  text_corr->SetTextAlign(11);
  text_corr->Draw("same");


  h1_reso     ->Draw("same"); 
  f1_gaus     ->Draw("same"); 
  h1_reso_corr->Draw("same");
  f1_gaus_corr->Draw("same");

  
  TPaveText* text_conf = (drawLeft) ? conf.get_labelConf(3) : conf.get_labelConf(4);
  text_conf->Draw("same");
  

  // avoid white boxes over the data
  h1_reso      ->Draw("same"); 
  h1_reso_corr ->Draw("same");

  BTLCommon::addLabels( c1, conf );

  c1->SaveAs( Form("plots/%s/reso%s.pdf", conf.get_confName().c_str(), suffix.c_str()) );
  c1->SaveAs( Form("plots/%s/eps/reso%s.eps", conf.get_confName().c_str(), suffix.c_str()) );
  c1->SaveAs( Form("plots/%s/png/reso%s.png", conf.get_confName().c_str(), suffix.c_str()) );



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

    c1->SaveAs( Form("plots/%s/reso%s_withHodo.pdf", conf.get_confName().c_str(), suffix.c_str()) );
    c1->SaveAs( Form("plots/%s/eps/reso%s_withHodo.eps", conf.get_confName().c_str(), suffix.c_str()) );
    c1->SaveAs( Form("plots/%s/png/reso%s_withHodo.png", conf.get_confName().c_str(), suffix.c_str()) );

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
  h2_axes_log->SetXTitle( axisName.c_str() );
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

  c1->SaveAs( Form("plots/%s/reso%s_log.pdf", conf.get_confName().c_str(), suffix.c_str()) );
  c1->SaveAs( Form("plots/%s/eps/reso%s_log.eps", conf.get_confName().c_str(), suffix.c_str()) );
  c1->SaveAs( Form("plots/%s/png/reso%s_log.png", conf.get_confName().c_str(), suffix.c_str()) );


  if( hodoCorr ) {

    h1_reso      ->SetLineWidth(1); 
    h1_reso_corr ->SetLineWidth(1);

    h1_reso_corr2->Draw("same");
    c1->SaveAs( Form("plots/%s/reso%s_withHodo_log.pdf", conf.get_confName().c_str(), suffix.c_str()) );
    c1->SaveAs( Form("plots/%s/eps/reso%s_withHodo_log.eps", conf.get_confName().c_str(), suffix.c_str()) );
    c1->SaveAs( Form("plots/%s/png/reso%s_withHodo_log.png", conf.get_confName().c_str(), suffix.c_str()) );

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
  if( h1_reso_corr2!=0 ) delete h1_reso_corr2;

}


TH1D* getResolutionHisto( BTLConf conf, const std::string& name, TTree* tree, const std::string& varName, const std::string& selection ) {

  float xMin = (conf.digiChSet()=="a") ? 2.4001 : 3.6001;
  float xMax = (conf.digiChSet()=="a") ? 3.429 : 4.99;
  if( conf.get_confName()=="Conf_5_a_500_28" ) {
    xMin += 0.8;
    xMax += 0.5;
  }
  int nBins = (int)( xMax-xMin )/0.0025;

  TH1D* histo = new TH1D( name.c_str(), "", nBins, xMin, xMax );

  tree->Project( histo->GetName(), varName.c_str(), selection.c_str() );

  return histo;

}



void drawResolutionSmear( BTLConf conf, const std::string& awType, TTree* tree, const std::string& name, const std::string& selection ) {

  std::vector<int> smearings;
  smearings.push_back( 3  );
  smearings.push_back( 5  );
  smearings.push_back( 10 );
  smearings.push_back( 15 );
  smearings.push_back( 20 );

  std::vector<TH1D*> vh1_resoSmear;
  vh1_resoSmear.push_back( getResolutionHisto( conf, "reso", tree, "0.5*(tLeft_corr + tRight_corr)", selection ) );

  for( unsigned i=0; i<smearings.size(); ++i )
    vh1_resoSmear.push_back( getResolutionHisto( conf, Form("reso_%d", smearings[i]), tree, Form("0.5*(tLeft_corr_smear%d + tRight_corr_smear%d)", smearings[i], smearings[i]), selection ) );

  TGraphErrors* graph_resoSmear = new TGraphErrors(0);

  float nSigma = 1.7;
  TF1* f1_gaus = BTLCommon::fitGaus( vh1_resoSmear[0], nSigma );
  float reso = BTLCommon::subtractResoPTK( 1000.*f1_gaus->GetParameter(2) );
  float resoErr = 1000.*f1_gaus->GetParError(2);

  for( unsigned i=0; i<smearings.size(); ++i ) {

    int iPoint = graph_resoSmear->GetN();

    TF1* f1_gaus_smear = BTLCommon::fitGaus( vh1_resoSmear[i+1] , nSigma );

    float x = smearings[i];

    float resoSmear = BTLCommon::subtractResoPTK( 1000.*f1_gaus_smear->GetParameter(2) );
    float resoSmearErr = 1000.*f1_gaus_smear->GetParError(2);

    float y = sqrt( resoSmear*resoSmear - reso*reso );
    float y_err = sqrt( resoErr*resoErr + resoSmearErr*resoSmearErr );

    graph_resoSmear->SetPoint( iPoint, x, y );
    graph_resoSmear->SetPointError( iPoint, 0., y_err );
  
  } // for smearings

  graph_resoSmear->SetMarkerStyle(20);
  graph_resoSmear->SetMarkerSize(1.5);
  //graph_resoSmear->SetMarkerColor(46);
  //graph_resoSmear->SetLineColor(46);

  float xMin = 0.;
  float xMax = 25.;

  TF1* f1_line = new TF1("lineSmear", "[0]*x", xMin, xMax );
  f1_line->SetLineColor(46);
  graph_resoSmear->Fit( f1_line, "QR0" );
  
  TCanvas* c1 = new TCanvas( "c1_smear", "", 600, 600 );
  c1->cd();

  TH2D* h2_axes = new TH2D( "axes_smear", "", 10, xMin, xMax, 10, 0., 60. );
  h2_axes->SetXTitle( "Amplitude Extra Smearing [%]" );
  h2_axes->SetYTitle( "Time Resolution Extra Smearing [ps]" );
  h2_axes->Draw();

  f1_line->Draw("L same");
  graph_resoSmear->Draw("P same");

  TPaveText* labelConf = conf.get_labelConf(4);
  labelConf->Draw("same");

  BTLCommon::addLabels( c1, conf );

  c1->SaveAs( Form( "plots/%s/reso_vs_smear.pdf", conf.get_confName().c_str()) );

  delete h2_axes;
  delete c1;

}
