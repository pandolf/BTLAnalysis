#include "../interface/BTLConf.h"

#include <iostream>
#include <stdlib.h>
#include <vector>




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

  if( parts[0]!="Conf" || parts.size()!=5 ) {

    std::cout << "ERROR! ConfName needs to be in the format: Conf_[sensorConfig]_[digiConfig]_[ninoThresh]_[vBias]" << std::endl;
    std::cout << "Setting default values (-99)." << std::endl;

    sensorConf_ = -99;
    digiConf_ = -99;
    ninoThresh_ = -99;
    vBias_ = -99;

  } else {

    sensorConf_ = atoi( parts[1].c_str() ); 
    digiConf_   = atoi( parts[2].c_str() ); 
    ninoThresh_ = (float)(atoi( parts[3].c_str() )); 
    vBias_      = (float)(atoi( parts[4].c_str() )); 

    std::cout << "sensorConf_: " <<  sensorConf_ << std::endl;
    std::cout << "digiConf_  : " <<  digiConf_   << std::endl;
    std::cout << "ninoThresh_: " <<  ninoThresh_ << std::endl;
    std::cout << "vBias_     : " <<  vBias_      << std::endl;

  }

}
    


std::string BTLConf::get_confName() const {

}
