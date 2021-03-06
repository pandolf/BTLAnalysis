#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TH1D.h"
#include "TProfile.h"
#include "TLine.h"
#include "TLegend.h"
#include "TString.h"
#include "TRandom3.h"

#include "../interface/BTLCommon.h"
#include "../interface/BTLConf.h"
#include "../interface/BTLCrystal.h"


bool SAVE_ALL_FITS = false;
bool centralAmpWalk = false;
bool do_hodoCorr = false;



std::pair<TH1D*,TH1D*> getMIPboundaries( BTLConf conf, TTree* tree, float fracMipLow, float fracMipHigh, const std::string& suffix, const std::string& selection, float& ampMaxLeft_minCut, float& ampMaxLeft_maxCut, float& ampMaxRight_minCut, float& ampMaxRight_maxCut );
TF1* fitLandau( BTLConf conf, TTree* tree, TH1D* histo, const std::string& varName, float fracMipLow, float fracMipHigh, const std::string& selection );
void addPointToGraph( TGraphErrors* graph, float x, TH1D* histo, float scaleFactor=1. );
void drawRadiography( BTLConf conf, TTree* tree, const std::string& varx, const std::string& vary, float ampMaxLeft_minCut, float ampMaxRight_minCut, const std::string& suffix, float line_xMin=-999., float line_xMax=-999., float line_yMin=-999, float line_yMax=-999., float line2_xMin=-999., float line2_xMax=-999., float line2_yMin=-999, float line2_yMax=-999. );
std::vector<float> getBins( int nBins, float xMin, float xMax );
int findBin( float var, std::vector<float> bins );
int findBin( float var, int nBins, float xMin, float xMax );
void drawEffGraph( BTLConf conf, TGraphAsymmErrors* gr_eff, float varMin, float varMax, const std::string& axisName, const std::string& cutText );
void findXrange( TGraph* graph, double& xMin, double& xMax );
std::vector<TF1*> getAmpWalkCorr( const BTLConf& conf, const std::vector<float> bins_ampMax, const std::vector< std::vector<TH1D*> >& vvh1_ampMax, const std::string& name );
void getAmpWalkYaxisRange( BTLConf conf, float& yMin_axes, float& yMax_axes );
void drawAmpWalkCorr( BTLConf conf, std::vector<TF1*> vf1_ampWalk, std::vector<float> bins_ampMax, std::vector<float> binsHodo, const std::string& lr, const std::string& suffix );
TGraphErrors* getHodoCorr( BTLConf conf, std::vector<float> xBins, std::vector<TH1D*> vh1, const std::string& yName, const std::string& xName );
void drawT_vs_hodo( BTLConf conf, TGraphErrors* gr_tLeft_vs_xHodo, TGraphErrors* gr_tRight_vs_xHodo, TGraphErrors* gr_tAve_vs_xHodo, const std::string& suffix );



int main( int argc, char* argv[] ) {


  std::string confName="";
  int nBinsHodo = 1;

  if( argc>2 ) {

    confName = std::string(argv[1]);
    std::string argv2(argv[2]);
    if( argv2=="Center" || argv2=="center" || argv2=="central" || argv2=="Central" ) {
      nBinsHodo = 1;
      centralAmpWalk = true;
    } else {
      nBinsHodo = atoi(argv[2]);
    }

  } else if( argc==2 ) {

    confName = std::string(argv[1]);
    nBinsHodo = 4; // default

  } else {

    std::cout << "USAGE: ./calibrateTreeLite [confName] [ampWalkBins= N or 'center']" << std::endl;
    exit(1);

  }

  BTLCommon::setStyle();

  const BTLConf conf(confName);

  std::string plotsDir(Form("plots/%s/", conf.get_confName().c_str()) );
  system( Form("mkdir -p %s", plotsDir.c_str()) );
  system( Form("mkdir -p %s/eps", plotsDir.c_str()) );
  system( Form("mkdir -p %s/png", plotsDir.c_str()) );
  system( Form("mkdir -p %s/landauFits", plotsDir.c_str()) );
  system( Form("mkdir -p %s/landauFits/png", plotsDir.c_str()) );
  system( Form("mkdir -p %s/landauFits/eps", plotsDir.c_str()) );

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

  float x_hodo;
  tree->SetBranchAddress( "x_hodo", &x_hodo );
  float y_hodo;
  tree->SetBranchAddress( "y_hodo", &y_hodo );



  // FIRST OF ALL WE NEED TO GET THE HODOSCOPE RIGHT

  // dry run of landaufit to find the values ampMaxLeft_minCut and ampMaxRight_minCut

  float fracMipLow  = 0.8; // for landau MIP peak cut
  float fracMipHigh = 3.;

  float ampMaxLeft_minCut_tmp, ampMaxLeft_maxCut_tmp, ampMaxRight_minCut_tmp, ampMaxRight_maxCut_tmp;
  getMIPboundaries( conf, tree, fracMipLow, fracMipHigh, "_tmp", "", ampMaxLeft_minCut_tmp, ampMaxLeft_maxCut_tmp, ampMaxRight_minCut_tmp, ampMaxRight_maxCut_tmp );


  // raw radiography:
  drawRadiography( conf, tree, "x_hodo", "y_hodo", ampMaxLeft_minCut_tmp, ampMaxRight_minCut_tmp, "" );



  //float crys.xLowFiducial()  = crys.xLow()  + 5.;
  //float crys.xHighFiducial() = crys.xHigh() - 5.;
  //float crys.yLowFiducial()  = crys.yLow()  + 1.;
  //float crys.yHighFiducial() = crys.yHigh() - 1.;


  BTLCrystal crys = conf.crystal();

  std::string x_corr_text( Form( "x_hodo*cos(%f)+y_hodo*sin(%f)", crys.angle(), crys.angle()) );
  std::string y_corr_text( Form("-x_hodo*sin(%f)+y_hodo*cos(%f)", crys.angle(), crys.angle()) );

  // draw rotated radiographies to check that everything is OK
  drawRadiography( conf, tree, x_corr_text, y_corr_text, ampMaxLeft_minCut_tmp, ampMaxRight_minCut_tmp, "_corr" );
  drawRadiography( conf, tree, x_corr_text, y_corr_text, ampMaxLeft_minCut_tmp, ampMaxRight_minCut_tmp, "_corr_withBoxes"   , crys.xLow()   , crys.xHigh()   , crys.yLow()   , crys.yHigh()   , crys.xLowFiducial()   , crys.xHighFiducial()   , crys.yLowFiducial()   , crys.yHighFiducial() );



  // NOW THAT THE ROTATING IS DONE, WE CAN PROCEED WITH THE REAL DEAL: MIP  PEAK SELECTION + AMP WALK

  float ampMaxLeft_minCut, ampMaxLeft_maxCut, ampMaxRight_minCut, ampMaxRight_maxCut;
  std::string hodoSelection( Form("%s > %f && %s < %f && %s > %f && %s < %f", x_corr_text.c_str(), crys.xLow(), x_corr_text.c_str(), crys.xHigh(), y_corr_text.c_str(), crys.yLow(), y_corr_text.c_str(), crys.yHigh()) );
  std::pair<TH1D*,TH1D*> mipPeaks = getMIPboundaries( conf, tree, fracMipLow, fracMipHigh, "", hodoSelection, ampMaxLeft_minCut, ampMaxLeft_maxCut, ampMaxRight_minCut, ampMaxRight_maxCut );

  float ampMaxLeft_maxBins  = ampMaxLeft_maxCut ;
  float ampMaxLeft_minBins  = ampMaxLeft_minCut ;
  float ampMaxRight_maxBins = ampMaxRight_maxCut;
  float ampMaxRight_minBins = ampMaxRight_minCut;

  // fit MIP peak as a function of hodoscope x:
  float xEdgeLow  = crys.xLow()   - 5.;
  float xEdgeHigh = crys.xHigh()  + 5.;
  int nBinsHodoScan = 30;
  float hodoBinWidth = (xEdgeHigh-xEdgeLow)/((float)nBinsHodoScan);
  float xCenter = 0.5*(crys.xLow()+crys.xHigh());

  TGraphErrors* gr_ampLeft_vs_x  = new TGraphErrors(0);
  TGraphErrors* gr_ampRight_vs_x = new TGraphErrors(0);

  float scaleFactorLeft = 1.;
  float yMaxPlot = 1.;

  for( unsigned i=0; i<nBinsHodoScan; ++i ) {

    float xMin_i = xEdgeLow + (float)i*hodoBinWidth;
    float x_i = xMin_i+0.5*hodoBinWidth;
    std::string hodoSelection_i( Form("%s > %f && %s < %f && %s > %f && %s < %f", x_corr_text.c_str(), xMin_i, x_corr_text.c_str(), xMin_i+hodoBinWidth, y_corr_text.c_str(), crys.yLow()+1., y_corr_text.c_str(), crys.yHigh()-1.) );
    
    float dummyCut;
    std::pair<TH1D*,TH1D*> mipPeaks_i = getMIPboundaries( conf, tree, fracMipLow, fracMipHigh, Form("_hodoScan_%d",i), hodoSelection_i, dummyCut, dummyCut, dummyCut, dummyCut );

    if( i>0 ) {

      float x_iPrev = x_i - hodoBinWidth;
      float xMin_iPrev = xMin_i - hodoBinWidth;

      if( xCenter>=x_iPrev && xCenter<x_i ) {

        std::string hodoSelection_iPrev( Form("%s > %f && %s < %f && %s > %f && %s < %f", x_corr_text.c_str(), xMin_iPrev, x_corr_text.c_str(), xMin_iPrev+hodoBinWidth, y_corr_text.c_str(), crys.yLow()+1., y_corr_text.c_str(), crys.yHigh()-1.) );
        std::pair<TH1D*,TH1D*> mipPeaks_iPrev = getMIPboundaries( conf, tree, fracMipLow, fracMipHigh, Form("_hodoScan_%d",i-1), hodoSelection_iPrev, dummyCut, dummyCut, dummyCut, dummyCut );

        float yLeft_i      = mipPeaks_i.first     ->GetFunction( Form("landau_%s", mipPeaks_i    .first ->GetName()) )->GetParameter(1);
        float yRight_i     = mipPeaks_i.second    ->GetFunction( Form("landau_%s", mipPeaks_i    .second->GetName()) )->GetParameter(1);
        float yLeft_iPrev  = mipPeaks_iPrev.first ->GetFunction( Form("landau_%s", mipPeaks_iPrev.first ->GetName()) )->GetParameter(1);
        float yRight_iPrev = mipPeaks_iPrev.second->GetFunction( Form("landau_%s", mipPeaks_iPrev.second->GetName()) )->GetParameter(1);

        float yLeft_atCenter  = 0.5*(yLeft_i  + yLeft_iPrev  );
        float yRight_atCenter = 0.5*(yRight_i + yRight_iPrev );
        scaleFactorLeft = yRight_atCenter/yLeft_atCenter;
        yMaxPlot = yRight_atCenter*4.;

      } // if center
    } // if i>0
    
    //float scaleFactorLeft = (conf.sensorConf()==4 && conf.digiChSet()=="a" ) ? 4.4 : 1.;
    addPointToGraph( gr_ampLeft_vs_x , x_i, mipPeaks_i.first );//, scaleFactorLeft );
    addPointToGraph( gr_ampRight_vs_x, x_i, mipPeaks_i.second);//, 1.              );

  }

  std::cout << "-> Found scale factor for left: " << scaleFactorLeft << std::endl;


  // first scale left:
  for( unsigned iPoint=0; iPoint<nBinsHodoScan; ++iPoint) {
    double x, yLeft;
    gr_ampLeft_vs_x->GetPoint( iPoint, x, yLeft );
    gr_ampLeft_vs_x->SetPoint( iPoint, x, yLeft*scaleFactorLeft );
  }

  TGraphErrors* gr_ampSum_vs_x  = new TGraphErrors(0);
  for( unsigned iPoint=0; iPoint<nBinsHodoScan; ++iPoint) {
    double x, yLeft, yRight;
    gr_ampLeft_vs_x ->GetPoint( iPoint, x, yLeft       );
    gr_ampRight_vs_x->GetPoint( iPoint, x, yRight      );
    gr_ampSum_vs_x  ->SetPoint( iPoint, x, (yLeft+yRight));
  }


  TCanvas* c1_hodo = new TCanvas( "c1_hodo", "", 600, 600 );
  c1_hodo->cd();

  TH2D* h2_axes = new TH2D( "axes", "", 10, xEdgeLow, xEdgeHigh, 10, 0., yMaxPlot );
  h2_axes->SetXTitle( "Hodoscope X [mm]" );
  h2_axes->SetYTitle( "MIP Peak Position [a.u.]" );
  h2_axes->Draw();

  gr_ampLeft_vs_x->SetMarkerStyle(20);
  gr_ampLeft_vs_x->SetMarkerSize(1.3);
  gr_ampLeft_vs_x->SetMarkerColor(38);
  gr_ampLeft_vs_x->SetLineColor(38);

  gr_ampRight_vs_x->SetMarkerStyle(20);
  gr_ampRight_vs_x->SetMarkerSize(1.3);
  gr_ampRight_vs_x->SetMarkerColor(46);
  gr_ampRight_vs_x->SetLineColor(46);

  gr_ampSum_vs_x->SetMarkerStyle(20);
  gr_ampSum_vs_x->SetMarkerSize(1.3);
  gr_ampSum_vs_x->SetMarkerColor(kGray+3);
  gr_ampSum_vs_x->SetLineColor(kGray+3);

  TLegend* legend = new TLegend( 0.2, 0.75, 0.5, 0.9 );
  legend->SetFillColor(0);
  legend->SetTextSize(0.035);
  legend->AddEntry( gr_ampLeft_vs_x, "Left SiPM", "P" );
  legend->AddEntry( gr_ampRight_vs_x, "Right SiPM", "P" );
  legend->AddEntry( gr_ampSum_vs_x, "Left+Right", "P" );
  legend->Draw("same");

  TPaveText* labelConf = conf.get_labelConf();
  labelConf->Draw("same");

  gr_ampRight_vs_x->Draw("P same");
  gr_ampLeft_vs_x->Draw("P same");
  gr_ampSum_vs_x->Draw("P same");

  BTLCommon::addLabels( c1_hodo, conf );

  c1_hodo->SaveAs( Form( "plots/%s/mip_vs_x.pdf", conf.get_confName().c_str() ) );




  // plot also ampMax after hodoCutFiducial
  float ampMaxLeft_minCut_fid, ampMaxLeft_maxCut_fid, ampMaxRight_minCut_fid, ampMaxRight_maxCut_fid;
  std::string hodoSelectionFiducial( Form("%s > %f && %s < %f && %s > %f && %s < %f", x_corr_text.c_str(), crys.xLowFiducial(), x_corr_text.c_str(), crys.xHighFiducial(), y_corr_text.c_str(), crys.yLowFiducial(), y_corr_text.c_str(), crys.yHighFiducial()) );
  getMIPboundaries( conf, tree, fracMipLow, fracMipHigh, "_fiducial", hodoSelectionFiducial, ampMaxLeft_minCut_fid, ampMaxLeft_maxCut_fid, ampMaxRight_minCut_fid, ampMaxRight_maxCut_fid );



  
  int nBins_ampMax = 100;
  std::vector<float> bins_ampMaxLeft  = getBins( nBins_ampMax, ampMaxLeft_minBins , ampMaxLeft_maxBins  );
  std::vector<float> bins_ampMaxRight = getBins( nBins_ampMax, ampMaxRight_minBins, ampMaxRight_maxBins );


  int nBinsT = 100;
  //float tMin = (conf.digiConf()=="6a") ? 2. : 3.;
  //float tMax = (conf.digiConf()=="6a") ? 4. : 5.;
  float tMin;
  float tMax;
  if( conf.digiChSet()=="a" ) {
    tMin = 2.;
    tMax = 4.;
  } else { //if( conf.digiChSet()=="b" ) {
    tMin = 3.;
    tMax = 5.;
  }

  // integrated distributions (need them later)
  TH1D* h1_tLeft_int  = new TH1D( "tLeft_int" , "", nBinsT, tMin, tMax );
  TH1D* h1_tRight_int = new TH1D( "tRight_int", "", nBinsT, tMin, tMax );


  std::vector<float> binsHodo  = getBins( nBinsHodo, crys.xLow() , crys.xHigh()  );

  if( centralAmpWalk ) {
    float barCenter = 0.5*( crys.xLow() + crys.xHigh() );
    binsHodo  = getBins( 1, barCenter-5., barCenter+5. );
  }

  //std::vector< TH1D* > vh1_tLeft, vh1_tRight;
  //std::vector< TH1D* > vh1_ampMaxLeft, vh1_ampMaxRight;

  std::vector< std::vector< TH1D* > > vvh1_tLeft, vvh1_tRight;


  for( int ihodo=0; ihodo<nBinsHodo; ++ihodo ) {

    std::vector< TH1D* > vh1_tLeft_iHodo, vh1_tRight_iHodo;

    for( int i=0; i<nBins_ampMax; ++i ) {

      TH1D* h1_tLeft = new TH1D( Form("tLeft_hodo%d_bin%d", ihodo, i), "", nBinsT, tMin, tMax );
      h1_tLeft->SetXTitle( "t_{Left} [ns]" );
      vh1_tLeft_iHodo.push_back( h1_tLeft );

      //TH1D* h1_ampMaxLeft = new TH1D( Form("ampMaxLeft_hodo%d_bin%d", ihodo, i), "", 50, bins_ampMaxLeft[i], bins_ampMaxLeft[i+1] );
      //h1_ampMaxLeft->SetXTitle( "Max Amplitude Left [a.u.]" );
      //vh1_ampMaxLeft_iHodo.push_back( h1_ampMaxLeft );

      TH1D* h1_tRight = new TH1D( Form("tRight_hodo%d_bin%d", ihodo, i), "", nBinsT, tMin, tMax );
      h1_tRight->SetXTitle( "t_{Right} [ns]" );
      vh1_tRight_iHodo.push_back( h1_tRight );

      //TH1D* h1_ampMaxRight = new TH1D( Form("ampMaxRight_hodo%d_bin%d", ihodo, i), "", 50, bins_ampMaxRight[i], bins_ampMaxRight[i+1] );
      //h1_ampMaxRight->SetXTitle( "Max Amplitude Right [a.u.]" );
      //vh1_ampMaxRight_iHodo.push_back( h1_ampMaxRight );

    } // for bins_ampMax

    vvh1_tLeft .push_back( vh1_tLeft_iHodo  );
    vvh1_tRight.push_back( vh1_tRight_iHodo );

  } // for ihodo



  int nEntries = tree->GetEntries();

  
  // FIRST LOOP (compute ampwalk)

  for( int iEntry = 0; iEntry<nEntries; ++iEntry ) {

    if( iEntry % 10000 == 0 ) std::cout << " Entry: " << iEntry << " / " << nEntries << std::endl;

    tree->GetEntry( iEntry );

    float x_hodo_corr_tmp =  x_hodo*cos(crys.angle()) + y_hodo*sin(crys.angle());  
    float y_hodo_corr_tmp = -x_hodo*sin(crys.angle()) + y_hodo*cos(crys.angle());  

    //// require hodo fiducial region
    //if( x_hodo_corr<crys.xLowFiducial() || x_hodo_corr>crys.xHighFiducial() || y_hodo_corr<crys.yLowFiducial() || y_hodo_corr>crys.yHighFiducial() ) continue;

    // require that bar is hit
    if( x_hodo_corr_tmp<crys.xLow() || x_hodo_corr_tmp>crys.xHigh() || y_hodo_corr_tmp<crys.yLow() || y_hodo_corr_tmp>crys.yHigh() ) continue;

    int thisBinHodo = findBin( x_hodo_corr_tmp, binsHodo );

    if( thisBinHodo<0 ) continue;


    if( ampMaxLeft>=ampMaxLeft_minCut && ampMaxLeft<=ampMaxLeft_maxCut ) {  // ampMaxLeft is good

      int thisBinLeft = findBin( ampMaxLeft, bins_ampMaxLeft );

      if( thisBinLeft>=0 ) {
        h1_tLeft_int->Fill( tLeft );
        vvh1_tLeft[thisBinHodo][thisBinLeft]->Fill( tLeft );
      }

    } // if ampMaxLeft is good

    if( ampMaxRight>=ampMaxRight_minCut && ampMaxRight<=ampMaxRight_maxCut ) {  // ampMaxRight is good

      int thisBinRight = findBin( ampMaxRight, bins_ampMaxRight );

      if( thisBinRight>=0 ) {
        h1_tRight_int->Fill( tRight );
        vvh1_tRight[thisBinHodo][thisBinRight]->Fill( tRight );
      }

    } // if ampMaxRight is good

  } // for Entries



  std::vector< TF1* > vf1_ampWalkLeft  = getAmpWalkCorr( conf, bins_ampMaxLeft , vvh1_tLeft , "Left"  );
  std::vector< TF1* > vf1_ampWalkRight = getAmpWalkCorr( conf, bins_ampMaxRight, vvh1_tRight, "Right" );


  std::string suffix = (centralAmpWalk) ? "_awCentral" : std::string(Form("_aw%dbins",nBinsHodo));

  if( vf1_ampWalkLeft.size()>0 ) {
    drawAmpWalkCorr( conf, vf1_ampWalkLeft , bins_ampMaxLeft , binsHodo, "Left" , suffix );
    drawAmpWalkCorr( conf, vf1_ampWalkRight, bins_ampMaxRight, binsHodo, "Right", suffix );
  }

    

  float target_ampWalkLeft  = h1_tLeft_int ->GetMean();
  float target_ampWalkRight = h1_tRight_int->GetMean();
  //float target_ampWalkLeft  = f1_ampWalkLeft ->Eval( bins_ampMaxLeft [0] );
  //float target_ampWalkRight = f1_ampWalkRight->Eval( bins_ampMaxRight[0] );


  // prepare new file
  TFile* outfile = TFile::Open( Form("treesLite/%s%s.root", confName.c_str(), suffix.c_str()), "RECREATE" );
  TTree* newtree = tree->CloneTree(0);

  float tLeft_corr;
  newtree->Branch( "tLeft_corr", &tLeft_corr );
  float tRight_corr;
  newtree->Branch( "tRight_corr", &tRight_corr );

  // smeared stuff:
  float tLeft_corr_smear3;
  newtree->Branch( "tLeft_corr_smear3", &tLeft_corr_smear3 );
  float tRight_corr_smear3;
  newtree->Branch( "tRight_corr_smear3", &tRight_corr_smear3 );

  float tLeft_corr_smear5;
  newtree->Branch( "tLeft_corr_smear5", &tLeft_corr_smear5 );
  float tRight_corr_smear5;
  newtree->Branch( "tRight_corr_smear5", &tRight_corr_smear5 );

  float tLeft_corr_smear10;
  newtree->Branch( "tLeft_corr_smear10", &tLeft_corr_smear10 );
  float tRight_corr_smear10;
  newtree->Branch( "tRight_corr_smear10", &tRight_corr_smear10 );

  float tLeft_corr_smear15;
  newtree->Branch( "tLeft_corr_smear15", &tLeft_corr_smear15 );
  float tRight_corr_smear15;
  newtree->Branch( "tRight_corr_smear15", &tRight_corr_smear15 );

  float tLeft_corr_smear20;
  newtree->Branch( "tLeft_corr_smear20", &tLeft_corr_smear20 );
  float tRight_corr_smear20;
  newtree->Branch( "tRight_corr_smear20", &tRight_corr_smear20 );


  float x_hodo_corr;
  newtree->Branch( "x_hodo_corr", &x_hodo_corr );
  float y_hodo_corr;
  newtree->Branch( "y_hodo_corr", &y_hodo_corr );

  bool hodoOnBar;
  newtree->Branch( "hodoOnBar", &hodoOnBar );
  bool hodoFiducial;
  newtree->Branch( "hodoFiducial", &hodoFiducial );

  //float tAveCorr = -1.;
  //newtree->Branch( "tAveCorr", &tAveCorr );


  std::vector< std::vector< TH1D* > > vvh1_tLeft_corr, vvh1_tRight_corr;

  for( int ihodo=0; ihodo<nBinsHodo; ++ihodo ) {

    std::vector< TH1D* > vh1_tLeft_corr, vh1_tRight_corr;

    for( int i=0; i<nBins_ampMax; ++i ) {

      TH1D* h1_tLeft_corr  = new TH1D( Form("tLeft_corr_hodo%d_bin%d" , ihodo, i), "", 100, tMin, tMax );
      vh1_tLeft_corr.push_back( h1_tLeft_corr );

      TH1D* h1_tRight_corr = new TH1D( Form("tRight_corr_hodo%d_bin%d", ihodo, i), "", 100, tMin, tMax );
      vh1_tRight_corr.push_back( h1_tRight_corr );

    } // for bins_ampMax

    vvh1_tLeft_corr .push_back( vh1_tLeft_corr  );
    vvh1_tRight_corr.push_back( vh1_tRight_corr );

  } // for ihodo


  float xMin_hodo = crys.xLow()  -5.;
  float xMax_hodo = crys.xHigh() +5.;
  float yMin_hodo = crys.yLow() -0.5;
  float yMax_hodo = crys.yHigh()+0.5;
  float binWidthX_hodo = 1.;
  int nBinsX_hodo = (int)((xMax_hodo-xMin_hodo)/binWidthX_hodo);
  float binWidthY_hodo = 0.25;
  int nBinsY_hodo = (int)((yMax_hodo-yMin_hodo)/binWidthY_hodo);

  
  std::vector<float> binsX_hodo;
  std::vector<TH1D*> vh1_tLeft_vs_xHodo;
  std::vector<TH1D*> vh1_tRight_vs_xHodo;
  std::vector<TH1D*> vh1_tAve_vs_xHodo;

  for( int i=0; i<nBinsX_hodo; ++i ) {

    binsX_hodo.push_back( xMin_hodo + (float)i*binWidthX_hodo );

    TH1D* h1_tLeft_vs_xHodo  = new TH1D( Form("tLeft_vs_xHodo_%d", i) , "", 100, tMin, tMax );
    vh1_tLeft_vs_xHodo .push_back( h1_tLeft_vs_xHodo  );

    TH1D* h1_tRight_vs_xHodo = new TH1D( Form("tRight_vs_xHodo_%d", i), "", 100, tMin, tMax );
    vh1_tRight_vs_xHodo.push_back( h1_tRight_vs_xHodo );

    TH1D* h1_tAve_vs_xHodo   = new TH1D( Form("tAve_vs_xHodo_%d"  , i), "", 100, tMin, tMax );
    vh1_tAve_vs_xHodo  .push_back( h1_tAve_vs_xHodo   );

  } // for bins xHodo

  binsX_hodo.push_back( xMin_hodo + (float)nBinsX_hodo*binWidthX_hodo );


  TH1D* h1_effAmpMax_vs_X_num   = new TH1D( "effAmpMax_vs_X_num"  , "", nBinsX_hodo, xMin_hodo, xMax_hodo );
  TH1D* h1_effAmpMax_vs_X_denom = new TH1D( "effAmpMax_vs_X_denom", "", nBinsX_hodo, xMin_hodo, xMax_hodo );

  TH1D* h1_effAmpMax_vs_Y_num   = new TH1D( "effAmpMax_vs_Y_num"  , "", nBinsY_hodo, yMin_hodo, yMax_hodo );
  TH1D* h1_effAmpMax_vs_Y_denom = new TH1D( "effAmpMax_vs_Y_denom", "", nBinsY_hodo, yMin_hodo, yMax_hodo );

  
  // random number generator for amp smearing studies:
  TRandom3* rand = new TRandom3(0);


  // SECOND LOOP (apply ampwalk, rotate hodo xy)

  for( int iEntry = 0; iEntry<nEntries; ++iEntry ) {

    tree->GetEntry( iEntry );


    x_hodo_corr =  x_hodo*cos(crys.angle()) + y_hodo*sin(crys.angle());  
    y_hodo_corr = -x_hodo*sin(crys.angle()) + y_hodo*cos(crys.angle());  

    bool hodoOnBarX = ( x_hodo_corr>=crys.xLow() && x_hodo_corr<=crys.xHigh() );
    bool hodoOnBarY = ( y_hodo_corr>=crys.yLow() && y_hodo_corr<=crys.yHigh() );

    bool hodoFiducialX = ( x_hodo_corr>=crys.xLowFiducial() && x_hodo_corr<=crys.xHighFiducial() );
    bool hodoFiducialY = ( y_hodo_corr>=crys.yLowFiducial() && y_hodo_corr<=crys.yHighFiducial() );


    bool ampMaxLeftGood  = ( ampMaxLeft >=ampMaxLeft_minCut  && ampMaxLeft <=ampMaxLeft_maxCut  );
    bool ampMaxRightGood = ( ampMaxRight>=ampMaxRight_minCut && ampMaxRight<=ampMaxRight_maxCut );

    bool ampMaxGood = ampMaxLeftGood && ampMaxRightGood;

    if( hodoFiducialX ) {
      h1_effAmpMax_vs_Y_denom->Fill( y_hodo_corr );
      if( ampMaxGood ) h1_effAmpMax_vs_Y_num->Fill( y_hodo_corr );
    }

    if( hodoFiducialY ) {
      h1_effAmpMax_vs_X_denom->Fill( x_hodo_corr );
      if( ampMaxGood ) h1_effAmpMax_vs_X_num->Fill( x_hodo_corr );
    }


    hodoOnBar    = hodoOnBarX    && hodoOnBarY;
    hodoFiducial = hodoFiducialX && hodoFiducialY;

    //if( !hodoFiducial  ) continue;
    if( !hodoOnBar  ) continue;
    if( !ampMaxGood ) continue;

    int hodoBin = (centralAmpWalk) ? 0 : findBin( x_hodo_corr, binsHodo );

    tLeft_corr  = tLeft  * ( target_ampWalkLeft  / vf1_ampWalkLeft [hodoBin]->Eval( ampMaxLeft  ) );
    tRight_corr = tRight * ( target_ampWalkRight / vf1_ampWalkRight[hodoBin]->Eval( ampMaxRight ) );

    // smeared stuff:

    tLeft_corr_smear3  = tLeft   * ( target_ampWalkLeft  / vf1_ampWalkLeft [hodoBin]->Eval( ampMaxLeft *rand->Gaus( 1., 0.03) ) );
    tRight_corr_smear3 = tRight  * ( target_ampWalkRight / vf1_ampWalkRight[hodoBin]->Eval( ampMaxRight*rand->Gaus( 1., 0.03) ) );

    tLeft_corr_smear5  = tLeft   * ( target_ampWalkLeft  / vf1_ampWalkLeft [hodoBin]->Eval( ampMaxLeft *rand->Gaus( 1., 0.05) ) );
    tRight_corr_smear5 = tRight  * ( target_ampWalkRight / vf1_ampWalkRight[hodoBin]->Eval( ampMaxRight*rand->Gaus( 1., 0.05) ) );

    tLeft_corr_smear10  = tLeft   * ( target_ampWalkLeft  / vf1_ampWalkLeft [hodoBin]->Eval( ampMaxLeft *rand->Gaus( 1., 0.10) ) );
    tRight_corr_smear10 = tRight  * ( target_ampWalkRight / vf1_ampWalkRight[hodoBin]->Eval( ampMaxRight*rand->Gaus( 1., 0.10) ) );

    tLeft_corr_smear15  = tLeft   * ( target_ampWalkLeft  / vf1_ampWalkLeft [hodoBin]->Eval( ampMaxLeft *rand->Gaus( 1., 0.15) ) );
    tRight_corr_smear15 = tRight  * ( target_ampWalkRight / vf1_ampWalkRight[hodoBin]->Eval( ampMaxRight*rand->Gaus( 1., 0.15) ) );

    tLeft_corr_smear20  = tLeft   * ( target_ampWalkLeft  / vf1_ampWalkLeft [hodoBin]->Eval( ampMaxLeft *rand->Gaus( 1., 0.20) ) );
    tRight_corr_smear20 = tRight  * ( target_ampWalkRight / vf1_ampWalkRight[hodoBin]->Eval( ampMaxRight*rand->Gaus( 1., 0.20) ) );


    int thisBinLeft  = findBin( ampMaxLeft , bins_ampMaxLeft  );
    if( thisBinLeft>=0 ) 
      vvh1_tLeft_corr [hodoBin][thisBinLeft] ->Fill( tLeft_corr  );

    int thisBinRight = findBin( ampMaxRight, bins_ampMaxRight );
    if( thisBinRight>=0 ) 
      vvh1_tRight_corr[hodoBin][thisBinRight]->Fill( tRight_corr );


    int thisBinX_hodo = findBin( x_hodo_corr, nBinsX_hodo, xMin_hodo, xMax_hodo );
    if( thisBinX_hodo>=0 ) {
      vh1_tLeft_vs_xHodo [thisBinX_hodo]->Fill( tLeft_corr  );
      vh1_tRight_vs_xHodo[thisBinX_hodo]->Fill( tRight_corr );
      vh1_tAve_vs_xHodo[thisBinX_hodo]->Fill( 0.5*(tLeft_corr+tRight_corr) );
    }

    newtree->Fill();

  } // for entries



  TGraphAsymmErrors* gr_effMaxAmp_vs_X = new TGraphAsymmErrors(h1_effAmpMax_vs_X_num->GetNbinsX());
  gr_effMaxAmp_vs_X->SetName( "effMaxAmp_vs_X" );
  gr_effMaxAmp_vs_X->Divide( h1_effAmpMax_vs_X_num, h1_effAmpMax_vs_X_denom );

  drawEffGraph( conf, gr_effMaxAmp_vs_X, crys.xLow(), crys.xHigh(), "Hodoscope X [mm]", Form( "[%.1f - %.1f]*MIP Selection", fracMipLow, fracMipHigh )  );

  TGraphAsymmErrors* gr_effMaxAmp_vs_Y = new TGraphAsymmErrors(h1_effAmpMax_vs_X_num->GetNbinsX());
  gr_effMaxAmp_vs_Y->SetName( "effMaxAmp_vs_Y" );
  gr_effMaxAmp_vs_Y->Divide( h1_effAmpMax_vs_Y_num, h1_effAmpMax_vs_Y_denom );

  drawEffGraph( conf, gr_effMaxAmp_vs_Y, crys.yLow(), crys.yHigh(), "Hodoscope Y [mm]", Form( "[%.1f - %.1f]*MIP Selection", fracMipLow, fracMipHigh )  );

  // check AW closure:
  getAmpWalkCorr( conf, bins_ampMaxLeft , vvh1_tLeft_corr , "Left_corr"  );
  getAmpWalkCorr( conf, bins_ampMaxRight, vvh1_tRight_corr, "Right_corr" );


  TTree* newtree2 = 0;
 
  if( do_hodoCorr ) {

    TGraphErrors* gr_tRight_vs_xHodo = getHodoCorr( conf, binsX_hodo, vh1_tRight_vs_xHodo, "tRight", "xHodo" );
    TGraphErrors* gr_tLeft_vs_xHodo  = getHodoCorr( conf, binsX_hodo, vh1_tLeft_vs_xHodo , "tLeft" , "xHodo" );
    TGraphErrors* gr_tAve_vs_xHodo   = getHodoCorr( conf, binsX_hodo, vh1_tAve_vs_xHodo  , "tAve"  , "xHodo" );

    drawT_vs_hodo( conf, gr_tLeft_vs_xHodo, gr_tRight_vs_xHodo, gr_tAve_vs_xHodo, "" );

    TF1* f1_tRight_vs_xHodo = gr_tRight_vs_xHodo->GetFunction( Form("f1_%s", gr_tRight_vs_xHodo->GetName()) );
    TF1* f1_tLeft_vs_xHodo  = gr_tLeft_vs_xHodo ->GetFunction( Form("f1_%s", gr_tLeft_vs_xHodo ->GetName()) );
    gr_tAve_vs_xHodo  ->GetFunction( Form("f1_%s", gr_tAve_vs_xHodo  ->GetName()) );

    newtree->SetBranchAddress( "tLeft_corr" , &tLeft_corr );
    newtree->SetBranchAddress( "tRight_corr", &tRight_corr );
    newtree->SetBranchAddress( "x_hodo", &x_hodo );

    newtree2 = newtree->CloneTree(0);
    newtree2->SetName( "treeLite" );

    float tLeft_corr2;
    newtree2->Branch( "tLeft_corr2", &tLeft_corr2 );
    float tRight_corr2;
    newtree2->Branch( "tRight_corr2", &tRight_corr2 );
    //float tAveCorr;
    //newtree2->Branch( "tAveCorr", &tAveCorr );

    int nentries2 = newtree->GetEntries();

    double xMin_fit, xMax_fit;
    f1_tRight_vs_xHodo->GetRange( xMin_fit, xMax_fit );

    std::vector<TH1D*> vh1_tRightCorr_vs_xHodo;
    std::vector<TH1D*> vh1_tLeftCorr_vs_xHodo;
    for( int i=0; i<nBinsX_hodo; ++i ) {
      TH1D* h1_tLeftCorr_vs_xHodo = new TH1D( Form("tLeftCorr_vs_xHodo_%d", i), "", 100, tMin, tMax );
      vh1_tLeftCorr_vs_xHodo.push_back( h1_tLeftCorr_vs_xHodo );
      TH1D* h1_tRightCorr_vs_xHodo = new TH1D( Form("tRightCorr_vs_xHodo_%d", i), "", 100, tMin, tMax );
      vh1_tRightCorr_vs_xHodo.push_back( h1_tRightCorr_vs_xHodo );
    } 

    std::cout << "-> Second round to correct vs position: " << std::endl;

    for( int iEntry=0; iEntry<nentries2; ++iEntry ) {

      newtree->GetEntry(iEntry);

      tLeft_corr2  = tLeft_corr *f1_tLeft_vs_xHodo ->Eval(5.)/f1_tLeft_vs_xHodo ->Eval(x_hodo);
      tRight_corr2 = tRight_corr*f1_tRight_vs_xHodo->Eval(5.)/f1_tRight_vs_xHodo->Eval(x_hodo);

      //float tAve = 0.5*( tLeft_corr + tRight_corr );
      //if( x_hodo >= xMin_fit && x_hodo < xMax_fit )
      //  tAveCorr  = tAve  * ( f1_tAve_vs_xHodo->Eval( 0.5*(xMin_fit+xMax_fit) )  / f1_tAve_vs_xHodo ->Eval( x_hodo ) );
      //else
      //  tAveCorr  = tAve;

      int thisBinX_hodo = findBin( x_hodo, nBinsX_hodo, xMin_hodo, xMax_hodo );
      if( thisBinX_hodo>=0 ) {
        vh1_tLeftCorr_vs_xHodo [thisBinX_hodo]->Fill( tLeft_corr2  );
        vh1_tRightCorr_vs_xHodo[thisBinX_hodo]->Fill( tRight_corr2 );
        //vh1_tAveCorr_vs_xHodo[thisBinX_hodo]->Fill( tAveCorr );
      }

      newtree2->Fill();

    } // for entries 2

    TGraphErrors* gr_tRightCorr_vs_xHodo = getHodoCorr( conf, binsX_hodo, vh1_tRightCorr_vs_xHodo, "tRight_corr2", "xHodo" );
    TGraphErrors* gr_tLeftCorr_vs_xHodo  = getHodoCorr( conf, binsX_hodo, vh1_tLeftCorr_vs_xHodo , "tLeft_corr2" , "xHodo" );
    //getHodoCorr( conf, xBins_xHodo, vh1_tAveCorr_vs_xHodo, "tAveCorr", "xHodo" );

    drawT_vs_hodo( conf, gr_tLeftCorr_vs_xHodo, gr_tRightCorr_vs_xHodo, gr_tAve_vs_xHodo, "_corr" );

  } // if doHodo

 
  outfile->cd();

  if( newtree2 != 0 )
    newtree2->Write();
  else
    newtree->Write();
  
  h1_tLeft_int ->Write();
  h1_tRight_int->Write();

  gr_effMaxAmp_vs_X->Write();
  gr_effMaxAmp_vs_Y->Write();

  mipPeaks.first->Write();
  mipPeaks.second->Write();

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



std::pair<TH1D*,TH1D*> getMIPboundaries( BTLConf conf, TTree* tree, float fracMipLow, float fracMipHigh, const std::string& suffix, const std::string& selection, float& ampMaxLeft_minCut, float& ampMaxLeft_maxCut, float& ampMaxRight_minCut, float& ampMaxRight_maxCut ) {

  float scaleFactor = 1.;
  if( conf.sensorConf()==4 ) scaleFactor = 72. - conf.vBias();
  if( scaleFactor==0. ) scaleFactor = 1.;

  float xMaxRight = 0.8;
  float xMaxLeft = ( conf.digiChSet()=="a" && conf.sensorConf()==4 ) ? 0.2 : xMaxRight;

  if( conf.sensorConf()==5 && conf.digiChSet()=="a" && conf.vBias()==28 ) {
    xMaxLeft  /= 40.;
    xMaxRight /= 10.;
  }

  TH1D* h1_ampMaxLeft = new TH1D( Form("ampMaxLeft%s", suffix.c_str()), "", 100, 0., xMaxLeft/scaleFactor );
  h1_ampMaxLeft ->SetXTitle( "Max Amplitude Left [a.u.]" );
  TH1D* h1_ampMaxRight = new TH1D( Form("ampMaxRight%s", suffix.c_str()), "", 100, 0., xMaxRight/scaleFactor  );
  h1_ampMaxRight->SetXTitle( "Max Amplitude Right [a.u.]" );

  TF1* fitLandauL = fitLandau( conf, tree, h1_ampMaxLeft , "ampMaxLeft" , fracMipLow, fracMipHigh, selection.c_str() );
  TF1* fitLandauR = fitLandau( conf, tree, h1_ampMaxRight, "ampMaxRight", fracMipLow, fracMipHigh, selection.c_str() );

  float xModeLeft  = h1_ampMaxLeft ->GetXaxis()->GetBinCenter(h1_ampMaxLeft ->GetMaximumBin());
  float xModeRight = h1_ampMaxRight->GetXaxis()->GetBinCenter(h1_ampMaxRight->GetMaximumBin());

  float mipRight = ( fitLandauR->GetParameter(1)<0.0005 ) ? xModeRight : fitLandauR->GetParameter(1);
  float mipLeft  = ( fitLandauL->GetParameter(1)<0.0005 ) ? xModeLeft  : fitLandauL->GetParameter(1);

  ampMaxLeft_maxCut   = mipLeft*fracMipHigh;
  ampMaxLeft_minCut   = mipLeft*fracMipLow;

  ampMaxRight_maxCut  = mipRight*fracMipHigh;
  ampMaxRight_minCut  = mipRight*fracMipLow;

  std::pair<TH1D*,TH1D*> ph1_mipPeaks;
  ph1_mipPeaks.first  = h1_ampMaxLeft;
  ph1_mipPeaks.second = h1_ampMaxRight;

  return ph1_mipPeaks;

}



TF1* fitLandau( BTLConf conf, TTree* tree, TH1D* histo, const std::string& varName, float fracMipLow, float fracMipHigh, const std::string& selection ) {

  std::string outdir( Form("plots/%s/landauFits", conf.get_confName().c_str()) );

  tree->Project( histo->GetName(), varName.c_str(), selection.c_str() );

  float xMode = histo->GetBinCenter(histo->GetMaximumBin());
  float xMin_fit = xMode*fracMipLow;
  float xMax_fit = xMode*fracMipHigh;

  TF1* f1_landau = new TF1( Form("landau_%s", histo->GetName()), "landau", xMin_fit, xMax_fit );
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

  float yMax = 1.2*histo->GetMaximum();
  TH2D* h2_axes = new TH2D( Form("axes_%s", histo->GetName()), "", 10, histo->GetXaxis()->GetXmin(), histo->GetXaxis()->GetXmax(), 10, 0., yMax );
  h2_axes->SetXTitle( histo->GetXaxis()->GetTitle() );
  h2_axes->SetYTitle( "Entries" );
  h2_axes->Draw();

  float xLineMin = (f1_landau->GetParameter(1)<0.0005) ? fracMipLow *xMode : fracMipLow *f1_landau->GetParameter(1);
  float xLineMax = (f1_landau->GetParameter(1)<0.0005) ? fracMipHigh*xMode : fracMipHigh*f1_landau->GetParameter(1);

  TLine* line_cutMin = new TLine( xLineMin, 0., xLineMin, yMax );
  TLine* line_cutMax = new TLine( xLineMax, 0., xLineMax, yMax );
 
  line_cutMin->SetLineStyle(2);
  line_cutMax->SetLineStyle(2);

  line_cutMin->SetLineColor(46);
  line_cutMax->SetLineColor(46);

  line_cutMin->Draw("same");
  line_cutMax->Draw("same");

  histo->Draw("same");

  BTLCommon::addLabels( c1, conf );

  c1->SaveAs( Form("%s/%s.pdf"    , outdir.c_str(), histo->GetName()) );
  //c1->SaveAs( Form("%s/eps/%s.eps", outdir.c_str(), histo->GetName()) );
  //c1->SaveAs( Form("%s/png/%s.png", outdir.c_str(), histo->GetName()) );

  delete c1;
  
  return f1_landau;

}


void addPointToGraph( TGraphErrors* graph, float x, TH1D* histo, float scaleFactor ) {

  if( histo->GetEntries()>100 ) {

    int iPoint = graph->GetN();

    TF1* f1 = histo->GetFunction( Form("landau_%s", histo->GetName()) );

    float y     = f1->GetParameter(1)*scaleFactor;
    float y_err = f1->GetParError(1)*scaleFactor;

    if( y_err < y ) {

      graph->SetPoint( iPoint, x, y );
      graph->SetPointError( iPoint, 0., y_err );

    }

  } // if entries

}


void drawRadiography( BTLConf conf, TTree* tree, const std::string& varx, const std::string& vary, float ampMaxLeft_minCut, float ampMaxRight_minCut, const std::string& suffix, float line_xMin, float line_xMax, float line_yMin, float line_yMax, float line2_xMin, float line2_xMax, float line2_yMin, float line2_yMax ) {

  TH2D* h2_radio = new TH2D( Form("radio%s", suffix.c_str()), "", 280, -19.999, 49.999, 72, -3.999, 9.999 );
  h2_radio->SetXTitle("Hodoscope X [mm]");
  h2_radio->SetYTitle("Hodoscope Y [mm]");

  std::string additionalSel = "";
  if( conf.sensorConf()==4 && conf.ninoThr()==100 && conf.vBias()==69 ) 
    additionalSel = " && x_hodo!=41.125 && y_hodo!=-1.125"; // hot cell!
  
  tree->Project( h2_radio->GetName(), Form("%s:%s", vary.c_str(), varx.c_str()), Form("ampMaxLeft>%f && ampMaxRight>%f%s", ampMaxLeft_minCut, ampMaxRight_minCut, additionalSel.c_str()) );

  TCanvas* c1_radio = new TCanvas( Form("c1_%s", h2_radio->GetName()), "", 600, 600 );
  c1_radio->cd();

  h2_radio->GetZaxis()->SetRangeUser( 0.05*h2_radio->GetMaximum(), h2_radio->GetMaximum() );
  h2_radio->Draw("col");

  if( line_xMin>-100. && line_xMax>-100. && line_yMin>-100. && line_yMax>=-100. ) {

    TLine* lineLeft   = new TLine( line_xMin, line_yMin, line_xMin, line_yMax );
    TLine* lineRight  = new TLine( line_xMax, line_yMin, line_xMax, line_yMax );
    TLine* lineTop    = new TLine( line_xMin, line_yMax, line_xMax, line_yMax );
    TLine* lineBottom = new TLine( line_xMin, line_yMin, line_xMax, line_yMin );

    lineLeft->SetLineColor(kRed);
    lineLeft->SetLineWidth(3);
    //lineLeft->SetLineStyle(2);
    lineLeft->Draw("same");

    lineRight->SetLineColor(kRed);
    lineRight->SetLineWidth(3);
    //lineRight->SetLineStyle(2);
    lineRight->Draw("same");

    lineTop->SetLineColor(kRed);
    lineTop->SetLineWidth(3);
    //lineTop->SetLineStyle(2);
    lineTop->Draw("same");

    lineBottom->SetLineColor(kRed);
    lineBottom->SetLineWidth(3);
    //lineBottom->SetLineStyle(2);
    lineBottom->Draw("same");

  }

  if( line2_xMin>-100. && line2_xMax>-100. && line2_yMin>-100. && line2_yMax>=-100. ) {

    TLine* line2Left   = new TLine( line2_xMin, line2_yMin, line2_xMin, line2_yMax );
    TLine* line2Right  = new TLine( line2_xMax, line2_yMin, line2_xMax, line2_yMax );
    TLine* line2Top    = new TLine( line2_xMin, line2_yMax, line2_xMax, line2_yMax );
    TLine* line2Bottom = new TLine( line2_xMin, line2_yMin, line2_xMax, line2_yMin );

    line2Left->SetLineColor(kRed);
    line2Left->SetLineWidth(3);
    line2Left->SetLineStyle(2);
    line2Left->Draw("same");

    line2Right->SetLineColor(kRed);
    line2Right->SetLineWidth(3);
    line2Right->SetLineStyle(2);
    line2Right->Draw("same");

    line2Top->SetLineColor(kRed);
    line2Top->SetLineWidth(3);
    line2Top->SetLineStyle(2);
    line2Top->Draw("same");

    line2Bottom->SetLineColor(kRed);
    line2Bottom->SetLineWidth(3);
    line2Bottom->SetLineStyle(2);
    line2Bottom->Draw("same");

  }

  BTLCommon::addLabels( c1_radio, conf );

  c1_radio->SaveAs( Form("plots/%s/%s.pdf", conf.get_confName().c_str(), h2_radio->GetName()) );
 
  delete c1_radio;
  delete h2_radio;

}



std::vector<float> getBins( int nBins, float xMin, float xMax ) {

  float step = (xMax-xMin)/((float)nBins);

  std::vector<float> bins;

  for( int i=0; i<nBins+1; ++i )
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
    
    
void drawEffGraph( BTLConf conf, TGraphAsymmErrors* gr_eff, float barMin, float barMax, const std::string& axisName, const std::string& cutText ) {

  //if( xMin<0. ) xMin *= 1.1;
  //else          xMin *= 0.9;

  //if( xMax>0. ) xMax *= 1.1;
  //else          xMax *= 0.9;

  bool isX = abs(barMax-barMin)>20.;

  float varMin = (isX) ? barMin-5. : barMin-0.5;
  float varMax = (isX) ? barMax+5. : barMax+0.5;

  TCanvas* c1 = new TCanvas( Form("c1_%s", gr_eff->GetName()), "", 600, 600 );
  c1->cd();

  float yMin = 0.5001;
  float yMax = 1.0999;

  TH2D* h2_axes = new TH2D( Form("axes_%s", gr_eff->GetName()), "", 10, varMin, varMax, 10, yMin, yMax );
  h2_axes->SetXTitle( axisName.c_str() );
  h2_axes->SetYTitle( Form("Efficiency of %s", cutText.c_str()) );
  h2_axes->Draw();

  TLine* lineOne = new TLine( varMin, 1., varMax, 1. );
  lineOne->Draw("same");

  TLine* line_barMin = new TLine( barMin, yMin, barMin, yMax );
  line_barMin->SetLineColor( kGray+1 );
  line_barMin->SetLineWidth( 3 );
  line_barMin->SetLineStyle( 2 );
  line_barMin->Draw("same");

  TLine* line_barMax = new TLine( barMax, yMin, barMax, yMax );
  line_barMax->SetLineColor( kGray+1 );
  line_barMax->SetLineWidth( 3 );
  line_barMax->SetLineStyle( 2 );
  line_barMax->Draw("same");

  gr_eff->SetMarkerStyle(20);
  gr_eff->SetMarkerColor(46);
  gr_eff->SetLineColor(46);
  gr_eff->SetMarkerSize(1.3);
  gr_eff->Draw("p same");

  //TPaveText* cutLabel = new TPaveText( 0.3, 0.2, 0.7, 0.35, "brNDC" );
  //cutLabel->SetTextSize(0.035);
  //cutLabel->SetFillColor(0);
  //cutLabel->AddText( cutText.c_str() );
  //cutLabel->Draw("same");

  BTLCommon::addLabels( c1, conf );

  c1->SaveAs( Form("plots/%s/%s.pdf", conf.get_confName().c_str(), gr_eff->GetName()) );

  delete c1;
  delete h2_axes;

}



void findXrange( TGraph* graph, double& xMin, double& xMax ) {

  xMin = 999.;
  xMax = -999.;

  int nPoints = graph->GetN();

  for( int iPoint=0; iPoint<nPoints; ++iPoint ) {

    double thisX, thisY;
    graph->GetPoint( iPoint, thisX, thisY );
    double xErrLow  = graph->GetErrorXlow ( iPoint );
    double xErrHigh = graph->GetErrorXhigh( iPoint );

    double thisXlow  = thisX-xErrLow;
    double thisXhigh = thisX+xErrHigh;

    if( thisXlow <xMin ) xMin = thisXlow  - 0.5*xErrLow;
    if( thisXhigh>xMax ) xMax = thisXhigh + 0.5*xErrHigh;

  }

}


std::vector< TF1* > getAmpWalkCorr( const BTLConf& conf, const std::vector<float> bins_ampMax, const std::vector< std::vector< TH1D* > > & vvh1_t, const std::string& name ) {


  int nBinsHodo = vvh1_t.size();
  std::string ampWalkName = (centralAmpWalk) ? "central" : std::string( Form("%dbins", nBinsHodo) );

  std::string fitsDir(Form("plots/%s/ampWalk_%s", conf.get_confName().c_str(), ampWalkName.c_str()));
  system( Form( "mkdir -p %s", fitsDir.c_str() ) );
  system( Form( "mkdir -p %s/eps", fitsDir.c_str() ) );
  system( Form( "mkdir -p %s/png", fitsDir.c_str() ) );
  system( Form( "mkdir -p %s/individualFits", fitsDir.c_str() ) );


  TString name_tstr(name);
  bool is_corr = name_tstr.EndsWith("_corr"); // ugly, i know

  float ampMax_min = bins_ampMax[0];
  float ampMax_max = bins_ampMax[ bins_ampMax.size()-1 ];

  if( conf.digiChSet()=="a" && conf.sensorConf()==4 ) {

    if( conf.ninoThr()==200. && conf.vBias()==69. ) 
      ampMax_max /= 1.5;

    if( conf.ninoThr()==60. ) {
      ampMax_max /= 2.;
      if( conf.vBias()==69. ) 
        ampMax_min *= 1.1;
    }

    if( conf.ninoThr()==40. ) 
      ampMax_max /= 2.;

  }


  std::vector< TF1* > vf1;

  for( int ihodo=0; ihodo<nBinsHodo; ++ihodo ) {

    std::string suffix = (centralAmpWalk) ? "_central" : std::string( Form("_%s_%d", ampWalkName.c_str(), ihodo) );

    TGraphErrors* gr_ampWalk = new TGraphErrors(0);
    gr_ampWalk->SetName( Form("gr_ampWalk%s%s", name.c_str(), suffix.c_str()) );

    TGraph* gr_ampWalk_sigmaUp = new TGraph(0);
    gr_ampWalk_sigmaUp->SetName( Form("gr_ampWalk%s_sigmaUp%s", name.c_str(), suffix.c_str()) );

    TGraph* gr_ampWalk_sigmaDn = new TGraph(0);
    gr_ampWalk_sigmaDn->SetName( Form("gr_ampWalk%s_sigmaDn%s", name.c_str(), suffix.c_str()) );

    for( unsigned i=0; i<vvh1_t[ihodo].size(); ++i ) {

      if( vvh1_t[ihodo][i]->GetEntries()<5 ) continue;

      TF1* f1_gaus = ( vvh1_t[ihodo][i]->GetEntries()>10 ) ? BTLCommon::fitGaus( vvh1_t[ihodo][i] ) : 0;

      if( SAVE_ALL_FITS && f1_gaus!=0 ) {

        TCanvas* c1 = new TCanvas( Form("c1_%s", vvh1_t[ihodo][i]->GetName()), "", 600, 600 );
        c1->cd();

        vvh1_t[ihodo][i]->Draw();

        BTLCommon::addLabels( c1, conf );

        c1->SaveAs( Form("%s/individualFits/%s.pdf", fitsDir.c_str(), vvh1_t[ihodo][i]->GetName()) );

        delete c1;

      }


      float x     = 0.5*(bins_ampMax[i]+bins_ampMax[i+1]);
      float x_err = (bins_ampMax[i+1]-bins_ampMax[i])/sqrt(12);

      bool useFit = (f1_gaus!=0) && (f1_gaus->GetParameter(2)<0.2);
      float y     = (useFit) ? f1_gaus->GetParameter( 1 ) : vvh1_t[ihodo][i]->GetMean();
      float y_err = (useFit) ? f1_gaus->GetParError ( 1 ) : vvh1_t[ihodo][i]->GetMeanError();
      float y_rms = (useFit) ? f1_gaus->GetParameter( 2 ) : vvh1_t[ihodo][i]->GetRMS();

      //float y     = f1_gaus->GetParameter( 1 );
      //float y_err = f1_gaus->GetParError ( 1 );
      //float y_rms = f1_gaus->GetParameter( 2 );

      if( y_err>0. ) {

        int iPoint = gr_ampWalk->GetN();
        gr_ampWalk->SetPoint     ( iPoint, x    , y     );
        gr_ampWalk->SetPointError( iPoint, x_err, y_err );
    
        gr_ampWalk_sigmaUp->SetPoint( iPoint, x    , y+y_rms     );
        gr_ampWalk_sigmaDn->SetPoint( iPoint, x    , y-y_rms     );

      } // if y_err > 0.

    } // for points


    TCanvas* c1= new TCanvas( Form("c1_ampWalk%s%s", name.c_str(), suffix.c_str()), "", 600, 600 );
    c1->cd();

    float yMin_axes=0., yMax_axes=0.;
    getAmpWalkYaxisRange( conf, yMin_axes, yMax_axes );

    TH2D* h2_axes = new TH2D( Form("axes%s%s", name.c_str(), suffix.c_str()), "", 10, ampMax_min, ampMax_max/1.3, 10, yMin_axes, yMax_axes );
    if( name_tstr.Contains("Left") )
      h2_axes->SetXTitle( "Max Amplitude Left [a.u.]" );
    else
      h2_axes->SetXTitle( "Max Amplitude Right [a.u.]" );
    h2_axes->SetYTitle( Form("t_{%s} - t_{MCP} [ns]", name.c_str()) );
    h2_axes->Draw();
    
    gr_ampWalk->SetMarkerStyle(20);
    gr_ampWalk->SetMarkerSize(1.);

    gr_ampWalk_sigmaUp->SetLineStyle(2);
    gr_ampWalk_sigmaUp->SetLineWidth(2);
    gr_ampWalk_sigmaUp->SetLineColor(38);

    gr_ampWalk_sigmaDn->SetLineStyle(2);
    gr_ampWalk_sigmaDn->SetLineWidth(2);
    gr_ampWalk_sigmaDn->SetLineColor(38);

    //std::string func = (name=="Right") ? "[0]+[1]/x+[2]/(x*x)+[3]/(x*x*x)+[4]/sqrt(x)" : "pol3";
    //std::string func = (name=="Right") ? "[0]+[1]*x+[2]*x*x+[3]*x*x*x+[4]/sqrt(x-[5])" : "pol3";
    std::string func = "pol5";
    //std::string func = "[0] + [1]*log([2]*(x-[3]))";
    //TF1* f1_ampWalk = new TF1( Form("fit_ampWalk%s", name.c_str()), func.c_str(), 0.16, 0.45 );
    TF1* f1_ampWalk = new TF1( Form("fit_ampWalk%s%s", name.c_str(), suffix.c_str()), func.c_str(), ampMax_min, ampMax_max );


    //TF1* f1_ampWalk = new TF1( Form("fit_ampWalk%s", name.c_str()),"ROOT::Math::crystalball_function(-x, 2, 1, 0.05, 0.2)", ampMax_min, ampMax_max );
    double xMid, yMid;
    gr_ampWalk_sigmaUp->GetPoint( (int)(gr_ampWalk_sigmaUp->GetN()*0.3), xMid, yMid );
    if( yMid>yMin_axes && yMid<yMax_axes )
      f1_ampWalk->SetParameter(0, yMid);
    else
      f1_ampWalk->SetParameter(0, 0.5*(yMin_axes+yMax_axes) );
    //f1_ampWalk->SetParameter(0, 0.5*(yMin_axes+yMax_axes) );
    f1_ampWalk->SetParameter(1, 0.);
    //f1_ampWalk->SetParameter(1, -0.1);
    f1_ampWalk->SetParameter(2, 0.);
    f1_ampWalk->SetParameter(3, 0.);
    f1_ampWalk->SetParameter(4, 0.);
    f1_ampWalk->SetParameter(5, 0.);
    f1_ampWalk->SetLineColor( 46 );
    if( !is_corr )
      gr_ampWalk->Fit( f1_ampWalk->GetName(), "R" );


    bool bottomLeft = (conf.vBias()<71. && conf.ninoThr()>=90. && name_tstr.Contains("Left")) || conf.sensorConf()==5;
    float xMin_leg = bottomLeft ? 0.2 : 0.55;
    float yMin_leg = bottomLeft ? 0.2 : 0.7 ;
    float xMax_leg = bottomLeft ? 0.65: 0.9 ;
    float yMax_leg = bottomLeft ? 0.4 : 0.9 ;
    if( is_corr ) yMin_leg += 0.07;
      
    TLegend* legend = new TLegend( xMin_leg, yMin_leg, xMax_leg, yMax_leg );
    legend->SetTextSize(0.035);
    legend->SetFillColor(0);
    legend->AddEntry( gr_ampWalk, "Data", "P" );
    legend->AddEntry( gr_ampWalk_sigmaDn, "68.3% band", "L" );
    if( !is_corr ) 
      legend->AddEntry( f1_ampWalk, "Fit", "L" );
    legend->Draw("same");

    TPaveText* label_conf = (bottomLeft) ? conf.get_labelConf(2) :  conf.get_labelConf(3);
    label_conf->Draw("same");

    gr_ampWalk->Draw( "P same" );
    gr_ampWalk_sigmaDn->Draw("L same");
    gr_ampWalk_sigmaUp->Draw("L same");


    BTLCommon::addLabels( c1, conf );

    c1->SaveAs( Form("%s/ampWalk%s%s.pdf"    , fitsDir.c_str(), name.c_str(), suffix.c_str()) );
    //c1->SaveAs( Form("%s/eps/ampWalk%s%s.eps", fitsDir.c_str(), name.c_str(), suffix.c_str()) );
    //c1->SaveAs( Form("%s/png/ampWalk%s%s.png", fitsDir.c_str(), name.c_str(), suffix.c_str()) );

    delete c1;

    vf1.push_back(f1_ampWalk);

  } // for ihodo

  return vf1;

}



void drawAmpWalkCorr( BTLConf conf, std::vector<TF1*> vf1_ampWalk, std::vector<float> bins_ampMax, std::vector<float> binsHodo, const std::string& lr, const std::string& suffix ) {

  std::string outdir( Form("plots/%s", conf.get_confName().c_str()) );

  TCanvas* c1_ampWalk = new TCanvas( Form("c1_ampWalk%s", lr.c_str()), "", 600, 600 );
  c1_ampWalk->cd();

  std::vector<int> colors = BTLCommon::colors();

  float yMin_axes=0., yMax_axes=0.;
  getAmpWalkYaxisRange( conf, yMin_axes, yMax_axes );

  TH2D* h2_axes_ampWalk = new TH2D( Form("axes_ampWalk%s", lr.c_str()), "", 10, bins_ampMax[0], bins_ampMax[bins_ampMax.size()-1], 10, yMin_axes, yMax_axes );
  h2_axes_ampWalk->SetXTitle( "Max Amplitude  [a.u.]" );
  h2_axes_ampWalk->SetYTitle( Form("t_{%s} - t_{MCP} [ns]", lr.c_str()) );
  h2_axes_ampWalk->Draw();

  TLegend* legend = new TLegend( 0.5, 0.7, 0.9, 0.9 );
  legend->SetTextSize(0.035);
  legend->SetFillColor(0);

  for( unsigned i=0; i<vf1_ampWalk.size(); ++i ) {

    vf1_ampWalk[i]->SetLineColor(colors[i]);
    vf1_ampWalk[i]->SetLineWidth(3);
    vf1_ampWalk[i]->Draw("same");

    legend->AddEntry( vf1_ampWalk[i], Form("%.2f < x < %.2f mm", binsHodo[i], binsHodo[i+1]), "L" );

  } // for i

  legend->Draw("same");

  BTLCommon::addLabels( c1_ampWalk, conf );

  c1_ampWalk->SaveAs( Form("%s/ampWalkCorr%s%s.pdf", outdir.c_str(), lr.c_str(), suffix.c_str()) );

  delete c1_ampWalk;
  delete h2_axes_ampWalk;

}



void getAmpWalkYaxisRange( BTLConf conf, float& yMin, float& yMax ) {

    yMin = (conf.digiChSet()=="a") ? 2.001 : 3.8;
    yMax = (conf.digiChSet()=="a") ? 3.7 : 5.5;
    //if( conf.sensorConf()==5 ) {
    //  yMin += 0.8;
    //  yMax += 0.5;
    //}

    if( conf.digiChSet()=="b" && conf.sensorConf()==4 && conf.ninoThr()==60. ) {
      yMin -= 0.6;
      yMax -= 0.6;
    }

}



TGraphErrors* getHodoCorr( BTLConf conf, std::vector<float> xBins, std::vector<TH1D*> vh1, const std::string& yName, const std::string& xName ) {

  
  std::string axisName = (xName=="xHodo") ? "Hodoscope X [mm]" : "Hodoscope Y [mm]";

  std::string fitsDir(Form("plots/%s/fits_%s/", conf.get_confName().c_str(), xName.c_str()) );
  system( Form("mkdir -p %s", fitsDir.c_str()) );

  
  TGraphErrors* graph = new TGraphErrors(0);
  graph->SetName( Form("%s_vs_%s", yName.c_str(), xName.c_str()) );

  for( unsigned i=0; i<vh1.size(); ++i ) {

    if( vh1[i]->GetEntries()<3 ) continue;

    float binWidth = xBins[i+1]-xBins[i];
    float x = xBins[i] + 0.5*binWidth;
    float x_err = binWidth/sqrt(12);

    TF1* f1_gaus = BTLCommon::fitGaus( vh1[i] );

    if( SAVE_ALL_FITS ) {

      TCanvas* c1 = new TCanvas( Form("c1_%d", i), "", 600, 600 );
      c1->cd();

      vh1[i]->Draw();

      //c1->SaveAs( Form( "plots/%s/fits_xHodo/%s.eps", conf.get_confName().c_str(), vh1_tAve_vs_xHodo[i]->GetName()) );
      c1->SaveAs( Form( "%s/%s.pdf", fitsDir.c_str(), vh1[i]->GetName()) );

      delete c1;

    }
  
    float y = f1_gaus->GetParameter(1);
    float y_err = f1_gaus->GetParError(1);

    int iPoint = graph->GetN();
    graph->SetPoint( iPoint, x, y );
    graph->SetPointError( iPoint, x_err, y_err );

  }

  graph->SetMarkerStyle(20);
  graph->SetMarkerSize(1.3);

  // correct vs xHodo:
  TF1* func = new TF1( Form("f1_%s", graph->GetName()), "pol1", -10, 40. );
  func->SetParameter( 0, 3. );
  func->SetLineColor(46);
  graph->Fit( func->GetName(), "RQ" );

  return graph;

}



void drawT_vs_hodo( BTLConf conf, TGraphErrors* gr_tLeft_vs_xHodo, TGraphErrors* gr_tRight_vs_xHodo, TGraphErrors* gr_tAve_vs_xHodo, const std::string& suffix ) {

  TCanvas* c1 = new TCanvas( Form("c1_t_vs_xHodo%s", suffix.c_str()), "", 600, 600 );
  c1->cd();

  double xMin, xMax;
  findXrange( gr_tRight_vs_xHodo, xMin, xMax );

  //float yMin_axes = (conf.digiChSet()=="a" ) ? 2.629 : 4.1;
  //float yMax_axes = (conf.digiChSet()=="a" ) ? 2.72  : 4.35;
  float yMin_axes = (conf.digiChSet()=="a" ) ? 2.5 : 3.;
  float yMax_axes = (conf.digiChSet()=="a" ) ? 4.  : 5.;

  TH2D* h2_axes = new TH2D( Form("axes_t_vs_xHodo%s", suffix.c_str()), "", 10, xMin, xMax, 10, yMin_axes, yMax_axes );
  //TH2D* h2_axes = new TH2D( Form("axes_%s_vs_%s", yName.c_str(), xName.c_str()), "", 10, -10., 15., 10, func->GetParameter(0)*0.98, func->GetParameter(0)*1.01 );
  h2_axes->SetXTitle( "Hodoscope X [mm]" );
  h2_axes->SetYTitle( "t(i) - t(MCP) [ns]" );
  h2_axes->Draw();

  gr_tRight_vs_xHodo ->SetLineColor(46);
  gr_tLeft_vs_xHodo  ->SetLineColor(38);
  gr_tAve_vs_xHodo   ->SetLineColor(kBlack);

  gr_tRight_vs_xHodo ->SetMarkerColor(46);
  gr_tLeft_vs_xHodo  ->SetMarkerColor(38);
  gr_tAve_vs_xHodo   ->SetMarkerColor(kBlack);

  gr_tRight_vs_xHodo ->Draw("Psame");
  gr_tLeft_vs_xHodo  ->Draw("Psame");
  gr_tAve_vs_xHodo   ->Draw("Psame");

  TF1* f1_tRight_vs_xHodo = gr_tRight_vs_xHodo->GetFunction( Form("f1_%s", gr_tRight_vs_xHodo->GetName()) );
  TF1* f1_tLeft_vs_xHodo  = gr_tLeft_vs_xHodo ->GetFunction( Form("f1_%s", gr_tLeft_vs_xHodo ->GetName()) );
  TF1* f1_tAve_vs_xHodo   = gr_tAve_vs_xHodo  ->GetFunction( Form("f1_%s", gr_tAve_vs_xHodo  ->GetName()) );

  f1_tRight_vs_xHodo ->SetLineColor(46);
  f1_tLeft_vs_xHodo  ->SetLineColor(38);
  f1_tAve_vs_xHodo   ->SetLineColor(kBlack);

  TLegend* legend = new TLegend( 0.75, 0.72, 0.9, 0.9 );
  legend->SetTextSize(0.035);
  legend->SetFillColor(0);
  legend->AddEntry( gr_tLeft_vs_xHodo , "t(Left)" , "P" );
  legend->AddEntry( gr_tRight_vs_xHodo, "t(Right)", "P" );
  legend->AddEntry( gr_tAve_vs_xHodo  , "t(Ave)"  , "P" );
  legend->Draw("same");

  BTLCommon::addLabels( c1, conf );

  c1->SaveAs( Form("plots/%s/t_vs_xHodo%s.pdf", conf.get_confName().c_str(), suffix.c_str()) );
  //c1->SaveAs( Form("plots/%s/eps/t_vs_xHodo%s.eps", conf.get_confName().c_str(), suffix.c_str()) );
  //c1->SaveAs( Form("plots/%s/png/t_vs_xHodo%s.png", conf.get_confName().c_str(), suffix.c_str()) );

  delete c1;
  delete h2_axes;

}
