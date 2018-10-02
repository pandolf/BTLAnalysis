#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TH1D.h"
#include "TLine.h"

#include "../interface/BTLCommon.h"


bool do_ampWalk = true;
bool do_tDiff = false;



TF1* fitLandau( const std::string& outdir, TTree* tree, TH1D* histo, const std::string& varName, const std::string& axisName = "" );
TF1* fitGaus( const std::string& outdir, TH1D* histo, const std::string& axisName );
std::vector<float> getBins( int nBins, float xMin, float xMax );
TF1* doAmpWalkCorr( const std::string& fitsDir, const std::vector<TH1D*>& vh1_t, const std::vector<TH1D*>& vh1_ampMax, const std::string& name );



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
  TH1D* h1_ampMaxRight = new TH1D( "ampMaxRight", "", 100, 0.1 , 0.8  );

  TF1* fitLandauL = fitLandau( outdir, tree, h1_ampMaxLeft, "ampMaxLeft", "Max Amplitude Left [a.u.]" );
  TF1* fitLandauR = fitLandau( outdir, tree, h1_ampMaxRight, "ampMaxRight", "Max Amplitude Right [a.u.]" );

  float ampMaxLeft_maxCut  = fitLandauL->GetParameter(1)*3.;
  float ampMaxLeft_minCut  = fitLandauL->GetParameter(1)*0.8;

  float ampMaxRight_maxCut  = fitLandauR->GetParameter(1)*3.;
  float ampMaxRight_minCut  = fitLandauR->GetParameter(1)*0.8;

  float ampMaxLeft_maxBins = fitLandauL->GetParameter(1)*2.;
  float ampMaxLeft_minBins = fitLandauL->GetParameter(1)*0.8;

  float ampMaxRight_maxBins = fitLandauR->GetParameter(1)*2.;
  float ampMaxRight_minBins = fitLandauR->GetParameter(1)*0.8;

  
  std::string suffix = "";
  if( do_ampWalk ) suffix = suffix + "_AW";
  if( do_tDiff ) suffix = suffix + "_TD";
  TFile* outfile = TFile::Open( Form("ntuplesLite/%s_corr%s.root", confName.c_str(), suffix.c_str()), "RECREATE" );
  TTree* newtree = tree->CloneTree(0);

  float tLeft_corr;
  newtree->Branch( "tLeft_corr", &tLeft_corr );
  float tRight_corr;
  newtree->Branch( "tRight_corr", &tRight_corr );

  int nBins_ampMax = 15;
  std::vector<float> bins_ampMaxLeft  = getBins( nBins_ampMax, ampMaxLeft_minBins , ampMaxLeft_maxBins  );
  std::vector<float> bins_ampMaxRight = getBins( nBins_ampMax, ampMaxRight_minBins, ampMaxRight_maxBins );

  std::vector< TH1D* > vh1_tLeft, vh1_tRight;
  std::vector< TH1D* > vh1_ampMaxLeft, vh1_ampMaxRight;

  for( unsigned i=0; i<nBins_ampMax-1; ++i ) {

    TH1D* h1_tLeft = new TH1D( Form("tLeft_bin%d", i), "", 100, 2., 4. );
    vh1_tLeft.push_back( h1_tLeft );

    TH1D* h1_ampMaxLeft = new TH1D( Form("ampMaxLeft_bin%d", i), "", 50, bins_ampMaxLeft[i], bins_ampMaxLeft[i+1] );
    vh1_ampMaxLeft.push_back( h1_ampMaxLeft );

    TH1D* h1_tRight = new TH1D( Form("tRight_bin%d", i), "", 100, 2., 4. );
    vh1_tRight.push_back( h1_tRight );

    TH1D* h1_ampMaxRight = new TH1D( Form("ampMaxRight_bin%d", i), "", 50, bins_ampMaxRight[i], bins_ampMaxRight[i+1] );
    vh1_ampMaxRight.push_back( h1_ampMaxRight );

  } // for bins_ampMax


  int nEntries = tree->GetEntries();

  
  for( int iEntry = 0; iEntry<nEntries; ++iEntry ) {

    if( iEntry % 10000 == 0 ) std::cout << " Entry: " << iEntry << " / " << nEntries << std::endl;

    tree->GetEntry( iEntry );

    if( ampMaxLeft>=ampMaxLeft_minCut && ampMaxLeft<=ampMaxLeft_maxCut ) {  // ampMaxLeft is good

      int thisBinLeft = -1;
      for( unsigned i=0; i<bins_ampMaxLeft.size()-1; ++i ) {
        if( ampMaxLeft>bins_ampMaxLeft[i] && ampMaxLeft<bins_ampMaxLeft[i+1] ) {
          thisBinLeft = i;
          break;
        } 
      }

      if( thisBinLeft<0 ) continue;

      vh1_tLeft[thisBinLeft]->Fill( tLeft );
      vh1_ampMaxLeft[thisBinLeft]->Fill( ampMaxLeft );

    } // if ampMaxLeft is good

    if( ampMaxRight>=ampMaxRight_minCut && ampMaxRight<=ampMaxRight_maxCut ) {  // ampMaxRight is good

      int thisBinRight = -1;
      for( unsigned i=0; i<bins_ampMaxRight.size()-1; ++i ) {
        if( ampMaxRight>bins_ampMaxRight[i] && ampMaxRight<bins_ampMaxRight[i+1] ) {
          thisBinRight = i;
          break;
        } 
      }

      if( thisBinRight<0 ) continue;

      vh1_tRight[thisBinRight]->Fill( tRight );
      vh1_ampMaxRight[thisBinRight]->Fill( ampMaxRight );

    } // if ampMaxRight is good

  } // for Entries


  std::string fitsDir(Form("%s/ampWalkFits", outdir.c_str()));
  system( Form("mkdir -p %s", fitsDir.c_str()) );

  TF1* f1_ampWalkLeft  = doAmpWalkCorr( fitsDir, vh1_tLeft , vh1_ampMaxLeft , "Left"  );
  TF1* f1_ampWalkRight = doAmpWalkCorr( fitsDir, vh1_tRight, vh1_ampMaxRight, "Right" );

 
  outfile->cd();

  for( unsigned i=0; i<vh1_tLeft     .size(); ++i ) vh1_tLeft[i]     ->Write();
  for( unsigned i=0; i<vh1_ampMaxLeft.size(); ++i ) vh1_ampMaxLeft[i]->Write();

  outfile->Close();

  return 0;

}



TF1* fitLandau( const std::string& outdir, TTree* tree, TH1D* histo, const std::string& varName, const std::string& axisName ) {

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
  if( axisName=="" )
    h2_axes->SetXTitle( varName.c_str() );
  else
    h2_axes->SetXTitle( axisName.c_str() );
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

  gPad->RedrawAxis();

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



TF1* fitGaus( const std::string& outdir, TH1D* histo, const std::string& axisName  ) {

  float mean_histo = histo->GetMean();
  float rms_histo  = histo->GetRMS();

  TF1* f1_gaus = new TF1( Form("gaus_%s", histo->GetName()), "gaus", mean_histo-rms_histo, mean_histo+rms_histo );
  f1_gaus->SetLineColor( 46 );
  
  histo->Fit( f1_gaus->GetName(), "RQ0" );

  float nSigma = 2.5;
  float xMin_fit = f1_gaus->GetParameter(1) - nSigma*f1_gaus->GetParameter(2);
  float xMax_fit = f1_gaus->GetParameter(1) + nSigma*f1_gaus->GetParameter(2);

  f1_gaus->SetRange( xMin_fit, xMax_fit );

  //std::cout << "Range: " << xMin_fit << " " << xMax_fit << std::endl;

  int n_iter = 5;

  for( int i=0; i<n_iter; ++i ) { // iterative fit

    if( i==n_iter-1 )
      histo->Fit( f1_gaus->GetName(), "RQ+" );
    else {
      histo->Fit( f1_gaus->GetName(), "RQ0" );
      xMin_fit = f1_gaus->GetParameter(1) - nSigma*f1_gaus->GetParameter(2);
      xMax_fit = f1_gaus->GetParameter(1) + nSigma*f1_gaus->GetParameter(2);
      f1_gaus->SetRange( xMin_fit, xMax_fit );
  //std::cout << "Range: " << xMin_fit << " " << xMax_fit << std::endl;
    }

  } // for iter

  TCanvas* c1 = new TCanvas( Form("c1_%s", histo->GetName()), "", 600, 600 );
  c1->cd();

  histo->SetXTitle( axisName.c_str() );
  histo->SetYTitle( "Entries" );
  histo->Draw();

  BTLCommon::addLabels( c1 );

  gPad->RedrawAxis();

  c1->SaveAs( Form("%s/%s.eps", outdir.c_str(), histo->GetName()) );
  c1->SaveAs( Form("%s/%s.pdf", outdir.c_str(), histo->GetName()) );

  delete c1;

  return f1_gaus;

}



TF1* doAmpWalkCorr( const std::string& fitsDir, const std::vector<TH1D*>& vh1_t, const std::vector<TH1D*>& vh1_ampMax, const std::string& name ) {


  float ampMax_min = vh1_ampMax[0]->GetXaxis()->GetXmin();
  float ampMax_max = vh1_ampMax[vh1_ampMax.size()-1]->GetXaxis()->GetXmax();

  TGraphErrors* gr_ampWalk = new TGraphErrors(0);
  gr_ampWalk->SetName( Form("gr_ampWalk%s", name.c_str()) );

  for( unsigned i=0; i<vh1_t.size(); ++i ) {

    TF1* f1_gaus = fitGaus( fitsDir, vh1_t[i], Form("t_{%s} [ns]", name.c_str()) );

    float x     = vh1_ampMax[i]->GetMean();
    float x_err = vh1_ampMax[i]->GetMeanError();

    float y     = f1_gaus->GetParameter( 1 );
    float y_err = f1_gaus->GetParError ( 1 );

    int iPoint = gr_ampWalk->GetN();
    gr_ampWalk->SetPoint     ( iPoint, x    , y     );
    gr_ampWalk->SetPointError( iPoint, x_err, y_err );

  } // for points


  TCanvas* c1= new TCanvas( Form("c1_ampWalk%s", name.c_str()), "", 600, 600 );
  c1->cd();

  TH2D* h2_axes = new TH2D( "axes", "", 10, ampMax_min, ampMax_max, 10, 2., 3.5 );
  h2_axes->SetXTitle( "Max Amplitude [a.u.]" );
  h2_axes->SetYTitle( Form("t_{%s} - t_{PTK} [ns]", name.c_str()) );
  h2_axes->Draw();
  
  gr_ampWalk->SetMarkerStyle(20);
  gr_ampWalk->SetMarkerSize(1.6);

  TF1* f1_ampWalk = new TF1( Form("fit_ampWalk%s", name.c_str()), "pol3", ampMax_min, ampMax_max );
  f1_ampWalk->SetLineColor( 46 );
  gr_ampWalk->Fit( f1_ampWalk->GetName(), "RQ" );

  gr_ampWalk->Draw( "P same" );

  BTLCommon::addLabels( c1 );

  gPad->RedrawAxis();

  c1->SaveAs( Form("%s/ampWalk%s.eps", fitsDir.c_str(), name.c_str()) );
  c1->SaveAs( Form("%s/ampWalk%s.pdf", fitsDir.c_str(), name.c_str()) );

  delete c1;

  return f1_ampWalk;

}
