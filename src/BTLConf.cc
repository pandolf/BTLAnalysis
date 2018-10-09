#include "../interface/BTLConf.h"

#include "TROOT.h"

#include <iostream>
#include <stdlib.h>
#include <vector>





BTLConf::BTLConf(  int sensorConf, const std::string& digiConf, float vBias, float ninoThr ) {

  sensorConf_ = sensorConf;
  digiConf_ = digiConf;
  vBias_ = vBias;
  ninoThr_ = ninoThr;

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


  if( parts.size()<5 || parts[0]!="Conf" ) {

    std::cout << "ERROR! ConfName needs to be in the format: Conf_[sensorConfig]_[digiConfig]_[ninoThr]_[vBias]" << std::endl;
    std::cout << "Setting default values (-99)." << std::endl;

    sensorConf_ = -99;
    digiConf_ = "xx";
    ninoThr_ = -99;
    vBias_ = -99;

  } else {

    sensorConf_ = atoi( parts[1].c_str() ); 
    digiConf_   = parts[2]; 
    ninoThr_ = (float)(atoi( parts[3].c_str() )); 
    vBias_   = (float)(atoi( parts[4].c_str() )); 

    std::string digich = this->digiChannelSet();
    if( digich!="a" && digich!="b" ) 
      std::cout << "WARNING!! Didn't pass a valid digiChannelSet: " << digich << " (valid values are 'a' and 'b')" << std::endl;

  }

}
    


int BTLConf::digiConfNumber() const {

  return atoi( digiConf_.substr(0, digiConf_.size()-1).c_str() );

}


std::string BTLConf::digiChannelSet() const {

  return (std::string)(digiConf_.substr(digiConf_.size()-1, digiConf_.size()));

}


std::string BTLConf::get_confName() const {

  std::string confName( Form("Conf_%d_%s_%.0f_%.0f", sensorConf_, digiConf_.c_str(), ninoThr_, vBias_) );

  return confName;

}



TFile* BTLConf::get_resoFile( const std::string& name ) const {

  std::string suffix(name);
  if( suffix !="" ) suffix = "_" + suffix;
  TFile* file = TFile::Open( Form("plots/%s/resoFile%s.root", this->get_confName().c_str(), suffix.c_str()) );

  return file;

}



std::string BTLConf::get_fileListName() const {

  std::string files;

  if( this->sensorConf()==4 ) {

    files = std::string(Form("files_Conf_%d_%d_%.0f_%.0f.txt", this->sensorConf(), this->digiConfNumber(), this->ninoThr(), this->vBias()));

  } else if( this->sensorConf()==5 ) {

    if( this->digiChannelSet()=="a" ) files = std::string(Form("files_Conf_%d_%d_%.0f_%.0f_%.0f.txt", this->sensorConf(), this->digiConfNumber(), this->ninoThr(), this->vBias()         , this->get_otherBias(2)));
    else                              files = std::string(Form("files_Conf_%d_%d_%.0f_%.0f_%.0f.txt", this->sensorConf(), this->digiConfNumber(), this->ninoThr(), this->get_otherBias(1), this->vBias()));

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
