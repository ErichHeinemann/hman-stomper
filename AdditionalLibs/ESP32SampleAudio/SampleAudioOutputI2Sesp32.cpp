/*
  AudioOutputI2SDAC
  Audio player for an I2S connected DAC, 16bps
  
  Copyright (C) 2017  Earle F. Philhower, III

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Arduino.h>
// #ifdef ARDUINO_ESP32_DEV
#include "driver/i2s.h"
  int i2s_num = I2S_NUM_0; // i2s port number
  const int I2S_PORT_NUM = 0;
// #else
// #include <i2s.h>
// #endif

#include "SampleAudioOutputI2Sesp32.h"


bool SampleAudioOutputI2Sesp32::i2sOn = false;

SampleAudioOutputI2Sesp32::SampleAudioOutputI2Sesp32()
{
  if (!i2sOn) 
  {
  // ESP8266
  // i2s_begin();
  //ESP32
  static const i2s_config_t i2s_config = {
   .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
   .sample_rate = 44100,
   .bits_per_sample      = I2S_BITS_PER_SAMPLE_16BIT,
   .channel_format       = I2S_CHANNEL_FMT_RIGHT_LEFT,
   .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
   .intr_alloc_flags     = ESP_INTR_FLAG_LEVEL1, // high interrupt priority
   .dma_buf_count = 6,
   .dma_buf_len   = 60
  };

 i2s_pin_config_t pin_config = {
   .bck_io_num   = 26, //BCK-Pin
   .ws_io_num    = 25, // LRCK-Pin
   .data_out_num = 33, // Daten-Pin
   .data_in_num = I2S_PIN_NO_CHANGE // Wird nicht benötigt
 };
  
  i2s_driver_install((i2s_port_t)I2S_PORT_NUM, &i2s_config, 0, NULL);
  i2s_set_pin((i2s_port_t)I2S_PORT_NUM, &pin_config);
  
  // Or only this??
  i2s_start( ( i2s_port_t )I2S_PORT_NUM );
  // Zero the Buffer to avoid sounds...
  i2s_zero_dma_buffer(( i2s_port_t )I2S_PORT_NUM);
  
  // ESP32 
  }
  
  
  i2sOn = true;
  mono = false;
  SetGain(1.0);
}

SampleAudioOutputI2Sesp32::~SampleAudioOutputI2Sesp32()
{
  if (i2sOn){
     //ESP32
     // #ifdef ARDUINO_ESP32_DEV
       i2s_zero_dma_buffer((i2s_port_t)I2S_PORT_NUM);
       i2s_stop((i2s_port_t) I2S_PORT_NUM);
     // #else  
     // ESP8266
     //   i2s_end();
     // #endif  
   }
  // i2sOn = false;
  
  // copied from the newer version of the AufioOutputI2S-Class (2018-01-30)  
  // #ifdef ARDUINO_ESP32_DEV
  // if (i2sOn) {
  //   Serial.printf("UNINSTALL I2S\n");
  //   i2s_driver_uninstall((i2s_port_t)I2S_PORT_NUM); //stop & destroy i2s driver
  // }	
  // #endif
  
  i2sOn = false;
  
}

bool SampleAudioOutputI2Sesp32::SetRate(int hz)
{
  // TODO - have a list of allowable rates from constructor, check them
  this->hertz = hz;
    // ESP32
  // #ifdef ARDUINO_ESP32_DEV
    i2s_set_sample_rates( (i2s_port_t) I2S_PORT_NUM, (uint32_t) hz);
  // #else  
  // ESP8266 
  //   i2s_set_rate(hz);
  // #endif  
  return true;
}

bool SampleAudioOutputI2Sesp32::SetBitsPerSample(int bits)
{
  if ( (bits != 16) && (bits != 8) ) return false;
  this->bps = bits;
  return true;
}

bool SampleAudioOutputI2Sesp32::SetChannels(int channels)
{
  if ( (channels < 1) || (channels > 2) ) return false;
  this->channels = channels;
  return true;
}

bool SampleAudioOutputI2Sesp32::SetOutputModeMono(bool mono)
{
  this->mono = mono;
  return true;
}

bool SampleAudioOutputI2Sesp32::begin()
{
  // Nothing to do here, i2s will start once data comes in
  return true;
}

bool SampleAudioOutputI2Sesp32::ConsumeSample(int16_t sample[2])
{
  MakeSampleStereo16( sample );

  if (this->mono) {
    // Average the two samples and overwrite
    uint32_t ttl = sample[LEFTCHANNEL] + sample[RIGHTCHANNEL];
    sample[LEFTCHANNEL] = sample[RIGHTCHANNEL] = (ttl>>1) & 0xffff;
  }
  uint32_t s32 = ((Amplify(sample[RIGHTCHANNEL]))<<16) | (Amplify(sample[LEFTCHANNEL]) & 0xffff)
  
  ;
  // ESP32
  // #ifdef ARDUINO_ESP32_DEV
    return i2s_push_sample((i2s_port_t) I2S_PORT_NUM, (const char *)&s32, 500); // Wait 500 ms
  // #else
  // ESP8266 
  //   return i2s_write_sample_nb(s32); // If we can't store it, return false.  OTW true
  // #endif
}

bool SampleAudioOutputI2Sesp32::stop()
{ 
  // ESP8266 
  // Nothing to do here.  Maybe mute control if DAC doesn't do it for you

  // ESP32
  if (i2sOn){
     //ESP32
     
      // #ifdef ARDUINO_ESP32_DEV
       i2s_zero_dma_buffer((i2s_port_t)I2S_PORT_NUM);
      // #endif
     // ESP8266
     // i2s_end();
   }
  //  i2sOn = false;
  
  
  return true;
}
