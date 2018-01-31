#include <Arduino.h>

#ifdef ESP32
    #include <WiFi.h>
#else
    #include <ESP8266WiFi.h>
#endif

#include "AudioFileSourcePROGMEM.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2SDAC.h"
#include "AudioMixerOutBuffer.h"
#include "AudioMixerInBuffer.h"


// Sample, 16 Bit, Mono, Midi c4 - Note 60
#include "angels.h"
#include "arabicbass.h"
AudioGeneratorWAV *wav1;
AudioGeneratorWAV *wav2;
AudioFileSourcePROGMEM *file1;
AudioFileSourcePROGMEM *file2;
AudioOutputI2SDAC *out;

AudioMixerInBuffer *channel1;
AudioMixerInBuffer *channel2;
AudioMixerOutBuffer *mainOut;

void setup()
{
  WiFi.mode(WIFI_OFF);

  Serial.begin(115200);
  delay(1000);
  file1 = new AudioFileSourcePROGMEM( angels, sizeof( angels ) ); // long duration
  // file1 = new AudioFileSourcePROGMEM( arabicbass, sizeof( arabicbass ) ); // sound with short duration
  file2 = new AudioFileSourcePROGMEM( arabicbass, sizeof( arabicbass ) ); // sound with short duration

  out = new AudioOutputI2SDAC();
  // Adjust Volume of Output
  out->SetGain(0.2);
  
  wav1 = new AudioGeneratorWAV();
  wav2 = new AudioGeneratorWAV();
  
  Serial.println("Define MainOut of Mixer");
  // new with Mixer, connect mainOut to the output
  mainOut = new AudioMixerOutBuffer( 2, out );
  mainOut->SetRate(44100);
  mainOut->SetChannels(2); // 2=Stereo, 1=Mono
  mainOut->SetBitsPerSample(16); // 16 Bit

  Serial.println("Begin MainOut");
  mainOut->begin();

  // define channel1 of the Mixer and strip it to mainOut as Strip 1
  Serial.println("Define Channel1 of Mixer");
  channel1 = new AudioMixerInBuffer( 2, mainOut, 1 );

  // define channel2 of the Mixer and strip it to mainOut as Strip 2
  Serial.println("Define Channel2 of Mixer");  
  channel2 = new AudioMixerInBuffer( 2, mainOut, 2 );
  Serial.println("Begin Wav2");
  channel2->begin();


  Serial.println("Begin Wav1");
  wav1->begin(file1, channel1); // channel1
    delay(200);
  wav2->begin(file2, channel2);
 
}

void loop()
{
  if (wav1->isRunning() ) {
    if (!wav1->loop() ) {
      
      wav1->stop(); 
      channel1->stop();
      }
  } else {
    Serial.printf("WAV1 done\n");
    wav1->begin(file1, channel1);
    Serial.printf("restart wav1\n");
    file1 = new AudioFileSourcePROGMEM( angels, sizeof( angels ) ); // long duration
    wav1->begin(file1, channel1); // channel1
    
  }

  if (wav2->isRunning() ) {
    if (!wav2->loop() ) {
      wav2->stop(); 
      channel2->stop();
    }
  } else {
    Serial.printf("WAV2 done\n");

    Serial.printf("restart wav2\n");
    file2 = new AudioFileSourcePROGMEM( arabicbass, sizeof( arabicbass ) ); // sound with short duration
    // channel2 = new AudioMixerInBuffer( 2, mainOut, 4);
    wav2->begin(file2, channel2);
  }

}

