#include <Bounce.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

//    WAVEFORM_SINE,
//    WAVEFORM_SQUARE,
//    WAVEFORM_SAWTOOTH,
//    WAVEFORM_PULSE,

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform1;      //xy=215,232
AudioOutputAnalog        dac1;           //xy=784,129
AudioConnection          patchCord1(waveform1, dac1);
// GUItool: end automatically generated code

void setup(){
    AudioMemory(50);
    waveform1.pulseWidth(0.5);
    waveform1.begin(0.9, 220, WAVEFORM_SINE);
}




void loop(){
//    waveform1.begin(0.9, 220, WAVEFORM_SINE);
//    delay(2000);
    waveform1.begin(0.9, 220, WAVEFORM_SQUARE);
//    delay(2000);
//    waveform1.begin(0.9, 220, WAVEFORM_SAWTOOTH);
//    delay(2000);
//    waveform1.begin(0.9, 220, WAVEFORM_PULSE);
//    delay(2000);



}






