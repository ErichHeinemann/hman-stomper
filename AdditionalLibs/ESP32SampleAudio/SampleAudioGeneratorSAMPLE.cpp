/*
  AudioGeneratorSAMPLE
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


#include "SAMPLEAudioGeneratorSAMPLE.h"
// #include "SampleAudioFilter.h"
// #include "AudioReverb.h"
// #include "SampleAudioMidiFrequencies.h"

SampleAudioGeneratorSAMPLE::SampleAudioGeneratorSAMPLE()
{
 // defaults
  running  = false;
  file     = NULL;
  output   = NULL;
  buffSize = 256; // 128
  buff     = NULL;
  buffPtr  = 0; // Pointer
  buffLen  = 0; // Length

  veloci   = 0;
  pit      = 65; // Mid of the range
  playedSamples = 0;
  fillSamples   = 0;
  
  //smoothvalue = 0;
  // rawvalue = 127;
  
  activeLPFilter = false;
  activeHPFilter = false;
  activeReverb = true;
  playtype = 0; // oneshot
  
  myReso = 65; // Mid of Range 0-127
  myFreq = 65; // Mid of Range 0-127
  
}

SampleAudioGeneratorSAMPLE::~SampleAudioGeneratorSAMPLE()
{
  free(buff);
  buff = NULL;
}

bool SampleAudioGeneratorSAMPLE::stop()
{
  if (!running) return true;
  running       = false;
  free(buff);
  buff          = NULL;
  playedSamples = 0;
  fillSamples   = 0;
  return file->close();
}

bool SampleAudioGeneratorSAMPLE::isRunning()
{
  return running;
}


// Handle buffered reading, reload each time we run out of data
// The Buffer does not know anything about the format (Stereo, 8/16 Bit etc.)
bool SampleAudioGeneratorSAMPLE::GetBufferedData(int bytes, void *dest)
{
  if (!running) return false; // Nothing to do here!
  uint8_t *p = reinterpret_cast<uint8_t*>(dest);
  while (bytes--) {
    // Potentially load next batch of data...
    if (buffPtr >= buffLen) {
      buffPtr = 0;
      uint32_t toRead = availBytes > buffSize ? buffSize : availBytes;
      buffLen = file->read( buff, buffSize );
      availBytes -= buffLen;
    }
    if (buffPtr >= buffLen)
      return false; // No data left!
    *(p++) = buff[buffPtr++];
     //  Serial.println(  buffPtr );
  }
  
  // Debugging
  //   yield();
  //   char *str = (char *) (intptr_t) buffPtr;
  //   char* str2 = (char*) &buffPtr;  
  //   String str="TEST";
  //   Serial.println( str2 );
  //   Serial.write(buffPtr);
  //   Serial.println();
  return true;
}

bool SampleAudioGeneratorSAMPLE::loop()
{
  /*
  if ( activeLPFilter == true ) {
    filter = new AudioFilter(); 
  }
  if ( activeLPFilter == true ) {
    filter = new AudioFilter(); 
  }
  
  if (activeReverb == true){
    reverb = new AudioReverb();
  }
  */
  
  if (!running) goto done; // Nothing to do here!
    
  if (!output->ConsumeSample(lastSample)) goto done; // Can't send, but no error detected

  // Try and stuff the buffer one sample at a time
  do
  {
    int v=0;
    
    if (bitsPerSample == 8) {
      uint8_t l, r;
      if (!GetBufferedData(1, &l)) stop();
      if (channels == 2) {
        if (!GetBufferedData(1, &r)) stop();
      } else {
        r = 0;
      }
      lastSample[SampleAudioOutput::LEFTCHANNEL]  = l;
      lastSample[SampleAudioOutput::RIGHTCHANNEL] = r;
    } else if (bitsPerSample == 16) {
        if (!GetBufferedData(2, &lastSample[SampleAudioOutput::LEFTCHANNEL])) {
          // reset for looping
          if ( playtype == 1) {
            file->seek( samplestartpos, SEEK_SET );
            yield();
            if (!GetBufferedData(2, &lastSample[SampleAudioOutput::LEFTCHANNEL])) {
              Serial.println("No New DATA Found !!");
            }
           } else { 
            stop(); 
           } 
        }
        
        
        // Pitch
        // http://rcarduino.blogspot.de/2013/02/arduino-notes-frequencies-and-scales.html
        // http://subsynth.sourceforge.net/midinote2freq.html
        // 60 = C5
        v = 0;     
        // Pitch higher than basenote   
        if (pit > 60) {
          playedSamples += 1;
          realsamples = playedSamples  * midifrequencies[pit-36] / 44100;

          // Pitch Left get the next 2 Values          
          v = realsamples - playedSamples - fillSamples;
          
          // Pitch
          if ( v >= 1  ) {
             // Serial.println(  " fill" );
             // Serial.print ("m:");
             // Serial.println( midifrequencies[pit-36] );
             lastSampleBuffer[ SampleAudioOutput::LEFTCHANNEL ] = lastSample[SampleAudioOutput::LEFTCHANNEL ];
             fillSamples += 1;
             if (!GetBufferedData(2, &lastSample[ SampleAudioOutput::LEFTCHANNEL])) {
               stop(); 
             }
             lastSample[ SampleAudioOutput::LEFTCHANNEL ] = ( lastSampleBuffer[ SampleAudioOutput::LEFTCHANNEL ] + lastSample[ SampleAudioOutput::LEFTCHANNEL ] ) / 2 ;
          }          
        } // end pitch up
        
        // Strech or Pitch down
        // Pitch under basenote
        if ( pit < 60) {
          // Stretch
          playedSamples += 1;
          realsamples = playedSamples  * midifrequencies[pit-36] / 44100;
          v =  playedSamples - realsamples - fillSamples;
          // man könnte den das letzte Sample sichern und den Durchschnittswert zwischen letztem und aktuellem Sample berechnen und ausgeben...        
        }
        
        
        if (channels == 2) {
          if (!GetBufferedData(2, &lastSample[ SampleAudioOutput::RIGHTCHANNEL ])) {
            stop(); 
          } 
        } else {
           // then we create a stereo sample
          lastSample[ SampleAudioOutput::RIGHTCHANNEL ] = lastSample[ SampleAudioOutput::LEFTCHANNEL ]; // 0; MONO
        }
        
        // PROCESS PITCH UP
        if (pit > 60) {
            // Pitch Right
            if ( v >= 1  ) {
              lastSampleBuffer[ SampleAudioOutput::RIGHTCHANNEL ] = lastSample[ SampleAudioOutput::RIGHTCHANNEL ];
              if (!GetBufferedData(2, &lastSample[ SampleAudioOutput::RIGHTCHANNEL ])) {
                stop(); 
              }
              lastSample[ SampleAudioOutput::RIGHTCHANNEL ] = ( lastSampleBuffer[ SampleAudioOutput::RIGHTCHANNEL ] + lastSample[ SampleAudioOutput::RIGHTCHANNEL ] ) / 2;
            }  
        } // end pitch 
        

    }


  // Filterbank
  
  if ( activeLPFilter == true && 1==2 ) {
    // int16_t l1 = lastSample[AudioOutput::LEFTCHANNEL];
    // int16_t r1 = lastSample[AudioOutput::RIGHTCHANNEL];
    lastSample[SampleAudioOutput::LEFTCHANNEL]  =  filter->doLPF( lastSample[SampleAudioOutput::LEFTCHANNEL], SampleAudioOutput::LEFTCHANNEL, myReso,  myFreq);
    lastSample[SampleAudioOutput::RIGHTCHANNEL] =  filter->doLPF( lastSample[SampleAudioOutput::RIGHTCHANNEL], SampleAudioOutput::RIGHTCHANNEL, myReso, myFreq);
  }
  
 
  if ( activeHPFilter == true && 1==2 ) {
    lastSample[SampleAudioOutput::LEFTCHANNEL] =  filter->doHPF( lastSample[SampleAudioOutput::LEFTCHANNEL], SampleAudioOutput::LEFTCHANNEL, myReso,myFreq);
    lastSample[SampleAudioOutput::RIGHTCHANNEL] =  filter->doHPF( lastSample[SampleAudioOutput::RIGHTCHANNEL], SampleAudioOutput::RIGHTCHANNEL, myReso,myFreq);
  }
   
  
  // Reverb
  if ( activeReverb == true && 1==2) {
    lastSample[SampleAudioOutput::LEFTCHANNEL]  =  reverb->doReverb( lastSample[SampleAudioOutput::LEFTCHANNEL], SampleAudioOutput::LEFTCHANNEL);
    lastSample[SampleAudioOutput::RIGHTCHANNEL] =  reverb->doReverb( lastSample[SampleAudioOutput::RIGHTCHANNEL], SampleAudioOutput::RIGHTCHANNEL);
  }
  
  // Velocity - Control  
  lastSample[SampleAudioOutput::LEFTCHANNEL]  =  AmplifyByVelocity( lastSample[SampleAudioOutput::LEFTCHANNEL] );
  lastSample[SampleAudioOutput::RIGHTCHANNEL] =  AmplifyByVelocity( lastSample[SampleAudioOutput::RIGHTCHANNEL] );  
    
    
   // PROCESS STRETCH OR PITCH DOWN
   if (pit < 60) {
     // Stretch right
     // Consume the lastSample twice
     if (v>=1) {
       fillSamples +=1;
       output->ConsumeSample(lastSample); // Can't send, but no error detected
     }
   }
    
        
} while (running && output->ConsumeSample(lastSample));

done:
  file->loop();
  output->loop();

  return running;
}

/*
bool AudioGeneratorSAMPLE::setLP( uint8_t lptone ){
 if (lptone <127){
   smoothvalue = lptone;
   rawvalue = 127-lptone;
 } else {
    smoothvalue = 0;
    rawvalue = 127;
 }
 return true;
 }
 */

bool SampleAudioGeneratorSAMPLE::disableFilter(){
  activeLPFilter = false;
  activeHPFilter = false;
  return true;
} 
 
bool SampleAudioGeneratorSAMPLE::setLPFilter( uint8_t freq, uint8_t reso ){
  activeLPFilter = true;
  activeHPFilter = false;
  myFreq = 65;
  myReso = 65;
  if ( freq <= 127 ) {
    myFreq = freq; 
  } 
  if (reso <= 127 ) {
    myReso = reso;
  } 
  return true; 
}

bool SampleAudioGeneratorSAMPLE::setHPFilter( uint8_t freq, uint8_t reso ){
  activeLPFilter = false;
  activeHPFilter = true;
  myFreq = 65;
  myReso = 65;
  if ( freq <= 127 ) {
    myFreq = freq; 
  } 
  if (reso <= 127 ) {
    myReso = reso;
  } 
  return true; 
} 


bool SampleAudioGeneratorSAMPLE::setReverb( ){
  activeReverb = true;
  return true;
}  


bool SampleAudioGeneratorSAMPLE::setSamplePlaytype( uint8_t sampleplaytype) {
  playtype = sampleplaytype;
}
    

bool SampleAudioGeneratorSAMPLE::ReadWAVInfo()
{
  uint32_t u32;
  uint16_t u16;

  // Header == "RIFF"
  if (!ReadU32(&u32)) return false;
  if (u32 != 0x46464952) return false; // "RIFF"
  // Skip ChunkSize
  if (!ReadU32(&u32)) return false; 
  // Format == "WAVE"
  if (!ReadU32(&u32)) return false;
  if (u32 != 0x45564157) return false; // "WAVE"

  // fmt subchunk has the info on the data format
  // id == "fmt "
  if (!ReadU32(&u32)) return false;
  if (u32 != 0x20746d66) return false; // "fmt "
  // subchunk size
  if (!ReadU32(&u32)) return false;
  if (u32 != 16) return false; // we only do standard PCM
  // AudioFormat
  if (!ReadU16(&u16)) return false;
  if (u16 != 1) return false; // we only do standard PCM
  // NumChannels
  if (!ReadU16(&channels)) return false;
  if ((channels<1) || (channels>2)) return false; // Mono or stereo support only
  // SampleRate
  if (!ReadU32(&sampleRate)) return false;
  if (sampleRate < 1) return false; // Weird rate, punt.  Will need to check w/DAC to see if supported
  // Ignore byterate and blockalign
  if (!ReadU32(&u32)) return false;
  if (!ReadU16(&u16)) return false;
  // Bits per sample
  if (!ReadU16(&bitsPerSample)) return false;
  if ((bitsPerSample!=8) && (bitsPerSample != 16)) return false; // Only 8 or 16 bits

  // look for data subchunk
  do {
    if (!ReadU32(&u32)) return false;
    /*
    // == sampl
    if (u32 == 0x736D706C) {
      if (!ReadU32(&chunkdatasize)) return false;
      if (!ReadU32(&manufacturer)) return false;
      if (!ReadU32(&product)) return false;
      if (!ReadU32(&sampleperiod)) return false;
      if (!ReadU32(&midiunitynote)) return false;
      if (!ReadU32(&midipitchfraction)) return false;
      if (!ReadU32(&smtpeformat)) return false;
      if (!ReadU32(&smtpeoffset)) return false;
      if (!ReadU32(&numsampleloops)) return false;
      if (!ReadU32(&samplerdata)) return false;
      
      if (samplerdata == 0x0 ){
		  if (!ReadU32(&loopcuepointid)) return false;
		  if (!ReadU32(&looptype)) return false;  
		  if (!ReadU32(&loopstart)) return false;
		  if (!ReadU32(&loopend)) return false; 
		  if (!ReadU32(&loopfraction)) return false; 
		  if (!ReadU32(&looopplaycount)) return false;   
      }
    } 
    */
    //  == "data"
    
    if (u32 == 0x61746164) break; // "data"
    // Skip size, read until end of chunk
    if (!ReadU32(&u32)) return false;

    
    file->seek(u32, SEEK_CUR);
    
  } while (1);
  samplestartpos = file->getPos();
  
  // Serial.print ("Position:");
  // Serial.println( file->getPos() );
    
      
  if (!file->isOpen()) return false;

  // Skip size, read until end of file...
  if (!ReadU32(&u32)) return false;
  availBytes = u32;
      
  // Now set up the buffer or fail
  buff = reinterpret_cast<uint8_t *>(malloc(buffSize));
  if (!buff) return false;
  buffPtr = 0;
  buffLen = 0;

  return true;
}

/*bool SampleAudioGeneratorSAMPLE::begin2(SampleAudioFileSourceSD *source, SampleAudioOutput output, uint8_t velocity, uint8_t pitch ){
  return ( SampleAudioGeneratorSAMPLE::begin (source, output, velocity, pitch ) );
}
*/

bool SampleAudioGeneratorSAMPLE::begin(SampleAudioFileSource *source, SampleAudioOutput *output, uint8_t velocity, uint8_t pitch )
{
  fillSamples =0;
  playedSamples =0;
  
  if ( velocity >=0 && velocity <=127) {
    veloci = velocity;
  }
  if ( pitch >=36 && pitch <=84 ) {
    pit = pitch;
  }  
  
  
  if ( activeLPFilter == true ) {
    filter = new SampleAudioFilter(); 
  }
  
  /*
  if ( activeLPFilter == true ) {
    filter = new AudioFilter(); 
  }
  
  if (activeReverb == true){
    reverb = new AudioReverb();
  }
  */
  
  
  if (!source) return false;
  file = source;
  if (!output) return false;
  this->output = output;
  if (!file->isOpen()) return false; // Error
  
  if (!ReadWAVInfo()) return false;

  if (!output->SetRate( sampleRate )) return false;
  if (!output->SetBitsPerSample( bitsPerSample )) return false;
  if (!output->SetChannels( channels )) return false;
  if (!output->begin()) return false;

  running = true;
  
  return true;
}

