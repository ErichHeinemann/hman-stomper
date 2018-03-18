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


/*
 * Connection to the DAC
 ESP32   - DAC98357A 
 GPIO 26 - BCLK-Pin  
 GPIO 25 - LRCK-Pin 
 GPIO 33 - DIN-Pin 
 5v      . Vin
 */


#include <Arduino.h>

#include "FS.h"
#include "SD.h"
#include "SPI.h"

#include "SampleAudioFileSourcePROGMEM.h"
// #include "SampleAudioGeneratorWAV.h"
#include "SampleAudioFileSourceSD.h" // This is the Lib to get files from SD
#include "SampleAudioGeneratorSAMPLE.h"
#include "SampleAudioOutputI2Sesp32.h"
#include "SampleAudioMixerOutBuffer.h"
#include "SampleAudioMixerInBuffer.h"


// Sample, 16 Bit, Mono, Midi c4 - Note 60
#include "angels.h"
#include "arabicbass.h"
#include "silence.h"

// More Libs to integrade SSD1306-OLEDs
#include <Wire.h>  
#include "SSD1306.h"

// for further development... Voicehandler should manage the Sounds and the voices 
// for example to create a Drumkit with 10 different drum-sounds, we need only 4 or 5 voices at the same time
// #include "VoiceHandler.h";

SampleAudioGeneratorSAMPLE *wav1;
SampleAudioGeneratorSAMPLE *wav2;
SampleAudioGeneratorSAMPLE *wav3;
SampleAudioGeneratorSAMPLE *wav4;

SampleAudioFileSourcePROGMEM *file1;
SampleAudioFileSourcePROGMEM *file2;
SampleAudioFileSourcePROGMEM *file3;
SampleAudioFileSourcePROGMEM *file4;

SampleAudioOutputI2Sesp32 *out;

SampleAudioMixerInBuffer *channel1;
SampleAudioMixerInBuffer *channel2;
SampleAudioMixerInBuffer *channel3;
SampleAudioMixerInBuffer *channel4;

SampleAudioMixerOutBuffer *mixer;

// The VoiceHandler is something for the future to implement a Voice-Management but for now, it is only a dummy.
// VoiceHandler voiceHandlers[4];

  uint8_t velocity = 100;
  uint8_t pitch = 60;
  int soundplays = 0;

  // Display
  SSD1306  display(0x3c, 5, 4); // OLED via I2C Labeled GPIO 5,4

  // SD-Card
  bool sdmounted = false;


// #####################################################
// ##
// ## SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP ##
// ##
// #####################################################
  
void setup()
{
  soundplays = 0;
  // WiFi.mode(WIFI_OFF);

  Serial.begin(115200);
  delay(100);

  // digitalWrite(27, LOW);
  if( !SD.begin( 13 )){ // Pin 13 used as CS-PIN for the SDCard
    Serial.println("Card Mount Failed");
    Serial.println("Missing SDCard" );
    
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_16);
    display.drawStringMaxWidth(0 , 10 , 128, "card mount failed" );
    display.drawStringMaxWidth(0 , 45 , 128, "missing SDCard" );
    display.display();
    yield(); 
    sdmounted = false;
    //  wavnum = 0;
  } else {
    sdmounted = true; 
    // wavnum = 0;         
  }
/*
  voiceHandlers[0].buttonTouchSensor = T8;
  // String filename1="/EMU_PC/Jazz Drums/Jazz Drums(01).wav"; //BD
  voiceHandlers[1].buttonTouchSensor = T7;
  voiceHandlers[2].buttonTouchSensor = T5;
  voiceHandlers[3].buttonTouchSensor = T6;
  
  voiceHandlers[0].filename="/mpc1000/105_Moody.wav"; // Loop
  voiceHandlers[1].filename="/EMU_PC/Jazz Drums/Jazz Drums(04).wav"; // SN
  voiceHandlers[2].filename="/EMU_PC/Jazz Drums/Jazz Drums(11).wav"; // Hihat
  voiceHandlers[3].filename="/EMU_PC/Jazz Drums/Jazz Drums(33).wav"; // Ride
*/
/*
  File dataFile = SD.open("/mpc1000/105_Moody.wav", FILE_READ);
  Serial.print("Size: ");
  int datasize =  dataFile.size();

  // max 100 KMByte 
  if (datasize > 100000) {
    datasize = 100000;
  }
  Serial.print(datasize);

  uint8_t String3_5 [100000];
  int String3_5_size = datasize;
  
  // int String3_5 [datasize];
  if (dataFile) {
    for ( int index = 0; index < datasize; index++) {
      uint8_t input = reinterpret_cast<uint8_t> ( dataFile.read()) ;
      String3_5[index] = input;
      // Serial.println(input);
    }
    dataFile.close();
  }
*/
/* 
  String str = voiceHandlers[0].filename ; 
  int str_len = str.length() + 1;  // Length (with one extra character for the null terminator)
  char char_array1[str_len]; // Prepare the character array (the buffer) 
  str.toCharArray( char_array1, str_len );       // Copy it over 
  file1 = new SampleAudioFileSourceSD( char_array1 ); // long duration
*/
 
/*
  String str2 = voiceHandlers[0].filename ; 
  int str_len2 = str2.length() + 1;  // Length (with one extra character for the null terminator)
  char char_array2[str_len2]; // Prepare the character array (the buffer) 
  str2.toCharArray( char_array2, str_len2 );       // Copy it over 
  // file2 = new SampleAudioFileSourceSD( char_array2 ); // long duration
  */
  file1  = new SampleAudioFileSourcePROGMEM( arabicbass, sizeof(arabicbass) ); // nothing, only zeros and mono
  file2  = new SampleAudioFileSourcePROGMEM( angels, sizeof( angels) ); // nothing, only zeros and mono
  file3  = new SampleAudioFileSourcePROGMEM( angels, sizeof( angels ) ); // short duration  
  file4  = new SampleAudioFileSourcePROGMEM( arabicbass, sizeof( arabicbass ) ); // short duration

  // define Output via I2S-Soundcard connected to the ESP32
  out = new SampleAudioOutputI2Sesp32();
  // Adjust Volume of Output
  out->SetGain(0.3);
  
  wav1 = new SampleAudioGeneratorSAMPLE();
  wav2 = new SampleAudioGeneratorSAMPLE();
  wav3 = new SampleAudioGeneratorSAMPLE();
  wav4 = new SampleAudioGeneratorSAMPLE();
  
  Serial.println("Define mixer of Mixer");
  // new with Mixer, connect mixer to the output
  mixer = new SampleAudioMixerOutBuffer( 32, out );

  // once we have to set the settings for the output...
  mixer->SetRate(44100);
  mixer->SetChannels(2); // 2=Stereo, 1=Mono
  mixer->SetBitsPerSample(16); // 16 Bit

  Serial.println("Begin mixer");
  mixer->begin();

  // define channel1 of the Mixer and strip it to mixer as Strip 1
  // defined with buffersize==32, sink==mixer, channelnumber==1
  Serial.println("Define Channel1 of Mixer");
  channel1 = new SampleAudioMixerInBuffer( 32, mixer, 1 );
  channel1->begin();

  // define channel2 of the Mixer and strip it to mixer as Strip 2
  Serial.println("Define Channel2 of Mixer");  
  channel2 = new SampleAudioMixerInBuffer( 32, mixer, 2 );
  channel2->begin();

  Serial.println("Define Channel3 of Mixer");  
  channel3 = new SampleAudioMixerInBuffer( 32, mixer, 3 );
  channel3->begin();
/*  
  Serial.println("Define Channel4 of Mixer");  
  channel4 = new SampleAudioMixerInBuffer( 2, mixer, 4 );
  channel4->begin();
*/
  Serial.println("Begin Wav1");
  wav1->setSamplePlaytype(1); // loop
  Serial.println("The Drum will play in a loop without the need to start it really new, - check the Serial-Log");
  wav1->begin(file1, channel1, velocity, pitch );

  Serial.println("Begin Wav2");
  wav2->begin(file2, channel2, velocity, uint8_t( pitch -7 ) );
  
  Serial.println("Begin Wav3");
  wav3->begin(file3, channel3, velocity, uint8_t( pitch +7 ) );
  // wav4->begin(file4, channel4, velocity, uint8_t( pitch + 12 ) );
 
}

void loop()
{

  yield();
  if (wav1->isRunning()  ) {
    // This Code is something like "noteOff"
    if (!wav1->loop() ) {
      soundplays =soundplays + 1;
      wav1->stop(); 
      channel1->stop();
      yield();
      delete file1;
    }
  }  

  if ( !wav1->isRunning()  ) {   
    if ( soundplays < 1 ){
        /*
        String str = voiceHandlers[0].filename ; 
        int str_len = str.length() + 1;  // Length (with one extra character for the null terminator)
        char char_array1[str_len]; // Prepare the character array (the buffer) 
        str.toCharArray( char_array1, str_len );       // Copy it over 
        
        file1 = new SampleAudioFileSourceSD( char_array1 ); // long duration
        */
        // the code inside this block could be used as a "NoteOn"
        file1 = new SampleAudioFileSourcePROGMEM( angels, sizeof( angels ) ); // long duration
        Serial.print( soundplays );
        Serial.println("Begin Wav1");
        wav1->begin(file1, channel1 , velocity, pitch); // channel1
      }  
    }  

  if ( wav2->isRunning()  ) {
    if ( !wav2->loop() ) {
      soundplays =soundplays + 1;
      wav2->stop(); 
      channel2->stop();
      yield();
      delete file2;
    }
  }

  if ( !wav2->isRunning()  ) {
    if ( soundplays < 40 ){
      /*String str2 = voiceHandlers[1].filename ; 
      int str_len2 = str2.length() + 1;  // Length (with one extra character for the null terminator)
        char char_array2[ str_len2 ]; // Prepare the character array (the buffer) 
        str2.toCharArray( char_array2, str_len2 );       // Copy it over 
        //file2 = new SampleAudioFileSourceSD( char_array2 ); // sound with short duration
      */
      file2 = new SampleAudioFileSourcePROGMEM( angels, sizeof(angels) ); // short duration
      Serial.print( soundplays );
      Serial.println("Begin Wav2");
      wav2->begin(file2, channel2, velocity, pitch );
    }  
  }
  
  
 

  yield();
  if (wav3->isRunning()  ) {
    if (!wav3->loop() ) {
      soundplays =soundplays + 1;
      wav3->stop(); 
      channel3->stop();
      yield();
      delete file3;
    }
  } 
    
  if ( !wav3->isRunning()  ) {   
    if ( soundplays < 40 ){
        file3 = new SampleAudioFileSourcePROGMEM( angels, sizeof(angels) ); // long duration
        // file1 = new SampleAudioFileSourcePROGMEM( angels, sizeof( angels ) ); // long duration
        Serial.print( soundplays );
        Serial.println("Begin Wav3");
        wav3->begin(file3, channel3, velocity, uint8_t( pitch + 7 ) ); // channel1
      }  
    }  
  
/*

  if (wav3->isRunning()   ) {
    if (!wav3->loop() ) {
      soundplays =soundplays + 1;
      wav3->stop(); 
      channel3->stop();
      yield();
      delete file3;
      if ( soundplays < 400 ){
        file3 = new SampleAudioFileSourcePROGMEM( arabicbass, sizeof( arabicbass ) ); // sound with short duration
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
      delete file4;
      if ( soundplays < 400 ){
        file4 = new SampleAudioFileSourcePROGMEM( arabicbass, sizeof( arabicbass ) ); // sound with short duration
        wav4->begin(file4, channel4, velocity, uint8_t(pitch-12) );
      }  
    }
  } 
*/


  if ( soundplays > 45 ){
    // Stop
    Serial.println ( soundplays );
    // soundplays = 0;
  }

/*  
  // Stop the Output if all is done ..
  if ( soundplays >=20 && soundplays < 27) {
    out->stop();
  //   out2->stop();
  }
  */
  
}
