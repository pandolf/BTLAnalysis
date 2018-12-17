#include "TFile.h"
#include "TTree.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TLegend.h"

#include "../interface/BTLConf.h"
#include "../interface/BTLCommon.h"



int main( int argc, char* argv[] ) {

  BTLCommon::setStyle();

  std::string awType = "aw4bins";

  std::vector<int> v_sensorConf;
  v_sensorConf.push_back(4);
  v_sensorConf.push_back(5);

  std::vector<std::string> v_digiChSet;
  v_digiChSet.push_back("a");
  v_digiChSet.push_back("b");


  std::vector<TGraphErrors*> v_graphs;

  for( std::vector<int>::const_iterator iSens=v_sensorConf.begin(); iSens!=v_sensorConf.end(); ++iSens ) {

    for( std::vector<std::string>::const_iterator iDigiChSet=v_digiChSet.begin(); iDigiChSet!=v_digiChSet.end(); ++iDigiChSet ) {

      BTLConf conf( *iSens, *iDigiChSet );
      
      std::vector<float> v_vBias = BTLCommon::get_vBiasThresholds( conf );

      TGraphErrors* graph = new TGraphErrors(0);
      //graph->SetName( Form("%s", conf.SiPMTypeText().c_str()) );
      graph->SetName( Form("%d %s", *iSens, iDigiChSet->c_str()) );

      for( unsigned i=0; i<v_vBias.size(); ++i ) {

        BTLConf conf_copy( *iSens, *iDigiChSet, 100, v_vBias[i] );

        TFile* file = TFile::Open( Form("treesLite/%s_%s.root", conf_copy.get_confName().c_str(), awType.c_str()) );

        TH1D* h1_ampMaxLeft  = (TH1D*)file->Get("ampMaxLeft" );
        TH1D* h1_ampMaxRight = (TH1D*)file->Get("ampMaxRight");

        TF1* f1_Left  = h1_ampMaxLeft  ->GetFunction( Form("landau_%s", h1_ampMaxLeft  ->GetName()) );
        TF1* f1_Right = h1_ampMaxRight ->GetFunction( Form("landau_%s", h1_ampMaxRight ->GetName()) );

        float mipPeakLeft     = f1_Left ->GetParameter(1);
        float mipPeakRight    = f1_Right->GetParameter(1);

        float mipPeakErrLeft  = f1_Left ->GetParError(1);
        float mipPeakErrRight = f1_Right->GetParError(1);

        float mipPeak    = 0.5*(mipPeakLeft   +mipPeakRight   );
        float mipPeakErr = 0.5*(mipPeakErrLeft+mipPeakErrRight);

        int iPoint = graph->GetN();

        float vOV = conf_copy.vOV();

        graph->SetPoint( iPoint, vOV, mipPeak );
        graph->SetPointError( iPoint, 0., mipPeakErr );

      } // for vBias

      v_graphs.push_back( graph );

    } // for digiChSet

  } // for sensorConf


  std::vector<int> colors = BTLCommon::colors();

  float xMin =  0.;
  float xMax = 10.;

  for( unsigned i=0; i<v_graphs.size(); ++i ) {

    v_graphs[i]->SetMarkerStyle(20+1);
    v_graphs[i]->SetMarkerSize(1.6);
    v_graphs[i]->SetMarkerColor(colors[i]);
    v_graphs[i]->SetLineColor(colors[i]);
    
    TF1* f1_line = new TF1( Form("f1_%s", v_graphs[i]->GetName()), "[0] + [1]*x", xMin, xMax );
    f1_line->SetLineColor(colors[i]);
    f1_line->SetLineWidth(2);

    v_graphs[i]->Fit( f1_line, "QR" );

  }
  
  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  TH2D* h2_axes = new TH2D( "axes", "", 10, xMin, xMax, 10, 0., 0.8 );
  h2_axes->SetYTitle( "MIP Peak [a.u.]" );
  h2_axes->SetXTitle( "Overvoltage [V]" );
  h2_axes->Draw();

  TLegend* legend = new TLegend( 0.2, 0.7, 0.5, 0.9 );
  legend->SetFillColor(0);
  legend->SetTextSize(0.035);

  for( unsigned i=0; i<v_graphs.size(); ++i ) {

    v_graphs[i]->Draw("P same");
    legend->AddEntry( v_graphs[i], v_graphs[i]->GetName(), "PL" );
    
  }

  legend->Draw("same");

  gPad->RedrawAxis();
   
  c1->SaveAs( "plots/amp_vs_vOV.pdf" );

  delete c1;
  delete h2_axes;

  return 0;

}
