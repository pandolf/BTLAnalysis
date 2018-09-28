#include <iostream>
#include <fstream>
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"


#define MAXIND 999 



int main( int argc, char* argv[] ) {

  if( argc==1 ) {
    std::cout << "-> USAGE: ./makeTreesLite [confName]" << std::endl;
    exit(1);
  }


  std::string confName = "None";
  if( argc > 1 ) 
    confName = std::string(argv[1]);


  system( "mkdir -p ntuplesLite" );

    
  std::cout << "-> Configuration: " << confName << std::endl;

  std::string fileListName = "files_" + confName + ".txt";

  std::ifstream ifs_files(fileListName.c_str());

  TChain* tree = new TChain("digi");
 
  if( ifs_files.good() ) {

    std::string line;

    while( getline(ifs_files,line) ) {
      tree->Add( Form("%s/digi", line.c_str()) );
      std::cout << "-> Added: " << line << std::endl;
    }

  }


  float time[MAXIND];
  tree->SetBranchAddress( "time", time );
  float amp_max[MAXIND];
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
  int LED;
  tree->SetBranchAddress( "LED", &LED );
  //int LED300; // not used: same as LED
  //tree->SetBranchAddress( "LED300", &LED300 );
  int CFD;
  tree->SetBranchAddress( "CFD", &CFD );


  TFile* outfile = TFile::Open( Form("ntuplesLite/%s.root", confName.c_str()), "RECREATE" );
  outfile->cd();
  TTree* outtree = new TTree( "digiLite", "" );

  float tR;
  outtree->Branch( "tR", &tR, "tR/F" );
  float tL;
  outtree->Branch( "tL", &tL, "tL/F" );
  float ampMaxR;
  outtree->Branch( "ampMaxR", &ampMaxR, "ampMaxR/F" );
  float ampMaxL;
  outtree->Branch( "ampMaxL", &ampMaxL, "ampMaxL/F" );

  int nentries = tree->GetEntries();

  for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) {

    if( iEntry % 100000 == 0 ) std::cout << "  Entry: " << iEntry << " / " << nentries << std::endl;

    tree->GetEntry(iEntry);

    float tPTK = time[PTK1+CFD];

    int iR = NINO1+LED;
    int iL = NINO2+LED;

    tR = time[iR]-tPTK;
    tL = time[iL]-tPTK;

    ampMaxR = amp_max[iR];
    ampMaxL = amp_max[iL];

    outtree->Fill();

  }

  outfile->cd();
  outtree->Write();
  outfile->Close();

  std::cout << "-> Find your stuff here: " << outfile->GetName() << std::endl;

  return 0;

}
