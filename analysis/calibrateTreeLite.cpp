#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TH1D.h"



bool do_ampWalk = true;
bool do_tDiff = false;



void fitLandau( const std::string& plotDir, TTree* tree, TH1D* histo, const std::string& varName );



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

  fitLandau( outdir, tree, h1_ampMaxL, "ampMaxL" );
  fitLandau( outdir, tree, h1_ampMaxR, "ampMaxR" );

  float ampMaxL_min, ampMaxL_max;
  float ampMaxR_min, ampMaxR_max;
  
  exit(1);
  std::string suffix = "";
  if( do_ampWalk ) suffix = suffix + "_AW";
  if( do_tDiff ) suffix = suffix + "_TD";
  TFile* outfile = TFile::Open( Form("ntuplesLite/%s_corr%s.root", confName.c_str(), suffix.c_str()), "RECREATE" );
  TTree* newTree = tree->CloneTree(0);

  float tLcorr;
  newTree->Branch( "tLcorr", &tLcorr );
  float tRcorr;
  newTree->Branch( "tRcorr", &tRcorr );


  std::vector<float> bins_ampMax;
  bins_ampMax.push_back(0.1);
  bins_ampMax.push_back(0.2);
  bins_ampMax.push_back(0.3);
  bins_ampMax.push_back(0.4);
  bins_ampMax.push_back(0.5);
  bins_ampMax.push_back(0.6);
  bins_ampMax.push_back(0.7);
  bins_ampMax.push_back(0.8);
  bins_ampMax.push_back(0.9);
  bins_ampMax.push_back(1.0);

  std::vector< TH1D* > vh1_tL;
  std::vector< TH1D* > vh1_ampMaxL;

  for( unsigned i=0; i<bins_ampMax.size()-1; ++i ) {

    TH1D* h1_tL = new TH1D( Form("tL_bin%d", i), "", 50, 0., 3. );
    vh1_tL.push_back( h1_tL );

    TH1D* h1_ampMaxL = new TH1D( Form("ampMaxL_bin%d", i), "", 50, bins_ampMax[i], bins_ampMax[i+1] );
    vh1_ampMaxL.push_back( h1_ampMaxL );

  } // for bins_ampMax

  float ampMaxMin = vh1_ampMaxL[0]->GetXaxis()->GetXmin();
  float ampMaxMax = vh1_ampMaxL[vh1_ampMaxL.size()-1]->GetXaxis()->GetXmax();

  int nentries = tree->GetEntries();

  
  for( unsigned iEntry = 0; iEntry<nentries; ++iEntry ) {

    if( iEntry % 100000 == 0 ) std::cout << " Entry: " << iEntry << " / " << nentries << std::endl;

    tree->GetEntry( iEntry );

    if( ampMaxL>=ampMaxMin && ampMaxL<=ampMaxMax ) {  // ampMaxL is good

      int thisBinL = -1;
      for( unsigned i=0; i<bins_ampMax.size()-1; ++i ) {
        if( ampMaxL>bins_ampMax[i] && ampMaxL<bins_ampMax[i+1] ) {
          thisBinL = i;
          break;
        } 
      }

      if( thisBinL<0 ) std::cout << "THIS SHOULDN'T BE POSSIBLE" << std::endl;

      vh1_tL[thisBinL]->Fill( tL );
      vh1_ampMaxL[thisBinL]->Fill( ampMaxL );

    } // if ampMaxL is good

  } // for entries


  return 0;

}



void fitLandau( const std::string& plotDir, TTree* tree, TH1D* histo, const std::string& varName ) {

  tree->Project( histo->GetName(), varName.c_str(), "" );

  float xMode = histo->GetBinCenter(histo->GetMaximumBin());
  float xMin_fit = xMode*0.8;
  float xMax_fit = xMode*3.;

  TF1* f1_landau0 = new TF1( Form("landau0_%s", varName.c_str()), "landau", xMin_fit, xMax_fit );
  
  histo->Fit( f1_landau0->GetName(), "R0" );

  xMin_fit = 0.9*f1_landau0->GetParameter(1);
  xMax_fit = 1.4*f1_landau0->GetParameter(1);

  int n_iter = 5;

  TF1* f1_landau;

  for( unsigned i=0; i<n_iter; ++i ) { // iterative fit

    f1_landau = new TF1( Form("landau_%s", varName.c_str()), "landau", xMin_fit, xMax_fit );


    if( i==n_iter-1 )
      histo->Fit( f1_landau->GetName(), "R+" );
    else {
      histo->Fit( f1_landau->GetName(), "R0" );
      xMin_fit = 0.9*f1_landau->GetParameter(1);
      xMax_fit = 1.4*f1_landau->GetParameter(1);
      delete f1_landau;
    }

  } // for iter

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  histo->Draw();

  c1->SaveAs( Form("%s/%s.eps", plotDir.c_str(), histo->GetName()) );
  c1->SaveAs( Form("%s/%s.pdf", plotDir.c_str(), histo->GetName()) );

  return;

}
