/*


float withoutTreble[2];
float bassOnly[2];
 
float AudioDoShelvingEQ(float input, uint8_t channel, uint8_t treblefreq, uint8_t bassfreq ) {
 
    // Treble calculations
    float distanceToGo = input - withoutTreble[channel];
    withoutTreble[2] += distanceToGo * (float)(400/treblefreq); // Number controls treble frequency
    float trebleOnly = input - withoutTreble[channel];
 
    // Bass calculations
    distanceToGo = withoutTreble[channel] - bassOnly[channel];
    bassOnly[channel] += distanceToGo * (float)(400/bassfreq); // Number controls bass frequency
 
    return withoutTreble[channel] + trebleOnly * 1 + bassOnly[channel] * 0;
    // The "1" controls treble. 0 = none; 1 = untouched; 2 = +6db
    // The "0" controls bass. -1 = none; 0 = untouched; 1 = +6db
}

 
*/ 
 
 
// C-File
#include <stdint.h>
#include "AudioFilter.h"
 
AudioFilter::AudioFilter() {

  // Moogfilter
  prevOutL [0]=0;
  prevOutL [1]=0;
  prevOutL [2]=0;
  prevOutL [3]=0;
  prevOutR [0]=0;
  prevOutR [1]=0;
  prevOutR [2]=0;
  prevOutR [3]=0;
  //  memset(prevOutL,0,sizeof(float)*4);
  //  memset(prevOutR,0,sizeof(float)*4);
  // variables
  f=0;//filter coefficients
  p=0;
  q=0;
  b0=0;//filter buffers (beware denormals!)
  b1=0;
  b2=0;
  b3=0;
  b4=0;
  t1=0; //temporary buffers
  t2=0;


  // Basic-Filter
 

  // LPF and HPF    
  filterLastInput[0]=0;
  filterLastInput[1]=0;
  filterLastOutput[0]=0;
  filterLastOutput[1]=0;
  momentum[0]=0;
  momentum[1]=0;
  	
            
}





int16_t AudioFilter::doLPF(int16_t input, uint8_t channel, uint8_t reso, uint8_t freq) {
    int16_t distanceToGo = input - ( filterLastOutput[channel] );
    momentum[channel] += distanceToGo * 0.125;//  (float) (freq/127); // Lower / higher number here will lower / raise the cutoff frequency
    filterLastOutput[channel] += (momentum[channel]) + distanceToGo * 0.125; // (float)(reso/127); // Higher number here (max 1) lessens resonance

    if (filterLastOutput[channel] < -32767){ filterLastOutput[channel]= -32767; }
    else if (filterLastOutput[channel] > 32767) { filterLastOutput[channel]= 32767; }   
    
    return filterLastOutput[channel];
}



int16_t AudioFilter::doHPF(int16_t input, uint8_t channel, uint8_t reso, uint8_t freq) {
    filterLastOutput[channel] += momentum[channel] - filterLastInput[channel] + input;
    filterLastInput[channel] = input;
    momentum[channel] = momentum[channel] * (float)(reso/127) - filterLastOutput[channel] * (float)(freq/127); // First number controls resonance; second controls cutoff frequency

    if (filterLastOutput[channel] < -32767){ filterLastOutput[channel]= -32767; }
    else if (filterLastOutput[channel] > 32767) { filterLastOutput[channel]= 32767; }   


    return filterLastOutput[channel];
}

/*


MoogFilter::MoogFilter(void) {
  int16_t prevOutL[4];
  int16_t prevOutR[4];
 
  // memset(prevOutL,0,sizeof(float)*4);
  // memset(prevOutR,0,sizeof(float)*4);
 
  // variables
  float f, p, q;             //filter coefficients
  float b0, b1, b2, b3, b4;  //filter buffers (beware denormals!)
  float t1, t2;              //temporary buffers
}
 
MoogFilter::~MoogFilter(void) {
   delete(prevOutL);
   delete(prevOutR);
}
*/



bool AudioFilter::setMoogCoefficients(float frequency, float resonance) {
 q = 1.0f - frequency;
 p = frequency + 0.8f * frequency * q;
 f = p + p - 1.0f;
 q = resonance * (1.0f + 0.5f * q * (1.0f - q + 5.6f * q * q));
 return true;
}

// MoogFilter
 bool AudioFilter::updateMoogPrevOut(int16_t out, int channel) {
  if (channel==0) {
    prevOutL[3] = prevOutL[2];
    prevOutL[2] = prevOutL[1];
    prevOutL[1] = prevOutL[0];
    prevOutL[0] = out;
    } else {
    prevOutR[3] = prevOutR[2];
    prevOutR[2] = prevOutR[1];
    prevOutR[1] = prevOutR[0];
    prevOutR[0] = out;
  } 
}

int16_t AudioFilter::processMoog(int16_t in ,int channel) {
  in -= q * b4;//feedback
  t1 = b1;
  b1 = (in + b0) * p - b1 * f;
  t2 = b2;
  b2 = (b1 + t1) * p - b2 * f;
  t1 = b3;
  b3 = (b2 + t2) * p - b3 * f;
  b4 = (b3 + t1) * p - b4 * f;
  b4 = b4 - b4 * b4 * b4 * 0.166667f;//clipping
  b0 = in;
  // ????
  // Lowpass  output:  b4
  // Highpass output:  in - b4;
  // Bandpass output:  3.0f * (b3 - b4);
  switch(channel) {
    case 0:
    return(int16_t) (b0 + (b1 * prevOutL[0]) + (b2 * prevOutL[1]) + (b3 * prevOutL[2]) + (b4 * prevOutL[3]));
      break;
    case 1:
      return(int16_t) (b0 + (b1 * prevOutR[0]) + (b2 * prevOutR[1]) + (b3 * prevOutR[2]) + (b4 * prevOutR[3]));
      break; 
      }
    return 0;
    }

int16_t AudioFilter::doMoog( int16_t in, int channel  ){
  outtmp = processMoog(in ,channel);
  updateMoogPrevOut(outtmp,channel);
  return outtmp;
}




