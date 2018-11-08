#ifndef BTLCrystal_h
#define BTLCrystal_h


class BTLCrystal {

 public:

  BTLCrystal( float xLow=-99., float yLow=-99., float xHigh=-99., float yHigh=-99., float angle=0. );
  ~BTLCrystal() {};

  float xLow()  const { return xLow_;  };
  float xHigh() const { return xHigh_; };
  float yLow()  const { return yLow_;  };
  float yHigh() const { return yHigh_; };

  float xLowFiducial()  const;
  float xHighFiducial() const;
  float yLowFiducial()  const;
  float yHighFiducial() const;

  float angle() const { return angle_; };


  void set_xLow ( float xLow  )  { xLow_  = xLow  ; };
  void set_xHigh( float xHigh )  { xHigh_ = xHigh ; };
  void set_yLow ( float yLow  )  { yLow_  = yLow  ; };
  void set_yHigh( float yHigh )  { yHigh_ = yHigh ; };

  void set_angle( float angle )  { angle_ = angle; };


  //TBox* getBox() const;

 private:

  float xLow_;
  float xHigh_;
  float yLow_;
  float yHigh_;

  float angle_;

};


#endif
