This is the last version of the ESp32-based Stomp-Box.
All  WAV-Files are only stored in the PROGMEM. That makes this solution a bit cheaper and more robust for rebuilding it.

The OLED-Display is connected to GPIO 4 and 5
2 Buttons are connected to +3.3V and to GPIO 15 (Trigger or Preset-Select) and GPIO16 (Menu-Select)
The MID-Pin of the Rotary-Encoder is connected to GPIO 22

The Piezo - if used is connected to ADC0 or GPIO 36
I have tried to get it working with Touch but it didn´t.

