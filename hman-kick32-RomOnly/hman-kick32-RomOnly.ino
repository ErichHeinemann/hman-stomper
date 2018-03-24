// #include <Arduino.h>

// Erich Heinemann 2018-03-24
// Open / ToDo:
// 1. SD-Card-Support
// 2. Storing the Settings in Eeprom
// 3. Volume not working
// 4. Touch not working

// Most Libraries where transformed from code from Erle Phil Hower from the ESP8266-Audio-Project
// My Code is not that stable and still changing. If I could get the right point, this code will join the code of the ES8266-Audio-Project!


#include "FS.h"
#include "SD.h"
#include "SPI.h"


#include <WiFi.h>
// #include "esp_task.h"
#include <math.h>

#include "SampleAudioFileSourcePROGMEM.h"
#include "SampleAudioFileSourceSD.h"
#include "SampleAudioGeneratorSAMPLE.h"
#include "SampleAudioOutputI2Sesp32.h"

#include "SampleAudioMixerOutBuffer.h"
#include "SampleAudioMixerInBuffer.h"


#include "PresetHandler.h"

#include <Wire.h>  
#include "SSD1306.h"

// WAV-Files
// WAV-Files, MONO 44.1 KBIT/s 16Bit
#include "fatbass.h"
#include "softkick.h"
#include "rockkick.h"
#include "cajonbass.h"
#include "arabicbass.h"
#include "acousticstomp1.h"
#include "acousticstomp2.h"
#include "r908.h"
#include "r809.h"
#include "tamborine1.h"
#include "christjingles.h"
#include "ride1.h"
#include "espkik13.h"
#include "espkik14.h"
#include "espkik15.h"
#include "espkik16.h"
#include "espkik17.h"
#include "espkik18.h"
#include "espkik19.h"
#include "espkik20.h" 
#include "espkik22.h"

int debuglevel = 10;

bool hasChanged = false; // to manage the tasks
 

// static int taskCore = 0;
// TaskHandle_t Task1, Task2;
// SemaphoreHandle_t baton

SampleAudioGeneratorSAMPLE   *wav1;
SampleAudioFileSourcePROGMEM *fileA; // Progmem
SampleAudioFileSourceSD      *fileB; // SD-Card

SampleAudioOutputI2Sesp32    *out;

// SampleAudioMixerInBuffer     *channel1;
// SampleAudioMixerOutBuffer    *mixer;

PresetHandler  *presethandlers[20];

bool usePROGMEM                = true;
unsigned short buttonState1    = 0; // Selectbutton for the sounds
unsigned short oldButtonState1 = 0;
unsigned short buttonState2    = 0; // Selectbutton for the menu (Rotary-Button)
unsigned short oldButtonState2 = 0;

unsigned short s     =  1;
unsigned short s_old = 20; // Sample-ID

// The names of the Files in Progmem
// You have to change these names to 
// good names for Your samples
const char *samplenames1[] ={"Fat",
     "Soft    "  , "Cajon"
   , "Rock    "  , "Ride"
   , "Arabic  "  , "Acoust" 
   , "Acoust  "  , "Acoust" 
   , "HipHop  "  , "Techno"
   , "Tamborin"  , "Christ."
   , "Cabasa "   , "Guiro"
   , "Rattle "   , "Cow"
   , "Claves "   , "Clap"
   , "Side   "   , "Hihat"
   , "xxx"
  };

const char *samplenames2[] ={"Drum",
     "Kick"   , "Bass"
   , "Kick"   , "Cymbal"
   , "Bass"   , "Stomp 1"
   , "Stomp 2", "Stomp 3"
   , "Synth"  , "Synth"
   , "Man"    , "Bells"
   , "Latin"  , "Latin" 
   , "Shaker" , "Bell"
   , " " , "Clap"   
   , "Stick"  , "Closed"  
   , "Cymbal"
  };


const char *menu1[] ={ 
     "Preset"
   , "PiezoSense"   
   , "Sound"   
   , "Pitch"
   , "Volume " 
   , "Play-Type"
   , "Bank"
   , "Save or Cancel"
  };

  

int activeMenuLevel = 0;
int oldMenuLevel    = 0;
int maxMenuLevel    = 7; // reflected by the array menu1!
int activePreset    = 0;
int lastPreset      = 20;
int maxPresets      = 20; // Count of Presets

int lastPlayedBank = 0;
  
unsigned short samplecount = 20;
SSD1306  display( 0x3c, 5, 4 ); // GPIO 5 and 4 are often used for ESP32-Devboards with integrated OLED!

const int buttonPin1    = 15; // oder 10 Labeled GPIO 13, Pin is high by default, button between this pin and GND to switch the sounds .. 34-39 cannot be pulled up via software ... 
const int buttonPin2    = 16; // oder 10 Labeled GPIO 13, Pin is high by default, button between this pin and GND to switch the sounds .. 34-39 cannot be pulled up via software ... 
const int piezoPin      = A0; // ADC0
const int triggermode   =  2; // 0= Piezo, 1=Touch, 2=Frontbutton

boolean   piezo         = false;
int       maxPiezo      = 127;
int       gaindevider   = 380;
int       touchPreLevel = 127; // Maximum .. we have to invert to value to get a comperable way like piezos, hard hit ->high value, soft hit -> low value

uint8_t   pitch         =  60;
uint8_t   velocity      = 100;

hw_timer_t * timer = NULL;
// portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

// Touchsensors are currently not used in this project but my test-hardware has the pins connected to coppertape
int buttonTouchSensor1 = T8; // Touch 8  GPIO 32
int buttonTouchSensor2 = T7; // Touch 7  GPIO 27
int buttonTouchSensor3 = T5; // Touch 5  GPIO 12
int buttonTouchSensor4 = T6; // Touch 6  GPIO 14

//Rotary Encoder No1
int encoder1PinA = 22;
int encoder1PinB = 21;
int encoder1Pos  = 0;
int encoder1PinALast = LOW;
int n1 = HIGH;

// Sensorvalues
int sensorValue= 0;
int sensorValueMax = 0;



// replacement of the debounce-delay
unsigned long currentMillis  = 0;
unsigned long previousMillis = 0;
unsigned long menuMillis = 0;
unsigned long previousmenuMillis = 0;

long interval   = 120; // debounce-delay in Millis
int  pthreshold =   1;

// SD-Card-Variables
bool   sdmounted = false;
int    wavnum = 0;
String category    = "";
// String wavfilesc[20]; // We load only 20 Sounds , .. the same number as internal sounds ...

int16_t buflastSample[2];

void clearscreen() { 
  for(int i=0;i<10;i++) {
    Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n");
  }
}

int readRotary1( int oldvalue, int minValue, int maxValue, bool swap=true   ) {
  encoder1Pos = oldvalue;
  n1 = digitalRead(encoder1PinA);
  if ((encoder1PinALast == LOW) && (n1 == HIGH)) {
    if (digitalRead(encoder1PinB) == HIGH) {
      encoder1Pos--;
    } else {
      encoder1Pos++;
    }
    if (swap) {
      if (encoder1Pos >maxValue) { encoder1Pos  = minValue; }
      if (encoder1Pos <minValue ) { encoder1Pos = maxValue; }
    } else {
      if (encoder1Pos >maxValue) { encoder1Pos  = maxValue; }
      if (encoder1Pos <minValue ) { encoder1Pos = minValue; }
    }
    // Serial.println (encoder1Pos);
    delay(10); // 40
  }
  encoder1PinALast = n1;
  return encoder1Pos;
}

/*
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
            
         // Max 3 MB            
         if ( wavnum < 20 && instrument.indexOf("/.") ==-1  && instrument.endsWith(".wav") == true && file.size() < 3000000 && file.size()>200 ){
            wavfilesc[wavnum] = instrument;
            wavnum = wavnum+1;
            if (debuglevel > 0) {
                Serial.print( " "+ instrument );
                // Serial.print( category +" "+ instrument + " " + displayname );
                Serial.print("  SIZE: ");
                Serial.println( file.size() );
              }
            }
            if (wavnum >=10){
              return;
            }
        }
        file = root.openNextFile();
    }
}
*/

void loadPresets(){
  // if no Presetdefinition was found in the Eeprom
  for(int i=0;i<maxPresets; i++) {
    presethandlers[i] = new PresetHandler();
    presethandlers[i]->name1 = samplenames1[i];
    presethandlers[i]->name2 = samplenames2[i];
    presethandlers[i]->pitch=60;         // 48 - 72 or -12 to +12 and then add 60 as default
    presethandlers[i]->sense=1;          // 0-1000
    presethandlers[i]->velocity=127;     // 0-127
    presethandlers[i]->playtype=0;       // 0 - oneshot, 1 - loop, 2 - while keypressed 
    presethandlers[i]->bank=0;           // 0 = PROGMEM, 1 = SD-Card
    presethandlers[i]->progmemNumber     = i;  // 0-20   
    // presethandlers[i]->filename ="/mpc1000/105_Moody.wav";      // filename of the played sound .. should be better: const unsigned char
    presethandlers[i]->setFilename("/mpc1000/TechnoC #001.wav");
    Serial.println(presethandlers[i]->filename);
    presethandlers[i]->vvolume=127;       // 0-127
  }
}

void savePresets(){
  // Save all Settings in EEPROM
  for(int i=0;i<maxPresets; i++) {
    /*
    presethandlers[i] = new PresetHandler();
    presethandlers[i]->name1 = samplenames1[i];
    presethandlers[i]->name2 = samplenames2[i];
    presethandlers[i]->pitch=60; // 48 - 72 or -12 to +12 and then add 60 as default
    presethandlers[i]->sense=1;          // 0-1000
    presethandlers[i]->velocity=127;     // 0-127
    presethandlers[i]->playtype=0;       // 0 - oneshot, 1 - loop, 2 - while keypressed 
    presethandlers[i]->bank=0;           // 0 = PROGMEM, 1 = SD-Card
    presethandlers[i]->progmemNumber     = i;  // 0-20   
    presethandlers[i]->filename ="/mpc1000/105_Moody.wav";      // filename of the played sound .. should be better: const unsigned char
    presethandlers[i]->vvolume=127;       // 0-127
    */
  }
  
}

void midiNoteOn( int &mypitch, int &midiVelocity, int &sound , int &mybank, String myfilename="/" ){
  // stop anyway

  Serial.print( "Note On Start" );  
/* schlcht  
 *  
 if (lastPlayedBank==0) { 
    wav1->stop();
  }
  // yield();
  */
  // Serial.print( "Pitch:" );
  // Serial.println( mypitch );
  Serial.print( "Velocity:" );
  Serial.println( midiVelocity );
  Serial.print( "Bank:" );
  Serial.println( mybank );
  // Serial.print( "Filename: " );
  // Serial.println( myfilename );
  
  if ( mybank == 0 || sdmounted == false) {
    if (sdmounted == false ) {
      lastPlayedBank = 0;
    }
    // Serial.print( "Source - Progmem - " );
    // Serial.print ( "Sound:" );
    // Serial.println( sound );
  } else {
    Serial.print ( myfilename );
    Serial.println( " Source - SD-Card" );  
  }
  


     /* 
      if ( lastPlayedBank == 0) {
        if ( fileA->isOpen() == true ){
          Serial.println("Closing FileA");
          fileA->close();
        } 
        // Serial.println("Delete FileA");
        // delete fileA; 
        // Serial.println("Delete FileA done");
        // yield();
      } else {
        if ( fileB->isOpen() == true ){
          Serial.println("Closing FileB");
          fileB->close();
          // yield();
          Serial.println("Closing FileB done");
        }
        // delete fileB; 
      }
      */
 /*    
 if (!wav1->isRunning() &  mybank == 1 && sdmounted == true) { // SD-Card
    if ( wav1->isRunning() ) {
      wav1->stop(); // wenn nicht mehr richtig looped dann stoppen
    }
    if (midiVelocity > 0) {
      // String str = myfilename; 
      //delete char_array;
      //delete str;
      // Define the FileSource
      // fileB->close();
      // delete fileB;
      // yield();
      
      // timerAlarmDisable(timer);
      // Serial.println("Timer disabled");
      wav1  = new SampleAudioGeneratorSAMPLE();
      
      // Serial.print("New FileB: );
      // Serial.println(myfilename );
      delete fileB;

      // int str_len = str.length() + 1;  // Length (with one extra character for the null terminator)
      // char char_array[str_len]; // Prepare the character array (the buffer) 
      // str.toCharArray( char_array, str_len );       // Copy it over 
      
      fileB = new SampleAudioFileSourceSD ( myfilename.c_str() ); // wavfilesc[wavbankb]
      lastPlayedBank = 1;
      // out = new SampleAudioOutputI2Sesp32();
      // out->SetGain( 1 );
      wav1->begin(fileB, out, 100, mypitch );   
      Serial.println("wav1 begin done");
    }  
  }
*/


  // ROM = Progmem
  if ( mybank == 0 || sdmounted == false) {
    // Progmem
    if (wav1->isRunning()  ) {
      wav1->stop();
      // yield();
      // delete fileA;
      if ( lastPlayedBank == 0) {
       //  delete fileA; 
       } else {
       //   delete fileB; 
      }
    } else {
      /// no wav stop, only delete...
      // delete fileA; // bei delete File wird ein Core-Dump ausgelöst !!
      // delete fileB;
    } 
    if (midiVelocity > 0) {
      switch ( sound ) {
      case 0:
         fileA = new SampleAudioFileSourcePROGMEM( fatbass, sizeof(fatbass) ); // mistake, - i started with 01 but the 00 could be valid
        break;
      case 1:
        fileA = new SampleAudioFileSourcePROGMEM( softkick, sizeof(softkick) );
        break;
      case 2:
        fileA = new SampleAudioFileSourcePROGMEM( cajonbass, sizeof(cajonbass) );
        break;
      case 3:
        fileA = new SampleAudioFileSourcePROGMEM( rockkick, sizeof(rockkick) );
        break;
       case 4:
         fileA = new SampleAudioFileSourcePROGMEM( ride1, sizeof(ride1) );
         break;
      case 5:
        fileA = new SampleAudioFileSourcePROGMEM( cajonbass, sizeof(cajonbass) );
        break;
      case 6:
        fileA = new SampleAudioFileSourcePROGMEM( arabicbass, sizeof(arabicbass) );
        break;
      case 7:
        fileA = new SampleAudioFileSourcePROGMEM( acousticstomp1, sizeof(acousticstomp1) );
        break;
      case 8:
        fileA = new SampleAudioFileSourcePROGMEM( acousticstomp2, sizeof(acousticstomp2) );
        break;
      case 9:
        fileA = new SampleAudioFileSourcePROGMEM( r908, sizeof(r908) );
        break;
      case 10:
        fileA = new SampleAudioFileSourcePROGMEM( r809, sizeof(r809) );
        break;
      case 11:
        fileA = new SampleAudioFileSourcePROGMEM( tamborine1, sizeof(tamborine1) );
        break;
      case 12:
        fileA = new SampleAudioFileSourcePROGMEM( christjingles, sizeof(christjingles) );
        break;       

      
      case 13:
        fileA = new SampleAudioFileSourcePROGMEM( espkik13, sizeof(espkik13) );
        break;
      case 14:
        fileA = new SampleAudioFileSourcePROGMEM( espkik14, sizeof(espkik14) );
        break;
      case 15:
        fileA = new SampleAudioFileSourcePROGMEM( espkik15, sizeof(espkik15) );
        break;
      case 16:
        fileA = new SampleAudioFileSourcePROGMEM( espkik16, sizeof(espkik16) );
        break;
       case 17:
        fileA = new SampleAudioFileSourcePROGMEM( espkik17, sizeof(espkik17) );
        break;
      case 18:
        fileA = new SampleAudioFileSourcePROGMEM( espkik18, sizeof(espkik18) );
        break;
      case 19:
        fileA = new SampleAudioFileSourcePROGMEM( espkik19, sizeof(espkik19) );
        break;

     // case 20:
     //   fileA = new SampleAudioFileSourcePROGMEM( espkik20, sizeof(espkik20) );
     //   break;
     // case 21:
     //   fileA = new SampleAudioFileSourcePROGMEM( espkik22, sizeof(espkik22) );
     //   break;
      default:
        fileA = new SampleAudioFileSourcePROGMEM( fatbass, sizeof(fatbass) ); 
        sound = 0;  
      }
      lastPlayedBank = 0;
      // out = new SampleAudioOutputI2Sesp32();
      // out->SetGain( 0.4 );
      // wav = new SampleAudioGeneratorSAMPLE();
      // wav->begin( fileA, out, midiVelocity, mypitch );
      wav1->begin(fileA, out, midiVelocity, mypitch ); // channel1
      // Serial.println( "wav->begin" );
    }
  }
 

  // file1 = new SampleAudioFileSourcePROGMEM( angels, sizeof( angels ) ); // long duration
  // Serial.println("Begin Wav1");
 
}


// ########################################################################
//
//         TASK for CORE 0     TASK for CORE 0     TASK for CORE 0 
// 
// ########################################################################



void IRAM_ATTR onTimer(){
//    portENTER_CRITICAL_ISR(&timerMux);
  /* 
 *  Demo-Mode
 *  !wav1->isRunning() && 
 */
 /*
  if ( currentMillis - previousMillis > 500 ) {   
    // notetest();
    midiNoteOn( presethandlers[activePreset]->pitch, 100, presethandlers[activePreset]->progmemNumber, presethandlers[activePreset]->bank, presethandlers[activePreset]->filename);
    previousMillis = millis();
  }
*/

  
//   portENTER_CRITICAL_ISR(&timerMux);

 // Check Sensor
   currentMillis = millis();  

  // Trigger new WAV-File
  // Piezo-Mode:
  if ( triggermode == 0 ){
     sensorValue = analogRead( piezoPin ); // ADC0, GPIO36
     // Serial.println("P: " + (String) sensorValue);
  }

  // Touch
  //if ( triggermode == 1 ){
    // did not work well ...  
    // sensorValue = 100 - (touchRead( buttonTouchSensor1 )); // buttonTouchSensor1 defined in Variables
  //}

  // Button
  if ( triggermode == 2 ){
    buttonState1 = digitalRead( buttonPin1 );
    if (  buttonState1 == HIGH   &&  oldButtonState1 == LOW ) {
      sensorValue = 100;
      oldButtonState1 = buttonState1;  
    } else {
      oldButtonState1 = buttonState1; 
      sensorValue =0; 
    }
   }

  int sensorValueMax=0;
  if ( piezo == false &&  (currentMillis - previousMillis) >= interval && sensorValue > pthreshold ) {
    previousMillis = millis();
    piezo = true;

    sensorValueMax = sensorValue;

    // Special for Piezo
    if ( triggermode == 0 ){    
      for (int thisLoop = 0; thisLoop < 5; thisLoop++) {
        sensorValue = analogRead( piezoPin );
      } 
      if (sensorValue > sensorValueMax) {
        sensorValueMax = sensorValue;
      }
      // yield(); // delay(1);     
    }

    if( sensorValueMax >= maxPiezo ){
      sensorValueMax = maxPiezo;
    }
    if( sensorValueMax <= 1 ){
      sensorValueMax = 1;
    }
    // Start the new Sound
    Serial.println("Send Note On");
    midiNoteOn( presethandlers[activePreset]->pitch,  sensorValueMax, presethandlers[activePreset]->progmemNumber, presethandlers[activePreset]->bank, presethandlers[activePreset]->filenamestr );
    // yield();    
  }
    
 if ( ( currentMillis - previousMillis) > interval || sensorValue < pthreshold || sensorValue == 0  ){
    piezo = false;
    // yield();     
 }

 
 //  portEXIT_CRITICAL_ISR(&timerMux);
}


void soundTask( ){
 
  // This Code is something like "noteOff" and garbagecollection !
  if ( wav1->isRunning()  ) {
    if ( !wav1->loop() ) {
      wav1->stop();
      for(int i=0;i<150;i++) {
        if ( !out->ConsumeSample(buflastSample) ) break;
      }
      yield();
      if ( lastPlayedBank == 0) {
        if ( fileA->isOpen() == true ){
          Serial.println("Closing FileA ");
          fileA->close();
          Serial.println("Closing FileA done");
        }
        yield();
       //  delete fileA; 
        yield();
       Serial.println("Deleted FileA done");
      } /*else {
        if ( fileB->isOpen() == true ){
          fileB->close();
          yield();
          Serial.println("Closing FileB done");
        }
        // delete fileB;  // 24.03.2018 disabled
      }
      */
    }

 }  else {
     // fill the output with some zeros because nothing is running here  
      for(int i=0;i<100;i++) {
        if (!out->ConsumeSample(buflastSample)) break;
      } 
  }

 


// checkSensor();
 
}

// ###############################################################################
//
// Task for the same or another Core
//
// ###############################################################################

void menuTask(  ){

  // Management of the Menu could be a good task for the second Core
  
   if (activeMenuLevel == 0 ) {
      //    case 0: // Preset
      // activeMenu is 0!!
      // activeMenuLevel is 0
      int oldPreset = activePreset;
      activePreset = readRotary1( activePreset, 0, maxPresets-1, true );

      // To shorten a Delay, in this Menu the Rotary is disabled!!
      if ( ( activeMenuLevel != oldMenuLevel || activePreset != lastPreset  || activePreset != oldPreset ) ){
        lastPreset = activePreset;
        display.clear();
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_24);
        display.drawStringMaxWidth(0 , 0 , 30, ((String) activePreset)  );
        yield(); 
        if ( presethandlers[activePreset]->bank == 0) {
          display.drawStringMaxWidth(0 , 30 , 20, "R"  );
        } else {
          display.drawStringMaxWidth(0 , 24 , 20, "S"  );
          display.drawStringMaxWidth(0 , 42 , 20, "D"  );
        }
        yield(); 
        display.setFont(ArialMT_Plain_10);
        display.drawStringMaxWidth( 28,  0, 128, menu1[0] );
        display.setFont(ArialMT_Plain_24);
        
        if ( presethandlers[activePreset]->bank == 0) {
          display.drawStringMaxWidth( 28, 14, 128, presethandlers[activePreset]->name1 );
          display.drawStringMaxWidth( 28, 36, 128, presethandlers[activePreset]->name2 ); // + (String) s);
        } else { 
          String filenameTemp = (String)  (presethandlers[activePreset]->filenamestr );
          filenameTemp.replace("/"," ");
          filenameTemp.replace(".wav"," ");
          display.drawStringMaxWidth( 28, 14, 128, filenameTemp.substring( 1,9) ); // + (String) s);
          display.drawStringMaxWidth( 28, 36, 128, filenameTemp.substring(10,18) ); // + (String) s);
        }
        display.display();
      } 
    } 
    yield();

       
    if ( !wav1->isRunning() && activeMenuLevel ==  1 ) {
      // Sense
      int oldsense = presethandlers[activePreset]->sense;
      presethandlers[activePreset]->sense = readRotary1( presethandlers[activePreset]->sense, 0, 1000, false );
      if ( activeMenuLevel != oldMenuLevel || oldsense != presethandlers[activePreset]->sense ) {
        display.clear();
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_24);
        display.drawStringMaxWidth(0 , 0 , 128, ((String) activePreset)  ); // Sense
        if ( presethandlers[activePreset]->bank == 0) {
          display.drawStringMaxWidth(0 , 30 , 20, "R"  );
        } else {
          display.drawStringMaxWidth(0 , 24 , 20, "S"  );
          display.drawStringMaxWidth(0 , 42 , 20, "D"  );
        }
        // display.setFont(ArialMT_Plain_24);
        display.drawStringMaxWidth( 30, 24, 128, (String) presethandlers[activePreset]->sense ); // + (String) s); 
        display.setFont(ArialMT_Plain_16);
        display.drawStringMaxWidth( 28,  0, 128, menu1[1]);
        display.display();
      }
    }
    yield();
   
    if ( !wav1->isRunning() && activeMenuLevel == 2 ){
      // Sound
      int oldprogmemNumber = activePreset;
      presethandlers[activePreset]->progmemNumber = readRotary1( presethandlers[activePreset]->progmemNumber, 0, 20, false );
      if ( activeMenuLevel != oldMenuLevel || oldprogmemNumber != presethandlers[activePreset]->progmemNumber ) {
        display.clear();
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_24);
        display.drawStringMaxWidth(0 , 0 , 128, ((String) activePreset)  );
        if ( presethandlers[activePreset]->bank == 0) {
          display.drawStringMaxWidth(0 , 30 , 20, "R"  );
        } else {
          display.drawStringMaxWidth(0 , 24 , 20, "S"  );
          display.drawStringMaxWidth(0 , 42 , 20, "D"  );
        }
        display.setFont(ArialMT_Plain_16);
        if (presethandlers[activePreset]->bank == 0) {
          display.drawStringMaxWidth( 28, 24, 128, (String) samplenames1[ presethandlers[activePreset]->progmemNumber ] ); // + (String) s);
          display.drawStringMaxWidth( 28, 40, 128, (String) samplenames2[ presethandlers[activePreset]->progmemNumber ] ); // + (String) s);
        }
        /* else {
          // split filename and show filename of SD-Card File...  
          String filenameTemp = (String)  (presethandlers[activePreset]->filenamestr );
          // First get the Progmem of the existing Filename or put them into the Array of wavfilesc
          // When loading the files, first check if existing Presets are using files from the SD-Card 
          // and add these to the Array
          
          filenameTemp = wavfilesc[ presethandlers[activePreset]->progmemNumber ];
          filenameTemp.replace("/"," ");
          display.drawStringMaxWidth( 28, 24, 128, filenameTemp.substring( 1,11) ); // + (String) s);
          display.drawStringMaxWidth( 28, 40, 128, filenameTemp.substring(12,21) ); // + (String) s);
        }
        */
        // display.setFont(ArialMT_Plain_16);
        display.drawStringMaxWidth( 28,  0, 128, menu1[2]);
        display.display();
      }  
    }
    
    if ( !wav1->isRunning() && activeMenuLevel ==  3) {
      // Pitch
      int oldpitch= presethandlers[activePreset]->pitch;
      presethandlers[activePreset]->pitch = 60+ readRotary1( presethandlers[activePreset]->pitch - 60, -12, 12, false ) ;
      if ( activeMenuLevel != oldMenuLevel || oldpitch != presethandlers[activePreset]->progmemNumber ) {
        display.clear();
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_24);
        display.drawStringMaxWidth(0 , 0 , 128, ((String) activePreset)  );
          
        if ( presethandlers[activePreset]->bank == 0) {
          display.drawStringMaxWidth(0 , 30 , 20, "R"  );
        } else {
          display.drawStringMaxWidth(0 , 24 , 20, "S"  );
          display.drawStringMaxWidth(0 , 42 , 20, "D"  );
        }
         
        // display.setFont(ArialMT_Plain_24);
        display.drawStringMaxWidth( 30, 24, 128, (String) (int) (-60 + presethandlers[activePreset]->pitch) ); // + (String) s); 

        display.setFont(ArialMT_Plain_16);
        display.drawStringMaxWidth( 28,  0, 128, menu1[3]);

        display.display();
      }
    }
    yield();
       
    if ( !wav1->isRunning() && activeMenuLevel == 4 ){
      // Volume
      int oldvvolume = presethandlers[activePreset]->vvolume;
      presethandlers[activePreset]->vvolume = readRotary1( presethandlers[activePreset]->vvolume, 0, 127, false );
      if ( activeMenuLevel != oldMenuLevel || oldvvolume != presethandlers[activePreset]->vvolume ) {

        out->SetGain ( (float)  (presethandlers[activePreset]->vvolume/180)  );
        display.clear();
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_24);
        display.drawStringMaxWidth(0 , 0 , 128, ((String) activePreset)  );
        if ( presethandlers[activePreset]->bank == 0) {
            display.drawStringMaxWidth(0 , 30 , 20, "R"  );
        } else {
          display.drawStringMaxWidth(0 , 24 , 20, "S"  );
          display.drawStringMaxWidth(0 , 42 , 20, "D"  );
        }
        // display.setFont(ArialMT_Plain_24);
        display.drawStringMaxWidth( 28, 24, 128, (String) presethandlers[activePreset]->vvolume ); // + (String) s);
        display.setFont(ArialMT_Plain_16);
        display.drawStringMaxWidth( 28,  0, 128, menu1[4]);
        display.display();
      }
    }
    yield();
    
    if ( !wav1->isRunning() && activeMenuLevel == 5) {
      // Play-Type 0 = Oneshot, 1==loop
      int oldplaytype = presethandlers[activePreset]->playtype;
      presethandlers[activePreset]->playtype = readRotary1( presethandlers[activePreset]->playtype, 0, 1, true );
      if ( activeMenuLevel != oldMenuLevel || oldplaytype != presethandlers[activePreset]->playtype ) {
        display.clear();
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_24);
        display.drawStringMaxWidth(0 , 0 , 128, ((String) activePreset)  );
        if ( presethandlers[activePreset]->bank == 0) {
          display.drawStringMaxWidth(0 , 30 , 20, "R"  );
        } else {
          display.drawStringMaxWidth(0 , 24 , 20, "S"  );
          display.drawStringMaxWidth(0 , 42 , 20, "D"  );
        }
         
        // display.setFont(ArialMT_Plain_24);
        if (presethandlers[activePreset]->playtype == 0 ){
          display.drawStringMaxWidth( 28, 24, 128, "OneShot"); // + (String) s);
        }
        if (presethandlers[activePreset]->playtype == 1 ){
          display.drawStringMaxWidth( 34, 24, 128, " Loop"); // + (String) s);
        }
        display.setFont(ArialMT_Plain_16);
        display.drawStringMaxWidth( 28,  0, 128, menu1[5]);
        display.display();
      }
    } 

    /*
    yield();
    if ( sdmounted==true && !wav1->isRunning() && activeMenuLevel == 6){
      // BANK 
    
      int oldbank = presethandlers[activePreset]->bank;
      presethandlers[activePreset]->bank = readRotary1( presethandlers[activePreset]->bank, 0, 1, true );
      if ( activeMenuLevel != oldMenuLevel || oldbank != presethandlers[activePreset]->bank ) {
        display.clear();
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_24);
        display.drawStringMaxWidth(0 , 0 , 128, ((String) activePreset)  );
        yield(); 
       if ( presethandlers[activePreset]->bank == 0) {
          display.drawStringMaxWidth(0 , 30 , 20, "R"  );
          display.drawStringMaxWidth( 30, 24, 128, " ROM" ); // + (String) s);
        } else {
          display.drawStringMaxWidth(0 , 24 , 20, "S"  );
          display.drawStringMaxWidth(0 , 42 , 20, "D"  );
          display.drawStringMaxWidth( 30, 24, 128, " SD-Card" ); // + (String) s);
        }
        display.setFont(ArialMT_Plain_16);
        display.drawStringMaxWidth( 30,  0, 128, menu1[6]);
        display.display();
      }
    }

    */
    yield();
    oldMenuLevel = activeMenuLevel;
  }

  

// ########################################################################
//
//         SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP
// 
// ########################################################################

void setup() {
  //  WiFi.forceSleepBegin();

  // simply a empty sample
  buflastSample[0] = 0;
  buflastSample[1] = 0;

  activePreset = 0;
  // Load the Presets from EEPROM or Prepare EEPROM
  loadPresets();
  // presethandlers[0]->bank=1;           //  1 = SD-Card
  
  display.init();
  // display.flipScreenVertically();
  display.setContrast(255);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_24);
  display.drawStringMaxWidth(0 ,  0 , 128, "Heinemann");
  display.drawStringMaxWidth(0 , 24 , 128, "Stomper");
  display.display();

  Serial.begin(115200);
  delay(300);
  
  lastPlayedBank == 0;

  fileA = new SampleAudioFileSourcePROGMEM( fatbass, sizeof(fatbass) ); // Short Demosound after Booting
  // fileB = new SampleAudioFileSourceSD ();
  wav1  = new SampleAudioGeneratorSAMPLE();

  out = new SampleAudioOutputI2Sesp32();
  out->SetGain( 1 );
  
 /* 
  mixer = new SampleAudioMixerOutBuffer( 32, out );
  mixer->SetRate( 44100 );
  mixer->SetChannels( 2 ); // 2=Stereo, 1=Mono
  mixer->SetBitsPerSample( 16 ); // 16 Bit
  Serial.println( "Begin mixer" );
  mixer->begin();
  */
  
  // midiNoteOn( 60, 127, 0, lastPlayedBank, "abc" );
  // ButtonPins
  pinMode( buttonPin1 , INPUT_PULLDOWN );
  pinMode( buttonPin2 , INPUT_PULLDOWN );

  //Rotary-Pins
  pinMode( encoder1PinA , INPUT_PULLDOWN );
  pinMode( encoder1PinB , INPUT_PULLDOWN );

  
  sdmounted = false;
  wavnum=0;

/*
  // Init the SD-Card
  if( !SD.begin( 13 )){ // Pin 13 used as CS-PIN for the SDCard
    Serial.println("Card Mount Failed");
    Serial.println("Missing SDCard" );
    
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_16);
    display.drawStringMaxWidth(0 , 10 , 128, "card mount failed" );
    display.drawStringMaxWidth(0 , 45 , 128, "missing SDCard" );
    display.display();
    delay (1000);
    yield(); 
    sdmounted = false;
    wavnum=0;
  } else {
    sdmounted = true; 
    wavnum=0;
    listDir( SD, "/", 0 );  
    fileB = new SampleAudioFileSourceSD ( wavfilesc[0].c_str() );
  }
  */
  /*
  channel1 = new SampleAudioMixerInBuffer( 32, mixer, 1 );
  channel1->begin();
  */
  // wav->begin( fileA, out, velocity, pitch);
  wav1->setSamplePlaytype(0); // Oneshot(0), not loop (1)

  if ( sdmounted == true){
    lastPlayedBank == 1;
    // wav1->begin( fileB, out , 100, 60 );
  } else {
    wav1->begin( fileA, out , 100, 60 );
  }

  
  previousmenuMillis = millis();

  /* Use 1st timer of 4 */
  /* 1 tick take 1/(80MHZ/800000) = 10ms so we set divider 800000 and count up */
  timer = timerBegin(1, 80, true);
  /* Attach onTimer function to our timer */
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 50000, true); // 150000 war etwas träge
  timerAlarmEnable(timer);
}




// ########################################################################
//
//         LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP
// 
// ########################################################################

void loop() {
  soundTask();

  if (( activeMenuLevel == 0 &&  millis() > previousmenuMillis +20 ) || ( activeMenuLevel != 0 &&  millis() > previousmenuMillis + 20 ) ) { 
    menuTask();
    previousmenuMillis = millis();
  }

  // Read the state of the 2 buttons
  if ( triggermode != 2){   
    buttonState1 = digitalRead( buttonPin1 );
    if (  buttonState1 == HIGH   &&  oldButtonState1 == LOW ) {
      // Serial.println ("B1" + (String) buttonState1 );
      oldButtonState1 = buttonState1;  
      activePreset = (activePreset +1) %20;    
      oldButtonState1 = buttonState1;  
      delay( 50 ); // bad debounce
    } else {
      oldButtonState1 = buttonState1;  
    }
  }

  yield();
  buttonState2 = digitalRead( buttonPin2 );
  if ( buttonState2 == HIGH   &&  oldButtonState2 == LOW ) {
    // Serial.println ("Rotarybutton B2" + (String) buttonState2 );
    oldMenuLevel = activeMenuLevel;
    activeMenuLevel = (activeMenuLevel +1) % maxMenuLevel;
    oldButtonState2 = buttonState2;
    delay( 50 ); // bad debounce
  } else {
    oldButtonState2 = buttonState2;  
  }
  yield();
  
}




