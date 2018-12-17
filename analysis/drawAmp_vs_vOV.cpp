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


  BTLConf conf( sensorConf, digiChSet );
  
  std::vector<float> v_vBias = BTLCommon::get_vBiasThresholds(  conf );

  TGraphErrors* graph = new TGraphErrors(0);

  for( unsigned i=0; i<v_vBias.size(); ++i ) {

    BTLConf conf_copy( sensorConf, digiChSet, 100, v_vBias[i] );

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

  TFile* fileTest = TFile::Open("test.root", "recreate");
  fileTest->cd();
  graph->Write();
  fileTest->Close();

  return 0;

}
