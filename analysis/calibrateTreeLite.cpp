#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TH1D.h"



bool do_ampWalk = true;
bool do_tDiff = false;



TF1* fitLandau( const std::string& outdir, TTree* tree, TH1D* histo, const std::string& varName );
TF1* fitGaus( const std::string& outdir, TH1D* histo );
std::vector<float> getBins( int nBins, float xMin, float xMax );



int main( int argc, char* argv[] ) {


  std::string confName="";

  if( argc>1 ) {

    confName = std::string(argv[1]);

  } else {

    std::cout << "USAGE: ./calibrateTreeLite [confName]" << std::endl;
    exit(1);

  }


  TFile* file = TFile::Open( Form("ntuplesLite/%s.root", confName.c_str()) );
  TTree* tree = (TTree*)file->Get("digiLite");

  float tL;
  tree->SetBranchAddress( "tL", &tL );
  float tR;
  tree->SetBranchAddress( "tR", &tR );
  float ampMaxL;
  tree->SetBranchAddress( "ampMaxL", &ampMaxL );
  float ampMaxR;
  tree->SetBranchAddress( "ampMaxR", &ampMaxR );

  std::string outdir( Form("plots/%s", confName.c_str()) );
  system( Form("mkdir -p %s", outdir.c_str()) );


  // first of all fit landau to find ampMax range:
  TH1D* h1_ampMaxL = new TH1D( "ampMaxL", "", 100, 0.1 , 0.8  );
  TH1D* h1_ampMaxR = new TH1D( "ampMaxR", "", 100, 0., 0.25 );

  TF1* fitLandauL = fitLandau( outdir, tree, h1_ampMaxL, "ampMaxL" );
  TF1* fitLandauR = fitLandau( outdir, tree, h1_ampMaxR, "ampMaxR" );

  float ampMaxL_maxCut  = fitLandauL->GetParameter(1)*3.;
  float ampMaxL_minCut  = fitLandauL->GetParameter(1)*0.8;

  float ampMaxR_maxCut  = fitLandauR->GetParameter(1)*3.;
  float ampMaxR_minCut  = fitLandauR->GetParameter(1)*0.8;

  float ampMaxL_maxBins = fitLandauL->GetParameter(1)*2.;
  float ampMaxL_minBins = fitLandauL->GetParameter(1)*0.8;

  float ampMaxR_maxBins = fitLandauR->GetParameter(1)*2.;
  float ampMaxR_minBins = fitLandauR->GetParameter(1)*0.8;

  
  std::string suffix = "";
  if( do_ampWalk ) suffix = suffix + "_AW";
  if( do_tDiff ) suffix = suffix + "_TD";
  TFile* outfile = TFile::Open( Form("ntuplesLite/%s_corr%s.root", confName.c_str(), suffix.c_str()), "RECREATE" );
  TTree* newTree = tree->CloneTree(0);

  float tLcorr;
  newTree->Branch( "tLcorr", &tLcorr );
  float tRcorr;
  newTree->Branch( "tRcorr", &tRcorr );

  int nBins_ampMax = 10;
  std::vector<float> bins_ampMaxL = getBins( nBins_ampMax, ampMaxL_minBins, ampMaxL_maxBins );
  std::vector<float> bins_ampMaxR = getBins( nBins_ampMax, ampMaxR_minBins, ampMaxL_maxBins );

  std::vector< TH1D* > vh1_tL;
  std::vector< TH1D* > vh1_ampMaxL;

  for( unsigned i=0; i<bins_ampMaxL.size()-1; ++i ) {

    TH1D* h1_tL = new TH1D( Form("tL_bin%d", i), "", 100, 2., 4. );
    vh1_tL.push_back( h1_tL );

    TH1D* h1_ampMaxL = new TH1D( Form("ampMaxL_bin%d", i), "", 50, bins_ampMaxL[i], bins_ampMaxL[i+1] );
    vh1_ampMaxL.push_back( h1_ampMaxL );

  } // for bins_ampMax


  int nentries = tree->GetEntries();

  
  for( unsigned iEntry = 0; iEntry<nentries; ++iEntry ) {

    if( iEntry % 100000 == 0 ) std::cout << " Entry: " << iEntry << " / " << nentries << std::endl;

    tree->GetEntry( iEntry );

    if( ampMaxL>=ampMaxL_minCut && ampMaxL<=ampMaxL_maxCut ) {  // ampMaxL is good

      int thisBinL = -1;
      for( unsigned i=0; i<bins_ampMaxL.size()-1; ++i ) {
        if( ampMaxL>bins_ampMaxL[i] && ampMaxL<bins_ampMaxL[i+1] ) {
          thisBinL = i;
          break;
        } 
      }

      if( thisBinL<0 ) continue;

      vh1_tL[thisBinL]->Fill( tL );
      vh1_ampMaxL[thisBinL]->Fill( ampMaxL );

    } // if ampMaxL is good

  } // for entries


  TGraphErrors* gr_ampWalkL = new TGraphErrors(0);

  std::string fitsDir(Form("%s/ampWalkFits", outdir.c_str()));
  system( Form("mkdir -p %s", fitsDir.c_str()) );

  for( unsigned i=0; i<vh1_tL.size(); ++i ) {

    TF1* f1_gausL = fitGaus( fitsDir, vh1_tL[i] );

    float xL     = vh1_ampMaxL[i]->GetMean();
    float xL_err = vh1_ampMaxL[i]->GetMeanError();

    float yL     = f1_gausL->GetParameter( 1 );
    float yL_err = f1_gausL->GetParError ( 1 );

    int iL = gr_ampWalkL->GetN();
    gr_ampWalkL->SetPoint     ( iL, xL    , yL     );
    gr_ampWalkL->SetPointError( iL, xL_err, yL_err );

  } // for points


  outfile->cd();

  for( unsigned i=0; i<vh1_tL     .size(); ++i ) vh1_tL[i]     ->Write();
  for( unsigned i=0; i<vh1_ampMaxL.size(); ++i ) vh1_ampMaxL[i]->Write();

  outfile->Close();

  return 0;

}



TF1* fitLandau( const std::string& outdir, TTree* tree, TH1D* histo, const std::string& varName ) {

  tree->Project( histo->GetName(), varName.c_str(), "" );

  float xMode = histo->GetBinCenter(histo->GetMaximumBin());
  float xMin_fit = xMode*0.8;
  float xMax_fit = xMode*3.;

  TF1* f1_landau = new TF1( Form("landau_%s", varName.c_str()), "landau", xMin_fit, xMax_fit );
  
  histo->Fit( f1_landau->GetName(), "R0" );

  xMin_fit = 0.9*f1_landau->GetParameter(1);
  xMax_fit = 1.4*f1_landau->GetParameter(1);

  f1_landau->SetRange( xMin_fit, xMax_fit );

  int n_iter = 5;

  for( unsigned i=0; i<n_iter; ++i ) { // iterative fit

    if( i==n_iter-1 )
      histo->Fit( f1_landau->GetName(), "R+" );
    else {
      histo->Fit( f1_landau->GetName(), "R0" );
      xMin_fit = 0.9*f1_landau->GetParameter(1);
      xMax_fit = 1.4*f1_landau->GetParameter(1);
      f1_landau->SetRange( xMin_fit, xMax_fit );
    }

  } // for iter

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  histo->Draw();

  c1->SaveAs( Form("%s/%s.eps", outdir.c_str(), histo->GetName()) );
  c1->SaveAs( Form("%s/%s.pdf", outdir.c_str(), histo->GetName()) );

  return f1_landau;

}


std::vector<float> getBins( int nBins, float xMin, float xMax ) {

  float step = (xMax-xMin)/((float)nBins);

  std::vector<float> bins;

  for( unsigned i=0; i<nBins; ++i )
    bins.push_back( xMin + (float)i*step );

  return bins;

}



TF1* fitGaus( const std::string& outdir, TH1D* histo ) {

  float mean_histo = histo->GetMean();
  float rms_histo  = histo->GetRMS();

  TF1* f1_gaus = new TF1( Form("gaus_%s", histo->GetName()), "gaus", mean_histo-rms_histo, mean_histo+rms_histo );
  
  histo->Fit( f1_gaus->GetName(), "R0" );

  float nSigma = 2.5;
  float xMin_fit = f1_gaus->GetParameter(1) - nSigma*f1_gaus->GetParameter(2);
  float xMax_fit = f1_gaus->GetParameter(1) + nSigma*f1_gaus->GetParameter(2);

  f1_gaus->SetRange( xMin_fit, xMax_fit );

  //std::cout << "Range: " << xMin_fit << " " << xMax_fit << std::endl;

  int n_iter = 5;

  for( unsigned i=0; i<n_iter; ++i ) { // iterative fit

    if( i==n_iter-1 )
      histo->Fit( f1_gaus->GetName(), "R+" );
    else {
      histo->Fit( f1_gaus->GetName(), "RQ0" );
      xMin_fit = f1_gaus->GetParameter(1) - nSigma*f1_gaus->GetParameter(2);
      xMax_fit = f1_gaus->GetParameter(1) + nSigma*f1_gaus->GetParameter(2);
      f1_gaus->SetRange( xMin_fit, xMax_fit );
  //std::cout << "Range: " << xMin_fit << " " << xMax_fit << std::endl;
    }

  } // for iter

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  histo->Draw();

  c1->SaveAs( Form("%s/%s.eps", outdir.c_str(), histo->GetName()) );
  c1->SaveAs( Form("%s/%s.pdf", outdir.c_str(), histo->GetName()) );

  return f1_gaus;

}
