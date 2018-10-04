#ifndef BTLConf_h
#define BTLConf_h

#include <string>


class BTLConf {


 public:

  BTLConf(  std::string confName );
  ~BTLConf() {};

  int sensorConf()   const { return sensorConf_; };
  int digiConf()     const { return digiConf_  ; };
  float vBias()      const { return vBias_     ; };
  float ninoThresh() const { return ninoThresh_; };
  
  //float posX() const { return posX_; };
  //float posY() const { return posY_; };

  void set_sensorConf( int sensorConf   ) { sensorConf_ = sensorConf; };
  void set_digiConf  ( int digiConf     ) { digiConf_   = digiConf  ; };
  void set_vBias     ( float vBias      ) { vBias_      = vBias     ; };
  void set_ninoThresh( float ninoThresh ) { ninoThresh_ = ninoThresh; };
  
  std::string get_confName() const;


 private:

  int sensorConf_;
  int digiConf_;
  float vBias_;
  float ninoThresh_;
  
  //float posX_;
  //float posY_;


};


#endif
