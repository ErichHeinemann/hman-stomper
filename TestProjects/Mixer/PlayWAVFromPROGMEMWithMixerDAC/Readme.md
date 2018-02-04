This version of the newer Testproject works well.
It creates 4 Samples and plays each sample in a loop. The loops are not synchronized. 
The goal was to play 4 samples randomly at the same time.
After 40 samples were played, the system stops playing new samples and shuts down to DAC.

This version uses a special version AudioOutputI2Sesp32 which implements not all functions of the current Class AudioOutput from Earlee Phil Hower III.

Test:
Play 4 Sounds -> works
Stop playing the sound and switch off the dac -> works

Open:
- Play WAV-Files from SD-Card .. currently I have only tested to play from PROGMEM.
- Fix the issues with the I2S-Lib which is used in the Main-Projekt from Earlee.

Best Regards
Erich
