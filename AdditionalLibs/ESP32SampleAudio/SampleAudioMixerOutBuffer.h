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
  
  Based on the great work from Earle F. Philhower, III

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

#ifndef _SAMPLEAUDIOMIXEROUTBUFFER_H
#define _SAMPLEAUDIOMIXEROUTBUFFER_H

#include "SampleAudioOutput.h"

class SampleAudioMixerOutBuffer : public SampleAudioOutput
{
  public:
    SampleAudioMixerOutBuffer(int bufferSizeSamples, SampleAudioOutput *dest );
    virtual ~SampleAudioMixerOutBuffer() override;
    virtual bool SetRate(int hz) override;
    virtual bool SetChannels(int channels) override;
    virtual bool SetBitsPerSample(int bits) override;
    virtual bool stop() override;
    virtual bool begin() override;
    
    virtual bool SetMixRate(int hz, int mixChannelNo=0);
    virtual bool SetMixBitsPerSample(int bits, int mixChannelNo=0);
    virtual bool SetMixChannels(int channels, int mixChannelNo=0 );

    virtual bool MixBegin( int mixChannelNo=0 );
    virtual bool MixStop( int mixChannelNo=0 );
    virtual bool MixConsumeSample(int16_t sample[2], int mixChannelNo=0 );
    // virtual bool MixConsumeMaster();
    
  inline int16_t LimitSample( int16_t s) {
    // this would be a good place to activate an PIN with an red Clipping- LED as used on many mixers...
    int32_t v = s; // (s * veloci)>>7;
    if (v < -32767) return -32767;
    else if (v > 32767) return 32767;
    else return (int16_t)( v );
  }
    
    
  protected:
    SampleAudioOutput *sink;
    int buffSize;
    int16_t leftMixSamples[16]; // 16 Mix-Cahnnels for Sounds, No: 0 - 15
    int16_t rightMixSamples[16];
	// int8_t mix_volumes[16]; first try without Volume-Control :)

    int16_t leftComuSample; 
    int16_t rightComuSample;
	
	int activeChannel[16];
	int updatedSample[16];
	
    // The Main-Outs to the I2C-DAC	or any other out
    int16_t *leftSample; 
    int16_t *rightSample;

    int writePtr;
    int readPtr;
    bool filled;
};

#endif