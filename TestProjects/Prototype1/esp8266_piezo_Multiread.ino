// for ESP8266 !!!
// This file uses WAV-Files which I converted into "h"-Files. It uses the Audioesp8266-Library in a version from december 2017. The main test was done in the if-then-clause starting with line 151. The trick here was to find a way to get a precise MAX of the Piezo
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "AudioFileSourcePROGMEM.h"
// #include "AudioFileSourceSPIFFS.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2SDAC.h"

#include <Wire.h>  
#include "SSD1306.h"

// WAV-Files
// WAV-Files, MONO 44.1 KBIT/s 16Bit
#include "fatbass.h"
#include "softkick.h"
#include "rockkick.h"
#include "ride1.h"
#include "cajonbass.h"
#include "arabicbass.h"
#include "acousticstomp1.h"
#include "acousticstomp2.h"
#include "r908.h"
#include "r809.h"
#include "tamborine1.h"
#include "christjingles.h"
#include "espkik13.h"
#include "espkik14.h"
#include "espkik15.h"
// #include "espkik16.h"
#include "espkik17.h"
#include "espkik18.h"
#include "espkik19.h"
#include "espkik20.h" 
#include "espkik22.h"
// #include "espkik28.h"

AudioGeneratorWAV *wav;
AudioFileSourcePROGMEM *file;
AudioOutputI2SDAC *out;

unsigned short buttonState = 0; // Selectbutton for the sounds
unsigned short oldButtonState = 1;
boolean piezo   = false;
int maxPiezo    = 400;
int gaindevider = 300; // 380

unsigned short s = 1;
unsigned short s_old = 20; // Sample-ID

// replacement of the debounce-delay
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
const long interval = 215; // debounce-delay in Millis
int pthreshold = 25;


const char *samplenames1[] ={" 0 Fat",
     " 1 Soft    " , " 2 Cajon  "
   , " 3 Rock   " , " 4 Ride  "
   , " 5 Arabic"
   
   , " 6 Acoust " 
   , " 7 Acoust " 
   , " 8 Acoust " 
   , " 9 HipHop   " , "10 Techno "
   
   , "11 Tambo- ", "12 Christ."
   , "13 Cabasa ", "14 Guiro  "
   , "15 Rattle "
   , "16 Cow    ", "17 Claves "
   , "18 Clap   ", "19 Side   "
   , "20 Hihat  ", "21 xxx"

   
  };

const char *samplenames2[] ={"Drum",
     "    Kick",  "    Bass"
   , "    Kick",  "    Cymbal"
   , "    Bass"
   
   , "    Stomp 1"
   , "    Stomp 2"
   , "    Stomp 3"
   , "    Synth", "    Synth"
   
   , "rine Man",  "    Bells"
   , "    Latin", "    Latin" 
   , "    Shaker"
   , "    Bell",  "         "
   , "    Clap",  "    Stick"
   , "   Closed","    Cymbal"


  };  

unsigned short samplecount = 21;
SSD1306  display(0x3c, 5, 4); // Labeled D1, D2

const int buttonPin = 0;      // Labeled D3, Pin is high by default, button between this pin and GND to switch the sounds


void setup() {
  WiFi.forceSleepBegin();
  display.init();
  // display.flipScreenVertically();
  display.setContrast(255);

  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_24);
  display.drawStringMaxWidth(0 , 0 , 128, " Heinemann");
  display.drawStringMaxWidth(0 , 24 , 128,"  Stomper");
  // display.drawXbm(20, 30, bassdrum_width, bassdrum_height, bassdrum_bits);
  display.display();

  // pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(115200);
  delay(2000);
  
  file = new AudioFileSourcePROGMEM( fatbass, sizeof(fatbass) ); // Short Demosound after Booting
  out = new AudioOutputI2SDAC();
  out->SetGain(0.02);
  wav = new AudioGeneratorWAV();
  wav->begin(file, out);
  
  pinMode( buttonPin, INPUT); // GPIO 0 == D03 on ESP8266 !!

}


void loop() {
  
  if ( s != s_old ) {
    if (s >= samplecount) {
      s=0;
    }
    s_old = s;
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_24);
    display.drawStringMaxWidth(0 , 0 , 128, samplenames1[s]);
    display.drawStringMaxWidth(0 , 24 , 128, samplenames2[s] ); // + (String) s);
    display.display();
  } 
  
  int sensorValue = analogRead(A0);
  int MAX;

  currentMillis = millis();
  
  if ( currentMillis - previousMillis >= interval && sensorValue > pthreshold &&  piezo == false) {
    MAX = sensorValue;
    int peak1 = 0;
    for (int thisLoop = 0; thisLoop < 5; thisLoop++) {
      peak1 = analogRead(A0);
      if (peak1 > MAX) {
        MAX = peak1;
      }
      yield(); // delay(1);
    }
    if(MAX >= maxPiezo){
      MAX = maxPiezo;
    }
    // MAX = map(MAX, 10, maxPiezo, 1, 127);
    if(MAX <= 1){
      MAX = 1;
    }

    midiNoteOn( MAX );
    previousMillis = millis();
    // Serial.println( MAX);

    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_24);
    display.drawStringMaxWidth(0 , 0 , 128 , (String) MAX);
    display.drawStringMaxWidth(0 , 30 , 128 , (String) (millis() - currentMillis)  );
    
    display.display();
    piezo = true;
    yield();
    
  }
  
  if (sensorValue <= 0 && piezo == true) {
    piezo = false;
  }

  
  buttonState = digitalRead(buttonPin);
  if ( buttonState == LOW &&  oldButtonState == HIGH ) {
    oldButtonState = buttonState;  
    s = s +1;    
    oldButtonState = buttonState;  
    // delay(50); // debounce
  } else {
    oldButtonState = buttonState;  
  }

  if (wav->isRunning()) {
    if (!wav->loop()) { 
      wav->stop(); // wenn nicht mehr richtig looped dann stoppen
    }
  }  

  // Serial.println( 0 );
  yield();

}

void midiNoteOn( int midiVelocity){
  if (midiVelocity > 0) {
  switch (s) {
      case 0:
        file = new AudioFileSourcePROGMEM( fatbass, sizeof(fatbass) ); // mistake, - i started with 01 but the 00 could be valid
        break;
      case 1:
        file = new AudioFileSourcePROGMEM( softkick, sizeof(softkick) );
        break;
      case 2:
        file = new AudioFileSourcePROGMEM( cajonbass, sizeof(cajonbass) );
        break;
      case 3:
        file = new AudioFileSourcePROGMEM( rockkick, sizeof(rockkick) );
        break;
      case 4:
        file = new AudioFileSourcePROGMEM( ride1, sizeof(ride1) );
        break;
      case 5:
        file = new AudioFileSourcePROGMEM( cajonbass, sizeof(cajonbass) );
        break;
      case 6:
        file = new AudioFileSourcePROGMEM( arabicbass, sizeof(arabicbass) );
        break;
      case 7:
        file = new AudioFileSourcePROGMEM( acousticstomp1, sizeof(acousticstomp1) );
        break;
      case 8:
        file = new AudioFileSourcePROGMEM( acousticstomp2, sizeof(acousticstomp2) );
        break;
      case 9:
        file = new AudioFileSourcePROGMEM( r908, sizeof(r908) );
        break;
      case 10:
        file = new AudioFileSourcePROGMEM( r809, sizeof(r809) );
        break;
      case 11:
        file = new AudioFileSourcePROGMEM( tamborine1, sizeof(tamborine1) );
        break;
      case 12:
        file = new AudioFileSourcePROGMEM( christjingles, sizeof(christjingles) );
        break;
      case 13:
        file = new AudioFileSourcePROGMEM( espkik13, sizeof(espkik13) );
        break;
      case 14:
        file = new AudioFileSourcePROGMEM( espkik14, sizeof(espkik14) );
        break;
      case 15:
        file = new AudioFileSourcePROGMEM( espkik15, sizeof(espkik15) );
        break;
      case 16:
        file = new AudioFileSourcePROGMEM( espkik17, sizeof(espkik17) );
        break;
      case 17:
        file = new AudioFileSourcePROGMEM( espkik18, sizeof(espkik18) );
        break;
      case 18:
        file = new AudioFileSourcePROGMEM( espkik19, sizeof(espkik19) );
        break;
      case 19:
        file = new AudioFileSourcePROGMEM( espkik20, sizeof(espkik20) );
        break;
      case 20:
        file = new AudioFileSourcePROGMEM( espkik22, sizeof(espkik22) );
        break;
      default:
        file = new AudioFileSourcePROGMEM( fatbass, sizeof(fatbass) ); 
        s = 0;  
      }

    // eigentlich p statt oldPiezoVal
    out->SetGain( 0.04 +  (float) midiVelocity/gaindevider ); // 0.8(float) p/130
    
    if (wav->isRunning()) {
      wav->stop(); // wenn nicht mehr richtig looped dann stoppen
    }
    wav->begin(file, out);
  }
}
