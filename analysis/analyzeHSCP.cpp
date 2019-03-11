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




  TFile* outfile = TFile::Open( Form("hscpFile%s.root", suffix.c_str()), "recreate" );
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


  std::vector<float> etaBins = HSCPCommon::etaBins();
  std::vector<float>   pBins = HSCPCommon::  pBins();

  std::vector< std::vector<TH1D*> > vvh1_invBeta;
  std::vector< std::vector<TH1D*> > vvh1_invBeta_pion;
  std::vector< std::vector<TH1D*> > vvh1_invBeta_kaon;
  std::vector< std::vector<TH1D*> > vvh1_invBeta_proton;

  std::vector< std::vector<TH1D*> > vvh1_trackP;
  std::vector< std::vector<TH1D*> > vvh1_trackP_pion;
  std::vector< std::vector<TH1D*> > vvh1_trackP_kaon;
  std::vector< std::vector<TH1D*> > vvh1_trackP_proton;

  std::vector< std::vector<TH1D*> > vvh1_trackMass;
  std::vector< std::vector<TH1D*> > vvh1_trackMass_pion;
  std::vector< std::vector<TH1D*> > vvh1_trackMass_kaon;
  std::vector< std::vector<TH1D*> > vvh1_trackMass_proton;

  std::vector< std::vector<TH1D*> > vvh1_deltaP;
  std::vector< std::vector<TH1D*> > vvh1_deltaP_pion;
  std::vector< std::vector<TH1D*> > vvh1_deltaP_kaon;
  std::vector< std::vector<TH1D*> > vvh1_deltaP_proton;

  std::vector< std::vector<TH1D*> > vvh1_deltaBeta;
  std::vector< std::vector<TH1D*> > vvh1_deltaBeta_pion;
  std::vector< std::vector<TH1D*> > vvh1_deltaBeta_kaon;
  std::vector< std::vector<TH1D*> > vvh1_deltaBeta_proton;

  std::vector< std::vector<TH1D*> > vvh1_deltaInvBeta;
  std::vector< std::vector<TH1D*> > vvh1_deltaInvBeta_pion;
  std::vector< std::vector<TH1D*> > vvh1_deltaInvBeta_kaon;
  std::vector< std::vector<TH1D*> > vvh1_deltaInvBeta_proton;

  std::vector< std::vector<TH1D*> > vvh1_betaFunc;
  std::vector< std::vector<TH1D*> > vvh1_betaFunc_pion;
  std::vector< std::vector<TH1D*> > vvh1_betaFunc_kaon;
  std::vector< std::vector<TH1D*> > vvh1_betaFunc_proton;

  for( unsigned i=0; i<etaBins.size()-1; ++i ) {

    std::vector<TH1D*> this_vh1_invBeta;
    std::vector<TH1D*> this_vh1_invBeta_pion;
    std::vector<TH1D*> this_vh1_invBeta_kaon;
    std::vector<TH1D*> this_vh1_invBeta_proton;

    std::vector<TH1D*> this_vh1_trackP;
    std::vector<TH1D*> this_vh1_trackP_pion;
    std::vector<TH1D*> this_vh1_trackP_kaon;
    std::vector<TH1D*> this_vh1_trackP_proton;

    std::vector<TH1D*> this_vh1_trackMass;
    std::vector<TH1D*> this_vh1_trackMass_pion;
    std::vector<TH1D*> this_vh1_trackMass_kaon;
    std::vector<TH1D*> this_vh1_trackMass_proton;

    std::vector<TH1D*> this_vh1_deltaP;
    std::vector<TH1D*> this_vh1_deltaP_pion;
    std::vector<TH1D*> this_vh1_deltaP_kaon;
    std::vector<TH1D*> this_vh1_deltaP_proton;

    std::vector<TH1D*> this_vh1_deltaBeta;
    std::vector<TH1D*> this_vh1_deltaBeta_pion;
    std::vector<TH1D*> this_vh1_deltaBeta_kaon;
    std::vector<TH1D*> this_vh1_deltaBeta_proton;

    std::vector<TH1D*> this_vh1_deltaInvBeta;
    std::vector<TH1D*> this_vh1_deltaInvBeta_pion;
    std::vector<TH1D*> this_vh1_deltaInvBeta_kaon;
    std::vector<TH1D*> this_vh1_deltaInvBeta_proton;

    std::vector<TH1D*> this_vh1_betaFunc;
    std::vector<TH1D*> this_vh1_betaFunc_pion;
    std::vector<TH1D*> this_vh1_betaFunc_kaon;
    std::vector<TH1D*> this_vh1_betaFunc_proton;

    for( unsigned j=0; j<pBins.size()-1; ++j ) {

      TH1D* this_h1_invBeta = new TH1D( Form( "invBeta_eta%d_p%d", i, j ), "", 100, 0.9, 1.1 );
      this_vh1_invBeta.push_back( this_h1_invBeta );
      TH1D* this_h1_invBeta_pion = new TH1D( Form( "invBeta_pion_eta%d_p%d", i, j ), "", 100, 0.9, 1.1 );
      this_vh1_invBeta_pion.push_back( this_h1_invBeta_pion );
      TH1D* this_h1_invBeta_kaon = new TH1D( Form( "invBeta_kaon_eta%d_p%d", i, j ), "", 100, 0.9, 1.1 );
      this_vh1_invBeta_kaon.push_back( this_h1_invBeta_kaon );
      TH1D* this_h1_invBeta_proton = new TH1D( Form( "invBeta_proton_eta%d_p%d", i, j ), "", 100, 0.9, 1.1 );
      this_vh1_invBeta_proton.push_back( this_h1_invBeta_proton );
   
      TH1D* this_h1_trackP = new TH1D( Form( "trackP_eta%d_p%d", i, j ), "", 100, 0., 50. );
      this_vh1_trackP.push_back( this_h1_trackP );
      TH1D* this_h1_trackP_pion = new TH1D( Form( "trackP_pion_eta%d_p%d", i, j ), "", 100, 0., 50. );
      this_vh1_trackP_pion.push_back( this_h1_trackP_pion );
      TH1D* this_h1_trackP_kaon = new TH1D( Form( "trackP_kaon_eta%d_p%d", i, j ), "", 100, 0., 50. );
      this_vh1_trackP_kaon.push_back( this_h1_trackP_kaon );
      TH1D* this_h1_trackP_proton = new TH1D( Form( "trackP_proton_eta%d_p%d", i, j ), "", 100, 0., 50. );
      this_vh1_trackP_proton.push_back( this_h1_trackP_proton );
   
      TH1D* this_h1_trackMass = new TH1D( Form( "trackMass_eta%d_p%d", i, j ), "", 100, 0., 2.5 );
      this_vh1_trackMass.push_back( this_h1_trackMass );
      TH1D* this_h1_trackMass_pion = new TH1D( Form( "trackMass_pion_eta%d_p%d", i, j ), "", 100, 0., 2.5 );
      this_vh1_trackMass_pion.push_back( this_h1_trackMass_pion );
      TH1D* this_h1_trackMass_kaon = new TH1D( Form( "trackMass_kaon_eta%d_p%d", i, j ), "", 100, 0., 2.5 );
      this_vh1_trackMass_kaon.push_back( this_h1_trackMass_kaon );
      TH1D* this_h1_trackMass_proton = new TH1D( Form( "trackMass_proton_eta%d_p%d", i, j ), "", 100, 0., 2.5 );
      this_vh1_trackMass_proton.push_back( this_h1_trackMass_proton );

      TH1D* this_h1_deltaP = new TH1D( Form( "deltaP_eta%d_p%d", i, j ), "", 100, -0.7, 0.7 );
      this_vh1_deltaP.push_back( this_h1_deltaP );
      TH1D* this_h1_deltaP_pion = new TH1D( Form( "deltaP_pion_eta%d_p%d", i, j ), "", 100, -0.7, 0.7 );
      this_vh1_deltaP_pion.push_back( this_h1_deltaP_pion );
      TH1D* this_h1_deltaP_kaon = new TH1D( Form( "deltaP_kaon_eta%d_p%d", i, j ), "", 100, -0.7, 0.7 );
      this_vh1_deltaP_kaon.push_back( this_h1_deltaP_kaon );
      TH1D* this_h1_deltaP_proton = new TH1D( Form( "deltaP_proton_eta%d_p%d", i, j ), "", 100, -0.7, 0.7 );
      this_vh1_deltaP_proton.push_back( this_h1_deltaP_proton );

      TH1D* this_h1_deltaBeta = new TH1D( Form( "deltaBeta_eta%d_p%d", i, j ), "", 100, -0.7, 0.7 );
      this_vh1_deltaBeta.push_back( this_h1_deltaBeta );
      TH1D* this_h1_deltaBeta_pion = new TH1D( Form( "deltaBeta_pion_eta%d_p%d", i, j ), "", 100, -0.7, 0.7 );
      this_vh1_deltaBeta_pion.push_back( this_h1_deltaBeta_pion );
      TH1D* this_h1_deltaBeta_kaon = new TH1D( Form( "deltaBeta_kaon_eta%d_p%d", i, j ), "", 100, -0.7, 0.7 );
      this_vh1_deltaBeta_kaon.push_back( this_h1_deltaBeta_kaon );
      TH1D* this_h1_deltaBeta_proton = new TH1D( Form( "deltaBeta_proton_eta%d_p%d", i, j ), "", 100, -0.7, 0.7 );
      this_vh1_deltaBeta_proton.push_back( this_h1_deltaBeta_proton );

      TH1D* this_h1_deltaInvBeta = new TH1D( Form( "deltaInvBeta_eta%d_p%d", i, j ), "", 100, -0.7, 0.7 );
      this_vh1_deltaInvBeta.push_back( this_h1_deltaInvBeta );
      TH1D* this_h1_deltaInvBeta_pion = new TH1D( Form( "deltaInvBeta_pion_eta%d_p%d", i, j ), "", 100, -0.7, 0.7 );
      this_vh1_deltaInvBeta_pion.push_back( this_h1_deltaInvBeta_pion );
      TH1D* this_h1_deltaInvBeta_kaon = new TH1D( Form( "deltaInvBeta_kaon_eta%d_p%d", i, j ), "", 100, -0.7, 0.7 );
      this_vh1_deltaInvBeta_kaon.push_back( this_h1_deltaInvBeta_kaon );
      TH1D* this_h1_deltaInvBeta_proton = new TH1D( Form( "deltaInvBeta_proton_eta%d_p%d", i, j ), "", 100, -0.7, 0.7 );
      this_vh1_deltaInvBeta_proton.push_back( this_h1_deltaInvBeta_proton );

      TH1D* this_h1_betaFunc = new TH1D( Form( "betaFunc_eta%d_p%d", i, j ), "", 100, 0., 1.);
      this_vh1_betaFunc.push_back( this_h1_betaFunc );
      TH1D* this_h1_betaFunc_pion = new TH1D( Form( "betaFunc_pion_eta%d_p%d", i, j ), "", 100, 0., 1.);
      this_vh1_betaFunc_pion.push_back( this_h1_betaFunc_pion );
      TH1D* this_h1_betaFunc_kaon = new TH1D( Form( "betaFunc_kaon_eta%d_p%d", i, j ), "", 100, 0., 1.);
      this_vh1_betaFunc_kaon.push_back( this_h1_betaFunc_kaon );
      TH1D* this_h1_betaFunc_proton = new TH1D( Form( "betaFunc_proton_eta%d_p%d", i, j ), "", 100, 0., 1.);
      this_vh1_betaFunc_proton.push_back( this_h1_betaFunc_proton );

    } // for p 

    vvh1_invBeta.push_back( this_vh1_invBeta );
    vvh1_invBeta_pion.push_back( this_vh1_invBeta_pion );
    vvh1_invBeta_kaon.push_back( this_vh1_invBeta_kaon );
    vvh1_invBeta_proton.push_back( this_vh1_invBeta_proton );

    vvh1_trackP.push_back( this_vh1_trackP );
    vvh1_trackP_pion.push_back( this_vh1_trackP_pion );
    vvh1_trackP_kaon.push_back( this_vh1_trackP_kaon );
    vvh1_trackP_proton.push_back( this_vh1_trackP_proton );

    vvh1_trackMass.push_back( this_vh1_trackMass );
    vvh1_trackMass_pion.push_back( this_vh1_trackMass_pion );
    vvh1_trackMass_kaon.push_back( this_vh1_trackMass_kaon );
    vvh1_trackMass_proton.push_back( this_vh1_trackMass_proton );

    vvh1_deltaP.push_back( this_vh1_deltaP );
    vvh1_deltaP_pion.push_back( this_vh1_deltaP_pion );
    vvh1_deltaP_kaon.push_back( this_vh1_deltaP_kaon );
    vvh1_deltaP_proton.push_back( this_vh1_deltaP_proton );

    vvh1_deltaBeta.push_back( this_vh1_deltaBeta );
    vvh1_deltaBeta_pion.push_back( this_vh1_deltaBeta_pion );
    vvh1_deltaBeta_kaon.push_back( this_vh1_deltaBeta_kaon );
    vvh1_deltaBeta_proton.push_back( this_vh1_deltaBeta_proton );

    vvh1_deltaInvBeta.push_back( this_vh1_deltaInvBeta );
    vvh1_deltaInvBeta_pion.push_back( this_vh1_deltaInvBeta_pion );
    vvh1_deltaInvBeta_kaon.push_back( this_vh1_deltaInvBeta_kaon );
    vvh1_deltaInvBeta_proton.push_back( this_vh1_deltaInvBeta_proton );

    vvh1_betaFunc.push_back( this_vh1_betaFunc );
    vvh1_betaFunc_pion.push_back( this_vh1_betaFunc_pion );
    vvh1_betaFunc_kaon.push_back( this_vh1_betaFunc_kaon );
    vvh1_betaFunc_proton.push_back( this_vh1_betaFunc_proton );

  } // for eta


  int nentries = tree->GetEntries();
  nentries = 1000000;

  for( int iEntry=0; iEntry<nentries; ++iEntry ) {

    tree->GetEntry( iEntry );

    if( iEntry % 10000 == 0 ) std::cout << "-> Entry: " << iEntry << " / " << nentries << std::endl;


    for( unsigned itrack=0; itrack<track_pt->size(); ++itrack ) {

      float pt  = track_pt ->at(itrack);
      float p   = track_p  ->at(itrack);
      float eta = track_eta->at(itrack);
      if( fabs(eta) > HSCPCommon::barrelEnd() && fabs(eta) < HSCPCommon::endcapStart() ) continue;

      h1_trackPt ->Fill( pt  );
      h1_trackP  ->Fill( p   );
      h1_trackEta->Fill( eta );

      float deltat0 = (track_t0->at(itrack)-track_mcMatch_genVtx_t->at(itrack));
      h1_deltaT->Fill( deltat0 );

      //float deltat = (track_mtdt->at(itrack)-track_t0->at(itrack));
      float deltat = (track_mtdt->at(itrack)-track_mcMatch_genVtx_t->at(itrack));
      float beta = track_path_len->at(itrack)/(2.99792458e1*deltat);
      float energy = p/beta;


      TLorentzVector v;
      v.SetPtEtaPhiE( pt, eta, track_phi->at(itrack), energy );
      
      h1_trackMass->Fill( v.M() );
      int pdgId = (int)(track_mcMatch_genPdgId->at(itrack));
      if( abs(pdgId)==211  ) h1_trackMass_pion  ->Fill( v.M() );
      if( abs(pdgId)==321  ) h1_trackMass_kaon  ->Fill( v.M() );
      if( abs(pdgId)==2212 ) h1_trackMass_proton->Fill( v.M() );

      h1_beta->Fill( beta );
      if( beta > 0. ) {
        h1_invBeta->Fill( 1./beta );
        if( abs(pdgId)==211  ) h1_invBeta_pion  ->Fill( 1./beta );
        if( abs(pdgId)==321  ) h1_invBeta_kaon  ->Fill( 1./beta );
        if( abs(pdgId)==2212 ) h1_invBeta_proton->Fill( 1./beta );
      }

      TLorentzVector vGen;
      vGen.SetPtEtaPhiE( track_mcMatch_genPt->at(itrack), track_mcMatch_genEta->at(itrack), track_mcMatch_genPhi->at(itrack), track_mcMatch_genE->at(itrack) );
      
      float betaGen = vGen.Beta();
      h1_deltaBeta->Fill( (beta-betaGen)/betaGen );


      int etaBin = -1;
      for( unsigned i=0; i<etaBins.size()-1; ++i ) {
        if( fabs(eta) >= etaBins[i] && fabs(eta) < etaBins[i+1] ) {
          etaBin = i;
          break;
        }
      } // for eta bins

      int pBin = -1;
      for( unsigned i=0; i<pBins.size()-1; ++i ) {
        if( p >= pBins[i] && p < pBins[i+1] ) {
          pBin = i;
          break;
        }
      } // for p bins


      if( etaBin>=0 && pBin>=0 ) {

        if( beta>0. ) vvh1_invBeta[etaBin][pBin]->Fill( 1./beta );
        vvh1_trackP [etaBin][pBin]->Fill( p );
        vvh1_trackMass [etaBin][pBin]->Fill( v.M() );
        vvh1_deltaP   [etaBin][pBin]->Fill( (p - vGen.P())/vGen.P() );
        vvh1_deltaBeta[etaBin][pBin]->Fill( (beta-betaGen)/betaGen );
        vvh1_deltaInvBeta[etaBin][pBin]->Fill( (1./beta-1./betaGen)/(1./betaGen) );
        if( beta<1. ) vvh1_betaFunc[etaBin][pBin]->Fill( sqrt( 1./(beta*beta) - 1. ) ) ;

        if( abs(pdgId)==211 ) { // charged pions
          if( beta>0. ) vvh1_invBeta_pion[etaBin][pBin]->Fill( 1./beta );
          vvh1_trackP_pion [etaBin][pBin]->Fill( p );
          vvh1_trackMass_pion [etaBin][pBin]->Fill( v.M() );
          vvh1_deltaP_pion    [etaBin][pBin]->Fill( (p - vGen.P())/vGen.P() );
          vvh1_deltaBeta_pion [etaBin][pBin]->Fill( (beta-betaGen)/betaGen );
          vvh1_deltaInvBeta_pion[etaBin][pBin]->Fill( (1./beta-1./betaGen)/(1./betaGen) );
          if( beta<1. ) vvh1_betaFunc_pion[etaBin][pBin]->Fill( sqrt( 1./(beta*beta) - 1. ) ) ;
        }

        if( abs(pdgId)==321 ) { // charged kaons
          if( beta>0. ) vvh1_invBeta_kaon[etaBin][pBin]->Fill( 1./beta );
          vvh1_trackP_kaon [etaBin][pBin]->Fill( p );
          vvh1_trackMass_kaon [etaBin][pBin]->Fill( v.M() );
          vvh1_deltaP_kaon    [etaBin][pBin]->Fill( (p - vGen.P())/vGen.P() );
          vvh1_deltaBeta_kaon [etaBin][pBin]->Fill( (beta-betaGen)/betaGen );
          vvh1_deltaInvBeta_kaon[etaBin][pBin]->Fill( (1./beta-1./betaGen)/(1./betaGen) );
          if( beta<1. ) vvh1_betaFunc_kaon[etaBin][pBin]->Fill( sqrt( 1./(beta*beta) - 1. ) ) ;
        }

        if( abs(pdgId)==2212 ) { //protons
          if( beta>0. ) vvh1_invBeta_proton[etaBin][pBin]->Fill( 1./beta );
          vvh1_trackP_proton [etaBin][pBin]->Fill( p );
          vvh1_trackMass_proton [etaBin][pBin]->Fill( v.M() );
          vvh1_deltaP_proton    [etaBin][pBin]->Fill( (p - vGen.P())/vGen.P() );
          vvh1_deltaBeta_proton [etaBin][pBin]->Fill( (beta-betaGen)/betaGen );
          vvh1_deltaInvBeta_proton[etaBin][pBin]->Fill( (1./beta-1./betaGen)/(1./betaGen) );
          if( beta<1. ) vvh1_betaFunc_proton[etaBin][pBin]->Fill( sqrt( 1./(beta*beta) - 1. ) );
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

  for( unsigned i=0; i<vvh1_invBeta.size(); ++i ) {

    for( unsigned j=0; j<vvh1_invBeta[i].size(); ++j ) {

      vvh1_invBeta[i][j]->Write();
      vvh1_invBeta_pion[i][j]->Write();
      vvh1_invBeta_kaon[i][j]->Write();
      vvh1_invBeta_proton[i][j]->Write();

      vvh1_trackP [i][j]->Write();
      vvh1_trackP_pion [i][j]->Write();
      vvh1_trackP_kaon [i][j]->Write();
      vvh1_trackP_proton [i][j]->Write();

      vvh1_trackMass [i][j]->Write();
      vvh1_trackMass_pion [i][j]->Write();
      vvh1_trackMass_kaon [i][j]->Write();
      vvh1_trackMass_proton [i][j]->Write();

      vvh1_deltaP [i][j]->Write();
      vvh1_deltaP_pion [i][j]->Write();
      vvh1_deltaP_kaon [i][j]->Write();
      vvh1_deltaP_proton [i][j]->Write();

      vvh1_deltaBeta [i][j]->Write();
      vvh1_deltaBeta_pion [i][j]->Write();
      vvh1_deltaBeta_kaon [i][j]->Write();
      vvh1_deltaBeta_proton [i][j]->Write();

      vvh1_deltaInvBeta [i][j]->Write();
      vvh1_deltaInvBeta_pion [i][j]->Write();
      vvh1_deltaInvBeta_kaon [i][j]->Write();
      vvh1_deltaInvBeta_proton [i][j]->Write();

      vvh1_betaFunc [i][j]->Write();
      vvh1_betaFunc_pion [i][j]->Write();
      vvh1_betaFunc_kaon [i][j]->Write();
      vvh1_betaFunc_proton [i][j]->Write();


    } // for p

  } // for eta

  outfile->Close();

  std::cout << "-> Find your stuff in: " << outfile->GetName() << std::endl;

  return 0;

}



