/* Notes:
 *  This INO sketch was created to test the classes for playing a WAV-Sample, 
 *  Mix different samples into one main-output and hand it over to an
 *  I2S-DAC.
 *  Finally after playing some samples in an unspecified loop the system 
 *  should stop playing new samples and should switch off the output without to much noise.
 *  I have tested to play 4 sounds out of progmem at the same time without any bigger issues.
 *  
 *  The purpose is to use this to create Your own homegrown music-sampleplayer, loop-playback, 
 *  e-drum, e-percussion, sound-art-installation or 
 *  an midi-instrument.
 *  
 *  I wanted to get the library to the point to play 4 sounds indipendently like a drummer could do.
 *  the sounds were sampled from an softsynth by myself. I used only 2 sounds, one short and one with a longer duration 
 *  to be able to test overlapping durations.
 *  
 *  Erich Heinemann
 */


#include <Arduino.h>

#ifdef ESP32
    #include <WiFi.h>
#else
    #include <ESP8266WiFi.h>
#endif

#include "AudioFileSourcePROGMEM.h"
#include "AudioGeneratorWAV.h"
#include "AudioGeneratorSAMPLE.h"
#include "AudioOutputI2Sesp32.h"
// #include "AudioOutputI2S.h"
#include "AudioMixerOutBuffer.h"
#include "AudioMixerInBuffer.h"


// Sample, 16 Bit, Mono, Midi c4 - Note 60
#include "angels.h"
#include "arabicbass.h"

// AudioGeneratorWAV *wav1;
// AudioGeneratorWAV *wav2;

AudioGeneratorSAMPLE *wav1;
AudioGeneratorSAMPLE *wav2;
AudioGeneratorSAMPLE *wav3;
AudioGeneratorSAMPLE *wav4;

AudioFileSourcePROGMEM *file1;
AudioFileSourcePROGMEM *file2;
AudioFileSourcePROGMEM *file3;
AudioFileSourcePROGMEM *file4;

AudioOutputI2Sesp32 *out;
// AudioOutputI2S *out2;

AudioMixerInBuffer *channel1;
AudioMixerInBuffer *channel2;
AudioMixerInBuffer *channel3;
AudioMixerInBuffer *channel4;

AudioMixerOutBuffer *mainOut;

  uint8_t velocity = 100;
  uint8_t pitch = 65;
  int soundplays = 0;

  
void setup()
{
  soundplays = 0;
  WiFi.mode(WIFI_OFF);

  Serial.begin(115200);
  delay(1000);
  file1 = new AudioFileSourcePROGMEM( angels, sizeof( angels ) ); // long duration
  file2 = new AudioFileSourcePROGMEM( angels, sizeof( angels ) ); // long duration
  file3 = new AudioFileSourcePROGMEM( arabicbass, sizeof( arabicbass ) ); // short duration
  file4 = new AudioFileSourcePROGMEM( arabicbass, sizeof( arabicbass ) ); // short duration

  out = new AudioOutputI2Sesp32();
  // Adjust Volume of Output
  out->SetGain(0.2);

  // out2 = new AudioOutputI2S();
  // out2->SetGain(0.2);
  // out2->SetPinout(26, 25, 33); // the original loibrary uses pin 22 rather than 33 for DIN
  
  wav1 = new AudioGeneratorSAMPLE();
  wav2 = new AudioGeneratorSAMPLE();
  wav3 = new AudioGeneratorSAMPLE();
  wav4 = new AudioGeneratorSAMPLE();
  
  Serial.println("Define MainOut of Mixer");
  // new with Mixer, connect mainOut to the output
  mainOut = new AudioMixerOutBuffer( 2, out );
  
  mainOut->SetRate(44100);
  mainOut->SetChannels(2); // 2=Stereo, 1=Mono
  mainOut->SetBitsPerSample(16); // 16 Bit

  Serial.println("Begin MainOut");
  mainOut->begin();

  // define channel1 of the Mixer and strip it to mainOut as Strip 1
  // defined with buffersize==8, sink==mainout, channelnumber==1
  Serial.println("Define Channel1 of Mixer");
  channel1 = new AudioMixerInBuffer( 8, mainOut, 1 );

  // define channel2 of the Mixer and strip it to mainOut as Strip 2
  Serial.println("Define Channel2 of Mixer");  
  channel2 = new AudioMixerInBuffer( 8, mainOut, 2 );
  channel2->begin();

  Serial.println("Define Channel3 of Mixer");  
  channel3 = new AudioMixerInBuffer( 8, mainOut, 3 );
  channel3->begin();
  
  Serial.println("Define Channel4 of Mixer");  
  channel4 = new AudioMixerInBuffer( 8, mainOut, 4 );
  channel4->begin();

  Serial.println("Begin Wav1");
  wav1->begin(file1, channel1, velocity, pitch );
  wav2->begin(file2, channel2, velocity, uint8_t( pitch + 4 ) );
  wav3->begin(file3, channel3, velocity, uint8_t( pitch + 7 ) );
  wav4->begin(file4, channel4, velocity, uint8_t( pitch + 12 ) );
 
}

void loop()
{
  if (wav1->isRunning()  ) {
    if (!wav1->loop() ) {
      soundplays =soundplays + 1;
      wav1->stop(); 
      channel1->stop();
      yield();
      if ( soundplays < 40 ){
       file1 = new AudioFileSourcePROGMEM( angels, sizeof( angels ) ); // long duration
        wav1->begin(file1, channel1, velocity, pitch); // channel1
      }  
    }  
  } 

  if (wav2->isRunning()  ) {
    if (!wav2->loop() ) {
      soundplays =soundplays + 1;
      wav2->stop(); 
      channel2->stop();
      yield();
      if ( soundplays < 40 ){
        file2 = new AudioFileSourcePROGMEM( angels, sizeof( angels ) ); // sound with short duration
        wav2->begin(file2, channel2, velocity, uint8_t(pitch+7) );
      }  
    }
  } 


  if (wav3->isRunning()   ) {
    if (!wav3->loop() ) {
      soundplays =soundplays + 1;
      wav3->stop(); 
      channel3->stop();
      yield();
      if ( soundplays < 40 ){
        file3 = new AudioFileSourcePROGMEM( arabicbass, sizeof( arabicbass ) ); // sound with short duration
        wav3->begin(file3, channel3, velocity, uint8_t(pitch) );
      }  
    }
  } 

  if (wav4->isRunning()  ) {
    if (!wav4->loop() ) {
      soundplays =soundplays + 1;
      wav4->stop(); 
      channel4->stop();
      yield();
      if ( soundplays < 40 ){
        file4 = new AudioFileSourcePROGMEM( arabicbass, sizeof( arabicbass ) ); // sound with short duration
        wav4->begin(file4, channel4, velocity, uint8_t(pitch-12) );
      }  
    }
  } 



  if ( soundplays > 50 ){
    // Stop
    soundplays = 0;
    }

/*  
  // Stop the Output if all is done ..
  if ( soundplays >=20 && soundplays < 27) {
    out->stop();
  //   out2->stop();
  }
  */
  
}

