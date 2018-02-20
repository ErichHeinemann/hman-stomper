/*
  SampleGenerator
  Base class of an audio output generator
  
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

#ifndef _SAMPLEAUDIOGENERATOR_H
#define _SAMPLEAUDIOGENERATOR_H

#include <Arduino.h>
#include "SampleAudioFileSource.h"
#include "SampleAudioOutput.h"

class SampleAudioGenerator
{
  public:
    SampleAudioGenerator() { lastSample[0] = 0; lastSample[1] = 0; };
    virtual ~SampleAudioGenerator() {};
    virtual bool begin(SampleAudioFileSource *source, SampleAudioOutput *output) { (void)source; (void)output; return false; };
    virtual bool begin(SampleAudioFileSource *source, SampleAudioOutput *output, uint8_t velocity, uint8_t pitch) { (void)source; (void)output; return false; };
    virtual bool loop() { return false; };
    virtual bool stop() { return false; };
    virtual bool isRunning() { return false;};

  protected:
    bool running;
    SampleAudioFileSource *file;
    SampleAudioOutput *output;
    int16_t lastSample[2];
    int16_t lastSampleBuffer[2]; // for Pitch-Shifting
};


#endif

