#include "../interface/BTLConf.h"

#include <iostream>
#include <stdlib.h>
#include <vector>





BTLConf::BTLConf(  const BTLConf& rhs ) {

  sensorConf_ = rhs.sensorConf();
  digiChSet_ = rhs.digiChSet();
  vBias_ = rhs.vBias();
  ninoThr_ = rhs.ninoThr();

  runNumber_ = rhs.runNumber();

}
  


BTLConf::BTLConf(  int sensorConf, const std::string& digiChSet, float ninoThr, float vBias, int runNumber ) {

  sensorConf_ = sensorConf;
  digiChSet_ = digiChSet;
  ninoThr_ = ninoThr;
  vBias_ = vBias;

  runNumber_ = runNumber;

}




BTLConf::BTLConf( std::string confName ) {

  std::string delimiter = "_";
  size_t pos = 0;
  std::vector<std::string> parts;
  std::string part;
  while ((pos = confName.find(delimiter)) != std::string::npos) {
    part = confName.substr(0, pos);
    confName.erase(0, pos + delimiter.length());
    parts.push_back(part);
  }
  parts.push_back(confName); // last piece


  if( parts.size()>=5 && parts[0]=="Conf" ) {

    sensorConf_ = atoi( parts[1].c_str() ); 
    digiChSet_  = parts[2]; 
    ninoThr_ = (float)(atoi( parts[3].c_str() )); 
    vBias_   = (float)(atoi( parts[4].c_str() )); 

    if( parts.size()>5 )
      runNumber_ = atoi(parts[5].c_str());
    else
      runNumber_ = -99;

    if( digiChSet_!="a" && digiChSet_!="b" ) 
      std::cout << "WARNING!! Didn't pass a valid digiChSet: " << digiChSet_ << " (valid values are 'a' and 'b')" << std::endl;

  } else if( parts.size()==2 && parts[0]=="Run" ) {

    sensorConf_ = -99;
    digiChSet_ = "xx";
    ninoThr_ = -99;
    vBias_ = -99;

    runNumber_ = atoi(parts[1].c_str());
 
  } else {

    std::cout << "ERROR! ConfName needs to be in the format: Conf_[sensorConfig]_[digiChSet]_[ninoThr]_[vBias]" << std::endl;
    std::cout << "Setting default values (-99)." << std::endl;

    sensorConf_ = -99;
    digiChSet_ = "xx";
    ninoThr_ = -99;
    vBias_ = -99;

    runNumber_ = -99;

  }

}
    



std::string BTLConf::get_confName() const {

  std::string confName;
  if( runNumber_>=0 ) 
    confName = std::string( Form("Conf_%d_%s_%.0f_%.0f_%d", sensorConf_, digiChSet_.c_str(), ninoThr_, vBias_, runNumber_) );
  else
    confName = std::string( Form("Conf_%d_%s_%.0f_%.0f", sensorConf_, digiChSet_.c_str(), ninoThr_, vBias_) );

  return confName;

}



TFile* BTLConf::get_resoFile( const std::string& suffix ) const {

  TFile* file = TFile::Open( Form("plots/%s/resoFile%s.root", this->get_confName().c_str(), suffix.c_str()) );

  return file;

}



std::string BTLConf::get_fileListName() const {

  std::string files;

  if( this->sensorConf()==4 ) {

    files = std::string(Form("files_Conf_%d_%.0f_%.0f.txt", this->sensorConf(), this->ninoThr(), this->vBias()));

  } else if( this->sensorConf()==5 ) {

    if( this->digiChSet()=="a" ) files = std::string(Form("files_Conf_%d_%.0f_%.0f_%.0f.txt", this->sensorConf(), this->ninoThr(), this->vBias()         , this->get_otherBias(2)));
    else                         files = std::string(Form("files_Conf_%d_%.0f_%.0f_%.0f.txt", this->sensorConf(), this->ninoThr(), this->get_otherBias(1), this->vBias()));

  }

  return files;

}


float BTLConf::get_otherBias( int i ) const {

  float returnBias = -1.;

  if( this->sensorConf()==5 && i==1) {

    if     ( this->vBias()==56. ) returnBias = 36.;
    else if( this->vBias()==54. ) returnBias = 32.;
    else if( this->vBias()==53. ) returnBias = 28.;

  } else if( this->sensorConf()==5 && i==2 ) {

    if     ( this->vBias()==36. ) returnBias = 56.;
    else if( this->vBias()==32. ) returnBias = 54.;
    else if( this->vBias()==28. ) returnBias = 53.;

  } else if( this->sensorConf()==4 ) {

    returnBias = this->vBias();

  }

  return returnBias;

}



float BTLConf::vBreakdown() const {

  float vBD = -1.;

  if( this->sensorConf()==4 ) {

    // HPK 3x3 mm^{2} (15 #mum)
    vBD = 66.5;
 
  } else if( this->sensorConf()==5 ) {

    if( this->digiChSet()=="a" ) {
      // FBK 5x5 mm^{2} (20 #mum)
      vBD = 26.7;
    } else  {
      // HPK 3x3 mm^{2} (50 #mum)
      vBD = 51.7;
    }

  }

  return vBD;

}


float BTLConf::vOV() const {

  return this->vBias()-vBreakdown();

}


TPaveText* BTLConf::get_labelConf( int quadrant ) const {

  if     ( quadrant == 1 ) return this->get_labelConf( 0.55, 0.8, 0.9  , 0.9 );
  else if( quadrant == 2 ) return this->get_labelConf( 0.2 , 0.8, 0.45 , 0.9 );
  else if( quadrant == 3 ) return this->get_labelConf( 0.2 , 0.2, 0.45 , 0.3 );
  else if( quadrant == 4 ) return this->get_labelConf( 0.55, 0.2, 0.9  , 0.3 );
  else {
    std::cout << "[BTLConf::get_labelSettings] Quadrant needs to be valid!" << std::endl;
    exit(3001);
  }

}



TPaveText* BTLConf::get_labelConf( float xMin, float yMin, float xMax, float yMax ) const {

  if( runNumber_>0 ) {
    yMax += 0.025;
    yMin -= 0.025;
  }

  TPaveText* label = new TPaveText( xMin, yMin, xMax, yMax, "brNDC" );
  label->SetTextSize(0.03);
  label->SetTextFont(42);
  label->SetFillColor(0);
  if( runNumber_>0 )
    label->AddText( Form("Run %d", this->runNumber()) );
  label->AddText( Form("NINO thr = %.0f mV", this->ninoThr()) );
  label->AddText( Form("V(bias) = %.0f V", this->vBias()) );

  return label;

}
   


BTLCrystal BTLConf::crystal() const {

  float pi = 3.14159;
  float oneDeg = pi/180.;

  BTLCrystal thisCrystal;

  if( this->sensorConf()==4 ) {

    if( this->digiChSet()=="a" ) {

      thisCrystal.set_angle(2.6*oneDeg);
      thisCrystal.set_xLow (-9.);
      thisCrystal.set_xHigh(37.);
      thisCrystal.set_yLow (0.25);
      thisCrystal.set_yHigh(3.25);

    } else {

      thisCrystal.set_angle(-0.95*oneDeg);
      thisCrystal.set_xLow (-11.);
      thisCrystal.set_xHigh(36.);
      thisCrystal.set_yLow (1.25);
      thisCrystal.set_yHigh(4.25);

    }

  } else if( this->sensorConf()==5 ) {

    if( this->digiChSet()=="a" ) {

      thisCrystal.set_angle(0.55*oneDeg);
      thisCrystal.set_xLow (-10.5);
      thisCrystal.set_xHigh(38.);
      thisCrystal.set_yLow (-0.5);
      thisCrystal.set_yHigh(2.5);

    } else {

      thisCrystal.set_angle(+0.3*oneDeg);
      thisCrystal.set_xLow (-10.);
      thisCrystal.set_xHigh(39.);
      thisCrystal.set_yLow (-1.25);
      thisCrystal.set_yHigh(1.75);

    }

  }
   
  return thisCrystal;

}


std::string BTLConf::crystalTypeText() const {

  std::string text = "LYSO:Ce";

  if( this->sensorConf()==4 ) {

    if( this->digiChSet()=="a" ) {
      text = "LYSO:Ce 3x3x50 mm^{3}";
    } else {
      text = "LYSO:Ce 3x4x50 mm^{3}";
    }

  } else if( this->sensorConf()==5 ) {

    if( this->digiChSet()=="a" ) {
      text = "LYSO:Ce 3x4x50 mm^{3}";
    } else  {
      text = "LYSO:Ce 3x3x50 mm^{3}";
    }

  }

  return text;

}


std::string BTLConf::SiPMTypeText() const {

  std::string text = "SiPM";

  if( this->sensorConf()==4 ) {

    if( this->digiChSet()=="a" ) {
      text = "HPK 3x3 mm^{2} (15 #mum)";
    } else {
      text = "HPK 3x3 mm^{2} (15 #mum)";
    }

  } else if( this->sensorConf()==5 ) {

    if( this->digiChSet()=="a" ) {
      text = "FBK 5x5 mm^{2} (20 #mum)";
    } else  {
      text = "HPK 3x3 mm^{2} (50 #mum)";
    }

  }

  return text;

}
