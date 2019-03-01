#include <iostream>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TLorentzVector.h"
#include "TH1D.h"
#include "TString.h"

#include "../interface/LLPCommon.h"




int main( int argc, char* argv[] ) {

  std::string fileName = "test_Pt1_48k.root";

  if( argc>1 ) {
    fileName = argv[1];
  }

  TString fileName_tstr(fileName);
  std::string suffix(fileName);
  if( fileName_tstr.BeginsWith("test_") ) {
    suffix.erase( 0, 4 );
  }

  TFile* file = TFile::Open( fileName.c_str() );
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
  std::vector<int> *track_mcMatch_genPdgId = 0;
  tree->SetBranchAddress( "track_mcMatch_genPdgId", &track_mcMatch_genPdgId, &btrack_mcMatch_genPdgId );




  TFile* outfile = TFile::Open( Form("llpFile%s", suffix.c_str()), "recreate" );
  outfile->cd();

  TH1D* h1_trackPt = new TH1D( "trackPt", "", 200, 0., 10. );
  TH1D* h1_trackEta = new TH1D( "trackEta", "", 200, -2.5, 2.5 );
  TH1D* h1_trackP  = new TH1D( "trackP" , "", 200, 0., 10. );

  TH1D* h1_trackMass        = new TH1D( "trackMass"       , "", 200, 0., 2.5 );
  TH1D* h1_trackMass_pion   = new TH1D( "trackMass_pion"  , "", 200, 0., 2.5 );
  TH1D* h1_trackMass_kaon   = new TH1D( "trackMass_kaon"  , "", 200, 0., 2.5 );
  TH1D* h1_trackMass_proton = new TH1D( "trackMass_proton", "", 200, 0., 2.5 );

  TH1D* h1_beta = new TH1D( "beta", "", 100, 0.6, 1.2 );
  TH1D* h1_deltaBeta = new TH1D( "deltaBeta", "", 100, -0.2, 0.2 );

  TH1D* h1_invBeta        = new TH1D( "invBeta"       , "", 100, 0.7, 1.3 );
  TH1D* h1_invBeta_pion   = new TH1D( "invBeta_pion"  , "", 100, 0.7, 1.3 );
  TH1D* h1_invBeta_kaon   = new TH1D( "invBeta_kaon"  , "", 100, 0.7, 1.3 );
  TH1D* h1_invBeta_proton = new TH1D( "invBeta_proton", "", 100, 0.7, 1.3 );

  TH1D* h1_deltaT = new TH1D( "deltaT", "", 100, -0.5, 0.5 );


  std::vector<float> etaBins = LLPCommon::etaBins();

  std::vector<TH1D*> vh1_invBeta;
  std::vector<TH1D*> vh1_trackP;

  std::vector<TH1D*> vh1_invBeta_pion;
  std::vector<TH1D*> vh1_trackP_pion;

  std::vector<TH1D*> vh1_invBeta_kaon;
  std::vector<TH1D*> vh1_trackP_kaon;

  std::vector<TH1D*> vh1_invBeta_proton;
  std::vector<TH1D*> vh1_trackP_proton;

  for( unsigned i=0; i<etaBins.size()-1; ++i ) {

    TH1D* this_h1_invBeta = new TH1D( Form( "invBeta_eta%d", i ), "", 100, 0.9, 1.1 );
    vh1_invBeta.push_back( this_h1_invBeta );

    TH1D* this_h1_trackP = new TH1D( Form( "trackP_eta%d", i ), "", 100, 0., 50. );
    vh1_trackP.push_back( this_h1_trackP );

    TH1D* this_h1_invBeta_pion = new TH1D( Form( "invBeta_pion_eta%d", i ), "", 100, 0.9, 1.1 );
    vh1_invBeta_pion.push_back( this_h1_invBeta_pion );

    TH1D* this_h1_trackP_pion = new TH1D( Form( "trackP_pion_eta%d", i ), "", 100, 0., 50. );
    vh1_trackP_pion.push_back( this_h1_trackP_pion );

    TH1D* this_h1_invBeta_kaon = new TH1D( Form( "invBeta_kaon_eta%d", i ), "", 100, 0.9, 1.1 );
    vh1_invBeta_kaon.push_back( this_h1_invBeta_kaon );

    TH1D* this_h1_trackP_kaon = new TH1D( Form( "trackP_kaon_eta%d", i ), "", 100, 0., 50. );
    vh1_trackP_kaon.push_back( this_h1_trackP_kaon );

    TH1D* this_h1_invBeta_proton = new TH1D( Form( "invBeta_proton_eta%d", i ), "", 100, 0.9, 1.1 );
    vh1_invBeta_proton.push_back( this_h1_invBeta_proton );

    TH1D* this_h1_trackP_proton = new TH1D( Form( "trackP_proton_eta%d", i ), "", 100, 0., 50. );
    vh1_trackP_proton.push_back( this_h1_trackP_proton );

  }


  int nentries = tree->GetEntries();

  for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) {

    tree->GetEntry( iEntry );

    if( iEntry % 10000 == 0 ) std::cout << "-> Entry: " << iEntry << " / " << nentries << std::endl;


    for( unsigned itrack=0; itrack<track_pt->size(); ++itrack ) {

      float eta = track_eta->at(itrack);
      if( fabs(eta) > LLPCommon::barrelEnd() && fabs(eta) < LLPCommon::endcapStart() ) continue;

      h1_trackPt ->Fill( track_pt ->at(itrack) );
      h1_trackP  ->Fill( track_p  ->at(itrack) );
      h1_trackEta->Fill( eta );

      float deltat0 = (track_t0->at(itrack)-track_mcMatch_genVtx_t->at(itrack));
      h1_deltaT->Fill( deltat0 );

      //float deltat = (track_mtdt->at(itrack)-track_t0->at(itrack));
      float deltat = (track_mtdt->at(itrack)-track_mcMatch_genVtx_t->at(itrack));
      float beta = track_path_len->at(itrack)/(2.99792458e1*deltat);
      float energy = track_p->at(itrack)/beta;


      TLorentzVector v;
      v.SetPtEtaPhiE( track_pt->at(itrack), eta, track_phi->at(itrack), energy );
      
      h1_trackMass->Fill( v.M() );
      if( abs(track_mcMatch_genPdgId->at(itrack))==211  ) h1_trackMass_pion  ->Fill( v.M() );
      if( abs(track_mcMatch_genPdgId->at(itrack))==321  ) h1_trackMass_kaon  ->Fill( v.M() );
      if( abs(track_mcMatch_genPdgId->at(itrack))==2212 ) h1_trackMass_proton->Fill( v.M() );

      h1_beta->Fill( beta );
      if( beta > 0. ) {
        h1_invBeta->Fill( 1./beta );
        if( abs(track_mcMatch_genPdgId->at(itrack))==211  ) h1_invBeta_pion  ->Fill( 1./beta );
        if( abs(track_mcMatch_genPdgId->at(itrack))==321  ) h1_invBeta_kaon  ->Fill( 1./beta );
        if( abs(track_mcMatch_genPdgId->at(itrack))==2212 ) h1_invBeta_proton->Fill( 1./beta );
      }

      TLorentzVector vGen;
      vGen.SetPtEtaPhiE( track_mcMatch_genPt->at(itrack), track_mcMatch_genEta->at(itrack), track_mcMatch_genPhi->at(itrack), track_mcMatch_genE->at(itrack) );
      
      float betaGen = vGen.Beta();
      h1_deltaBeta->Fill( beta-betaGen );


      int etaBin = -1;

      for( unsigned i=0; i<etaBins.size()-1; ++i ) {
        if( eta >= etaBins[i] && eta < etaBins[i+1] ) {
          etaBin = i;
          break;
        }
      } // for eta bins

      if( etaBin>=0 ) {

        if( beta>0. ) vh1_invBeta[etaBin]->Fill( 1./beta );
        vh1_trackP [etaBin]->Fill( track_p->at(itrack) );

        if( abs(track_mcMatch_genPdgId->at(itrack))==211 ) { // charged pions
          if( beta>0. ) vh1_invBeta_pion[etaBin]->Fill( 1./beta );
          vh1_trackP_pion [etaBin]->Fill( track_p->at(itrack) );
        }

        if( abs(track_mcMatch_genPdgId->at(itrack))==321 ) { // charged kaons
          if( beta>0. ) vh1_invBeta_kaon[etaBin]->Fill( 1./beta );
          vh1_trackP_kaon [etaBin]->Fill( track_p->at(itrack) );
        }

        if( abs(track_mcMatch_genPdgId->at(itrack))==2212 ) { //protons
          if( beta>0. ) vh1_invBeta_proton[etaBin]->Fill( 1./beta );
          vh1_trackP_proton [etaBin]->Fill( track_p->at(itrack) );
        }

      } // if found eta bin

    } // for tracks

  } // for events

  outfile->cd();

  h1_trackPt->Write();
  h1_trackP->Write();
  h1_trackEta->Write();
  h1_beta->Write();
  h1_deltaBeta->Write();
  h1_deltaT->Write();

  h1_trackMass->Write();
  h1_trackMass_pion->Write();
  h1_trackMass_kaon->Write();
  h1_trackMass_proton->Write();

  h1_invBeta->Write();
  h1_invBeta_pion->Write();
  h1_invBeta_kaon->Write();
  h1_invBeta_proton->Write();

  for( unsigned i=0; i<vh1_invBeta.size(); ++i ) {

    vh1_invBeta[i]->Write();
    vh1_trackP [i]->Write();

    vh1_invBeta_pion[i]->Write();
    vh1_trackP_pion [i]->Write();

    vh1_invBeta_kaon[i]->Write();
    vh1_trackP_kaon [i]->Write();

    vh1_invBeta_proton[i]->Write();
    vh1_trackP_proton [i]->Write();

  }

  outfile->Close();

  std::cout << "-> Find your stuff in: " << outfile->GetName() << std::endl;

  return 0;

}



