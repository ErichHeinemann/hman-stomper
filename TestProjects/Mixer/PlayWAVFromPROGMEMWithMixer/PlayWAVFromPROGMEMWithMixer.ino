#include <Arduino.h>

#ifdef ESP32
    #include <WiFi.h>
#else
    #include <ESP8266WiFi.h>
#endif

#include "AudioFileSourcePROGMEM.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2SDAC.h"


#include "AudioOutputBuffer.h"

#include "AudioMixerOutBuffer.h"
#include "AudioMixerInBuffer.h"


// VIOLA sample taken from https://ccrma.stanford.edu/~jos/pasp/Sound_Examples.html
#include "viola.h"

AudioGeneratorWAV *wav1;
AudioGeneratorWAV *wav2;
AudioFileSourcePROGMEM *file1;
AudioFileSourcePROGMEM *file2;
AudioOutputI2SDAC *out;

AudioOutputBuffer *buf1;

AudioMixerInBuffer *channel1;
AudioMixerInBuffer *channel2;
AudioMixerOutBuffer *mainOut;

void setup()
{
  WiFi.mode(WIFI_OFF);

  Serial.begin(115200);
  delay(1000);
  file1 = new AudioFileSourcePROGMEM( viola, sizeof(viola) );
  // file2 = new AudioFileSourcePROGMEM( viola, sizeof(viola) );
  out = new AudioOutputI2SDAC();
  buf1 = new AudioOutputBuffer(8, out );
  //Adjust Volume of Output
  out->SetGain(0.2);
  
  wav1 = new AudioGeneratorWAV();

  Serial.println("Define MainOut of Mixer");
  // new with Mixer, connect mainOut to the output
  mainOut = new AudioMixerOutBuffer( 8, out );
  mainOut->SetRate(44100);
  mainOut->SetChannels(1); // 2=Stereo, 1=Mono
  mainOut->SetBitsPerSample(16); // 16 Bit



  Serial.println("Define Channel1 of Mixer");
  // define channel1 of the Mixer and strip it to mainOut as Strip 1
  channel1 = new AudioMixerInBuffer( 8, mainOut, 1 );
  mainOut->begin();

  
  // channel1->begin();
  Serial.println("Define Channel2 of Mixer");  
  // define channel2 of the Mixer and strip it to mainOut as Strip 2
  channel2 = new AudioMixerInBuffer( 64, mainOut, 2 );
  wav2 = new AudioGeneratorWAV();
  Serial.println("Begin Wav1");
  wav1->begin(file1, channel1); // channel 1
  delay(100);

Serial.println("Begin Wav2");
  channel2->begin();
  wav2->begin(file2, channel2);
Serial.println("Begin MainOut");


 
}

void loop()
{
  if (wav1->isRunning()) {
    if (!wav1->loop()) wav1->stop();
  } else {
    Serial.printf("WAV1 done\n");
    delay(1000);
  }

  if (wav2->isRunning()) {
    if (!wav2->loop()) wav2->stop();
  } else {
    Serial.printf("WAV2 done\n");
    delay(1000);
  }

}

