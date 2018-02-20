/*
  Implements a Reverb based on an Array with 1000 Counters as a buffer
  10 Reflections
*/


#include <stdint.h>
#include "SampleAudioReverb.h"
 
SampleAudioReverb::SampleAudioReverb() {
  buffersizeReverb = 1000;

  for (int i = 0; i < 1000; i++) {
    bufferReverbRight[i]=0;
    bufferReverbLeft[i]=0;
  }
  
  lastOutputReverb[0] = 0;
  lastOutputReverb[1] = 0;
  
  reflectionpos[0] = 700;
  reflectionpos[1] = 750;
  reflectionpos[2] = 776;
  reflectionpos[3] = 800;
  reflectionpos[4] = 829;
  reflectionpos[5] = 840;
  reflectionpos[6] = 852;
  reflectionpos[7] = 950;
  reflectionpos[8] = 963;
  reflectionpos[9] = 999;
  
  feedbackReverb = 20; // 
  effectbalanceReverb = 20; // 65; // 50 percent... 0==NoEffect, 127==OnlyEffect
  
  // LPF and HPF  
  /*            
    lastInput[0]=0;
    lastInput[1]=0;
    lastOutput[0]=0;
    lastOutput[1]=0;
	momentum[0]=0;
	momentum[1]=0;
    */            
}

int16_t SampleAudioReverb::doReverb( int16_t input, uint8_t channel){
  // insert current value into loop
  bufferPosReverb = bufferPosReverb +1;
  if ( bufferPosReverb>=buffersizeReverb) {
    bufferPosReverb = 0;
  }
    
  if ( channel==0 ) {
    bufferReverbLeft[bufferPosReverb]=( input*(127-feedbackReverb) + lastOutputReverb[channel]*(effectbalanceReverb))/127;

    // read all the reflections
    // uint16_t nextpos  = getNextPos( 0, bufferPos);
    momentumReverb[channel] = bufferReverbLeft[ getNextPos( 0) ] * (12/127)
      + bufferReverbLeft[  getNextPos(  1 ) ] * (10/127)
      + bufferReverbLeft[  getNextPos(  2 ) ] * (9/127)
      + bufferReverbLeft[  getNextPos(  3 ) ] * (7/127)
      + bufferReverbRight[ getNextPos( 4 ) ] * (6/127)
      + bufferReverbLeft[  getNextPos(  5 ) ] * (5/127)
      + bufferReverbRight[ getNextPos( 6 ) ] * (4/127)
      + bufferReverbLeft[  getNextPos(  7 ) ] * (6/127)
      + bufferReverbLeft[  getNextPos(  8 ) ] * (2/127)
      + bufferReverbLeft[  getNextPos(  9 ) ] * (6/127)    
    ; 
  }
  
  if ( channel==1 ) {
    bufferReverbRight[bufferPosReverb]=(input*(127-feedbackReverb) + lastOutputReverb[channel]*(effectbalanceReverb))/127;

    // read all the reflections
    // uint16_t nextpos  = getNextPos( 0, bufferPos);
    momentumReverb[channel] = bufferReverbRight[ getNextPos( 0) ] * (12/127)
      + bufferReverbRight[ getNextPos( 1 ) ] * (10/127)
      + bufferReverbRight[ getNextPos( 2 ) ] * (9/127)
      + bufferReverbLeft[  getNextPos(  3 ) ] * (7/127)
      + bufferReverbLeft[  getNextPos(  4 ) ] * (6/127)
      + bufferReverbLeft[  getNextPos(  5 ) ] * (5/127)
      + bufferReverbRight[ getNextPos( 6 ) ] * (5/127)
      + bufferReverbLeft[  getNextPos(  7 ) ] * (6/127)
      + bufferReverbRight[ getNextPos( 8 ) ] * (4/127)
      + bufferReverbLeft[  getNextPos(  9 ) ] * (5/127)    
    ;
  }
  
  lastOutputReverb[channel] =  (input*((127-effectbalanceReverb)) + momentumReverb[channel]*(effectbalanceReverb) )/127;
  return lastOutputReverb[channel];
}

bool SampleAudioReverb::setReverb ( uint16_t reverbtime, uint8_t balance, uint8_t reverbfeedback){
  if (balance <= 127) {
    effectbalanceReverb = balance;
  } else {
    effectbalanceReverb = 10;
  }
    if (reverbfeedback <= 127) {
    feedbackReverb = reverbfeedback;
  } else {
    feedbackReverb = 10;
  }
  return true;
}

/* 
uint16_t AudioReverb::update(uint16_t input, uint8_t channel, uint8_t reso, uint8_t freq) {
    uint16_t distanceToGo = input - lastOutput[channel];
    momentum[channel] += distanceToGo *  (freq/500); // Lower / higher number here will lower / raise the cutoff frequency
    return lastOutput[channel] += momentum[channel] + distanceToGo * (reso/500); // Higher number here (max 1) lessens resonance
}

uint16_t AudioReverb::doHPF(uint16_t input, uint8_t channel, uint8_t reso, uint8_t freq) {
    lastOutput[channel] += momentum[channel] - lastInput[channel] + input;
    lastInput[channel] = input;
    momentum[channel] = momentum[channel] * (float) (400/reso) - lastOutput[channel] * (float) (400/freq); // First number controls resonance; second controls cutoff frequency
    return lastOutput[channel];
}
*/ 
