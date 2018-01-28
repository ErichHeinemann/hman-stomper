#ifndef _AUDIOFILTER_H
#define _AUDIOFILTER_H

#include <Arduino.h>
#include <stdint.h>

class AudioFilter
{
  private:
    int16_t filterLastInput[2];
    int16_t filterLastOutput[2];
    int16_t momentum[2];
    // Moogfilter
    int16_t prevOutL [4];
    int16_t prevOutR [4];
    int16_t outtmp;
    //  memset(prevOutL,0,sizeof(float)*4);
    //  memset(prevOutR,0,sizeof(float)*4);
    // variables
   float f;
   float p;
   float q;//filter coefficients
   int16_t b0;
   int16_t b1;
   int16_t b2;
   int16_t b3;
   int16_t b4; //filter buffers (beware denormals!)
   int16_t t1;
   int16_t t2; //temporary buffers

  public:
   AudioFilter();
    int16_t doLPF( int16_t input, uint8_t channel, uint8_t reso, uint8_t freq ) ;
    int16_t doHPF( int16_t input, uint8_t channel, uint8_t reso, uint8_t freq ) ;
    int16_t doMoog( int16_t in, int channel  );
    int16_t processMoog(   int16_t in , int channel );
    bool updateMoogPrevOut(int16_t out, int channel );
    bool setMoogCoefficients(float frequency, float resonance);
};
#endif