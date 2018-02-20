/*
  AudioGeneratorWAV
  Audio output generator that reads 8 and 16-bit WAV files
    
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

#ifndef _SAMPLEAUDIOGENERATORSAMPLE_H
#define _SAMPLEAUDIOGENERATORSAMPLE_H

#include "SampleAudioGenerator.h"
#include "SampleAudioMidiFrequencies.h"
#include "SampleAudioFilter.h"
#include "SampleAudioReverb.h"

class SampleAudioGeneratorSAMPLE : SampleAudioGenerator
{
  public:
    SampleAudioGeneratorSAMPLE();
    virtual ~SampleAudioGeneratorSAMPLE() override;
    virtual bool begin( SampleAudioFileSource *source, SampleAudioOutput *output, uint8_t velocity, uint8_t pitch ) override;
    virtual bool loop() override;
    virtual bool stop() override;
    virtual bool isRunning() override;
    // virtual bool setLP( uint8_t lptone );
    virtual bool setLPFilter( uint8_t freq, uint8_t reso );
    virtual bool setHPFilter( uint8_t freq, uint8_t reso );
    virtual bool setReverb( );
    void SetBufferSize(int sz) { buffSize = sz; }

  private:
    bool ReadU32(uint32_t *dest) { return file->read(reinterpret_cast<uint8_t*>(dest), 4); }
    bool ReadU16(uint16_t *dest) { return file->read(reinterpret_cast<uint8_t*>(dest), 2); }
    bool ReadU8(uint8_t   *dest) { return file->read(reinterpret_cast<uint8_t*>(dest), 1); }
    bool GetBufferedData(int bytes, void *dest);
    bool ReadWAVInfo();
    
    bool activeLPFilter;
    bool activeHPFilter;
    bool activeReverb;
    
    uint8_t myFreq; //Frequency, unclassified Value from 0 .. 127
    uint8_t myReso; //Resonance, unclassified value from 0 .. 127  
    
    SampleAudioFilter *filter;
    SampleAudioReverb *reverb;
    

  inline int16_t AmplifyByVelocity( int16_t s) {
    int32_t v = (s * veloci)>>7;
    if (v < -32767) return -32767;
    else if (v > 32767) return 32767;
    else return (int16_t)(v&0xffff);
  }
    
    
  protected:
    // WAV info
    uint16_t channels;
    uint32_t sampleRate;
    uint16_t bitsPerSample;
    uint8_t  veloci;
    uint8_t  pit;
    uint32_t playedSamples;
    uint32_t realsamples;
    uint32_t fillSamples;
        
    // We need to buffer some data in-RAM to avoid doing 1000s of small reads
    uint32_t buffSize;
    uint8_t *buff;
    uint16_t buffPtr;
    uint16_t buffLen;
    // uint16_t smoothvalue; //  = 0;
    // uint16_t rawvalue; //  = 127;
    
    int16_t lastSampleLP ;
};

#endif

