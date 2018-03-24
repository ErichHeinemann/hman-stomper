#ifndef PRESETHANDLER_H
#define PRESETHANDLER_H

// #include <Arduino.h>


class PresetHandler
{
  public:
    PresetHandler() {};
     ~PresetHandler() {};
     String   name1;
     String   name2;
     int      playtype;     // 0 - oneshot, 1 - loop, 2 - while keypressed 
     int      bank;              // 0 = PROGMEM, 1 = SD-Card
     
     // sound is reflected by progmemNumber and filename
     int  progmemNumber; // 0-19   
     
     char   filename='/';     // filename of the played sound .. should be better: const unsigned char
     String filenamestr;
     
     int  pitch;        // 48 - 72 .. Center is 60 which will not modify the WAV
     int  velocity;     // 0-127
     int  vvolume;       // 0-127
     int  sense;        // 0-1000 for the Piezo

     
     boolean setFilename( String filenameStr) {
       filenamestr = filenameStr; // String
       /*
       String str = filenameStr;
       int str_len = str.length() + 1;  // Length (with one extra character for the null terminator)
       char filename[str_len]; // Prepare the character array (the buffer) 
       str.toCharArray( filename, str_len );       // Copy it over 
       */
       return true; 
     }
     
    
  protected:
    // char filename; // filename as char_array;

};

#endif
