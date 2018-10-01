#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TH1D.h"



bool do_ampWalk = true;
bool do_tDiff = false;



int main( int argc, char* argv[] ) {


  std::string confName="";

  if( argc>1 ) {

    confName = std::string(argv[1]);

  } else {

    std::cout << "USAGE: ./calibrateTreeLite [confName]" << std::endl;
    exit(1);

  }


  TFile* file = TFile::Open( "ntuplesLite/%s.root", confName.c_str() );
  TTree* tree = (TTree*)file->Get("digiLite");

  float tL;
  tree->SetBranchAddress( "tL", &tL );
  float tR;
  tree->SetBranchAddress( "tR", &tR );
  float ampMaxL;
  tree->SetBranchAddress( "ampMaxL", &ampMaxL );
  float ampMaxR;
  tree->SetBranchAddress( "ampMaxR", &ampMaxR );

  
  std::string suffix = "";
  if( do_ampWalk ) suffix = suffix + "_AW";
  if( do_tDiff ) suffix = suffix + "_TD";
  TFile* outfile = TFile( "ntuplesLite/%s_corr%s.root", confName.c_str(), suffix.c_str() );
  TFile* newTree = tree->CloneTree(0);

  float tLcorr;
  newTree->Branch( "tLcorr", &tLcorr );
  float tRcorr;
  newTree->Branch( "tRcorr", &tRcorr );


  int bins_ampMax[11];
  bins_ampMax[0] = 0.1;
  bins_ampMax[1] = 0.2;
  bins_ampMax[2] = 0.3;
  bins_ampMax[3] = 0.4;
  bins_ampMax[4] = 0.5;
  bins_ampMax[5] = 0.6;
  bins_ampMax[6] = 0.7;
  bins_ampMax[7] = 0.8;
  bins_ampMax[8] = 0.9;
  bins_ampMax[9] = 1.0
  bins_ampMax[10] = 1.1;

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


