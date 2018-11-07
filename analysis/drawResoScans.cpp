#include <iostream>
#include <fstream>

#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TLegend.h"
#include "TPaveText.h"
#include "TF1.h"
#include "TGraphErrors.h"

#include "../interface/BTLCommon.h"
#include "../interface/BTLConf.h"





void drawScans( BTLConf conf, const std::string& awType, const std::string& name="" );
std::pair< TGraphErrors*, TGraphErrors* >  getScan( BTLConf conf, const std::string& awType, const std::string& var, float value, const std::string& name );
void drawScan( BTLConf conf, const std::string& awType, const std::string& scanName, std::vector< std::pair< TGraphErrors*, TGraphErrors* > > scans, float xMin, float xMax, const std::string& axisName, const std::string& legendTitle, const std::string& name );
std::vector<float> get_vBiasThresholds( BTLConf conf );
std::vector<float> get_ninoThresholds( BTLConf conf );


int main( int argc, char* argv[] ) {


  int sensorConf = 4;
  std::string digiChSet = "a";
  std::string awType = "aw4bins";

  if( argc>=3 ) {

    sensorConf = atoi(argv[1]);
    digiChSet = std::string(argv[2]);

    if( argc>=4 ) {

      int nBinsHodo = 1;
      bool centralAmpWalk = false;
      std::string argv3(argv[3]);
      if( argv3=="Center" || argv3=="center" || argv3=="central" || argv3=="Central" ) {
        nBinsHodo = 1;
        centralAmpWalk = true;
      } else {
        nBinsHodo = atoi(argv[3]);
      }

      if( centralAmpWalk ) {

        awType = "awCentral";
 
      } else {

        awType = std::string(Form("aw%dbins", nBinsHodo));

      }

    } // if argv>=3

  } else {
    std::cout << "USAGE: ./drawResoScans [sensorConf] [digiChSet] [awType=4]" << std::endl;
    exit(1);
  }

  system( "mkdir -p plots/eps" );
  system( "mkdir -p plots/png" );
  
  BTLCommon::setStyle();

  BTLConf conf( sensorConf, digiChSet );

  drawScans( conf, awType, "hodoOnBar" );
  drawScans( conf, awType, "hodoFiducial" );

  return 0;

}


void drawScans( BTLConf conf, const std::string& awType, const std::string& name ) {

  // Vbias scan

  std::vector<float> ninoThresholds = get_ninoThresholds( conf );

  std::vector< std::pair<TGraphErrors*,TGraphErrors*> > scans_vBias;

  for( unsigned i=0; i<ninoThresholds.size(); ++i )
    scans_vBias.push_back( getScan(conf, awType, "ninoThr",  ninoThresholds[i], name) );

  std::vector<float> vBiasThresholds = get_vBiasThresholds( conf );

  float vBias_xMin = vBiasThresholds[0] - 2.;
  float vBias_xMax = vBiasThresholds[vBiasThresholds.size()-1] + 5.99;

  drawScan( conf, awType, "vBias", scans_vBias, vBias_xMin, vBias_xMax, "V(bias) [V]", "NINO threshold", name );


  for( unsigned i=0; i<scans_vBias.size(); ++i ) {
    delete scans_vBias[i].first;
    delete scans_vBias[i].second;
  }


  // NINO scan

  std::vector< std::pair<TGraphErrors*,TGraphErrors*> > scans_nino;
  for( unsigned i=0; i<vBiasThresholds.size(); ++i )
    scans_nino.push_back( getScan(conf, awType, "vBias",  vBiasThresholds[i], name) );



  drawScan( conf, awType, "ninoThr", scans_nino, 0., ninoThresholds[ninoThresholds.size()-1]+80., "NINO threshold [mV]", "V(bias)", name );

  for( unsigned i=0; i<scans_nino.size(); ++i ) {
    delete scans_nino[i].first;
    delete scans_nino[i].second;
  }

}




std::pair<TGraphErrors*,TGraphErrors*> getScan( BTLConf conf, const std::string& awType, const std::string& var, float value, const std::string& name ) {


  std::string suffix = "_" + awType;
  if( name!="" ) suffix = suffix + "_" + name;


  TGraphErrors* graph = new TGraphErrors(0);
  if( var=="ninoThr" )
    graph->SetName( Form( "%.0f mV", value ) );
  else
    graph->SetName( Form( "%.0f V", value ) );

  TGraphErrors* graph_sigmaEff = new TGraphErrors(0);
  graph_sigmaEff->SetName(Form("gr_%s_sigmaEff", var.c_str()));


  std::vector<float> x_values;
  if( var=="ninoThr" ) { // then scan vBias
    x_values = get_vBiasThresholds( conf );
  } else {
    x_values = get_ninoThresholds( conf );
  }




  for( unsigned i=0; i<x_values.size(); ++i ) {

    BTLConf conf_copy( conf );
    if( var=="ninoThr" ) {
      conf_copy.set_ninoThr( value );
      conf_copy.set_vBias( x_values[i] );
    } else if( var=="vBias" ) {
      conf_copy.set_ninoThr( x_values[i] );
      conf_copy.set_vBias( value );
    }


    TFile* resoFile = conf_copy.get_resoFile(suffix);

    if( resoFile!=0 ) {

      TH1D* h1_reso = (TH1D*)resoFile->Get("reso_corr");

      if( h1_reso==0 ) continue;

      TF1* f1_gaus = h1_reso->GetFunction( Form("gaus_%s", h1_reso->GetName()) );

      if( f1_gaus==0 ) continue;

      float y = f1_gaus->GetParameter(2);
      float y_err = f1_gaus->GetParError(2);

      if( y>0. && y<10000. ) {

        float reso = BTLCommon::subtractResoPTK(y);
        int iPoint = graph->GetN();
        graph->SetPoint( iPoint, x_values[i], reso*1000. );
        graph->SetPointError( iPoint, 0., y_err*1000. );

        graph_sigmaEff->SetPoint( iPoint, x_values[i], BTLCommon::subtractResoPTK(BTLCommon::getSigmaEff(h1_reso))*1000. );

      } else {

        std::cout << "-> WARNING! Didn't add: " << resoFile->GetName() << std::endl;
 
      }

    }

  } // for configs

  

  std::pair< TGraphErrors*, TGraphErrors* > gr_pair;
  gr_pair.first  = graph; 
  gr_pair.second = graph_sigmaEff; 

  return gr_pair;

}


void drawScan( BTLConf conf, const std::string& awType, const std::string& scanName, std::vector< std::pair<TGraphErrors*,TGraphErrors*> > scans, float xMin, float xMax, const std::string& axisName, const std::string& legendTitle, const std::string& name ) {

  std::string suffix = "_" + awType;
  if( name!="" ) suffix = suffix + "_" + name;

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  TH2D* h2_axes = new TH2D( Form("axes_%s", scanName.c_str()), "", 10, xMin, xMax, 10, 0., 95. );
  h2_axes->SetXTitle( axisName.c_str() );
  h2_axes->SetYTitle( "Time Resolution [ps]" );
  h2_axes->Draw();

  TLine* line30 = new TLine( xMin, 30., xMax, 30. );
  line30->SetLineStyle(3);
  line30->SetLineColor(kBlack);
  line30->SetLineWidth(2);
  line30->Draw("same");

  float xMin_leg = ( conf.sensorConf()==5 && scanName=="ninoThr" ) ? 0.27 : 0.63;
  float xMax_leg = ( conf.sensorConf()==5 && scanName=="ninoThr" ) ? 0.57 : 0.9;

  TLegend* legend = new TLegend( xMin_leg, 0.9 - 0.08*scans.size(), xMax_leg, 0.9, legendTitle.c_str() );
  legend->SetFillColor(0);
  legend->SetTextSize(0.035);

  std::vector<int> colors = BTLCommon::colors();


  // first round for sigmaEff (behind):
  for( unsigned i=0; i<scans.size(); ++i ) {

    scans[i].second->SetLineColor( colors[i] );
    scans[i].second->SetLineWidth(2);
    scans[i].second->SetLineStyle(2);
    scans[i].second->Draw("Lsame");

  }

  // second round to draw reso graphs:
  for( unsigned i=0; i<scans.size(); ++i ) {

    legend->AddEntry( scans[i].first, scans[i].first->GetName(), "LP" );
    // then reso graph:
    scans[i].first->SetLineColor( colors[i] );
    scans[i].first->SetLineWidth(2);
    scans[i].first->SetMarkerStyle(20+i);
    scans[i].first->SetMarkerSize(1.3);
    scans[i].first->SetMarkerColor(colors[i]);
    scans[i].first->Draw("PLsame");

  }

  legend->Draw("same");

  TGraph* gr_dummyLine = new TGraph(0);
  gr_dummyLine->SetLineColor( kGray+1 );
  gr_dummyLine->SetLineWidth( 2 );
  gr_dummyLine->SetLineStyle( 1 );

  TGraph* gr_dummyLine2 = new TGraph(0);
  gr_dummyLine2->SetLineColor( kGray+1 );
  gr_dummyLine2->SetLineWidth( 2 );
  gr_dummyLine2->SetLineStyle( 2 );

  TLegend* legend2 = new TLegend( 0.63, 0.2, 0.9, 0.33 );
  legend2->SetFillColor(0);
  legend2->SetTextSize(0.038);
  legend2->AddEntry( gr_dummyLine2, "#sigma_{eff}", "L" );
  legend2->AddEntry( gr_dummyLine , "#sigma_{fit}", "L" );
  legend2->Draw("same");


  BTLCommon::addLabels( c1, conf );

  c1->SaveAs( Form("plots/scan_%s_%d%s%s.pdf"                  , scanName.c_str(), conf.sensorConf(), conf.digiChSet().c_str(), suffix.c_str()) );
  c1->SaveAs( Form("plots/eps/scan_%s_%d%s%s.eps"              , scanName.c_str(), conf.sensorConf(), conf.digiChSet().c_str(), suffix.c_str()) );

  delete c1;
  delete h2_axes;


  // write to table

  std::ofstream resoTable( Form("plots/resoTable_%s_%d%s%s.txt", scanName.c_str(), conf.sensorConf(), conf.digiChSet().c_str(), suffix.c_str()) );

  resoTable << "Configuration " << conf.sensorConf() << conf.digiChSet() << std::endl;
  resoTable << std::endl;

  for( unsigned i=0; i<scans.size(); ++i ) {

    resoTable << scanName << " = " << scans[i].first->GetName() << std::endl;

    for( unsigned iPoint=0; iPoint<scans[i].first->GetN(); ++iPoint ) {

      double this_x, this_y;
      scans[i].first->GetPoint( iPoint, this_x, this_y );
      double this_y_err = scans[i].first->GetErrorY(iPoint);

      resoTable << "   " << this_x << " " << this_y << " " << this_y_err << std::endl;

    } // for points

    resoTable << std::endl;

  } // for scans

  resoTable.close();

}


std::vector<float> get_vBiasThresholds( BTLConf conf ) {

  std::vector<float> thresholds;

  if( conf.sensorConf()==4 ) {

    //thresholds.push_back(68.);
    thresholds.push_back(69.);
    thresholds.push_back(70.);
    thresholds.push_back(72.);

  } else if( conf.sensorConf()==5 ) {

    if( conf.digiChSet()=="a" ) {

      thresholds.push_back(28.);
      thresholds.push_back(32.);
      thresholds.push_back(36.);

    } else if( conf.digiChSet()=="b" ) {

      thresholds.push_back(53.);
      thresholds.push_back(54.);
      thresholds.push_back(56.);

    }

  }

  return thresholds;

}

std::vector<float> get_ninoThresholds( BTLConf conf ) {

  std::vector<float> thresholds;
  thresholds.push_back(40.);
  thresholds.push_back(60.);
  thresholds.push_back(100.);
  thresholds.push_back(200.);
  thresholds.push_back(500.);

  return thresholds;

}
