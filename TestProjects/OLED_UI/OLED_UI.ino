/**

 */

// Include the correct display library
#include <Wire.h>        // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306.h"     // alias for `#include "SSD1306Wire.h"`
// #include "HmanRotary.h"  // Custom lIbrary for Rotary-Encoder

// Probably this for fixedStrings
// https://github.com/toomasz/ArduinoFixedString
 

// Initialize the OLED display using Wire library
SSD1306  display(0x3c, 5, 4);

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

void oledValueSelection ( String menuname, int mark, String paramname, int paramvalue , int parammax=127, String confirmtext="save") {
  display.clear();
  //   String menuname  ="osc 1";
  //   String paramname  ="Freq:";
  //  int    paramvalue = 105;
  // String confirmtext = "save|r"; // save or reset selected by rotary
  // uint8_t mark = 4;

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
    display.drawHorizontalLine( 128 - display.getStringWidth( confirmtext), 17, display.getStringWidth( confirmtext) );
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
  
  if ( paramvalue>0 ) {
    int p =  paramvalue * 100 / parammax;
    display.drawProgressBar(48, 44, 78, 13, p);
  }  
  
  if (mark == 3) {
    // Draw a line horizontally
    display.drawHorizontalLine(0, 63,  display.getStringWidth((String) paramvalue));
  }
  
  display.display();

}





// Concept to select something out of a List of Values (LOV)
void oledValueSelectionLOV( String menuname, int mark,  String paramname, int pParamvalue , String lov[], int sizeOfLov, String confirmtext="save") {
  int paramvalue = 0;
  if ( sizeOfLov > 0) {
    paramvalue = pParamvalue%sizeOfLov;
  } 
  display.clear();
  // String confirmtext = "save|r"; // save or reset selected by rotary

  // detect position of selected val in the array and use 3 before and 3 after it ... 
  // Menuname
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, menuname );

  display.drawString(3+display.getStringWidth( menuname ), 0,  paramname);

  if (mark == 1) {
    // Draw a line horizontally
    display.drawHorizontalLine(0, 17,  display.getStringWidth( menuname ) );
  }
  if (mark == 2) {
    // Draw a line horizontally
    display.drawHorizontalLine(display.getStringWidth( menuname ) , 20,  display.getStringWidth( paramname ) );
  }

  // Confirmation
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(128, 0, confirmtext );
  if (mark == 4) {
    // Draw a line horizontally
    display.drawHorizontalLine(90 , 17,  38 );
  }
  
  String label1=" ";
  String label2=" ";
  String label3=" ";
 

  if ( paramvalue>0 & sizeOfLov>1 ){
      label1=lov[paramvalue-1];
  }

  if (sizeOfLov>0){  
    label2=lov[paramvalue];
  }
   
  if ( sizeOfLov > paramvalue+1 ){
      label3=lov[paramvalue+1];
  }
       
  // display Name of Parameter:
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(1, 24, label1 );

  // selected Value .. else the value is printed in Arial 10
  display.setFont(ArialMT_Plain_16);
  display.drawString(5, 36, label2 );
  if ( mark == 3 ) {
    // Draw a line horizontally
    display.drawHorizontalLine( 5, 52,  display.getStringWidth( label2 ) );
  }
  if (label3 != " ") {   
    display.setFont(ArialMT_Plain_10);
    display.drawString(1, 54, label3 );
  }
  // not enough place to show 4 or more values
  // display.drawString(5, 51, lov[4]);    
  // draw bar on the right side
  drawLOVScroll( paramvalue, sizeOfLov );
  display.display();

}


void drawLOVScroll( int pLOV, int sLOV ){
  // pLOV == Position
  // sLOV = Size of LOV
  int rPos = 20+(pLOV * 45/(sLOV) );
  display.drawVerticalLine(122, 21, 44);
  display.drawVerticalLine(127, 21, 44);
  display.drawHorizontalLine(122, 21, 6);
  display.drawHorizontalLine(122, 63, 6);  
  display.fillRect(122, rPos, 6, 5 );
}



void loop() {


  oledValueSelection("menu", 3 , "Param", 10, 200);
  delay(200);
  oledValueSelection("menu", 3 , "Param", 20, 200);
  delay(200);
  oledValueSelection("menu", 3 , "Param", 30, 200);
  delay(200);
  oledValueSelection("menu", 3 , "Param", 40, 200);
  delay(200);
  oledValueSelection("menu", 3 , "Param", 50, 200);
  delay(200);
  oledValueSelection("menu", 3 , "Param", 60, 200);
  delay(200);
  oledValueSelection("menu", 3 , "Param", 70, 200);
  delay(200);
  oledValueSelection("menu", 3 , "Param", 80, 200);
  delay(200);
  oledValueSelection("menu", 3 , "Param", 90, 200);
  delay(200);
  oledValueSelection("menu", 3 , "Param", 100, 200);
  delay(1000);

  String lov[7]; // we need only 6 for the UI at the same time ..  perhaps we use more but at another place!
 
  lov[ 0 ] = "oneshot simple";
  lov[ 1 ] = "oneshot layered";
  lov[ 2 ] = "oneshot splitted";
  lov[ 3 ] = "loop";
  lov[ 4 ] = "synth";
  lov[ 5 ] = "harmonics";
  lov[ 6 ] = "randomgroup";

  oledValueSelectionLOV( "sound", 1, "type", 0 , lov, 7 );
  delay(1000);

  oledValueSelectionLOV( "sound", 2, "type", 0 , lov, 7 );
  delay(1000);
  oledValueSelectionLOV( "sound", 3, "type", 0 , lov, 7 );
  delay(1000);
  oledValueSelectionLOV( "sound", 3, "type", 1 , lov, 7 );
  delay(1000);
  oledValueSelectionLOV( "sound", 3, "type", 2 , lov, 7 );
  delay(1000);
  oledValueSelectionLOV( "sound", 3, "type", 3 , lov, 7 );
  delay(1000);
  oledValueSelectionLOV( "sound", 3, "type", 4 , lov, 7 );
  delay(1000);
  oledValueSelectionLOV( "sound", 3, "type", 5 , lov, 7 );
  delay(1000);
  oledValueSelectionLOV( "sound", 3, "type", 6 , lov, 7 );
  delay(1000);
  oledValueSelectionLOV( "sound", 4, "type", 6 , lov, 7 );
  delay(3000);    


 
}
