#include <iostream>
#include <vector>
#include <fstream>

#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TLorentzVector.h"
#include "TH1D.h"
#include "TString.h"

#include "../interface/HSCPCommon.h"




int main( int argc, char* argv[] ) {

  std::string fileName = "files_DY_crab_yesMTD.txt";

  if( argc>1 ) {
    fileName = argv[1];
  }

  TChain* tree = new TChain("DumpHits");
  std::string suffix("");

  TString fileName_tstr(fileName);

  if( fileName_tstr.BeginsWith("test_") ) {

    suffix = fileName;
    suffix.erase( 0, 4 );
    suffix.erase( suffix.end()-5, suffix.end() ); // remove the .root
  
    tree->Add( Form("%s/DumpHits", fileName.c_str()) );

  } else if( fileName_tstr.BeginsWith("files_") ) {

    suffix = fileName;
    suffix.erase( 0, 5 );
    suffix.erase( suffix.end()-4, suffix.end() ); // remove the .txt

    std::ifstream ifs_files(fileName.c_str());

    if( ifs_files.good() ) {

      std::string line;

      while( getline(ifs_files,line) ) {
        TString line_tstr(line);
        if( line_tstr.BeginsWith("#") ) continue;
        tree->Add( Form("%s/DumpHits", line.c_str()) );
        std::cout << "-> Added: " << line << std::endl;
      }

    } // if files good
 
  }


  int event;
  tree->SetBranchAddress( "event", &event );
  int lumi;
  tree->SetBranchAddress( "lumi", &lumi );
  int run;
  tree->SetBranchAddress( "run", &run );

  TBranch *btrack_pt = 0;
  std::vector<float> *track_pt = 0;
  tree->SetBranchAddress( "track_pt", &track_pt, &btrack_pt );

  TBranch *btrack_eta = 0;
  std::vector<float> *track_eta = 0;
  tree->SetBranchAddress( "track_eta", &track_eta, &btrack_eta );

  TBranch *btrack_phi = 0;
  std::vector<float> *track_phi = 0;
  tree->SetBranchAddress( "track_phi", &track_phi, &btrack_phi );

  TBranch *btrack_p = 0;
  std::vector<float> *track_p = 0;
  tree->SetBranchAddress( "track_p", &track_p, &btrack_p );

  TBranch *btrack_energy = 0;
  std::vector<float> *track_energy = 0;
  tree->SetBranchAddress( "track_energy", &track_energy, &btrack_energy );

  TBranch *btrack_x = 0;
  std::vector<float> *track_x = 0;
  tree->SetBranchAddress( "track_x", &track_x, &btrack_x );

  TBranch *btrack_y = 0;
  std::vector<float> *track_y = 0;
  tree->SetBranchAddress( "track_y", &track_y, &btrack_y );

  TBranch *btrack_z = 0;
  std::vector<float> *track_z = 0;
  tree->SetBranchAddress( "track_z", &track_z, &btrack_z );

  TBranch *btrack_t = 0;
  std::vector<float> *track_t = 0;
  tree->SetBranchAddress( "track_t", &track_t, &btrack_t );

  TBranch *btrack_mtdt = 0;
  std::vector<float> *track_mtdt = 0;
  tree->SetBranchAddress( "track_mtdt", &track_mtdt, &btrack_mtdt );

  TBranch *btrack_t0 = 0;
  std::vector<float> *track_t0 = 0;
  tree->SetBranchAddress( "track_t0", &track_t0, &btrack_t0 );

  TBranch *btrack_path_len = 0;
  std::vector<float> *track_path_len = 0;
  tree->SetBranchAddress( "track_path_len", &track_path_len, &btrack_path_len );

  TBranch *btrack_hasMTD = 0;
  std::vector<bool> *track_hasMTD = 0;
  tree->SetBranchAddress( "track_hasMTD", &track_hasMTD, &btrack_hasMTD );

  TBranch *btrack_mcMatch_genVtx_t = 0;
  std::vector<float> *track_mcMatch_genVtx_t = 0;
  tree->SetBranchAddress( "track_mcMatch_genVtx_t", &track_mcMatch_genVtx_t, &btrack_mcMatch_genVtx_t );

  TBranch *btrack_mcMatch_genEta = 0;
  std::vector<float> *track_mcMatch_genEta = 0;
  tree->SetBranchAddress( "track_mcMatch_genEta", &track_mcMatch_genEta, &btrack_mcMatch_genEta );

  TBranch *btrack_mcMatch_genPhi = 0;
  std::vector<float> *track_mcMatch_genPhi = 0;
  tree->SetBranchAddress( "track_mcMatch_genPhi", &track_mcMatch_genPhi, &btrack_mcMatch_genPhi );

  TBranch *btrack_mcMatch_genPt = 0;
  std::vector<float> *track_mcMatch_genPt = 0;
  tree->SetBranchAddress( "track_mcMatch_genPt", &track_mcMatch_genPt, &btrack_mcMatch_genPt );

  TBranch *btrack_mcMatch_genE = 0;
  std::vector<float> *track_mcMatch_genE = 0;
  tree->SetBranchAddress( "track_mcMatch_genE", &track_mcMatch_genE, &btrack_mcMatch_genE );

  TBranch *btrack_mcMatch_genPdgId = 0;
  std::vector<float> *track_mcMatch_genPdgId = 0;
  tree->SetBranchAddress( "track_mcMatch_genPdgId", &track_mcMatch_genPdgId, &btrack_mcMatch_genPdgId );




  TFile* outfile = TFile::Open( Form("hscpLite%s.root", suffix.c_str()), "recreate" );
  outfile->cd();

  TTree* treeLite = new TTree( "treeLite", "" );

  float p;
  treeLite->Branch( "p", &p );
  float pt;
  treeLite->Branch( "pt", &pt );
  float eta;
  treeLite->Branch( "eta", &eta );
  float phi;
  treeLite->Branch( "phi", &phi );
  float mass;
  treeLite->Branch( "mass", &mass );
  float beta;
  treeLite->Branch( "beta", &beta );
  float betaGen;
  treeLite->Branch( "betaGen", &betaGen );
  float pathLength;
  treeLite->Branch( "pathLength", &pathLength );


  std::vector<float> etaBins = HSCPCommon::etaBins();
  std::vector<float>   pBins = HSCPCommon::  pBins();


  int nentries = tree->GetEntries();
  nentries = 1000000;

  for( int iEntry=0; iEntry<nentries; ++iEntry ) {

    tree->GetEntry( iEntry );

    if( iEntry % 10000 == 0 ) std::cout << "-> Entry: " << iEntry << " / " << nentries << std::endl;


    for( unsigned itrack=0; itrack<track_pt->size(); ++itrack ) {


      int pdgId = (int)(track_mcMatch_genPdgId->at(itrack));
      if( abs(pdgId) != 2212 ) continue; // only protons

      pt  = track_pt ->at(itrack);
      p   = track_p  ->at(itrack);
      eta = track_eta->at(itrack);
      if( fabs(eta) > HSCPCommon::barrelEnd() && fabs(eta) < HSCPCommon::endcapStart() ) continue;

      float deltat = (track_mtdt->at(itrack)-track_mcMatch_genVtx_t->at(itrack));
      pathLength = track_path_len->at(itrack);
      beta = pathLength/(2.99792458e1*deltat);
      float energy = p/beta;


      TLorentzVector v;
      v.SetPtEtaPhiE( pt, eta, track_phi->at(itrack), energy );
      
      mass = v.M();

      TLorentzVector vGen;
      vGen.SetPtEtaPhiE( track_mcMatch_genPt->at(itrack), track_mcMatch_genEta->at(itrack), track_mcMatch_genPhi->at(itrack), track_mcMatch_genE->at(itrack) );
      
      betaGen = vGen.Beta();

      treeLite->Fill();

    } // for tracks

  } // for events

  outfile->cd();

  treeLite->Write();

  outfile->Close();

  std::cout << "-> Find your stuff in: " << outfile->GetName() << std::endl;

  return 0;

}



