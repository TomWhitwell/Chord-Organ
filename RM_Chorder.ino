#include <Bounce.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#define CHAN_POT_PIN 9 // pin for Channel pot
#define CHAN_CV_PIN 6 // pin for Channel CV 
#define TIME_POT_PIN 7 // pin for Time pot
#define TIME_CV_PIN 8 // pin for Time CV
#define RESET_BUTTON 8 // Reset button 
#define RESET_LED 11 // Reset LED indicator 
#define RESET_CV 9 // Reset pulse input 
#define BANK_BUTTON 2 // Bank Button 
#define LED0 6
#define LED1 5
#define LED2 4
#define LED3 3
// REBOOT CODES 
#define RESTART_ADDR       0xE000ED0C
#define READ_RESTART()     (*(volatile uint32_t *)RESTART_ADDR)
#define WRITE_RESTART(val) ((*(volatile uint32_t *)RESTART_ADDR) = (val))
#define SINECOUNT 8

int notesSD[16][8] = {
    {        
        999,999,999,999,999,999,999,999                                                                                }
    ,    
    {        
        999,999,999,999,999,999,999,999                                                                                 }
    ,    
    {   
        999,999,999,999,999,999,999,999                                                                                 }
    ,    
    {
        999,999,999,999,999,999,999,999                                                                                 }
    ,    
    {
        999,999,999,999,999,999,999,999                                                                                 }
    ,    
    {
        999,999,999,999,999,999,999,999                                                                                 }
    ,    
    {
        999,999,999,999,999,999,999,999                                                                                 }
    ,    
    {
        999,999,999,999,999,999,999,999                                                                                 }
    ,    
    {
        999,999,999,999,999,999,999,999                                                                                 }
    ,    
    {
        999,999,999,999,999,999,999,999                                                                                 }
    ,    
    {
        999,999,999,999,999,999,999,999                                                                                 }
    ,    
    {
        999,999,999,999,999,999,999,999                                                                                 }
    ,    
    {
        999,999,999,999,999,999,999,999                                                                                 }
    ,    
    {
        999,999,999,999,999,999,999,999                                                                                 }
    ,    
    {
        999,999,999,999,999,999,999,999                                                                                 }
    ,    
    {
        999,999,999,999,999,999,999,989                                                                                }
    ,    
};




Bounce resetCV = Bounce( RESET_CV, 20 ); 
Bounce resetSwitch = Bounce( RESET_BUTTON, 20 ); // Bounce setup for Reset
boolean resetButton = false;
File root;
File settingsFile;
int chordCount = 16;
short wave_type[4] = {
    WAVEFORM_SINE,
    WAVEFORM_SQUARE,
    WAVEFORM_SAWTOOTH,
    WAVEFORM_PULSE,
};
int waveform = 0; 
float FREQ[SINECOUNT] = {
    55,110, 220, 440, 880,1760,3520,7040};
float AMP[SINECOUNT] = { 
    0.9, 0.9, 0.9, 0.9,0.9, 0.9, 0.9, 0.9};
int startNote; 
int chordPick; 
int startChosen;
int chordChosen;
int startOld;
int chordOld;
boolean changed = true;
boolean ResetCV;
boolean ASR = true; 
int ASRstep; 
elapsedMillis ASRtimeout;
elapsedMillis waveChange; 
elapsedMillis noteChange; 

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform1;      //xy=215,232
AudioSynthWaveform       waveform2;      //xy=243,295
AudioSynthWaveform       waveform3;      //xy=273,354
AudioSynthWaveform       waveform4;      //xy=292,394
AudioSynthWaveform       waveform5;      //xy=215,232
AudioSynthWaveform       waveform6;      //xy=243,295
AudioSynthWaveform       waveform7;      //xy=273,354
AudioSynthWaveform       waveform8;      //xy=292,394
AudioMixer4              mixer1;         //xy=424,117
AudioMixer4              mixer2;         //xy=424,181
AudioMixer4              mixer3;         //xy=571,84
AudioEffectEnvelope      envelope1;      //xy=652,281
AudioOutputAnalog        dac1;           //xy=784,129
AudioConnection          patchCord1(waveform1, 0, mixer2, 0);
AudioConnection          patchCord2(waveform2, 0, mixer2, 1);
AudioConnection          patchCord7(waveform3, 0, mixer2, 2);
AudioConnection          patchCord8(waveform4, 0, mixer2, 3);
AudioConnection          patchCord3(waveform5, 0, mixer1, 2);
AudioConnection          patchCord4(waveform6, 0, mixer1, 3);
AudioConnection          patchCord5(waveform7, 0, mixer1, 0);
AudioConnection          patchCord6(waveform8, 0, mixer1, 1);
AudioConnection          patchCord9(mixer1, 0, mixer3, 0);
AudioConnection          patchCord10(mixer2, 0, mixer3, 1);
AudioConnection          patchCord11(mixer3, envelope1);
AudioConnection          patchCord12(envelope1, dac1);
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
    delay(2000);
    Serial.begin(9600);
    Serial.println("starting");
    ledWrite(waveform);
    // SD CARD SETTINGS FOR MODULE 
    SPI.setMOSI(7);
    SPI.setSCK(14);
    // OPEN SD CARD 
    int crashCountdown = 0; 
    if (!(SD.begin(10))) {
        while (!(SD.begin(10))) {
            ledWrite(15);
            delay(100);
            ledWrite(0);
            delay(100);
            crashCountdown++;
            if (crashCountdown > 6)     
                reBoot(500);
        }
    }
    // READ SETTINGS FROM SD CARD 
    root = SD.open("/");  
    if (SD.exists("CHORDORG.TXT")) {
        readSDSettings();
    }
    else { 
        writeSDSettings();
        readSDSettings();
    };

    waveform1.pulseWidth(0.2);
    waveform2.pulseWidth(0.5);
    waveform3.pulseWidth(0.5);
    waveform4.pulseWidth(0.5);
    waveform5.pulseWidth(0.5);
    waveform6.pulseWidth(0.5);
    waveform7.pulseWidth(0.5);
    waveform8.pulseWidth(0.5);
    mixer1.gain(0,0.98);
    mixer1.gain(1,0.98);
    mixer1.gain(2,0.98);
    mixer1.gain(3,0.98);
    mixer2.gain(0,0.98);
    mixer2.gain(1,0.98);
    mixer2.gain(2,0.98);
    mixer2.gain(3,0.98);
    mixer3.gain(0,0.49);
    mixer3.gain(1,0.49);
    mixer3.gain(2,0);
    mixer3.gain(3,0);
    envelope1.attack(2);
    envelope1.decay(1);
    envelope1.sustain(1.0);
    envelope1.release(2);
}


void loop(){
    checkInterface();

    startChosen = map(startNote,0,1024,36,88);
    chordChosen = map(chordPick, 0, 1024, 0, chordCount);
    if (startChosen != startOld || chordChosen != chordOld) changed = true; 
    startOld = startChosen; 
    chordOld = chordChosen; 


    int result;

    if (ASR){

        if (ResetCV == 1 && ASRtimeout > 40){
            Serial.println("inside asr loop");
            //    result = startChosen + notesSD[chordChosen][ASRstep];
            result = startChosen ;
            FREQ[ASRstep] =  numToFreq(result);
            ASRstep++;
            ASRstep = ASRstep % 4;
            ASRtimeout = 0; 
            Serial.print(" 0:");
            Serial.print(FREQ[0]);
            Serial.print(" 1:");
            Serial.print(FREQ[1]);
            Serial.print(" 2:");
            Serial.print(FREQ[2]);
            Serial.print(" 3:");
            Serial.println(FREQ[3]);
            changed = true;

        }   
    }
    else if (changed) {
        float voiceCount = 0;
        float voiceTotal = 0;
        for(int i = 0; i< SINECOUNT; i++){
            if (notesSD[chordChosen][i] != 999) {
                result = startChosen + notesSD[chordChosen][i];
                FREQ[i] =  numToFreq(result);
                voiceCount++;
            }
        }
        for (int i = 0; i< SINECOUNT; i++){
            if (notesSD[chordChosen][i] != 999) {
                AMP[i] = 0.95/voiceCount;
                voiceTotal += 0.95/voiceCount;
            }
            else{
                AMP[i] = 0;   
            }
        }
        Serial.println(voiceTotal);
    }


    if (resetButton && waveChange > 250){
        waveform++;
        waveform = waveform % 4;
        ledWrite(waveform);
        updateSines();
        waveChange = 0;

    }
    if (changed)  {
        updateSines();
        changed = false;

    }
}





void updateSines(){
            envelope1.noteOff();
        delay(3);

    AudioNoInterrupts();  
    waveform1.begin(AMP[0],FREQ[0],wave_type[waveform]);
    waveform2.begin(AMP[1],FREQ[1],wave_type[waveform]);
    waveform3.begin(AMP[2],FREQ[2],wave_type[waveform]);
    waveform4.begin(AMP[3],FREQ[3],wave_type[waveform]);
    waveform5.begin(AMP[4],FREQ[4],wave_type[waveform]);
    waveform6.begin(AMP[5],FREQ[5],wave_type[waveform]);
    waveform7.begin(AMP[6],FREQ[6],wave_type[waveform]);
    waveform8.begin(AMP[7],FREQ[7],wave_type[waveform]); 
    AudioInterrupts();
        envelope1.noteOn();
        delay(3);

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

    for (int i = 0; i<10; i++){

        chanPot += analogRead(CHAN_POT_PIN); 
        chanCV += analogRead(CHAN_CV_PIN); 
        timPot += analogRead(TIME_POT_PIN); 
        timCV += analogRead(TIME_CV_PIN); 

    }

    chanPot  = chanPot/10; 
    chanCV = chanCV/10; 
    timPot = timPot/10; 
    timCV = timCV/10; 



    time = timPot + timCV;   
    time = constrain(time, 0U, 1023U); 
    channel = chanPot + chanCV; 
    channel = constrain(channel, 0, 1023);

    startNote = time; 
    chordPick = channel; 

    resetSwitch.update();
    resetButton = resetSwitch.read();

    if ( resetCV.update() ) {
        //if (resetCV.read()) ResetCV = !ResetCV;
        ResetCV = resetCV.risingEdge();
        //        Serial.println(ResetCV);
        digitalWrite(RESET_LED, ResetCV);
    }




}

//////////////////////////////////////////////
////READ AND WRITE SETTINGS ON THE SD CARD ///
//////////////////////////////////////////////


void readSDSettings(){
    char character;
    int entry = 0;
    int note = 0;
    String settingValue;
    boolean inBracket = false;
    settingsFile = SD.open("CHORDORG.TXT");
    while (settingsFile.available()) {
        character = settingsFile.read();   
        if (character == '[') {
            inBracket = true; 
        }
        if (character == ','  && inBracket){
            notesSD[entry][note] = settingValue.toInt();
            settingValue = "";   
            note++;
        }

        if (character == ']'  && inBracket) {
            notesSD[entry][note] = settingValue.toInt();
            settingValue = "";
            entry++;
            note = 0;
            inBracket = false;
        }

        else if (inBracket && character != '[' && character != ',' && character != ']'){
            settingValue += character; 
        }

    }   
    settingsFile.close();
}


// converting string to Float
float toFloat(String settingValue){
    char floatbuf[settingValue.length()];
    settingValue.toCharArray(floatbuf, sizeof(floatbuf));
    float f = atof(floatbuf);
    return f;
}
// Converting String to integer and then to boolean
// 1 = true
// 0 = false
boolean toBoolean(String settingValue) {
    if(settingValue.toInt()==1){
        return true;
    } 
    else {
        return false;
    }
}
// Writes A Configuration file
void writeSDSettings() {
    // Delete the old One
    SD.remove("CHORDORG.TXT");
    // Create new one
    settingsFile = SD.open("CHORDORG.TXT", FILE_WRITE);
    //  // writing in the file works just like regular print()/println() function
    settingsFile.println("Edit chord shapes into the spaces below. No more than 16 chords up to 8 notes per chord");

    settingsFile.println("1  [0,4,7,12]");
    settingsFile.println("2  [0,3,7,12]");
    settingsFile.println("3  [0,3,7,11]");
    settingsFile.println("4  [0,4,5,7]");
    settingsFile.println("5  [0,4,7,14]");
    settingsFile.println("6  [0,2,4,7]");
    settingsFile.println("7  [0,3,6,9]");
    settingsFile.println("8  [0,7,12]");
    settingsFile.println("9  [0,4,7,11]");
    settingsFile.println("10 [0,3,7,10]");
    settingsFile.println("11 [0,3,7,9]");
    settingsFile.println("12 [0,4,6,10]");
    settingsFile.println("13 [0,5,7,10]");
    settingsFile.println("14 [0,4,7,9]");
    settingsFile.println("15 [0,4,7,9]");
    settingsFile.println("16 [-36,-24,-12,0,12,24,36,48]");

    //


    // close the file:
    settingsFile.close();
    //Serial.println("Writing done.");
}


void reBoot(int delayTime){
    if (delayTime > 0)
        delay (delayTime);
    WRITE_RESTART(0x5FA0004);
}







































