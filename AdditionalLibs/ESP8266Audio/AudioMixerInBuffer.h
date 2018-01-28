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

#ifndef _AUDIOMIXERINBUFFER_H
#define _AUDIOMIXERINBUFFER_H

#include "AudioOutput.h"
#include "AudioMixerOutBuffer.h"

class AudioMixerInBuffer : public AudioOutput
{
  public:
    AudioMixerInBuffer(int bufferSizeSamples, AudioMixerOutBuffer *dest, int mixChannelNo=0 );
    virtual ~AudioMixerInBuffer() override;
    virtual bool SetRate(int hz) override;
    virtual bool SetBitsPerSample(int bits) override;
    virtual bool SetChannels(int channels) override;
    virtual bool begin() override;
    virtual bool ConsumeSample(int16_t sample[2]) override;
    virtual bool stop() override;
    
  protected:
    AudioMixerOutBuffer *sink;
    int buffSize;
    int16_t *leftSample;
    int16_t *rightSample;
    int writePtr;
    int readPtr;
    bool filled;
    int channelNo;
};

#endif