#include <iostream>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TLorentzVector.h"
#include "TH1D.h"






int main() {

  TFile* file = TFile::Open( "test.root" );
  TTree* tree = (TTree*)file->Get( "DumpHits" );

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




  TFile* outfile = TFile::Open( "llpFile.root", "recreate" );
  outfile->cd();

  TH1D* h1_trackMass = new TH1D( "trackMass", "", 100, 0., 1 );

  TH1D* h1_beta = new TH1D( "beta", "", 100, 0.6, 1.2 );


  int nentries = tree->GetEntries();

  for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) {

    tree->GetEntry( iEntry );

    if( iEntry % 10000 == 0 ) std::cout << "-> Entry: " << iEntry << " / " << nentries << std::endl;


    for( unsigned itrack=0; itrack<track_pt->size(); ++itrack ) {

      //if( !(track_hasMTD->at(itrack)) ) continue;

      float deltat = (track_mtdt->at(itrack)-track_t0->at(itrack));
      //float deltat = (track_mtdt->at(itrack)-track_mcMatch_genVtx_t->at(itrack));
      float beta = track_path_len->at(itrack)/(2.99792458e1*deltat);
      float energy = track_p->at(itrack)/beta;

      TLorentzVector v1;
      v1.SetPtEtaPhiE( track_pt->at(itrack), track_eta->at(itrack), track_phi->at(itrack), energy );
      
      h1_trackMass->Fill( v1.M() );
      h1_beta->Fill( beta );

    }

  }

  outfile->cd();

  h1_trackMass->Write();
  h1_beta->Write();

  outfile->Close();

  return 0;

}
