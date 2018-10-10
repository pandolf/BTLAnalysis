#ifndef BTLConf_h
#define BTLConf_h

#include <string>
#include "TFile.h"
#include "TPaveText.h"


class BTLConf {


 public:

  BTLConf(  int sensorConf, const std::string&  digiConf, float vBias=-1., float ninoThr=-1. );
  BTLConf(  std::string confName );
  BTLConf(  const BTLConf& rhs );
  ~BTLConf() {};

  int sensorConf()       const { return sensorConf_; };
  std::string digiConf() const { return digiConf_  ; };
  float vBias()          const { return vBias_     ; };
  float ninoThr()        const { return ninoThr_   ; };

  int digiConfNumber() const;
  std::string digiChannelSet() const;
  
  //float posX() const { return posX_; };
  //float posY() const { return posY_; };

  void set_sensorConf( int sensorConf               ) { sensorConf_ = sensorConf; };
  void set_digiConf  ( const std::string&  digiConf ) { digiConf_   = digiConf  ; };
  void set_vBias     ( float vBias                  ) { vBias_      = vBias     ; };
  void set_ninoThr   ( float ninoThr                ) { ninoThr_    = ninoThr   ; };
  
  std::string get_confName() const;
  std::string get_fileListName() const;
  float  get_otherBias( int i ) const;

  TFile* get_resoFile( const std::string& name="" ) const;

  TPaveText* get_labelConf( int quadrant=1 ) const;
  TPaveText* get_labelConf( float xMin, float yMin, float xMax, float yMax ) const;

 private:

  int sensorConf_;
  std::string digiConf_;
  float vBias_;
  float ninoThr_;
  
  //float posX_;
  //float posY_;


};


#endif
