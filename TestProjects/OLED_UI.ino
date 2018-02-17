/**

 */

// Include the correct display library
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`

// Include custom images
// #include "images.h"

// Initialize the OLED display using Wire library
SSD1306  display(0x3c, 5, 4);

#define DEMO_DURATION 3000
typedef void (*Demo)(void);

int demoMode = 0;
int counter = 1;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();


  // Initialising the UI will init the display too.
  display.init();

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);

}

void oledValueSelection() {
  // ( String menuname, String paramname, int paramvalue ) 
  String menuname  ="osc 1";
  String paramname  ="Freq:";
  int    paramvalue = 105;
  String confirmtext = "save|r"; // save or reset selected by rotary
  uint8_t  mark = 4;

  // Menuname
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, menuname);
  if (mark == 1) {
    // Draw a line horizontally
    display.drawHorizontalLine(0, 17,  display.getStringWidth( menuname ) );
  }
  
  // Confirmation
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(128, 0, confirmtext);
  if (mark == 4) {
    // Draw a line horizontally
    display.drawHorizontalLine(127- display.getStringWidth( confirmtext), 17, display.getStringWidth( confirmtext) );
  }
  
  // display Name of Parameter:
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 20, paramname);
  if (mark == 2) {
    // Draw a line horizontally
    display.drawHorizontalLine(0, 37,  display.getStringWidth( paramname) );
  }

  // display Value of Parameter:
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_24);
  display.drawString(0, 38, (String) paramvalue);
  int p = 127/paramvalue *100;
  display.drawProgressBar(48, 44, 78, 13, p);
  if (mark == 3) {
    // Draw a line horizontally
    display.drawHorizontalLine(0, 63,  display.getStringWidth((String) paramvalue));
  }
}


// Concept to select something out of a List of Values (LOV)
void oledValueSelectionLOV() {
  // ( String menuname, String paramname, int paramvalue , array of const char as LOV, String selectedVal) 
  String menuname  ="sound";
  String paramname ="Type";
  String lov[7]; // we need only 6 for the UI at the same time ..  perhaps we use more but at another place!
  uint8_t  mark = 2;
  String confirmtext = "save|r"; // save or reset selected by rotary
 
  lov[ 0 ] = "oneshot simple";
  lov[ 1 ] = "oneshot layered";
  lov[ 2 ] = "oneshot splitted";
  lov[ 3 ] = "loop";
  lov[ 4 ] = "synth";
  lov[ 5 ] = "harmonics";
  lov[ 6 ] = "randomgroup";


  // detect position of selected val in the array and use 3 before and 3 after it ... 
 
  // Menuname
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, menuname + "" + paramname);

  if (mark == 1) {
    // Draw a line horizontally
    display.drawHorizontalLine(0, 17,  display.getStringWidth( menuname ) );
  }
  if (mark == 2) {
    // Draw a line horizontally
    display.drawHorizontalLine(display.getStringWidth( menuname ) , 17,  display.getStringWidth( paramname ) );
  }
  

  // Confirmation
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(128, 0, confirmtext );
  
  // display Name of Parameter:
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(2, 21, lov[1]);

  // selected Value .. else the value is printed in Arial 10
  display.setFont(ArialMT_Plain_16);
  display.drawString(4, 32, lov[2]);
  
  display.setFont(ArialMT_Plain_10);
  display.drawString(2, 49, lov[3]);
  // display.drawString(5, 51, lov[4]);    

  // draw bar on the right side
  drawLOVScroll( 70, 70 );
  
}


void drawLOVScroll( int pLOV, int sLOV ){
  // pLOV == Position
  // sLOV = Size of LOV
  int rPos = 20+(44/sLOV* (pLOV) );
  
  display.drawVerticalLine(123, 21, 44);
  display.drawVerticalLine(127, 21, 44);
  display.drawHorizontalLine(123, 21, 5);
  display.drawHorizontalLine(123, 63, 5);  
  display.fillRect(123, rPos, 5, 5 );
  }

  
void drawFontFaceDemo() {
    // Font Demo1
    // create more fonts at http://oleddisplay.squix.ch/
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "Hello world");
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 10, "Hello world");
    display.setFont(ArialMT_Plain_24);
    display.drawString(0, 26, "Hello world");
}

void drawTextFlowDemo() {
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawStringMaxWidth(0, 0, 128,
      "Lorem ipsum\n dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore." );
}

void drawTextAlignmentDemo() {
    // Text alignment demo
  display.setFont(ArialMT_Plain_10);

  // The coordinates define the left starting point of the text
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 10, "Left aligned (0,10)");

  // The coordinates define the center of the text
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 22, "Center aligned (64,22)");

  // The coordinates define the right end of the text
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(128, 33, "Right aligned (128,33)");
}

void drawRectDemo() {
      // Draw a pixel at given position
    for (int i = 0; i < 10; i++) {
      display.setPixel(i, i);
      display.setPixel(10 - i, i);
    }
    display.drawRect(12, 12, 20, 20);

    // Fill the rectangle
    display.fillRect(14, 14, 17, 17);

    // Draw a line horizontally
    display.drawHorizontalLine(0, 40, 20);

    // Draw a line horizontally
    display.drawVerticalLine(40, 0, 20);
}

void drawCircleDemo() {
  for (int i=1; i < 8; i++) {
    display.setColor(WHITE);
    display.drawCircle(32, 32, i*3);
    if (i % 2 == 0) {
      display.setColor(BLACK);
    }
    display.fillCircle(96, 32, 32 - i* 3);
  }
}

void drawProgressBarDemo() {
  int progress = (counter / 5) % 100;
  // draw the progress bar
  display.drawProgressBar(0, 32, 120, 10, progress);

  // draw the percentage as String
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 15, String(progress) + "%");
}

void drawImageDemo() {
    // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
    // on how to create xbm files
    display.drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
}

Demo demos[] = {oledValueSelection,  oledValueSelectionLOV };
int demoLength = (sizeof(demos) / sizeof(Demo));
long timeSinceLastModeSwitch = 0;

void loop() {
  // clear the display
  display.clear();
  // draw the current demo method
  demos[demoMode]();

  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(10, 128, String(millis()));
  // write the buffer to the display
  display.display();

  if (millis() - timeSinceLastModeSwitch > DEMO_DURATION) {
    demoMode = (demoMode + 1)  % demoLength;
    timeSinceLastModeSwitch = millis();
  }
  counter++;
  delay(50);
}
