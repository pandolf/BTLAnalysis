#include "TFile.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TPaveText.h"

#include "../interface/LLPCommon.h"





int main( int argc, char* argv[] ) {

  std::string fileName 

  TFile* file = TFile::Open( "llpFile.root" );

  

  std::vector<float> etaBins = LLPCommon::etaBins();


  for( unsigned i=0 i<etaBins.size()-1; ++i ) {

    TH1D* h1_trackP  = (TH1D*)file->Get( Form("trackP_eta%d" , i) );
    TH1D* h1_invBeta = (TH1D*)file->Get( Form("invBeta_eta%d", i) );

    float aveP = h1_trackP->GetMean();

    TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
    c1->cd();

    h1_invBeta->Draw();
