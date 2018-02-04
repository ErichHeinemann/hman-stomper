# hman-stomper
Stomp Box based on ESP32

These files have to be added to the Arduino-libraries of the lib ESP8266 (https://github.com/earlephilhower/ESP8266Audio).

I am still working with these files and they are not yet stable. 
After getting to this point I will add them to the ES8266Audio-Lib.

Any help is welcome.

Function of these files:

Sample-Player: - works as described but the code needs some cleanup
 	AudioGeneratorSAMPLE.cpp 
	AudioGeneratorSAMPLE.h
	AudioMidiFrequencies.h
  -> provide a AudioGenerator for Sample-Playing based on the original AudioGeneratorWAV but additional functions Pitch and Velocity.
  Pitch should be a value between 48 and 72. Based on a Center-Pitch of 60, this allows to pitch the sound up and down by 1 octave.
  I use a very simple way to stretch and expand the size of a sound by filling or dropping samples.
  This uses the file AudioMidiFrequencies to get the compared amount of needed samples.
  
 Plugins for Sampleplayer: - not working (2018-02-04)
  I have planned to add some further features like some Reverb, Filter etc. to make the sound more dynamic compared to static playing the same sample all the time.
  The plugins are not well implemented and not tested!
  I hope to get the time to get it working. The goal could be the flexibility like we could see in an MPC for defining one PAD with different samples, LP-, BW- and HP-filters and effects etc.
    AudioReverb.cpp
    AudioReverb.h
    AudioFilter.cpp
    AudioFilter.h
    AudioFilters.h (older version)
  
 Mixer: - now works!! (2018-02-04) 
    AudioMixerInBuffer.cpp
    AudioMixerInBuffer.h
  -> these files create the Input-Channel of a Mixer.
  Each Input-Mixer has to be connected to a AudioMixerOutBuffer
    AudioMixerOutBuffer.cpp
    AudioMixerOutBuffer.h
  -> these files are the Mixer-Mainoutputs.
With the mixer, You could trigger in theory up to 16 sounds at the same time but technically it was tested with 4 sounds which sould be enough to simulate a cheap e-drum. An test-project is available here:
https://github.com/ErichHeinemann/hman-stomper/tree/master/TestProjects/Mixer/PlayWAVFromPROGMEMWithMixerDAC
  
To play around with these addition Libs, 
please use the Library AudioOutputI2Sesp32 rather than the current AudioOutputI2S out of the master-branch from the original project (https://github.com/earlephilhower/ESP8266Audio).
I have compared both and my older version sounds better and we have to find the differences.
AudioOutputI2Sesp32.h
AudioOutputI2Sesp32.cpp
This library works fine with the ESP32 but is not verified with the ESP8266!!



  
