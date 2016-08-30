#include <Bounce.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>


#define CHAN_POT_PIN 9 // pin for Channel pot
#define CHAN_CV_PIN 8 // pin for Channel CV 
#define TIME_POT_PIN 7 // pin for Time pot
#define TIME_CV_PIN 6 // pin for Time CV
#define RESET_BUTTON 8 // Reset button 
#define RESET_LED 11 // Reset LED indicator 
#define RESET_CV 9 // Reset pulse input 
#define BANK_BUTTON 2 // Bank Button 
#define LED0 6
#define LED1 5
#define LED2 4
#define LED3 3
Bounce bankSwitch = Bounce( RESET_BUTTON, 20 ); 
boolean resetButton = false;

int chordCount = 15;
int notes[15][4] = {
{-12,0,12,24},  
{0,4,7,12},
{0,3,7,12},
{0,3,7,11},
{0, 4, 5, 7},
{0, 4, 7, 14},
{0, 2, 4, 7},
{0, 3, 6, 9},
{0, 7,12, 0},
{0, 4, 7, 11},
{0, 3, 7, 10},
{0, 3, 7, 9},
{0, 4, 6, 10},
{0, 5, 7, 10},
{0, 4, 7, 9}
};
#define SINECOUNT 4

short wave_type[4] = {
  WAVEFORM_SINE,
  WAVEFORM_SQUARE,
  WAVEFORM_SAWTOOTH,
  WAVEFORM_PULSE,
};
int waveform = 0; 
elapsedMillis waveChange; 

float FREQ[SINECOUNT] = {110, 220, 440, 880};
float AMP[SINECOUNT] = { 0.9, 0.9, 0.9, 0.9};
float increment = 0.01;
//#define SMOOTHSTEP(x) ((x) * (x) * (3 - 2 * (x))) //SMOOTHSTEP expression.
Bounce 		resetSwitch = Bounce( RESET_BUTTON, 20 ); // Bounce setup for Reset
int startNote; 
int chordPick; 


// GUItool: begin automatically generated code
AudioSynthWaveform       waveform1;      //xy=215,232
AudioSynthWaveform       waveform2;      //xy=243,295
AudioSynthWaveformSine   sine3;          //xy=252,143
AudioSynthWaveformSine   sine4;          //xy=257,178
AudioSynthWaveformSine   sine1;          //xy=258,70
AudioSynthWaveformSine   sine2;          //xy=258,105
AudioSynthWaveform       waveform3;      //xy=273,354
AudioSynthWaveform       waveform4;      //xy=292,394
AudioMixer4              mixer1;         //xy=424,117
AudioMixer4              mixer2;         //xy=424,181
AudioMixer4              mixer3;         //xy=571,84
AudioOutputAnalog        dac1;           //xy=784,129
AudioConnection          patchCord1(waveform1, 0, mixer2, 0);
AudioConnection          patchCord2(waveform2, 0, mixer2, 1);
AudioConnection          patchCord3(sine3, 0, mixer1, 2);
AudioConnection          patchCord4(sine4, 0, mixer1, 3);
AudioConnection          patchCord5(sine1, 0, mixer1, 0);
AudioConnection          patchCord6(sine2, 0, mixer1, 1);
AudioConnection          patchCord7(waveform3, 0, mixer2, 2);
AudioConnection          patchCord8(waveform4, 0, mixer2, 3);
AudioConnection          patchCord9(mixer1, 0, mixer3, 0);
AudioConnection          patchCord10(mixer2, 0, mixer3, 1);
AudioConnection          patchCord11(mixer3, dac1);
// GUItool: end automatically generated code





void setup(){
    pinMode(BANK_BUTTON,INPUT);
  pinMode(RESET_BUTTON, INPUT);
  pinMode(RESET_CV, INPUT);
  pinMode(RESET_LED, OUTPUT);
  pinMode(LED0,OUTPUT);
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  pinMode(LED3,OUTPUT);
  AudioMemory(50);
  Serial.begin(9600);

  waveform1.pulseWidth(0.5);
  waveform1.begin(0.4, 220, WAVEFORM_PULSE);
    waveform2.pulseWidth(0.5);
  waveform2.begin(0.4, 220, WAVEFORM_PULSE);
    waveform3.pulseWidth(0.5);
  waveform3.begin(0.4, 220, WAVEFORM_PULSE);
    waveform4.pulseWidth(0.5);
  waveform4.begin(0.4, 220, WAVEFORM_PULSE);

  mixer1.gain(0,0.25);
  mixer1.gain(1,0.25);
  mixer1.gain(2,0.25);
  mixer1.gain(3,0.25);
  mixer2.gain(0,0.25);
  mixer2.gain(1,0.25);
  mixer2.gain(2,0.25);
  mixer2.gain(3,0.25);

  mixer3.gain(0,0.25);
  mixer3.gain(1,0.25);
  mixer3.gain(2,0.25);
  mixer3.gain(3,0.25);
 


}

int startChosen;
int chordChosen;

void loop(){
    resetButton = resetSwitch.read();
    Serial.println(resetButton);
checkInterface();
startChosen = map(startNote,0,1024,36,72);
chordChosen = map(chordPick, 0, 1024, 0, chordCount);
int result;

for(int i = 0; i< SINECOUNT; i++){
result = startChosen + notes[chordChosen][i];


 FREQ[i] =  numToFreq(result);
 
}


if (resetButton && waveChange > 250){
waveform++;
waveform = waveform % 4;
ledWrite(waveform);
AudioNoInterrupts();
waveform1.begin(0.9,FREQ[0],wave_type[waveform]);
waveform2.begin(0.9,FREQ[0],wave_type[waveform]);
waveform3.begin(0.9,FREQ[0],wave_type[waveform]);
waveform4.begin(0.9,FREQ[0],wave_type[waveform]);
  AudioInterrupts();
waveChange = 0;
}

    updateSines();
}





void updateSines(){
  AudioNoInterrupts();  

  sine1.frequency(FREQ[0]);
  sine2.frequency(FREQ[1]);
  sine3.frequency(FREQ[2]);
  sine4.frequency(FREQ[3]);
  
   waveform1.frequency(FREQ[0]);
   waveform2.frequency(FREQ[1]);
   waveform3.frequency(FREQ[2]);
   waveform4.frequency(FREQ[3]);

  sine1.amplitude(0);
  sine2.amplitude(00);
  sine3.amplitude(0);
  sine4.amplitude(0);
  
    waveform1.amplitude(0.9);
    waveform2.amplitude(0.9);    
    waveform3.amplitude(0.9);   
       waveform4.amplitude(0.9); 
  AudioInterrupts();

}




float numToFreq(int input) {
  int number = input - 21; // set to midi note numbers = start with 21 at A0 
  number = number - 48; // A0 is 48 steps below A4 = 440hz
  float result; 
  result = 440*(pow (1.059463094359,number));
  return result;   
}



// WRITE A 4 DIGIT BINARY NUMBER TO LED0-LED3 
void ledWrite(int n){
  digitalWrite(LED3, HIGH && (n==0));
  digitalWrite(LED2, HIGH && (n==1));
  digitalWrite(LED1, HIGH && (n==2));
  digitalWrite(LED0, HIGH && (n==3)); 
}

void checkInterface(){



  int chanPot = 0; 
  int chanCV = 0; 
  int timPot = 0; 
  int timCV = 0; 

int time;
int channel;

    chanPot = analogRead(CHAN_POT_PIN); 
    chanCV = analogRead(CHAN_CV_PIN); 
    timPot = analogRead(TIME_POT_PIN); 
    timCV = analogRead(TIME_CV_PIN); 

    time = timPot + timCV;   
  time = constrain(time, 0U, 1023U); 
    channel = chanPot + chanCV; 
  channel = constrain(channel, 0, 1023);
  
  startNote = time; 
  chordPick = channel; 
  
    if ( resetSwitch.update() ) {
    resetButton = resetSwitch.read();
    Serial.println(resetButton);
  }
  
}

