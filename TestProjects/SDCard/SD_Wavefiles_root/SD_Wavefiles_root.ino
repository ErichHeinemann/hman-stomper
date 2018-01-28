/* Get all .wav in root or Directories and play the first 100 in a loop with different volume!!
 *  
 * Connect the SD card to the following pins:
 *
 * SD Card | ESP32
 *    D2       -
 *    D3       SS
 *    CMD      MOSI
 *    VSS      GND
 *    VDD      3.3V
 *    CLK      SCK
 *    VSS      GND
 *    D0       MISO
 *    D1       -
* ESP32 - SD-Card
* IO23 = MOSI
* IO19 = MISO
* IO18 = CLK / SCLK
* IO05 = CS
* Bei MicroSD-Adaptern mit Spannungswandler AMS117 dann auf VSS auf VIN/5 Volt setzen
* 
* 
* /* Pinout:
** ESP32    SD-Card - Adapter (active Adapter for Microsd-Cards)
 GPIO 23 = MOSI
 GPIO 19 = MISO
 GPIO 18 = CLK / SCLK
 GPIO 05 = CS
 GND = GND
 The MicroSD-Adaptern needs Power 5V if a local regulator as AMS117 is installed on it.
 https://www.banggood.com/de/Micro-SD-Card-High-Speed-Module-For-3_3V-5V-Logic-For-MicroSD-MMC-Card-p-1244285.html?gmcCountry=DE&currency=EUR&createTmp=1&utm_source=googleshopping&utm_medium=cpc_elc&utm_content=zouzou&utm_campaign=pla-de-elc2-pc&gclid=EAIaIQobChMIubeLs8-22AIVgRXTCh2rOgU8EAkYAyABEgJmq_D_BwE&cur_warehouse=CN
 

ESP32 - DAC98357A or PCM5102
 GPIO 26  - BCK-Pin  
 GPIO 25, - LRCK-Pin 
 GPIO 33, : DIN-Pin 
 Vin/5V or 3.3V  :  Vin  ( if 3.3 Volts was used, do not connect a Loadspeaker directly!! )
 https://www.banggood.com/PCM5102-DAC-Decoder-Module-I2S-Player-32bit-384K-Assembled-Board-A2-012-p-1080430.html?rmmds=search&cur_warehouse=CN
 https://www.amazon.de/Aihasd-Schnittstelle-GY-PCM5102-Spieler-Modul-Audio-Board/dp/B075P33R8T/ref=sr_1_2?ie=UTF8&qid=1514810089&sr=8-2&keywords=i2s+dac

PCM5102
VCC - not connected!
3.3V -> 3.3 Volt (analog side)
GND -> ( Analog or Digital GND??)
FLT (Filter)  -> GND
DMP (DEMP?) -> GND (eigentlich analog Ground)
SCL -> GND
BCK -> GPIO 26
DIN  -> GPIO 33
LCK -> GPIO 25
FMT(Filter) -> GND
XMT -> 3.3V (0V = Mute, 3.3V = unmute)

*/

/*

Hint found via Google but not used ...
Changes in SD.C-Lib:
I can reproduce this issue also with SD card reader attached but no SD Card present
Using spi.end(); neither SD.end(); after mount failed help to avoid issue
@Aleksandrovas
Aleksandrovas commented on 2 Nov 2017 •

Soliution is verry simple, open SD.c and in 39 line replace sdcard_uninit(_pdrv); to sdcard_unmount(_pdrv);
Espressif please fix it.
 
*/

 
#include "FS.h"
#include "SD.h"
#include "SPI.h"


// In Setup I trigger one sound through local Progmem ...
#include "AudioFileSourcePROGMEM.h"
// THis is the Lib to get files from SD
#include "AudioFileSourceSD.h"
#include "AudioFilter.h"
#include "AudioGeneratorSAMPLE.h" 
#include "AudioGeneratorWAV.h" 
#include "AudioOutputI2SDAC.h"

#include "AudioMixerOutBuffer.h"
#include "AudioMixerInBuffer.h"

// #include "AudioOutputSerialWAV.h"

#include "fatbass.h"

// More Libs to integrade SSD1306-OLEDs
// #include <Wire.h>  
// #include "SSD1306.h"

String category = "";
String displayname = "";

// Array of the Display-Names
// String wavfiles[100];
// Array of the filenames
// const char* wavfilesc[100];
String wavfilesc[100];

int wavnum = 0;

int debuglevel = 0;
uint8_t velocity = 127;
uint8_t pitch = 60; 

uint8_t lpFreq = 20;
uint8_t lpReso = 126;

// float gainset = 0.018;

// AudioGeneratorWAV *wav2; // AudioGeneratorSAMPLE is nearly the same as AudioGeneratorWAV but provides a simple Pitch and Velocity.
AudioGeneratorSAMPLE *wav;
AudioGeneratorSAMPLE *wav2;
AudioFileSourcePROGMEM *file; // Bank "P"
AudioFileSourceSD *file2; // Bank "SD"
AudioOutputI2SDAC *out;
AudioMixerInBuffer *channel1;
AudioMixerInBuffer *channel2;
AudioMixerOutBuffer *mainOut;


// Function to get the first 100 WAV-Files from the SD-Card
void listDir(fs::FS &fs, const char * dirname, uint8_t levels){

    if ( debuglevel > 0) {
      Serial.printf("Listing directory: %s\n", dirname);
    }
    
    File root = fs.open(dirname);
    
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if( file.isDirectory() ){
            category = reinterpret_cast<const char*>(file.name());

            // get the WAV-Files out of the next Directory
            if( levels==0 && category.startsWith("/.") == false ){
              if ( file.name()   ){
                if ( debuglevel >0 ) {
                  Serial.print("  DIR : ");
                  Serial.println( category ); 
                }
               }
               listDir(fs, file.name(), 0);
            }

            if( levels ){
                listDir(fs, file.name(), levels -1);
            }
            
        } else {
            // Serial.print("  FILE: ");
            // String instrument = file.name();
            
            const char* filename = file.name();
            String instrument = reinterpret_cast<const char*>(filename);
            
            /*
            displayname = instrument;
            // instrument.replace( category,"") ;
            // displayname = instrument;
            displayname.replace( ".wav" , "") ;
            displayname.replace( "/" , " ") ;
            displayname.replace( "_" , " ") ;
            */
                        
            if ( wavnum < 100 && instrument.indexOf("/.") ==-1 && instrument.endsWith(".wav") == true && file.size() < 300000 && file.size()>200 ){
              // wavfilesc[wavnum] = instrument;
              // char charBuf[instrument.length()];
              // const char* filenameX =  instrument.c_str(); // Convert to char-Array
              // wavfilesc[wavnum] =  file.name(); // instrument;
              wavfilesc[wavnum] = instrument;
              // wavfiles[wavnum] = displayname;
              wavnum = wavnum+1;
               
              // instrument.replace( category," ") ;
              if (debuglevel > 0) {
                Serial.print( " "+ instrument );
                // Serial.print( category +" "+ instrument + " " + displayname );
                Serial.print("  SIZE: ");
                Serial.println( file.size() );
              }

            }
            if (wavnum >=100){
              return;
            }
            
        }
        file = root.openNextFile();
    }
}

// SSD1306  display(0x3c, 17, 16); // OLED via I2C Labeled GPIO 17, 16
bool sdmounted = false;

void setup(){
    Serial.begin(115200);
    //  display.init();
  
    file  = new AudioFileSourcePROGMEM( fatbass, sizeof(fatbass) ); // Short Demosound after Booting
    file2 = new AudioFileSourceSD (); // only defined ...
    out   = new AudioOutputI2SDAC();
    out->SetGain( 0.2);

    // new with Mixer, connect mainOut to the output
    mainOut = new AudioMixerOutBuffer( 8, out );
    mainOut->SetRate(44100);
    mainOut->SetChannels(2); // Stereo
    mainOut->SetBitsPerSample(16);
    mainOut->begin();
    
    // define channel1 of the Mixer and strip it to mainOut
    channel1 = new AudioMixerInBuffer( 64, mainOut );
    channel1->begin();
    
    // wav2 = new AudioGeneratorWAV();
    wav = new AudioGeneratorSAMPLE();

    // the file will be played on Channel 1 of the Mixer
    wav->begin( file, channel1, velocity, pitch);
    Serial.println("Played WAV");


    if(!SD.begin()){
        Serial.println("Card Mount Failed");
        Serial.println("Missing SDCard" );
        sdmounted = false;
        wavnum = 0;
    } else {
        sdmounted = true; 
        wavnum = 0;         
    }
    
}

int wavbankb = 0;

void loop(){
   if ( wavnum > 0 ) {
   // wav->isRunning()  ) {
    if ( !wav->loop() ) { 
        Serial.println( "wav->stop" );
        wav->stop(); 
        // do we have to channel1->stop(); ???

        // perhaps this is needed, perhaps not
        if ( file2->isOpen() == true ){
          file2->close();
          Serial.println("Closing File");
         }
          
        if ( wavbankb < wavnum) {
          // file = new AudioFileSourcePROGMEM( fatbass, sizeof(fatbass) );
          // load complete file
  
         /*
          * // Copy file into RAM ...
          File f = SD.open( wavfilesc[wavbankb] , FILE_READ );
          uint8_t data[f.size()];
          const unsigned char allC = f.read(reinterpret_cast<uint8_t*>(data), f.size() );
          f.close();  
          file = new AudioFileSourcePROGMEM( allC, f.size() );
        */
          // yield();

          // Convert the String-Filename into a CharArray 
          String str = wavfilesc[wavbankb]; 
          // String str = "/D4-EFX/Efx009-DoorSlam-90.wav";
          // String str = "/60_C4.wav";

          // Length (with one extra character for the null terminator)
          int str_len = str.length() + 1; 
          // Prepare the character array (the buffer) 
          char char_array[str_len];
          // Copy it over 
          str.toCharArray(char_array, str_len);
          
          // Define the FileSource
          file2 = new AudioFileSourceSD ( char_array ); // wavfilesc[wavbankb]

          // Modify the Gain
          out->SetGain( 0.4 );

          if (pitch >= 65 ) {
            pitch = 55;  // only 12 Steps down and 12 Steps up .. 2 Octaves to keep the pitch-algorithm simple
          } else {
            pitch ++; //C4
          }
          // pitch = 60;
          
          
          // Serial.println( char_array );
          Serial.print ("Prgm:");
          Serial.print ( wavbankb );

          Serial.print ( "     Velocity:" );
          Serial.print ( velocity );
          
          Serial.print ( "  LP:" );
          Serial.print ( lpFreq );
          
          Serial.print ( " Pitch:" );
          Serial.print ( pitch );
          
          // Serial.print (" Name:");
          // Serial.print ( wavfilesc[wavbankb] ); // Convert to char-Array );
          
          Serial.println (" ");
          
          yield();

          // Play the file
          // wav->setLP(0);
          lpFreq = 15;
          lpReso = 15;

          // Filter is not implemented well
          // wav->setLPFilter( lpFreq, lpReso);
          // Reverb is not implemented well
          // wav->setReverb(); // time, balance, feedback planned
          wav->begin( file2, out, velocity, pitch ); // ,127,65);
          // wav->setLPFilter( lpFreq, lpReso);

          yield();
          // delay(50);
          /*
           // Change the settings of the filter while a sample is still playing...  
            for (int i = 5; i < 70; i++) {
            // wav->setLPFilter(  i, 15); // Freq, Reso
            Serial.println ( i );
            yield();
            delay(1);
            i = i +10;
          } 
            
          lpFreq = lpFreq +10;
          if ( lpFreq  >= 127 || velocity < 0 ) {
             lpFreq = 15; // 0 works fine too!  
          }
          */
          
          // Change Velocity for next sound
          velocity = velocity + 30 ;
          if ( velocity >= 127 || velocity < 0 ) {
            velocity = 30; // 0 works fine too!  
          }
          wavbankb = wavbankb +1;
          
        } else {
          wavbankb = 0;
        }
 
    }
       //     delay(1000); //mask time
  }  

   
   if ( sdmounted == false ){
        Serial.println("No SDCard mounted" );
        delay(1000); //mask time

        // If You plug in a new SD-Card, You have to reset the ESP32  :(
   }

   // Get the WAV-Files which are stored on the SD
   if (sdmounted == true && wavnum == 0 ){
        wavnum = 0; // Reset the Array to zero
        listDir(SD, "/", 0);
        /*        
        for (int thiswav = 0; thiswav < wavnum; thiswav++) {
            Serial.print("  File:: " + wavfiles[thiswav][0]);
            Serial.print("  Display:: " + wavfiles[thiswav][1]);
            Serial.println(" " + (String) thiswav );
            //  const char* filesd = wavfiles[thiswav][0].toCharArray(wavfiles[thiswav][0], sizeof(wavfiles[thiswav][0])) ;

        } 
        */      
        // Do not end the SD-Card-Support here
        // SD.end();//           SD = false;
        delay(1000); //mask time
    }
    
}
