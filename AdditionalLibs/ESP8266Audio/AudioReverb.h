#ifndef _AUDIOREVERB_H
#define _AUDIOREVERB_H

#include <Arduino.h>
#include <stdint.h>

class AudioReverb
{
  // public:
  //  uint16_t G, a1, a2, b0, b1, b2;
    
  private:
  //  uint16_t u[3], y[3];
    int16_t  bufferReverbRight[1000];
    int16_t  bufferReverbLeft[1000];
    uint16_t bufferPosReverb; // Write-Position in the buffer

    int16_t lastInputReverb[2];
    int16_t lastOutputReverb[2];
    int16_t momentumReverb[2];
    
    uint16_t reflectionpos[10];
    uint8_t  effectbalanceReverb; 
    uint8_t  feedbackReverb;
    uint16_t timeReverb; // time in ms
    uint16_t buffersizeReverb; // something between 1 and 1000
 
 // from Milliseconds to Buffersie
 inline int16_t getBuffersizeFromTime( int16_t ms) {
    int16_t v = ms * 44100/1000;
    if (v < 1) return 1;
    else if (v >= 1000) return 1000;
    else return v;
  }
  
 inline int16_t getNextPos( uint8_t thisReflectionPos ){
   uint16_t thisPos = reflectionpos[thisReflectionPos] +bufferPosReverb;
   if ( thisPos < buffersizeReverb ) {
     return thisPos ;
   } else {
     return ( thisPos - buffersizeReverb); 
   }
 }
 
 public:
   AudioReverb();
     // uint16_t update(uint16_t u0);
     int16_t doReverb(int16_t input, uint8_t channel ) ;
     // uint16_t doHPF(uint16_t input, uint8_t channel, uint8_t reso, uint8_t freq) ;
     bool setReverb ( uint16_t reverbtime, uint8_t balance, uint8_t reverbfeedback);
 
};

#endif