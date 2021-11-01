#include <math.h> 
#include <assert.h>
#include <iostream>
#include <fstream>
#include <string>

#include "signal.hpp"

/***************************************************/
/************* Output stream ************************/
/***************************************************/

ostream& operator<<(ostream& os, const Signal_t& signal) {
  os << signal.intensity;
  return os;
}

/***************************************************/
/************* Input stream ************************/
/***************************************************/

istream& operator>> (istream& is, Signal_t& signal) {
  is >> signal.intensity;
  return is;
}
