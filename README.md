# hman-stomper
Stomp Box based on ESP32

Current status is "under development"

# Background / Story
Since the last years some companies provided some professional Stompboxes for guitarrists and cajon-players to play the bassdrum or other percussion-sounds by feet.
After some research I come to the final to build a stomper like this based on a sample player.
I did it because I had a lot of concerns if a ready stomper from Ortega or Roland could match to the needs which I had.

If You do not have any idea which type of box I am writing about:[Google](https://www.google.de/search?client=firefox-b-ab&dcr=0&biw=1440&bih=805&tbm=isch&sa=1&ei=w9ltWsfpJsH3kwXo35KYCg&q=stompbox+wood&oq=stompbox+wood&gs_l=psy-ab.3...102477.103193.0.103405.5.5.0.0.0.0.81.359.5.5.0....0...1c.1.64.psy-ab..0.2.153...0j0i30k1j0i19k1j0i30i19k1.0.5D-yvMPc-pk) most of these Stomboxes are only using wood and a piezo or a mic or a guitar-pickup.
My one will use a small microprocessor (ESP32) and play different sounds based on samples.

First I did not have so many technical needs, the only requirement was: The sounds should match to the music and it should be simple as possible to select the sounds on stage.
Ok, thats not a well defined requirement but many requirements could be derived from it.
.. many sounds, unlimited sound-manipulation when editing the sounds, but simple gui for the stage.

It depends mostly on the sounds which are already defined in these existing tools. 
In most cases the sounds are fixed and You have to pay a money for many sounds you never use in your music.

I did some first tests with an ESP8266 and used around 20 WAV-Files.
After the first jamsession I dropped around 6 sounds, added 2 new sounds and change the order of the sounds.

To fulfill the requirement of flexible editing and simple GUI, I came up with the idea to use a Web-UI for the editing and for the use on stage only a display and a button would be enough.
I am still researching this way.

To make it more simple to change the WAV-Files I will use only a few sounds in the ROM and use a SD-Card to add more sounds.

The sound should be triggered by a simple Piezo-Element.

The microprocessor which I use for testing is an ESP32 PICO D4. The DACs which I am testing are PCM1502a and DAC98357A.
Not yet tested is an alternative using Delta-Sigma-Converter which works simply with one transistor.

Most work is done by the library created by Earlee Philhower. You need this library to start: [Audio8266](https://github.com/earlephilhower/ESP8266Audio) This Lib provides a lot of functions to play music like a mediaplayer but less functions around making music out of the perspective of an musician. I try to fix this a bit.

Connections:
- ESP32 - DAC98357A or PCM5102
- GPIO 26  - BCK-Pin  
- GPIO 25  - LRCK-Pin 
- GPIO 33  - DIN-Pin 
 
 special for PCM5102 
- VCC - not connected!
- 3.3V -> 3.3 Volt (analog side)
- GND -> ( Analog or Digital GND??)
- FLT (Filter)  -> GND
- DMP (DEMP?) -> GND (eigentlich analog Ground)
- SCL -> GND
- BCK -> GPIO 26
- DIN  -> GPIO 33
- LCK -> GPIO 25
- XMT -> 3.3V (0V = Mute, 3.3V = unmute)

The MicroSD-Card-Reader is an active one which transforms the voltage locally from 5volts to 3.3volts.
- ESP32    SD-Card - Adapter (active Adapter for Microsd-Cards)
- GPIO 23 = MOSI
- GPIO 19 = MISO
- GPIO 18 = CLK / SCLK
- GPIO 05 = CS has to be changed to GIO 27 because some boards have fixed connected OLED-Displays on GPIO 05!
- GND = GND

Rotary Encoder 1
GPIO 9 Pushbutton 
GPIO 12 Rotary Pin1 
GPIO 13 Rotary Pin3 
GND Rotary Pin2 and Pushbutton

Rotary Encoder 2
GPIO 10 Pushbutton 
GPIO 21 Rotary Pin1 
GPIO 22 Rotary Pin3 
GND Rotary Pin2 and Pushbutton

A passive SDCard-adaptor does not work without additional resistors. If You would like to only use a simple Micro-SD to SD-Card-Adaptor, then pullup all pins (MOSI, MISO, CLK) to 3.3 via 10KOhm resistors.
I will test this setup later to make the entrypoint for this project as chaep as possible. 

Additional Pins are used for a "Next-Sound-Select-Button", an optional OLED-display attached via I2C and 2 rotary encoders but this Pinout is not yet fixed.

The minimum hardware-setup then: 1 ESP32-Devboard, 1 Piezo, 1 transistor, 2 resistors, 1 diode, 1 button, 1 capacitor,one socket for 6.3mm jacks
The maximum hardware-setup then: 1 ESP32-Devboard, 1 Piezo, DAC pcm5102, 1x SDCard-Adaptor,1x SD-Card, 2 Rotary encoders, OLED-Display, 1 button, 2 sockets 6.3mm

I used a USB-Powerbank as the Powersupply which works well.

Status 2018-03-24
- A version for a Stompbox is now available but only for WAV-Files in PROGMEM.
- The best place to get Your own Samples is http://samples.kb6.de/downloads.php, You only have to search the right once and then transform them into Hex-Files.

Status 2018-02-17
- Sampleplay works
- Sample could be tuned/pitched by 12 semitones up and down (basenote is 60 = c4, range 48 to 72)
- Velocity to play each sample with different volume ( velocity 0-127)
- Mixer-function to play some samples at the same time (tested with 4 samples, theoretical up to 16 channels)
- effetcs (reverb / filter) do not work

First Video of Mixer-Tests:
https://youtu.be/iml-vkQtub4

Any comments:
stomp@hman-projects.de

Erich Heinemann
