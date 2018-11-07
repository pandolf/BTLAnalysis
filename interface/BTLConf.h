#ifndef BTLConf_h
#define BTLConf_h

#include <string>
#include "TFile.h"
#include "TPaveText.h"


class BTLConf {


 public:

  BTLConf(  int sensorConf=4, const std::string& digiChSet="a", float ninoThr=-1., float vBias=-1., int runNumber=-99 );
  //BTLConf(  int runNumber );
  BTLConf(  std::string confName );
  BTLConf(  const BTLConf& rhs );
  ~BTLConf() {};

  int sensorConf()        const { return sensorConf_; };
  std::string digiChSet() const { return digiChSet_ ; };
  float vBias()           const { return vBias_     ; };
  float ninoThr()         const { return ninoThr_   ; };
  int runNumber()         const { return runNumber_ ; };

  void set_sensorConf( int sensorConf               ) { sensorConf_ = sensorConf; };
  void set_digiChSet ( const std::string& digiChSet ) { digiChSet_  = digiChSet ; };
  void set_vBias     ( float vBias                  ) { vBias_      = vBias     ; };
  void set_ninoThr   ( float ninoThr                ) { ninoThr_    = ninoThr   ; };
  void set_runNumber ( float runNumber              ) { runNumber_  = runNumber ; };
  
  std::string get_confName() const;
  std::string get_fileListName() const;
  float  get_otherBias( int i ) const;

  TFile* get_resoFile( const std::string& suffix="" ) const;

  TPaveText* get_labelConf( int quadrant=1 ) const;
  TPaveText* get_labelConf( float xMin, float yMin, float xMax, float yMax ) const;

 private:

  int sensorConf_;
  std::string digiChSet_;
  float vBias_;
  float ninoThr_;

  //extra
  int runNumber_;

};


#endif
