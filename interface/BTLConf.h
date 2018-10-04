#ifndef BTLConf_h
#define BTLConf_h

#include <string>
#include "TFile.h"


class BTLConf {


 public:

  BTLConf(  std::string confName );
  ~BTLConf() {};

  int sensorConf()   const { return sensorConf_; };
  int digiConf()     const { return digiConf_  ; };
  float vBias()      const { return vBias_     ; };
  float ninoThr()    const { return ninoThr_   ; };
  
  //float posX() const { return posX_; };
  //float posY() const { return posY_; };

  void set_sensorConf( int sensorConf   ) { sensorConf_ = sensorConf; };
  void set_digiConf  ( int digiConf     ) { digiConf_   = digiConf  ; };
  void set_vBias     ( float vBias      ) { vBias_      = vBias     ; };
  void set_ninoThr   ( float ninoThr    ) { ninoThr_    = ninoThr   ; };
  
  std::string get_confName() const;

  TFile* get_resoFile() const;

 private:

  int sensorConf_;
  int digiConf_;
  float vBias_;
  float ninoThr_;
  
  //float posX_;
  //float posY_;


};


#endif
