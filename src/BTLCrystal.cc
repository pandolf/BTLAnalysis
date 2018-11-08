#include "../interface/BTLCrystal.h"



BTLCrystal::BTLCrystal( float xLow, float yLow, float xHigh, float yHigh, float angle ) {

  xLow_  = xLow;
  xHigh_ = xHigh;
  yLow_  = yLow;
  yHigh_ = yHigh;
  angle_ = angle;

}


float BTLCrystal::xLowFiducial() const {

  return xLow_+5.;

}

float BTLCrystal::xHighFiducial() const {

  return xHigh_-5.;

}

float BTLCrystal::yLowFiducial() const {

  return yLow_+1.;

}

float BTLCrystal::yHighFiducial() const {

  return yHigh_-1.;

}
