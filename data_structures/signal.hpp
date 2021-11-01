#ifndef BOOST_SIMULATION_MESSAGE_HPP
#define BOOST_SIMULATION_MESSAGE_HPP

#include <assert.h>
#include <iostream>
#include <string>

using namespace std;

/*******************************************/
/**************** Signal_t ****************/
/*******************************************/
struct Signal_t {
    Signal_t() {}

    Signal_t(float i_intensity)
            : intensity(i_intensity) {}

    float intensity;
};

istream& operator>> (istream& is, Signal_t& signal);

ostream& operator<<(ostream& os, const Signal_t& signal);


#endif // BOOST_SIMULATION_MESSAGE_HPP