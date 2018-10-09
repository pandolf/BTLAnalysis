#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TH1D.h"
#include "TProfile.h"
#include "TLine.h"
#include "TLegend.h"
#include "TString.h"

#include "../interface/BTLCommon.h"
#include "../interface/BTLConf.h"


bool do_hodoCorr = true;



TF1* fitLandau( BTLConf conf, TTree* tree, TH1D* histo, const std::string& varName );
std::vector<float> getBins( int nBins, float xMin, float xMax );
int findBin( float var, std::vector<float> bins );
int findBin( float var, int nBins, float xMin, float xMax );
TF1* getAmpWalkCorr( const BTLConf& conf, const std::vector<TH1D*>& vh1_t, const std::vector<TH1D*>& vh1_ampMax, const std::string& name );
TF1* getHodoCorr( BTLConf conf, std::vector<float> xBins, std::vector<TH1D*> vh1, const std::string& yName, const std::string& xName );



int main( int argc, char* argv[] ) {


  std::string confName="";

  if( argc>1 ) {

    confName = std::string(argv[1]);

  } else {

    std::cout << "USAGE: ./calibrateTreeLite [confName]" << std::endl;
    exit(1);

  }

  BTLCommon::setStyle();

  const BTLConf conf(confName);

  TFile* file = TFile::Open( Form("treesLite/%s.root", confName.c_str()) );
  TTree* tree = (TTree*)file->Get("treeLite");

  float tLeft;
  tree->SetBranchAddress( "tLeft", &tLeft );
  float tRight;
  tree->SetBranchAddress( "tRight", &tRight );
  float ampMaxLeft;
  tree->SetBranchAddress( "ampMaxLeft", &ampMaxLeft );
  float ampMaxRight;
  tree->SetBranchAddress( "ampMaxRight", &ampMaxRight );


  float scaleFactor = 72. - conf.vBias();
  if( scaleFactor==0. ) scaleFactor = 1.;

  float xMaxRight = 0.8;
  float xMaxLeft = ( conf.digiConf()=="6a" ) ? 0.2 : xMaxRight;

  // first of all fit landau to find ampMax range:
  TH1D* h1_ampMaxLeft = new TH1D( "ampMaxLeft", "", 100, 0., xMaxLeft/scaleFactor );
  h1_ampMaxLeft ->SetXTitle( "Max Amplitude Left [a.u.]" );
  TH1D* h1_ampMaxRight = new TH1D( "ampMaxRight", "", 100, 0., xMaxRight/scaleFactor  );
  h1_ampMaxRight->SetXTitle( "Max Amplitude Right [a.u.]" );

  TF1* fitLandauL = fitLandau( conf, tree, h1_ampMaxLeft , "ampMaxLeft"  );
  TF1* fitLandauR = fitLandau( conf, tree, h1_ampMaxRight, "ampMaxRight" );

  float ampMaxLeft_maxCut  = fitLandauL->GetParameter(1)*3.;
  float ampMaxLeft_minCut  = fitLandauL->GetParameter(1)*0.8;

  float ampMaxRight_maxCut  = fitLandauR->GetParameter(1)*3.;
  float ampMaxRight_minCut  = fitLandauR->GetParameter(1)*0.8;

  float ampMaxLeft_maxBins = fitLandauL->GetParameter(1)*3.;
  float ampMaxLeft_minBins = fitLandauL->GetParameter(1)*0.8;

  float ampMaxRight_maxBins = fitLandauR->GetParameter(1)*3.;
  float ampMaxRight_minBins = fitLandauR->GetParameter(1)*0.8;

  
  int nBins_ampMax = 50;
  std::vector<float> bins_ampMaxLeft  = getBins( nBins_ampMax, ampMaxLeft_minBins , ampMaxLeft_maxBins  );
  std::vector<float> bins_ampMaxRight = getBins( nBins_ampMax, ampMaxRight_minBins, ampMaxRight_maxBins );

  std::vector< TH1D* > vh1_tLeft, vh1_tRight;
  std::vector< TH1D* > vh1_ampMaxLeft, vh1_ampMaxRight;

  int nBinsT = 100;
  float tMin = (conf.digiConf()=="6a") ? 2. : 3.;
  float tMax = (conf.digiConf()=="6a") ? 4. : 5.;

  TH1D* h1_tLeft_int  = new TH1D( "tLeft_int" , "", nBinsT, tMin, tMax );
  TH1D* h1_tRight_int = new TH1D( "tRight_int", "", nBinsT, tMin, tMax );

  for( int i=0; i<nBins_ampMax-1; ++i ) {

    TH1D* h1_tLeft = new TH1D( Form("tLeft_bin%d", i), "", nBinsT, tMin, tMax );
    h1_tLeft->SetXTitle( "t_{Left} [ns]" );
    //h1_tLeft->SetYTitle( "Entries" );
    vh1_tLeft.push_back( h1_tLeft );

    TH1D* h1_ampMaxLeft = new TH1D( Form("ampMaxLeft_bin%d", i), "", 50, bins_ampMaxLeft[i], bins_ampMaxLeft[i+1] );
    h1_ampMaxLeft->SetXTitle( "Max Amplitude Left [a.u.]" );
    //h1_ampMaxLeft->SetYTitle( "Entries" );
    vh1_ampMaxLeft.push_back( h1_ampMaxLeft );

    TH1D* h1_tRight = new TH1D( Form("tRight_bin%d", i), "", nBinsT, tMin, tMax );
    h1_tRight->SetXTitle( "t_{Right} [ns]" );
    //h1_tRight->SetYTitle( "Entries" );
    vh1_tRight.push_back( h1_tRight );

    TH1D* h1_ampMaxRight = new TH1D( Form("ampMaxRight_bin%d", i), "", 50, bins_ampMaxRight[i], bins_ampMaxRight[i+1] );
    h1_ampMaxRight->SetXTitle( "Max Amplitude Right [a.u.]" );
    //h1_ampMaxRight->SetYTitle( "Entries" );
    vh1_ampMaxRight.push_back( h1_ampMaxRight );

  } // for bins_ampMax


  int nEntries = tree->GetEntries();

  
  for( int iEntry = 0; iEntry<nEntries; ++iEntry ) {

    if( iEntry % 10000 == 0 ) std::cout << " Entry: " << iEntry << " / " << nEntries << std::endl;

    tree->GetEntry( iEntry );

    if( ampMaxLeft>=ampMaxLeft_minCut && ampMaxLeft<=ampMaxLeft_maxCut ) {  // ampMaxLeft is good

      int thisBinLeft = findBin( ampMaxLeft, bins_ampMaxLeft );

      if( thisBinLeft>=0 ) {
        h1_tLeft_int->Fill( tLeft );
        vh1_tLeft[thisBinLeft]->Fill( tLeft );
        vh1_ampMaxLeft[thisBinLeft]->Fill( ampMaxLeft );
      }

    } // if ampMaxLeft is good

    if( ampMaxRight>=ampMaxRight_minCut && ampMaxRight<=ampMaxRight_maxCut ) {  // ampMaxRight is good

      int thisBinRight = findBin( ampMaxRight, bins_ampMaxRight );

      if( thisBinRight>=0 ) {
        h1_tRight_int->Fill( tRight );
        vh1_tRight[thisBinRight]->Fill( tRight );
        vh1_ampMaxRight[thisBinRight]->Fill( ampMaxRight );
      }

    } // if ampMaxRight is good

  } // for Entries


  std::string fitsDir(Form("plots/%s/", conf.get_confName().c_str()));
  system( Form("mkdir -p %s/ampWalkFits", fitsDir.c_str()) );


  TF1* f1_ampWalkLeft  = getAmpWalkCorr( conf, vh1_tLeft , vh1_ampMaxLeft , "Left"  );
  TF1* f1_ampWalkRight = getAmpWalkCorr( conf, vh1_tRight, vh1_ampMaxRight, "Right" );

  float target_ampWalkLeft  = h1_tLeft_int ->GetMean();
  float target_ampWalkRight = h1_tRight_int->GetMean();
  //float target_ampWalkLeft  = f1_ampWalkLeft ->Eval( bins_ampMaxLeft [0] );
  //float target_ampWalkRight = f1_ampWalkRight->Eval( bins_ampMaxRight[0] );


  // prepare new file
  std::string suffix = "_corr";
  TFile* outfile = TFile::Open( Form("treesLite/%s%s.root", confName.c_str(), suffix.c_str()), "RECREATE" );
  TTree* newtree = tree->CloneTree(0);

  float tLeft_corr;
  newtree->Branch( "tLeft_corr", &tLeft_corr );
  float tRight_corr;
  newtree->Branch( "tRight_corr", &tRight_corr );

  //float tAveCorr = -1.;
  //newtree->Branch( "tAveCorr", &tAveCorr );


  std::vector< TH1D* > vh1_tLeft_corr, vh1_tRight_corr;

  for( int i=0; i<nBins_ampMax-1; ++i ) {

    TH1D* h1_tLeft_corr = new TH1D( Form("tLeft_corr_bin%d", i), "", 100, tMin, tMax );
    vh1_tLeft_corr.push_back( h1_tLeft_corr );

    TH1D* h1_tRight_corr = new TH1D( Form("tRight_corr_bin%d", i), "", 100, tMin, tMax );
    vh1_tRight_corr.push_back( h1_tRight_corr );

  } // for bins_ampMax


  float x_hodo;
  tree->SetBranchAddress( "x_hodo", &x_hodo );
  float y_hodo;
  tree->SetBranchAddress( "y_hodo", &y_hodo );


  int nBins_xHodo = 100;
  float xMin_xHodo = -20.;
  float xMax_xHodo = 20.;
  float binWidth_xHodo = (xMax_xHodo-xMin_xHodo)/((float)nBins_xHodo);
  
  std::vector<float> xBins_xHodo;
  std::vector<TH1D*> vh1_tAve_vs_xHodo;

  for( int i=0; i<nBins_xHodo; ++i ) {

    xBins_xHodo.push_back( xMin_xHodo + (float)i*binWidth_xHodo );

    TH1D* h1_tAve_vs_xHodo = new TH1D( Form("tAve_vs_xHodo_%d", i), "", 100, tMin, tMax );
    vh1_tAve_vs_xHodo.push_back( h1_tAve_vs_xHodo );

  } // for bins xHodo

  xBins_xHodo.push_back( xMin_xHodo + (float)nBins_xHodo*binWidth_xHodo );


  for( int iEntry = 0; iEntry<nEntries; ++iEntry ) {

    tree->GetEntry( iEntry );

    if( ampMaxLeft <=ampMaxLeft_minCut  || ampMaxLeft >=ampMaxLeft_maxCut  ) continue;
    if( ampMaxRight<=ampMaxRight_minCut || ampMaxRight>=ampMaxRight_maxCut ) continue;

    tLeft_corr  = tLeft  * ( target_ampWalkLeft  / f1_ampWalkLeft ->Eval( ampMaxLeft  ) );
    tRight_corr = tRight * ( target_ampWalkRight / f1_ampWalkRight->Eval( ampMaxRight ) );

    int thisBinLeft  = findBin( ampMaxLeft , bins_ampMaxLeft  );
    if( thisBinLeft>=0 ) 
      vh1_tLeft_corr [thisBinLeft] ->Fill( tLeft_corr  );

    int thisBinRight = findBin( ampMaxRight, bins_ampMaxRight );
    if( thisBinRight>=0 ) 
      vh1_tRight_corr[thisBinRight]->Fill( tRight_corr );


    int thisBin_xHodo = findBin( x_hodo, nBins_xHodo, xMin_xHodo, xMax_xHodo );
    if( thisBin_xHodo>=0 )
      vh1_tAve_vs_xHodo[thisBin_xHodo]->Fill( 0.5*(tLeft_corr+tRight_corr) );

    newtree->Fill();

  } // for entries

  // check AW closure:
  getAmpWalkCorr( conf, vh1_tLeft_corr , vh1_ampMaxLeft , "Left_corr"  );
  getAmpWalkCorr( conf, vh1_tRight_corr, vh1_ampMaxRight, "Right_corr" );


  TTree* newtree2 = 0;
 
  if( do_hodoCorr ) {

    TF1* f1_tAve_vs_xHodo = getHodoCorr( conf, xBins_xHodo, vh1_tAve_vs_xHodo, "tAve", "xHodo" );

    newtree->SetBranchAddress( "tLeft_corr" , &tLeft_corr );
    newtree->SetBranchAddress( "tRight_corr", &tRight_corr );
    newtree->SetBranchAddress( "x_hodo", &x_hodo );

    newtree2 = newtree->CloneTree(0);
    newtree2->SetName( "treeLite" );

    float tAveCorr;
    newtree2->Branch( "tAveCorr", &tAveCorr );

    int nentries2 = newtree->GetEntries();

    double xMin_fit, xMax_fit;
    f1_tAve_vs_xHodo->GetRange( xMin_fit, xMax_fit );

    std::vector<TH1D*> vh1_tAveCorr_vs_xHodo;
    for( int i=0; i<nBins_xHodo; ++i ) {
      TH1D* h1_tAveCorr_vs_xHodo = new TH1D( Form("tAveCorr_vs_xHodo_%d", i), "", 100, tMin, tMax );
      vh1_tAveCorr_vs_xHodo.push_back( h1_tAveCorr_vs_xHodo );
    } 

    std::cout << "-> Second round to correct vs position: " << std::endl;

    for( int iEntry=0; iEntry<nentries2; ++iEntry ) {

      newtree->GetEntry(iEntry);

      float tAve = 0.5*( tLeft_corr + tRight_corr );
      if( x_hodo >= xMin_fit && x_hodo < xMax_fit )
        tAveCorr  = tAve  * ( f1_tAve_vs_xHodo->Eval( 0.5*(xMin_fit+xMax_fit) )  / f1_tAve_vs_xHodo ->Eval( x_hodo ) );
      else
        tAveCorr  = tAve;

      int thisBin_xHodo = findBin( x_hodo, nBins_xHodo, xMin_xHodo, xMax_xHodo );
      if( thisBin_xHodo>=0 )
        vh1_tAveCorr_vs_xHodo[thisBin_xHodo]->Fill( tAveCorr );

      newtree2->Fill();

    } // for entries 2

    getHodoCorr( conf, xBins_xHodo, vh1_tAveCorr_vs_xHodo, "tAveCorr", "xHodo" );

  } // if doHodo

 
  outfile->cd();

  if( newtree2 != 0 )
    newtree2->Write();
  else
    newtree->Write();
  
  h1_tLeft_int ->Write();
  h1_tRight_int->Write();

  //for( unsigned i=0; i<vh1_tLeft      .size(); ++i ) vh1_tLeft[i]      ->Write();
  //for( unsigned i=0; i<vh1_tRight     .size(); ++i ) vh1_tRight[i]     ->Write();
  //for( unsigned i=0; i<vh1_tLeft_corr .size(); ++i ) vh1_tLeft_corr[i] ->Write();
  //for( unsigned i=0; i<vh1_tRight_corr.size(); ++i ) vh1_tRight_corr[i]->Write();
  //for( unsigned i=0; i<vh1_ampMaxLeft .size(); ++i ) vh1_ampMaxLeft[i] ->Write();
  //for( unsigned i=0; i<vh1_ampMaxRight.size(); ++i ) vh1_ampMaxRight[i]->Write();

  outfile->Close();

  std::cout << "-> Find your stuff here: " << outfile->GetName() << std::endl;

  return 0;

}



TF1* fitLandau( BTLConf conf, TTree* tree, TH1D* histo, const std::string& varName ) {

  std::string outdir( Form("plots/%s", conf.get_confName().c_str()) );

  tree->Project( histo->GetName(), varName.c_str(), "" );

  float xMode = histo->GetBinCenter(histo->GetMaximumBin());
  float xMin_fit = xMode*0.8;
  float xMax_fit = xMode*3.;

  TF1* f1_landau = new TF1( Form("landau_%s", varName.c_str()), "landau", xMin_fit, xMax_fit );
  f1_landau->SetLineColor(46);

  histo->Fit( f1_landau->GetName(), "RQ0" );

  xMin_fit = 0.9*f1_landau->GetParameter(1);
  xMax_fit = 1.4*f1_landau->GetParameter(1);

  f1_landau->SetRange( xMin_fit, xMax_fit );

  int n_iter = 5;

  for( int i=0; i<n_iter; ++i ) { // iterative fit

    if( i==n_iter-1 )
      histo->Fit( f1_landau->GetName(), "RQ+" );
    else {
      histo->Fit( f1_landau->GetName(), "RQ0" );
      xMin_fit = 0.9*f1_landau->GetParameter(1);
      xMax_fit = 1.4*f1_landau->GetParameter(1);
      f1_landau->SetRange( xMin_fit, xMax_fit );
    }

  } // for iter

  TCanvas* c1 = new TCanvas( Form("c1_%s", histo->GetName()), "", 600, 600 );
  c1->cd();

  float yMax = 1.3*histo->GetMaximum();
  TH2D* h2_axes = new TH2D( Form("axes_%s", histo->GetName()), "", 10, histo->GetXaxis()->GetXmin(), histo->GetXaxis()->GetXmax(), 10, 0., yMax );
  h2_axes->SetXTitle( histo->GetXaxis()->GetTitle() );
  h2_axes->SetYTitle( "Entries" );
  h2_axes->Draw();

  TLine* line_cutMin = new TLine( 0.8*f1_landau->GetParameter(1), 0., 0.8*f1_landau->GetParameter(1), yMax );
  TLine* line_cutMax = new TLine(  3.*f1_landau->GetParameter(1), 0.,  3.*f1_landau->GetParameter(1), yMax );
 
  line_cutMin->SetLineStyle(2);
  line_cutMax->SetLineStyle(2);

  line_cutMin->SetLineColor(46);
  line_cutMax->SetLineColor(46);

  line_cutMin->Draw("same");
  line_cutMax->Draw("same");

  histo->Draw("same");

  BTLCommon::addLabels( c1, conf );

  c1->SaveAs( Form("%s/%s.eps", outdir.c_str(), histo->GetName()) );
  c1->SaveAs( Form("%s/%s.pdf", outdir.c_str(), histo->GetName()) );

  delete c1;
  
  return f1_landau;

}


std::vector<float> getBins( int nBins, float xMin, float xMax ) {

  float step = (xMax-xMin)/((float)nBins);

  std::vector<float> bins;

  for( int i=0; i<nBins; ++i )
    bins.push_back( xMin + (float)i*step );

  return bins;

}



int findBin( float var, std::vector<float> bins ) {

  int bin = -1;

  for( unsigned i=0; i<bins.size()-1; ++i ) {
    if( var>bins[i] && var<bins[i+1] ) {
      bin = i;
      break;
    } 
  }

  return bin;

}



int findBin( float var, int nBins, float xMin, float xMax ) {

  int bin = -1;
  float width = (xMax-xMin)/((float)nBins);

  for( int i=0; i<nBins-1; ++i ) {

    float binMin = xMin + i*width;
    if( var >= binMin && var < (binMin+width) ) {
      bin = i;
      break;
    }

  } // for bins

  return bin;

}
    
    


TF1* getAmpWalkCorr( const BTLConf& conf, const std::vector<TH1D*>& vh1_t, const std::vector<TH1D*>& vh1_ampMax, const std::string& name ) {


  std::string fitsDir(Form("plots/%s/", conf.get_confName().c_str()));

  TString name_tstr(name);
  bool is_corr = name_tstr.EndsWith("_corr");

  float ampMax_min = vh1_ampMax[0]->GetXaxis()->GetXmin();
  float ampMax_max = vh1_ampMax[vh1_ampMax.size()-1]->GetXaxis()->GetXmax();

  if( conf.ninoThr()==200. && conf.vBias()==69. ) 
    ampMax_max /= 1.5;

  if( conf.ninoThr()==60. ) {
    ampMax_max /= 2.;
    if( conf.vBias()==69. ) 
      ampMax_min *= 1.1;
  }

  if( conf.ninoThr()==40. ) 
    ampMax_max /= 2.;

  TGraphErrors* gr_ampWalk = new TGraphErrors(0);
  gr_ampWalk->SetName( Form("gr_ampWalk%s", name.c_str()) );

  TGraph* gr_ampWalk_sigmaUp = new TGraph(0);
  gr_ampWalk_sigmaUp->SetName( Form("gr_ampWalk%s_sigmaUp", name.c_str()) );

  TGraph* gr_ampWalk_sigmaDn = new TGraph(0);
  gr_ampWalk_sigmaDn->SetName( Form("gr_ampWalk%s_sigmaDn", name.c_str()) );

  for( unsigned i=0; i<vh1_t.size(); ++i ) {

    TF1* f1_gaus = BTLCommon::fitGaus( vh1_t[i] );

    TCanvas* c1 = new TCanvas( Form("c1_%s", vh1_t[i]->GetName()), "", 600, 600 );
    c1->cd();

    vh1_t[i]->Draw();

    BTLCommon::addLabels( c1, conf );

    c1->SaveAs( Form("%s/ampWalkFits/%s.eps", fitsDir.c_str(), vh1_t[i]->GetName()) );
    c1->SaveAs( Form("%s/ampWalkFits/%s.pdf", fitsDir.c_str(), vh1_t[i]->GetName()) );

    delete c1;


    float x     = vh1_ampMax[i]->GetMean();
    float x_err = vh1_ampMax[i]->GetMeanError();

    float y     = f1_gaus->GetParameter( 1 );
    float y_err = f1_gaus->GetParError ( 1 );
    float y_rms = f1_gaus->GetParameter( 2 );

    int iPoint = gr_ampWalk->GetN();
    gr_ampWalk->SetPoint     ( iPoint, x    , y     );
    gr_ampWalk->SetPointError( iPoint, x_err, y_err );

    gr_ampWalk_sigmaUp->SetPoint( iPoint, x    , y+y_rms     );
    gr_ampWalk_sigmaDn->SetPoint( iPoint, x    , y-y_rms     );

  } // for points


  TCanvas* c1= new TCanvas( Form("c1_ampWalk%s", name.c_str()), "", 600, 600 );
  c1->cd();

  float yMin_axes = (conf.digiConf()=="6a") ? 2.3 : 3.2;
  float yMax_axes = (conf.digiConf()=="6a") ? 3.9 : 4.9;

  TH2D* h2_axes = new TH2D( Form("axes%s", name.c_str()), "", 10, ampMax_min, ampMax_max, 10, yMin_axes, yMax_axes );
  if( name_tstr.Contains("Left") )
    h2_axes->SetXTitle( "Max Amplitude Left [a.u.]" );
  else
    h2_axes->SetXTitle( "Max Amplitude Right [a.u.]" );
  h2_axes->SetYTitle( Form("t_{%s} - t_{PTK} [ns]", name.c_str()) );
  h2_axes->Draw();
  
  gr_ampWalk->SetMarkerStyle(20);
  gr_ampWalk->SetMarkerSize(1.2);

  gr_ampWalk_sigmaUp->SetLineStyle(2);
  gr_ampWalk_sigmaUp->SetLineWidth(2);
  gr_ampWalk_sigmaUp->SetLineColor(38);

  gr_ampWalk_sigmaDn->SetLineStyle(2);
  gr_ampWalk_sigmaDn->SetLineWidth(2);
  gr_ampWalk_sigmaDn->SetLineColor(38);

  //std::string func = (name=="Right") ? "[0]+[1]/x+[2]/(x*x)+[3]/(x*x*x)+[4]/sqrt(x)" : "pol3";
  //std::string func = (name=="Right") ? "[0]+[1]*x+[2]*x*x+[3]*x*x*x+[4]/sqrt(x-[5])" : "pol3";
  std::string func = "pol5";
  //TF1* f1_ampWalk = new TF1( Form("fit_ampWalk%s", name.c_str()), func.c_str(), 0.16, 0.45 );
  TF1* f1_ampWalk = new TF1( Form("fit_ampWalk%s", name.c_str()), func.c_str(), ampMax_min, ampMax_max );
  //TF1* f1_ampWalk = new TF1( Form("fit_ampWalk%s", name.c_str()),"ROOT::Math::crystalball_function(-x, 2, 1, 0.05, 0.2)", ampMax_min, ampMax_max );
  f1_ampWalk->SetParameter(0, 6.);
  f1_ampWalk->SetLineColor( 46 );
  if( !is_corr )
    gr_ampWalk->Fit( f1_ampWalk->GetName(), "R" );

  gr_ampWalk->Draw( "P same" );
  gr_ampWalk_sigmaDn->Draw("L same");
  gr_ampWalk_sigmaUp->Draw("L same");

  bool bottomLeft = conf.vBias()<71. && conf.ninoThr()>=90. && name_tstr.Contains("Left");
  float xMin_leg = bottomLeft ? 0.2 : 0.55;
  float yMin_leg = bottomLeft ? 0.2 : 0.7 ;
  float xMax_leg = bottomLeft ? 0.65: 0.9 ;
  float yMax_leg = bottomLeft ? 0.4 : 0.9 ;
  if( is_corr ) yMin_leg += 0.07;
    
  TLegend* legend = new TLegend( xMin_leg, yMin_leg, xMax_leg, yMax_leg );
  legend->SetTextSize(0.035);
  legend->SetFillColor(0);
  legend->AddEntry( gr_ampWalk, "Data", "P" );
  legend->AddEntry( gr_ampWalk_sigmaDn, "#pm 1#sigma Gaus", "L" );
  if( !is_corr ) 
    legend->AddEntry( f1_ampWalk, "Fit", "L" );
  legend->Draw("same");

  BTLCommon::addLabels( c1, conf );

  c1->SaveAs( Form("%s/ampWalk%s.eps", fitsDir.c_str(), name.c_str()) );
  c1->SaveAs( Form("%s/ampWalk%s.pdf", fitsDir.c_str(), name.c_str()) );

  delete c1;

  return f1_ampWalk;

}



TF1* getHodoCorr( BTLConf conf, std::vector<float> xBins, std::vector<TH1D*> vh1, const std::string& yName, const std::string& xName ) {

  
  std::string axisName = (xName=="xHodo") ? "Hodoscope X [mm]" : "Hodoscope Y [mm]";

  std::string fitsDir(Form("plots/%s/fits_%s/", conf.get_confName().c_str(), xName.c_str()) );
  system( Form("mkdir -p %s", fitsDir.c_str()) );

  
  TGraphErrors* graph = new TGraphErrors(0);
  graph->SetName( Form("%s_vs_%s", yName.c_str(), xName.c_str()) );

  for( unsigned i=0; i<vh1.size(); ++i ) {

    if( vh1[i]->GetEntries()<2 ) continue;

    float binWidth = xBins[i+1]-xBins[i];
    float x = xBins[i] + 0.5*binWidth;
    float x_err = binWidth/sqrt(12);

    TF1* f1_gaus = BTLCommon::fitGaus( vh1[i] );

    TCanvas* c1 = new TCanvas( Form("c1_%d", i), "", 600, 600 );
    c1->cd();

    vh1[i]->Draw();

    //c1->SaveAs( Form( "plots/%s/fits_xHodo/%s.eps", conf.get_confName().c_str(), vh1_tAve_vs_xHodo[i]->GetName()) );
    c1->SaveAs( Form( "%s/%s.pdf", fitsDir.c_str(), vh1[i]->GetName()) );

    delete c1;
  
    float y = f1_gaus->GetParameter(1);
    float y_err = f1_gaus->GetParError(1);

    int iPoint = graph->GetN();
    graph->SetPoint( iPoint, x, y );
    graph->SetPointError( iPoint, x_err, y_err );

  }

  graph->SetMarkerStyle(20);
  graph->SetMarkerSize(1.3);

  // correct vs xHodo:
  TF1* func = new TF1( Form("func_%s_vs_%s", yName.c_str(), xName.c_str()), "pol4", -9.7, 11. );
  func->SetParameter( 0, 2.8 );
  func->SetLineColor(46);
  if( yName != "tAveCorr" )
    graph->Fit( func->GetName(), "R" );

  TCanvas* c1 = new TCanvas( Form("c1_%s_vs_%s", yName.c_str(), xName.c_str()), "", 600, 600 );
  c1->cd();


  float yMin_axes = (conf.digiConf()=="6a") ? 2.629 : 3.5;
  float yMax_axes = (conf.digiConf()=="6b") ? 2.72  : 4.7;

  TH2D* h2_axes = new TH2D( Form("axes_%s_vs_%s", yName.c_str(), xName.c_str()), "", 10, -10., 15., 10, yMin_axes, yMax_axes );
  //TH2D* h2_axes = new TH2D( Form("axes_%s_vs_%s", yName.c_str(), xName.c_str()), "", 10, -10., 15., 10, func->GetParameter(0)*0.98, func->GetParameter(0)*1.01 );
  h2_axes->SetXTitle( axisName.c_str() );
  h2_axes->SetYTitle( "0.5 * ( t_{Left} + t_{Right} ) [ns]" );
  h2_axes->Draw();

  graph->Draw("P same" );
  if( yName != "tAveCorr" )
    func->Draw("same");

  BTLCommon::addLabels( c1, conf );

  c1->SaveAs( Form( "plots/%s/%s_vs_%s.pdf", conf.get_confName().c_str(), yName.c_str(), xName.c_str()) );

  delete c1;
  delete h2_axes;

  return func;

}
