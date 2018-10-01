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



  int nentries = tree->GetEntries();

  
  for( unsigned iEntry = 0; iEntry<nentries; ++iEntry ) {

    if( iEntry % 100000 == 0 ) std::cout << " Entry: " << iEntry << " / " << nentries << std::endl;

    tree->GetEntry( iEntry );

     
