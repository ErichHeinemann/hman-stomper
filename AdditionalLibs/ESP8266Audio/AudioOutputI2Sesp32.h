/*
  This is derived from an Class  AudioOutputI2SDAC created by Earle F. Philhower, III.
  
  He has released a new version which does not work as well as this older modified version does.
  Thats why i transformed his older version into this special version - specially for the esp32.
  
  E.Heinemann
  

  Audio player for an I2S connected DAC, 16bps
  
  Copyright (C) 2017  Earle F. Philhower, III

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

#ifndef _AUDIOOUTPUTI2SESP32_H
#define _AUDIOOUTPUTI2SESP32_H

#include "AudioOutput.h"

class AudioOutputI2Sesp32 : public AudioOutput
{
  public:
    AudioOutputI2Sesp32();
    virtual ~AudioOutputI2Sesp32() override;
    virtual bool SetRate(int hz) override;
    virtual bool SetBitsPerSample(int bits) override;
    virtual bool SetChannels(int channels) override;
    virtual bool begin() override;
    virtual bool ConsumeSample(int16_t sample[2]) override;
    virtual bool stop() override; // stopps
    // virtual bool mute() override; // mutes only the output .. no new begin needed...
    
    bool SetOutputModeMono(bool mono);  // Force mono output no matter the input
    
  protected:
    bool mono;
    static bool i2sOn; // One per machine, not per instance...
};

#endif

