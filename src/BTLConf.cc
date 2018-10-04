#include "../interface/BTLConf.h"

#include "TROOT.h"

#include <iostream>
#include <stdlib.h>
#include <vector>





BTLConf::BTLConf(  int sensorConf, int digiConf, float vBias, float ninoThr ) {

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
    digiConf_ = -99;
    ninoThr_ = -99;
    vBias_ = -99;

  } else {

    sensorConf_ = atoi( parts[1].c_str() ); 
    digiConf_   = atoi( parts[2].c_str() ); 
    ninoThr_ = (float)(atoi( parts[3].c_str() )); 
    vBias_   = (float)(atoi( parts[4].c_str() )); 

  }

}
    


std::string BTLConf::get_confName() const {

  std::string confName( Form("Conf_%d_%d_%.0f_%.0f", sensorConf_, digiConf_, ninoThr_, vBias_) );

  return confName;

}



TFile* BTLConf::get_resoFile() const {

  TFile* file = TFile::Open( Form("plots/%s/resoFile.root", this->get_confName().c_str()) );

  return file;

}
