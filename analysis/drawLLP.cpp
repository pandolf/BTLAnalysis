#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TLine.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TPaveText.h"

#include "../interface/LLPCommon.h"





int main( int argc, char* argv[] ) {


  LLPCommon::setStyle();


  std::string fileName = "llpFile.root";

  if( argc>1 ) 
    fileName = std::string(argv[1]);

  std::string suffix="";
  TString fileName_tstr(fileName);

  if( fileName_tstr.BeginsWith("llpFile_") ) {
    suffix = fileName;
    suffix.erase( 0, 7 );
    suffix.erase( suffix.end()-5, suffix.end() );
  }


  std::string outdir(Form("plots%s", suffix.c_str()));
  system( Form("mkdir -p %s", outdir.c_str()) );

  TFile* file = TFile::Open( fileName.c_str() );


  std::vector<float> etaBins = LLPCommon::etaBins();


  for( unsigned i=0; i<etaBins.size()-1; ++i ) {

    TH1D* h1_trackP  = (TH1D*)file->Get( Form("trackP_eta%d" , i) );
    TH1D* h1_invBeta = (TH1D*)file->Get( Form("invBeta_eta%d", i) );


    TCanvas* c1 = new TCanvas( Form("c1_%d", i), "", 600, 600 );
    c1->cd();

    float xMin = h1_invBeta->GetXaxis()->GetXmin();
    float xMax = h1_invBeta->GetXaxis()->GetXmax();
    float yMax = 1.2*h1_invBeta->GetMaximum();
    TH2D* h2_axes = new TH2D( Form("axes_%d", i), "", 10, xMin, xMax, 10, 0., yMax );
    h2_axes->SetXTitle( "1/#beta" );
    h2_axes->SetYTitle( "Tracks" );
    h2_axes->Draw();

    float mpi = 0.1396;
    float p = h1_trackP->GetMean();
    float invBeta_theory = 1. + mpi*mpi/(p*p);

    TLine* line_pi = new TLine( invBeta_theory, 0., invBeta_theory, yMax );
    line_pi->SetLineStyle(2);
    line_pi->SetLineWidth(2);
    line_pi->SetLineColor(46);
    line_pi->Draw("same");

    h1_invBeta->Draw("same");

    TPaveText* etaText = new TPaveText( 0.2, 0.8, 0.45, 0.9, "brNDC" );
    etaText->SetTextSize(0.035);
    etaText->SetFillColor(0);
    etaText->AddText( Form( "%.1f < |#eta| < %.1f", etaBins[i], etaBins[i+1] ) );
    etaText->Draw("same");

    gPad->RedrawAxis();

    c1->SaveAs( Form("%s/invBeta_eta%d.pdf", outdir.c_str(), i) );

    delete c1;
    delete h2_axes;

  } // for eta bins

  return 0;

}
