#ifndef BTLConf_h
#define BTLConf_h

#include <string>
#include "TFile.h"


class BTLConf {


 public:

  BTLConf(  int sensorConf, const std::string& digiChSet, float ninoThr=-1., float vBias=-1. );
  BTLConf(  std::string confName );
  ~BTLConf() {};

  int sensorConf()        const { return sensorConf_; };
  std::string digiChSet() const { return digiChSet_ ; };
  float vBias()           const { return vBias_     ; };
  float ninoThr()         const { return ninoThr_   ; };

  void set_sensorConf( int sensorConf               ) { sensorConf_ = sensorConf; };
  void set_digiChSet ( const std::string& digiChSet ) { digiChSet_  = digiChSet ; };
  void set_vBias     ( float vBias                  ) { vBias_      = vBias     ; };
  void set_ninoThr   ( float ninoThr                ) { ninoThr_    = ninoThr   ; };
  
  std::string get_confName() const;
  std::string get_fileListName() const;
  float  get_otherBias( int i ) const;

  TFile* get_resoFile( const std::string& name="" ) const;

 private:

  int sensorConf_;
  std::string digiChSet_;
  float vBias_;
  float ninoThr_;

};


#endif
