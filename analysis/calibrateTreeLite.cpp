#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TH1D.h"
#include "TLine.h"
#include "TLegend.h"

#include "../interface/BTLCommon.h"


bool do_ampWalk = true;
bool do_tDiff = false;



TF1* fitLandau( const std::string& outdir, TTree* tree, TH1D* histo, const std::string& varName );
std::vector<float> getBins( int nBins, float xMin, float xMax );
int findBin( float var, std::vector<float> bins );
TF1* getAmpWalkCorr( const std::string& fitsDir, const std::vector<TH1D*>& vh1_t, const std::vector<TH1D*>& vh1_ampMax, const std::string& name );



int main( int argc, char* argv[] ) {


  std::string confName="";

  if( argc>1 ) {

    confName = std::string(argv[1]);

  } else {

    std::cout << "USAGE: ./calibrateTreeLite [confName]" << std::endl;
    exit(1);

  }

  BTLCommon::setStyle();


  TFile* file = TFile::Open( Form("ntuplesLite/%s.root", confName.c_str()) );
  TTree* tree = (TTree*)file->Get("digiLite");

  float tLeft;
  tree->SetBranchAddress( "tLeft", &tLeft );
  float tRight;
  tree->SetBranchAddress( "tRight", &tRight );
  float ampMaxLeft;
  tree->SetBranchAddress( "ampMaxLeft", &ampMaxLeft );
  float ampMaxRight;
  tree->SetBranchAddress( "ampMaxRight", &ampMaxRight );

  std::string outdir( Form("plots/%s", confName.c_str()) );
  system( Form("mkdir -p %s", outdir.c_str()) );


  // first of all fit landau to find ampMax range:
  TH1D* h1_ampMaxLeft = new TH1D( "ampMaxLeft", "", 100, 0., 0.25 );
  h1_ampMaxLeft ->SetXTitle( "Max Amplitude [a.u.]" );
  TH1D* h1_ampMaxRight = new TH1D( "ampMaxRight", "", 100, 0.1 , 0.8  );
  h1_ampMaxRight->SetXTitle( "Max Amplitude [a.u.]" );

  TF1* fitLandauL = fitLandau( outdir, tree, h1_ampMaxLeft , "ampMaxLeft"  );
  TF1* fitLandauR = fitLandau( outdir, tree, h1_ampMaxRight, "ampMaxRight" );

  float ampMaxLeft_maxCut  = fitLandauL->GetParameter(1)*3.;
  float ampMaxLeft_minCut  = fitLandauL->GetParameter(1)*0.8;

  float ampMaxRight_maxCut  = fitLandauR->GetParameter(1)*3.;
  float ampMaxRight_minCut  = fitLandauR->GetParameter(1)*0.8;

  float ampMaxLeft_maxBins = fitLandauL->GetParameter(1)*2.;
  float ampMaxLeft_minBins = fitLandauL->GetParameter(1)*0.8;

  float ampMaxRight_maxBins = fitLandauR->GetParameter(1)*2.;
  float ampMaxRight_minBins = fitLandauR->GetParameter(1)*0.8;

  
  int nBins_ampMax = 15;
  std::vector<float> bins_ampMaxLeft  = getBins( nBins_ampMax, ampMaxLeft_minBins , ampMaxLeft_maxBins  );
  std::vector<float> bins_ampMaxRight = getBins( nBins_ampMax, ampMaxRight_minBins, ampMaxRight_maxBins );

  std::vector< TH1D* > vh1_tLeft, vh1_tRight;
  std::vector< TH1D* > vh1_ampMaxLeft, vh1_ampMaxRight;

  int nBinsT = 100;
  float tMin = 2.;
  float tMax = 4.;

  TH1D* h1_tLeft  = new TH1D( "tLeft" , "", nBinsT, tMin, tMax );
  TH1D* h1_tRight = new TH1D( "tRight", "", nBinsT, tMin, tMax );

  for( unsigned i=0; i<nBins_ampMax-1; ++i ) {

    TH1D* h1_tLeft = new TH1D( Form("tLeft_bin%d", i), "", nBinsT, tMin, tMax );
    h1_tLeft->SetXTitle( "t_{Left} [ns]" );
    //h1_tLeft->SetYTitle( "Entries" );
    vh1_tLeft.push_back( h1_tLeft );

    TH1D* h1_ampMaxLeft = new TH1D( Form("ampMaxLeft_bin%d", i), "", 50, bins_ampMaxLeft[i], bins_ampMaxLeft[i+1] );
    h1_ampMaxLeft->SetXTitle( "Max Amplitude Left [a.u.]" );
    //h1_ampMaxLeft->SetYTitle( "Entries" );
    vh1_ampMaxLeft.push_back( h1_ampMaxLeft );

    TH1D* h1_tRight = new TH1D( Form("tRight_bin%d", i), "", nBinsT, tMin, tMax );
    h1_tRight->SetXTitle( "t_{Right} [ns]" );
    //h1_tRight->SetYTitle( "Entries" );
    vh1_tRight.push_back( h1_tRight );

    TH1D* h1_ampMaxRight = new TH1D( Form("ampMaxRight_bin%d", i), "", 50, bins_ampMaxRight[i], bins_ampMaxRight[i+1] );
    h1_ampMaxRight->SetXTitle( "Max Amplitude Right [a.u.]" );
    //h1_ampMaxRight->SetYTitle( "Entries" );
    vh1_ampMaxRight.push_back( h1_ampMaxRight );

  } // for bins_ampMax


  int nEntries = tree->GetEntries();

  
  for( int iEntry = 0; iEntry<nEntries; ++iEntry ) {

    if( iEntry % 10000 == 0 ) std::cout << " Entry: " << iEntry << " / " << nEntries << std::endl;

    tree->GetEntry( iEntry );

    if( ampMaxLeft>=ampMaxLeft_minCut && ampMaxLeft<=ampMaxLeft_maxCut ) {  // ampMaxLeft is good

      int thisBinLeft = findBin( ampMaxLeft, bins_ampMaxLeft );

      if( thisBinLeft>=0 ) {
        h1_tLeft->Fill( tLeft );
        vh1_tLeft[thisBinLeft]->Fill( tLeft );
        vh1_ampMaxLeft[thisBinLeft]->Fill( ampMaxLeft );
      }

    } // if ampMaxLeft is good

    if( ampMaxRight>=ampMaxRight_minCut && ampMaxRight<=ampMaxRight_maxCut ) {  // ampMaxRight is good

      int thisBinRight = findBin( ampMaxRight, bins_ampMaxRight );

      if( thisBinRight>=0 ) {
        h1_tRight->Fill( tRight );
        vh1_tRight[thisBinRight]->Fill( tRight );
        vh1_ampMaxRight[thisBinRight]->Fill( ampMaxRight );
      }

    } // if ampMaxRight is good

  } // for Entries


  std::string fitsDir(Form("%s/ampWalkFits", outdir.c_str()));
  system( Form("mkdir -p %s/bins", fitsDir.c_str()) );

  TF1* f1_ampWalkLeft  = getAmpWalkCorr( fitsDir, vh1_tLeft , vh1_ampMaxLeft , "Left"  );
  TF1* f1_ampWalkRight = getAmpWalkCorr( fitsDir, vh1_tRight, vh1_ampMaxRight, "Right" );

  float target_ampWalkLeft  = h1_tLeft ->GetMean();
  float target_ampWalkRight = h1_tRight->GetMean();
  //float target_ampWalkLeft  = f1_ampWalkLeft ->Eval( bins_ampMaxLeft [0] );
  //float target_ampWalkRight = f1_ampWalkRight->Eval( bins_ampMaxRight[0] );


  // prepare new file
  std::string suffix = "";
  if( do_ampWalk ) suffix = suffix + "_AW";
  if( do_tDiff ) suffix = suffix + "_TD";
  TFile* outfile = TFile::Open( Form("ntuplesLite/%s_corr%s.root", confName.c_str(), suffix.c_str()), "RECREATE" );
  TTree* newtree = tree->CloneTree(0);

  float tLeft_corr;
  newtree->Branch( "tLeft_corr", &tLeft_corr );
  float tRight_corr;
  newtree->Branch( "tRight_corr", &tRight_corr );

  std::vector< TH1D* > vh1_tLeft_corr, vh1_tRight_corr;

  for( unsigned i=0; i<nBins_ampMax-1; ++i ) {

    TH1D* h1_tLeft_corr = new TH1D( Form("tLeft_corr_bin%d", i), "", 100, 2., 4. );
    vh1_tLeft_corr.push_back( h1_tLeft_corr );

    TH1D* h1_tRight_corr = new TH1D( Form("tRight_corr_bin%d", i), "", 100, 2., 4. );
    vh1_tRight_corr.push_back( h1_tRight_corr );

  } // for bins_ampMax


  for( int iEntry = 0; iEntry<nEntries; ++iEntry ) {

    tree->GetEntry( iEntry );

    if( ampMaxLeft <=ampMaxLeft_minCut  || ampMaxLeft >=ampMaxLeft_maxCut  ) continue;
    if( ampMaxRight<=ampMaxRight_minCut || ampMaxRight>=ampMaxRight_maxCut ) continue;

    tLeft_corr  = tLeft  * ( target_ampWalkLeft  / f1_ampWalkLeft ->Eval( ampMaxLeft  ) );
    tRight_corr = tRight * ( target_ampWalkRight / f1_ampWalkRight->Eval( ampMaxRight ) );

    int thisBinLeft  = findBin( ampMaxLeft , bins_ampMaxLeft  );
    if( thisBinLeft>=0 ) 
      vh1_tLeft_corr [thisBinLeft] ->Fill( tLeft_corr  );

    int thisBinRight = findBin( ampMaxRight, bins_ampMaxRight );
    if( thisBinRight>=0 ) 
      vh1_tRight_corr[thisBinRight]->Fill( tRight_corr );

    newtree->Fill();

  } // for entries


  TF1* f1_ampWalkLeft_corr  = getAmpWalkCorr( fitsDir, vh1_tLeft_corr , vh1_ampMaxLeft , "Left_corr"  );
  TF1* f1_ampWalkRight_corr = getAmpWalkCorr( fitsDir, vh1_tRight_corr, vh1_ampMaxRight, "Right_corr" );
 
  outfile->cd();

  newtree->Write();
  
  h1_tLeft ->Draw();
  h1_tRight->Draw();

  //for( unsigned i=0; i<vh1_tLeft      .size(); ++i ) vh1_tLeft[i]      ->Write();
  //for( unsigned i=0; i<vh1_tRight     .size(); ++i ) vh1_tRight[i]     ->Write();
  //for( unsigned i=0; i<vh1_tLeft_corr .size(); ++i ) vh1_tLeft_corr[i] ->Write();
  //for( unsigned i=0; i<vh1_tRight_corr.size(); ++i ) vh1_tRight_corr[i]->Write();
  //for( unsigned i=0; i<vh1_ampMaxLeft .size(); ++i ) vh1_ampMaxLeft[i] ->Write();
  //for( unsigned i=0; i<vh1_ampMaxRight.size(); ++i ) vh1_ampMaxRight[i]->Write();

  outfile->Close();

  return 0;

}



TF1* fitLandau( const std::string& outdir, TTree* tree, TH1D* histo, const std::string& varName ) {

  tree->Project( histo->GetName(), varName.c_str(), "" );

  float xMode = histo->GetBinCenter(histo->GetMaximumBin());
  float xMin_fit = xMode*0.8;
  float xMax_fit = xMode*3.;

  TF1* f1_landau = new TF1( Form("landau_%s", varName.c_str()), "landau", xMin_fit, xMax_fit );
  f1_landau->SetLineColor(46);

  histo->Fit( f1_landau->GetName(), "RQ0" );

  xMin_fit = 0.9*f1_landau->GetParameter(1);
  xMax_fit = 1.4*f1_landau->GetParameter(1);

  f1_landau->SetRange( xMin_fit, xMax_fit );

  int n_iter = 5;

  for( int i=0; i<n_iter; ++i ) { // iterative fit

    if( i==n_iter-1 )
      histo->Fit( f1_landau->GetName(), "RQ+" );
    else {
      histo->Fit( f1_landau->GetName(), "RQ0" );
      xMin_fit = 0.9*f1_landau->GetParameter(1);
      xMax_fit = 1.4*f1_landau->GetParameter(1);
      f1_landau->SetRange( xMin_fit, xMax_fit );
    }

  } // for iter

  TCanvas* c1 = new TCanvas( Form("c1_%s", histo->GetName()), "", 600, 600 );
  c1->cd();

  float yMax = 1.3*histo->GetMaximum();
  TH2D* h2_axes = new TH2D( Form("axes_%s", histo->GetName()), "", 10, histo->GetXaxis()->GetXmin(), histo->GetXaxis()->GetXmax(), 10, 0., yMax );
  h2_axes->SetXTitle( histo->GetXaxis()->GetTitle() );
  h2_axes->SetYTitle( "Entries" );
  h2_axes->Draw();

  TLine* line_cutMin = new TLine( 0.8*f1_landau->GetParameter(1), 0., 0.8*f1_landau->GetParameter(1), yMax );
  TLine* line_cutMax = new TLine(  3.*f1_landau->GetParameter(1), 0.,  3.*f1_landau->GetParameter(1), yMax );
 
  line_cutMin->SetLineStyle(2);
  line_cutMax->SetLineStyle(2);

  line_cutMin->SetLineColor(46);
  line_cutMax->SetLineColor(46);

  line_cutMin->Draw("same");
  line_cutMax->Draw("same");

  histo->Draw("same");

  BTLCommon::addLabels( c1 );

  c1->SaveAs( Form("%s/%s.eps", outdir.c_str(), histo->GetName()) );
  c1->SaveAs( Form("%s/%s.pdf", outdir.c_str(), histo->GetName()) );

  delete c1;
  
  return f1_landau;

}


std::vector<float> getBins( int nBins, float xMin, float xMax ) {

  float step = (xMax-xMin)/((float)nBins);

  std::vector<float> bins;

  for( int i=0; i<nBins; ++i )
    bins.push_back( xMin + (float)i*step );

  return bins;

}



int findBin( float var, std::vector<float> bins ) {

  int bin = -1;

  for( unsigned i=0; i<bins.size()-1; ++i ) {
    if( var>bins[i] && var<bins[i+1] ) {
      bin = i;
      break;
    } 
  }

  return bin;

}




TF1* getAmpWalkCorr( const std::string& fitsDir, const std::vector<TH1D*>& vh1_t, const std::vector<TH1D*>& vh1_ampMax, const std::string& name ) {


  float ampMax_min = vh1_ampMax[0]->GetXaxis()->GetXmin();
  float ampMax_max = vh1_ampMax[vh1_ampMax.size()-1]->GetXaxis()->GetXmax();

  TGraphErrors* gr_ampWalk = new TGraphErrors(0);
  gr_ampWalk->SetName( Form("gr_ampWalk%s", name.c_str()) );

  TGraph* gr_ampWalk_sigmaUp = new TGraph(0);
  gr_ampWalk_sigmaUp->SetName( Form("gr_ampWalk%s_sigmaUp", name.c_str()) );

  TGraph* gr_ampWalk_sigmaDn = new TGraph(0);
  gr_ampWalk_sigmaDn->SetName( Form("gr_ampWalk%s_sigmaDn", name.c_str()) );

  for( unsigned i=0; i<vh1_t.size(); ++i ) {

    TF1* f1_gaus = BTLCommon::fitGaus( vh1_t[i] );

    TCanvas* c1 = new TCanvas( Form("c1_%s", vh1_t[i]->GetName()), "", 600, 600 );
    c1->cd();

    vh1_t[i]->Draw();

    BTLCommon::addLabels( c1 );

    c1->SaveAs( Form("%s/bins/%s.eps", fitsDir.c_str(), vh1_t[i]->GetName()) );
    c1->SaveAs( Form("%s/bins/%s.pdf", fitsDir.c_str(), vh1_t[i]->GetName()) );

    delete c1;


    float x     = vh1_ampMax[i]->GetMean();
    float x_err = vh1_ampMax[i]->GetMeanError();

    float y     = f1_gaus->GetParameter( 1 );
    float y_err = f1_gaus->GetParError ( 1 );
    float y_rms = f1_gaus->GetParameter( 2 );

    int iPoint = gr_ampWalk->GetN();
    gr_ampWalk->SetPoint     ( iPoint, x    , y     );
    gr_ampWalk->SetPointError( iPoint, x_err, y_err );

    gr_ampWalk_sigmaUp->SetPoint( iPoint, x    , y+y_rms     );
    gr_ampWalk_sigmaDn->SetPoint( iPoint, x    , y-y_rms     );

  } // for points


  TCanvas* c1= new TCanvas( Form("c1_ampWalk%s", name.c_str()), "", 600, 600 );
  c1->cd();

  TH2D* h2_axes = new TH2D( Form("axes%s", name.c_str()), "", 10, ampMax_min, ampMax_max, 10, 2.3, 3.5 );
  h2_axes->SetXTitle( "Max Amplitude [a.u.]" );
  h2_axes->SetYTitle( Form("t_{%s} - t_{PTK} [ns]", name.c_str()) );
  h2_axes->Draw();
  
  gr_ampWalk->SetMarkerStyle(20);
  gr_ampWalk->SetMarkerSize(1.3);

  gr_ampWalk_sigmaUp->SetLineStyle(2);
  gr_ampWalk_sigmaUp->SetLineWidth(2);
  gr_ampWalk_sigmaUp->SetLineColor(38);

  gr_ampWalk_sigmaDn->SetLineStyle(2);
  gr_ampWalk_sigmaDn->SetLineWidth(2);
  gr_ampWalk_sigmaDn->SetLineColor(38);

  TF1* f1_ampWalk = new TF1( Form("fit_ampWalk%s", name.c_str()), "pol3", ampMax_min, ampMax_max );
  f1_ampWalk->SetLineColor( 46 );
  gr_ampWalk->Fit( f1_ampWalk->GetName(), "RQ" );

  gr_ampWalk->Draw( "P same" );
  gr_ampWalk_sigmaDn->Draw("L same");
  gr_ampWalk_sigmaUp->Draw("L same");

  TLegend* legend = new TLegend( 0.55, 0.7, 0.9, 0.9 );
  legend->SetTextSize(0.035);
  legend->SetFillColor(0);
  legend->AddEntry( gr_ampWalk, "Data", "P" );
  legend->AddEntry( gr_ampWalk_sigmaDn, "68% band", "L" );
  legend->AddEntry( f1_ampWalk, "Fit", "L" );
  legend->Draw("same");

  BTLCommon::addLabels( c1 );

  c1->SaveAs( Form("%s/ampWalk%s.eps", fitsDir.c_str(), name.c_str()) );
  c1->SaveAs( Form("%s/ampWalk%s.pdf", fitsDir.c_str(), name.c_str()) );

  delete c1;

  return f1_ampWalk;

}
