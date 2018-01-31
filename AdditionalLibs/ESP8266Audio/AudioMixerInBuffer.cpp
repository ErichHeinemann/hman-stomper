/*
AudioMixerInBuffer

  Adds additional bufferspace and a Mixer-Input-Channelstrip to the Mixer 
  
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
#include "AudioMixerInBuffer.h"
#include "AudioMixerOutBuffer.h"

// #define AudioDebug;

AudioMixerInBuffer::AudioMixerInBuffer(int buffSizeSamples, AudioMixerOutBuffer *dest, int mixChannelNo )
{
  buffSize    = buffSizeSamples;
  leftSample  = (int16_t*)malloc(sizeof(int16_t) * buffSize);
  rightSample = (int16_t*)malloc(sizeof(int16_t) * buffSize);
  writePtr    = 0;
  readPtr     = 0;
  mychannels  = 2; // Mono
  sink        = dest;
  channelNo   = mixChannelNo; // ChannelNo should be between 0 and 15 ... 16 channels for testing!
}

AudioMixerInBuffer::~AudioMixerInBuffer()
{
  free(leftSample);
  free(rightSample);
  
}

bool AudioMixerInBuffer::SetRate(int hz)
{
  return sink->SetMixRate( hz, channelNo ); // 44.100
}

bool AudioMixerInBuffer::SetBitsPerSample(int bits) {
  return sink->SetMixBitsPerSample( bits , channelNo); // 16 Bit
}

bool AudioMixerInBuffer::SetChannels(int channels){
  mychannels = channels;
  return sink->SetMixChannels(channels , channelNo); // Stereo or Mono
}

bool AudioMixerInBuffer::begin(){
  filled = false;
  return sink->MixBegin( channelNo );
}



bool AudioMixerInBuffer::ConsumeSample(int16_t sample[2]){
  // `!!!!!! In this Class, we could only forward the Sample to the Sink of the type AudioMixerOutBuffer!!!!!
  if (filled) {
    while (readPtr != writePtr) {
      int16_t s[2] = {leftSample[readPtr], rightSample[readPtr]};
      if (!sink->MixConsumeSample( s, channelNo )) break; // Can't stuff any more in I2S...
      readPtr = (readPtr + 1) % buffSize;
    }
  }
  
 #ifdef AudioDebug 
   Serial.println( channelNo );
 #endif 
  
  // Now, do we have space for a new sample?
  int nextWritePtr = (writePtr + 1) % buffSize;
  if (nextWritePtr == readPtr) {
    filled = true;
    return false;
  }
  leftSample[writePtr]  = sample[LEFTCHANNEL];
  rightSample[writePtr] = sample[RIGHTCHANNEL];
  writePtr = nextWritePtr;
  return true;
}

bool AudioMixerInBuffer::stop()
{
  return sink->MixStop( channelNo );
}