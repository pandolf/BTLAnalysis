#include <iostream>
#include <fstream>
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"




int main( int argc, char* argv[] ) {

  if( argc==1 ) {
    std::cout << "-> USAGE: ./makeTreesLite [confName]" << std::endl;
    exit(1);
  }


  std::string confName = "None";
  if( argc > 1 ) 
    confName = std::string(argv[1]);

    
  std::cout << "-> Configuration: " << confName << std::endl;

  std::string fileListName = "files_" + confName + ".txt";

  std::ifstream ifs_files(fileListName.c_str());

  TChain* tree = new TChain("digi");
 
  if( ifs_files.good() ) {

    std::string line;

    while( getline(ifs_files,line) )
      tree->Add( Form("%s/digi", line.c_str()) );

  }


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


  TFile* outfile = TFile::Open( Form("ntuplesLite/%s.root", confName.c_str()), "RECREATE" );
  outfile->cd();
  TTree* outtree = new TTree( "digiLite", "" );

  float tR;
  outtree->Branch( "tR", &tR );
  float tL;
  outtree->Branch( "tL", &tL );
  float ampMaxL;
  outtree->Branch( "ampMaxL", &ampMaxL );
  float ampMaxR;
  outtree->Branch( "ampMaxR", &ampMaxR );

  int nentries = tree->GetEntries();

  for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) {

    tree->GetEntry(iEntry);

    float tPTK = time[PTK1+CFD];

    int iR = NINO1+LED300;
    int iL = NINO2+LED300;

    tR = time[iR]-tPTK;
    tL = time[iL]-tPTK;

    ampMaxR = amp_max[iR];
    ampMaxL = amp_max[iL];

    outtree->Fill();

  }

  outfile->cd();
  outtree->Write();
  outfile->Close();

  return 0;

}
