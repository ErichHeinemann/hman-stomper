/*
  AudioFileSourceBuffer
  Double-buffered input file using system RAM
  
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

#ifndef _SAMPLEAUDIOFILESOURCEBUFFER_H
#define _SAMPLEAUDIOFILESOURCEBUFFER_H

#include "SampleAudioFileSource.h"


class SampleAudioFileSourceBuffer : public SampleAudioFileSource
{
  public:
    SampleAudioFileSourceBuffer(SampleAudioFileSource *in, int bufferBytes);
    virtual ~SampleAudioFileSourceBuffer() override;
    
    virtual uint32_t read(void *data, uint32_t len) override;
    virtual bool seek(int32_t pos, int dir) override;
    virtual bool close() override;
    virtual bool isOpen() override;
    virtual uint32_t getSize() override;
    virtual uint32_t getPos() override;
    virtual bool loop() override;

  private:
    virtual void fill();

  private:
    SampleAudioFileSource *src;
    uint16_t buffSize;
    uint8_t *buffer;
    uint16_t writePtr;
    uint16_t readPtr;
    uint16_t length;
    bool filled;
};


#endif

