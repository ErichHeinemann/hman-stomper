/*
  AudioMixerOutBuffer
  Adds additional bufferspace and a Mixer-Master-Out to the Mixer 
  
  Like in the real world with the electronic equipment from musicians or a DJ, 
  the DJ defines the Audio-Filesource, ... from SD-Card. .. 
  The File could be a WAV-File and should be played from WAV-Player (Generator)
  The Player is connected to an Input-Channel to the Mixer
  Internally the Input-Channel of the Mixer is connected to an Output-Cahnnel and finally into an Amplifier.
  
  Example-Flow could be:
  - multiple different AudioFileSourceSD
  - are mapped 1:1 to multiple AudioGeneratorWAV - Instances
  - each GeneratorWAV is dynamically or statically mapped to an AudioMixerInBuffer
  - All AudioMixerInBuffers are mapped to one main AudioMixerOutBuffer
  - AudioMixerOutBuffer is finally mapped to AudioOutputI2CDac
  
  AudioMixerInBuffer is less or more a very simple Mixer-Channel
  AudioMixerOutBuffer is comperable to the Mixer-Master-Channel
  
  Currently no Volume, Gain or EQ-Functions are implemented. It is only implemented to mix up to 16 samples in theory into one sample
  
  E.Heinemann 2018
  
  Based on the work from Earle F. Philhower, III

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <Arduino.h>
#include "AudioMixerOutBuffer.h"

// #define AudioDebug;

// build instance
AudioMixerOutBuffer::AudioMixerOutBuffer(int buffSizeSamples, AudioOutput *dest )
{
  buffSize = buffSizeSamples;
  leftSample  = (int16_t*)malloc(sizeof(int16_t) * buffSize);
  rightSample = (int16_t*)malloc(sizeof(int16_t) * buffSize);
  writePtr    = 0;
  readPtr     = 0;
  sink        = dest;
  
  for(int i=0; i<= 15; i++)
  {
    activeChannel[ i ] = 0;
    updatedSample[ i ] = 0; 
  }  
  
}

// destroy
AudioMixerOutBuffer::~AudioMixerOutBuffer()
{
  free(leftSample);
  free(rightSample);
}

bool AudioMixerOutBuffer::begin(){
  filled = false;
}

bool AudioMixerOutBuffer::SetBitsPerSample( int bits ) 
{
  return sink->SetBitsPerSample(bits ); // 16 Bit .. but should better be 24bit to get extra headroom for mixing multiple 16bit-samples
}

bool AudioMixerOutBuffer::SetChannels( int channels )
{
  return sink->SetChannels( channels ); // Stereo
}

bool AudioMixerOutBuffer::SetRate(int hz)
{ 
  return sink->SetRate( hz );
}

bool AudioMixerOutBuffer::stop()
{
  return sink->stop();
}

// Special Mixer Functions with mixChannelNo
// Most of them are not well implemented and only palceholders!

bool AudioMixerOutBuffer::MixBegin( int mixChannelNo )
{
  // register this active mixerchannel
  activeChannel[ mixChannelNo ] = 1;
  updatedSample[ mixChannelNo ] = 0;   
  return true;  
}

// should allways be 44100
bool AudioMixerOutBuffer::SetMixRate( int hz, int mixChannelNo ) 
{
	return true;
}

// should allways be 16
bool AudioMixerOutBuffer::SetMixBitsPerSample(int bits , int mixChannelNo) 
{
	return true;
}

// should allways be 2
bool AudioMixerOutBuffer:: SetMixChannels(int channels , int mixChannelNo)
{
	return true;
}



bool AudioMixerOutBuffer::MixConsumeSample(int16_t sample[2], int mixChannelNo )
{
  bool storeSampleFlag = false; // internal flag, true if we were able to store the current sample in the store
  bool vreturn = false; // calculated returnvalue
  
    // Wrong MixChannelNo?
  if ( mixChannelNo > 15 ) 
  {
	  return false;
  } 
  
  // is the buffer of this channel already updated then we don´t need that sample yet??
  if ( updatedSample[ mixChannelNo] == 0 )
  {
     leftComuSample  += sample[LEFTCHANNEL]; // equivalent of value = value + Mat_data_vektor[i];
     rightComuSample += sample[RIGHTCHANNEL]; // equivalent of value = value + Mat_data_vektor[i];
     storeSampleFlag = true;  
     vreturn = true;
     updatedSample[ mixChannelNo] = 1;
  }
  

  
  // First, try and fill I2S...
  if ( filled ) {
   #ifdef AudioDebug 
     Serial.println( "MOB filled" );
   #endif
  
    while ( readPtr != writePtr ) {
      int16_t s[2] = {leftSample[readPtr], rightSample[readPtr]};
      #ifdef AudioDebug 
        Serial.println( "MOB Sink ConsumeSample" );
      #endif
	  if ( !sink->ConsumeSample( s )) break; // Can't stuff any more in I2S...
	  readPtr = (readPtr + 1) % buffSize;  // produces a looping bufferpointer :)
    }
   }
   
   
   int checkval = 0; // it counts the mixerchannels which did not get an update
   for( int i=0; i<= 15; i++ )
   {
     if ( activeChannel[i] == 1 && updatedSample[ i ] ==0  ) 
     {
       checkval +=1;
	 } 
   }
   
   if ( checkval== 0 )
   {    
      #ifdef AudioDebug 
        Serial.println( "MOB Free Up" );
      #endif
     // get next writepointer	  
     int nextWritePtr = (writePtr + 1) % buffSize;
	  
     // if all is filled up, then we cannot store the sample
     if (nextWritePtr == readPtr ) {
       filled = true;
   	   // return vreturn; // this is the wrong answer ... but anyhow
     }
     
	 // calculate a new sample based on a mix of comulated values ...
     leftSample[writePtr]  = LimitSample(leftComuSample); // sample[LEFTCHANNEL];
     rightSample[writePtr] = LimitSample(rightComuSample); // sample[RIGHTCHANNEL];
     writePtr = nextWritePtr;
     
	 // Reset all Values in the buffers
	 for( int i=0; i<= 15; i++ )
	 {
		updatedSample[ i ] =0;
	  }
	  leftComuSample   = 0;
	  rightComuSample  = 0;
   }
   
   if ( storeSampleFlag == false && updatedSample[ mixChannelNo] == 0 )
   {
      leftComuSample  += sample[LEFTCHANNEL]; 
      rightComuSample += sample[RIGHTCHANNEL]; 
      storeSampleFlag = true;  
      vreturn = true;
      updatedSample[ mixChannelNo] == 1; 
   }
   return vreturn;

}

// Stop one channel
bool AudioMixerOutBuffer::MixStop( int mixChannelNo )
{
  activeChannel[ mixChannelNo ] = 0;
  updatedSample[ mixChannelNo ] = 0;   
  return true;
}