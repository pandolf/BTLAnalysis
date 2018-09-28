#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"




int main( int argc, char* argv[] ) {

  if( argc==1 ) {
    std::cout << "-> USAGE: ./makeTreesLite [confName]" << std::endl;
    exit(1);
  }


  std::steing confName = "None";
  if( argc > 1 ) 
    confName = std::string(argv[1]);

    
  std::cout << "-> Configuration: " << confName << std::endl;

  std::string fileListName = "files_" + confName + ".txt";

  ifstream ifs_files(fileListName.c_str());

  TChain* tree = new TChain("digi");
 
  while( ifs_files.isGood() )
    tree->Add( Form("%s/digi", ifs_files.getLine()) );


  float time[9999];
  tree->SetBranchAddress( "time", time );
  float amp_max[9999];
  tree->SetBranchAddress( "amp_max", amp_max );
  int NINO1;
  tree->SetBranchAddress( "NINO1", &NINO1 );
  int NINO2;
  tree->SetBranchAddress( "NINO2", &NINO2 );
  int NINO3;
  tree->SetBranchAddress( "NINO3", &NINO3 );
  int NINO4;
  tree->SetBranchAddress( "NINO4", &NINO4 );
  int AMP1;
  tree->SetBranchAddress( "AMP1", &AMP1 );
  int AMP2;
  tree->SetBranchAddress( "AMP2", &AMP2 );
  int AMP3;
  tree->SetBranchAddress( "AMP3", &AMP3 );
  int AMP4;
  tree->SetBranchAddress( "AMP4", &AMP4 );
  int PTK1;
  tree->SetBranchAddress( "PTK1", &PTK1 );
  int LED300;
  tree->SetBranchAddress( "LED300", &LED300 );
  int CFD;
  tree->SetBranchAddress( "CFD", &CFD );


  TFile* outfile = TFile::Open( Form("ntuplesLite/%.root", confName.c_str()), "RECREATE" );
  outfile->cd();
  TTree* outtree = new TTree( "digiLite", "" );

  float tR;
  outtree->Branch( "tR", &tR );
  float tL;
  outtree->Branch( "tL", &tL );
  float ampMaxL;
  outtree->Branch( "ampMaxL", &ampMaxL );

  int nentries = tree->GetEntries();

  return 0;

}
