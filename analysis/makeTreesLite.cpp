#include <iostream>
#include <fstream>
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TChain.h"
#include "TString.h"

#include "../interface/BTLConf.h"


#define MAXIND 999 


BTLConf getConfFromRunNumber( int runNumber ); // , int digiChSet) {
float getHodoPosition( int nFibres[2], float var[2] );


int main( int argc, char* argv[] ) {

  if( argc==1 ) {
    std::cout << "-> USAGE: ./makeTreesLite [confName]" << std::endl;
    exit(1);
  }


  std::string confName = "None";
  if( argc > 1 ) 
    confName = std::string(argv[1]);


  BTLConf conf(confName);

  if( conf.ninoThr()<0 && conf.runNumber()>0 ) 
    conf = getConfFromRunNumber( conf.runNumber() );

  system( "mkdir -p treesLite" );

    
  std::cout << "-> Configuration: " << conf.get_confName() << std::endl;

  //std::string fileListName(Form("files_Conf_%d_%d_%.0f_%.0f_*.txt", conf.sensorConf(), conf.digiConfNumber(), conf.ninoThr(), conf.vBias() ));
  //std::string fileListName(Form("files_Conf_%d_%d_%.0f_%.0f_%.0f.txt", conf.sensorConf(), conf.digiConfNumber(), conf.ninoThr(), conf.vBias1(), conf.vBias2()));


  TChain* tree = new TChain("digi");
  TChain* hodo = new TChain("hodo");
  TChain* info = new TChain("info");


  if( conf.runNumber()>0 ) { // open only one file:

    std::string fileName( Form("/eos/cms/store/group/dpg_mtd/comm_mtd/TB/MTDTB_H4_Sep2018/ntuples_v2/%d.root", conf.runNumber()) );

    std::cout << "-> Opening file: " << fileName << std::endl;

    tree->Add( Form("%s/digi", fileName.c_str()) );
    hodo->Add( Form("%s/hodo", fileName.c_str()) );
    info->Add( Form("%s/info", fileName.c_str()) );

  } else { // go through file list:

    std::ifstream ifs_files(conf.get_fileListName().c_str());

    std::cout << "-> Opened file: " << conf.get_fileListName() << std::endl;
   
    if( ifs_files.good() ) {

      std::string line;

      while( getline(ifs_files,line) ) {
        TString line_tstr(line);
        if( line_tstr.BeginsWith("#") ) continue;
        tree->Add( Form("%s/digi", line.c_str()) );
        hodo->Add( Form("%s/hodo", line.c_str()) );
        info->Add( Form("%s/info", line.c_str()) );
        std::cout << "-> Added: " << line << std::endl;
      }

    }

  } // if/else runnumber


  // digi tree branches:
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

  // hodo tree branches:
  float hodox[2];
  hodo->SetBranchAddress( "X", hodox );
  float hodoy[2];
  hodo->SetBranchAddress( "Y", hodoy );
  int nFibresOnX[2];
  hodo->SetBranchAddress( "nFibresOnX", nFibresOnX );
  int nFibresOnY[2];
  hodo->SetBranchAddress( "nFibresOnY", nFibresOnY );

  // info tree branches
  float sensorConf;
  info->SetBranchAddress( "sensorConf", &sensorConf );
  //float digiConf;
  //info->SetBranchAddress( "digiConf", &digiConf );
  float NINOthr;
  info->SetBranchAddress( "NINOthr", &NINOthr );
  float Vbias1;
  info->SetBranchAddress( "Vbias1", &Vbias1 );
  float Vbias2;
  info->SetBranchAddress( "Vbias2", &Vbias2 );
  float tableX;
  info->SetBranchAddress( "tableX", &tableX );
  float tableY;
  info->SetBranchAddress( "tableY", &tableY );



  TFile* outfile = TFile::Open( Form("treesLite/%s.root", conf.get_confName().c_str()), "RECREATE" );
  outfile->cd();
  TTree* outtree = new TTree( "treeLite", "" );

  float tRight;
  outtree->Branch( "tRight", &tRight, "tRight/F" );
  float tLeft;
  outtree->Branch( "tLeft", &tLeft, "teftL/F" );
  float ampMaxRight;
  outtree->Branch( "ampMaxRight", &ampMaxRight, "ampMaxRight/F" );
  float ampMaxLeft;
  outtree->Branch( "ampMaxLeft", &ampMaxLeft, "ampMaxLeft/F" );
  float x_hodo;
  outtree->Branch( "x_hodo", &x_hodo, "x_hodo/F" );
  float y_hodo;
  outtree->Branch( "y_hodo", &y_hodo, "y_hodo/F" );

  TH1D* h1_ampMaxMCP = new TH1D( "ampMaxMCP", "", 110, 0., 1.1 );

  int nentries = tree->GetEntries();

  for( int iEntry=0; iEntry<nentries; ++iEntry ) {

    if( iEntry % 100000 == 0 ) std::cout << "  Entry: " << iEntry << " / " << nentries << std::endl;

    tree->GetEntry(iEntry);
    hodo->GetEntry(iEntry);
    info->GetEntry(iEntry);

    //if( Vbias1 != conf.vBias1() ) continue;
    //if( Vbias2 != conf.vBias2() ) continue;
    if( NINOthr != conf.ninoThr() && conf.ninoThr()>-1) continue;
    if( sensorConf != (float)conf.sensorConf() && conf.sensorConf()>-1 ) continue;
    //if( conf.digiConf()==6 ) {
    //  if( digiConf!=6 && digiConf!=7 ) continue; // 6 and 7 are the same for me for now
    //} else {
    //  if( digiConf   != (float)conf.digiConfNumber() && conf.digiConfNumber()>-1 ) continue;
    //}

    x_hodo = getHodoPosition( nFibresOnX, hodox );
    y_hodo = getHodoPosition( nFibresOnY, hodoy );

    // correct for table position
    x_hodo = x_hodo + (38. -tableX);
    y_hodo = y_hodo + (143.-tableY);


    float ampMaxMCP = amp_max[PTK1]/4096.;
    h1_ampMaxMCP->Fill( ampMaxMCP );

    if( ampMaxMCP<0.1 || ampMaxMCP>0.55 ) continue;

    float tMCP = time[PTK1+CFD];

    int iLeft  = (conf.digiChSet()=="a") ? NINO1+LED : NINO3+LED ;
    int iRight = (conf.digiChSet()=="a") ? NINO2+LED : NINO4+LED ;

    // remove DC events:
    if( time[iLeft ]<14. || time[iLeft ]>34. ) continue;
    if( time[iRight]<14. || time[iRight]>34. ) continue;

    tLeft  = time[iLeft]-tMCP;
    tRight = time[iRight]-tMCP;

    ampMaxLeft  = (conf.digiChSet()=="a") ? amp_max[AMP1]/4096. : amp_max[AMP3]/4096.;
    ampMaxRight = (conf.digiChSet()=="a") ? amp_max[AMP2]/4096. : amp_max[AMP4]/4096.;

    if( ampMaxRight>0.003 || ampMaxLeft>0.003 ) // cut obvious noise events
      outtree->Fill();

  }

  outfile->cd();
  outtree->Write();
  h1_ampMaxMCP->Write();
  outfile->Close();

  std::cout << "-> Find your stuff here: " << outfile->GetName() << std::endl;

  return 0;

}



float getHodoPosition( int nFibres[2], float var[2] ) {

  int nFibresMax = 4;

  bool showering0 = nFibres[0]>=nFibresMax;
  bool showering1 = nFibres[1]>=nFibresMax;

  bool bad0 = nFibres[0]==0 || var[0]<-900. || showering0;
  bool bad1 = nFibres[1]==0 || var[1]<-900. || showering1;

  float pos = -999.;

  //if( showering0 || showering1 ) pos = -999.;  // showering (at least one plane)
  //else if( bad0 && bad1 ) pos = -999.;  // empty (both planes have 0 fibres)

  if( bad0 && bad1 ) pos = -999.;  // empty (both planes have 0 fibres)

  else {

    if( bad0 ) pos = var[1];

    else if( bad1 ) pos = var[0];

    else pos = 0.5*(var[0]+var[1]);

  } // else

  return pos;

}


BTLConf getConfFromRunNumber( int runNumber ) { // , int digiChSet) {

  std::cout << "-> Setting conf from runNumber: "  << runNumber << std::endl;

  BTLConf thisConf;

  TFile* file = TFile::Open( Form("/eos/cms/store/group/dpg_mtd/comm_mtd/TB/MTDTB_H4_Sep2018/ntuples_v2/%d.root", runNumber) );

  TTree* info = (TTree*)file->Get("info");

  // info tree branches
  float sensorConf;
  info->SetBranchAddress( "sensorConf", &sensorConf );
  float NINOthr;
  info->SetBranchAddress( "NINOthr", &NINOthr );
  float Vbias1;
  info->SetBranchAddress( "Vbias1", &Vbias1 );
  float Vbias2;
  info->SetBranchAddress( "Vbias2", &Vbias2 );

  info->GetEntry(1);

  thisConf.set_digiChSet( "a" ); // for now
  thisConf.set_ninoThr( NINOthr );
  thisConf.set_vBias( Vbias1 );
  thisConf.set_runNumber( runNumber );

  std::cout << "-> Done: digiChSet: a  sensorConf: " << sensorConf << "  NINOthr: " << NINOthr << "  Vbias: " << Vbias1 << std::endl;

  return thisConf;

}
