#include <Bounce.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

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
// REBOOT CODES 
#define RESTART_ADDR       0xE000ED0C
#define READ_RESTART()     (*(volatile uint32_t *)RESTART_ADDR)
#define WRITE_RESTART(val) ((*(volatile uint32_t *)RESTART_ADDR) = (val))

Bounce resetCV = Bounce( RESET_CV, 20 ); 
Bounce resetSwitch = Bounce( RESET_BUTTON, 20 ); // Bounce setup for Reset

boolean resetButton = false;

File root;
File settingsFile;

int chordCount = 16;
int notesSD[16][8] = {
    {
        999,999,999,999,999,999,999,999                                        }
    ,    
    {
        999,999,999,999,999,999,999,999                                         }
    ,    
    {
        999,999,999,999,999,999,999,999                                         }
    ,    
    {
        999,999,999,999,999,999,999,999                                         }
    ,    
    {
        999,999,999,999,999,999,999,999                                         }
    ,    
    {
        999,999,999,999,999,999,999,999                                         }
    ,    
    {
        999,999,999,999,999,999,999,999                                         }
    ,    
    {
        999,999,999,999,999,999,999,999                                         }
    ,    
    {
        999,999,999,999,999,999,999,999                                         }
    ,    
    {
        999,999,999,999,999,999,999,999                                         }
    ,    
    {
        999,999,999,999,999,999,999,999                                         }
    ,    
    {
        999,999,999,999,999,999,999,999                                         }
    ,    
    {
        999,999,999,999,999,999,999,999                                         }
    ,    
    {
        999,999,999,999,999,999,999,999                                         }
    ,    
    {
        999,999,999,999,999,999,999,999                                         }
    ,    
    {
        999,999,999,999,999,999,999,989                                        }
    ,    
};

#define SINECOUNT 8

short wave_type[4] = {
    WAVEFORM_SINE,
    WAVEFORM_SQUARE,
    WAVEFORM_SAWTOOTH,
    WAVEFORM_PULSE,
};
int waveform = 0; 
elapsedMillis waveChange; 

float FREQ[SINECOUNT] = {
    55,110, 220, 440, 880,1760,3520,7040};
float AMP[SINECOUNT] = { 
    0.9, 0.9, 0.9, 0.9,0.9, 0.9, 0.9, 0.9};

int startNote; 
int chordPick; 

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
    if (SD.exists("chorder.txt")) {
        readSDSettings();
    }
    else { 
        writeSDSettings();
        readSDSettings();
    };

    waveform1.pulseWidth(0.5);
    waveform1.begin(0.4, 220, WAVEFORM_SINE);
    waveform2.pulseWidth(0.5);
    waveform2.begin(0.4, 220, WAVEFORM_SINE);
    waveform3.pulseWidth(0.5);
    waveform3.begin(0.4, 220, WAVEFORM_SINE);
    waveform4.pulseWidth(0.5);
    waveform4.begin(0.4, 220, WAVEFORM_SINE);
    waveform5.pulseWidth(0.5);
    waveform5.begin(0.4, 220, WAVEFORM_SINE);
    waveform6.pulseWidth(0.5);
    waveform6.begin(0.4, 220, WAVEFORM_SINE);
    waveform7.pulseWidth(0.5);
    waveform7.begin(0.4, 220, WAVEFORM_SINE);
    waveform8.pulseWidth(0.5);
    waveform8.begin(0.4, 220, WAVEFORM_SINE);
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
boolean ResetCV;
boolean ASR = false; 
int ASRstep; 
elapsedMillis ASRtimeout;

void loop(){
    checkInterface();
    
    startChosen = map(startNote,0,1024,36,88);
    chordChosen = map(chordPick, 0, 1024, 0, chordCount);
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

        }   
    }
    else {
        for(int i = 0; i< SINECOUNT; i++){
            if (notesSD[chordChosen][i] != 999) {
                result = startChosen + notesSD[chordChosen][i];

                FREQ[i] =  numToFreq(result);
                AMP[i] = 0.9;
            }

            else{
                AMP[i] = 0;
            }

        }
                    Serial.print(" 0:");
            Serial.print(FREQ[0]);
            Serial.print(" 1:");
            Serial.print(FREQ[1]);
            Serial.print(" 2:");
            Serial.print(FREQ[2]);
            Serial.print(" 3:");
            Serial.println(FREQ[3]);
    }


    if (resetButton && waveChange > 250){
        waveform++;
        waveform = waveform % 4;
        ledWrite(waveform);
        AudioNoInterrupts();
        waveform1.begin(0.9,FREQ[0],wave_type[waveform]);
        waveform2.begin(0.9,FREQ[1],wave_type[waveform]);
        waveform3.begin(0.9,FREQ[2],wave_type[waveform]);
        waveform4.begin(0.9,FREQ[3],wave_type[waveform]);
        waveform5.begin(0.9,FREQ[4],wave_type[waveform]);
        waveform6.begin(0.9,FREQ[5],wave_type[waveform]);
        waveform7.begin(0.9,FREQ[6],wave_type[waveform]);
        waveform8.begin(0.9,FREQ[7],wave_type[waveform]);
        AudioInterrupts();
        waveChange = 0;

    }

    updateSines();
}





void updateSines(){
    AudioNoInterrupts();  

    waveform1.frequency(FREQ[0]);
    waveform2.frequency(FREQ[1]);
    waveform3.frequency(FREQ[2]);
    waveform4.frequency(FREQ[3]);
    waveform5.frequency(FREQ[4]);
    waveform6.frequency(FREQ[5]);
    waveform7.frequency(FREQ[6]);
    waveform8.frequency(FREQ[7]);

    waveform1.amplitude(AMP[0]);
    waveform2.amplitude(AMP[1]);    
    waveform3.amplitude(AMP[2]);   
    waveform4.amplitude(AMP[3]); 
    waveform5.amplitude(AMP[4]);
    waveform6.amplitude(AMP[5]);    
    waveform7.amplitude(AMP[6]);   
    waveform8.amplitude(AMP[7]); 

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
////VIA http://overskill.alexshu.com/?p=107 ///
//////////////////////////////////////////////




void readSDSettings(){

    // print out array 
    Serial.println("---start array - before ---");
    for (int a = 0; a< chordCount; a++){
        for(int i = 0; i< SINECOUNT; i++){
            //            Serial.print (a);
            //            Serial.print("/");
            //            Serial.print(i); 
            //            Serial.print(":");
            Serial.print(notesSD[a][i]);
            Serial.print("|");
        }
        Serial.println("");
    }
    Serial.println("---end array---");
    // end array printout 




    char character;
    int entry = 0;
    int note = 0;
    String settingValue;
    boolean inBracket = false;
    settingsFile = SD.open("chorder.txt");

    //    if (settingsFile) {
    while (settingsFile.available()) {


        character = settingsFile.read();   
        Serial.println("---");
        Serial.print("I have read:");
        Serial.println(character);

        if (character == '[') {
            Serial.print("Ignore this character, but start concentrating to fill line ");   
            Serial.println(entry);

            inBracket = true; 
        }

        if (character == ','  && inBracket){
            Serial.print("Ignore this character, write ");
            Serial.print (settingValue);
            Serial.print(" to array at "); 
            Serial.println(note); 

            notesSD[entry][note] = settingValue.toInt();
            settingValue = "";   
            note++;

        }

        if (character == ']'  && inBracket) {
            Serial.print("Ignore this character, write ");
            Serial.print (settingValue);
            Serial.print(" to array at "); 
            Serial.println(note); 
            notesSD[entry][note] = settingValue.toInt();
            settingValue = "";
            entry++;
            note = 0;
            inBracket = false;
        }

        else if (inBracket && character != '[' && character != ',' && character != ']'){
            settingValue += character; 
            Serial.print("add to store. Store is now ");
            Serial.println (settingValue);

        }

    }   
    //        character = settingsFile.read();
    //        Serial.print("---1:");
    //        Serial.println(character);
    //        while(character != '[' && settingsFile.available()){ // skip past anything not an open bracket 
    //            character = settingsFile.read();
    //            Serial.print("---2:");
    //            Serial.println(character);
    //
    //        }
    //
    //        while(character != ']' && settingsFile.available()){ // keep scanning until a close bracket 
    //            if (character != ','){
    //                settingValue += character;   
    //            }
    //            else{
    //
    //                Serial.print("A writing ");
    //                Serial.print(settingValue.toInt());
    //                Serial.print(" to row ");
    //                Serial.print(entry);
    //                Serial.print(" column ");
    //                Serial.println(note);
    //
    //                notesSD[entry][note] = settingValue.toInt();
    //                note++;
    //                settingValue = "";  
    //            }
    //            character = settingsFile.read();
    //            Serial.print("---3:");
    //            Serial.println(character);
    //        }
    //        if (character == ']'){
    //
    //            Serial.print("B writing ");
    //            Serial.print(settingValue.toInt());
    //            Serial.print(" to row ");
    //            Serial.print(entry);
    //            Serial.print(" column ");
    //            Serial.println(note);
    //
    //            notesSD[entry][note] = settingValue.toInt();
    //            settingValue = "";  
    //            note = 0;
    //            entry++;
    //        }
    //
    //    }
    //    // close the file:
    settingsFile.close();
    //    } 
    //    else {
    //        // if the file didn't open, print an error:
    //        Serial.println("error opening chorder.txt");
    //    }


    // print out array 
    Serial.println("---start array - after ---");

    for (int a = 0; a< chordCount; a++){
        for(int i = 0; i< SINECOUNT; i++){
            //            Serial.print (a);
            //            Serial.print("/");
            //            Serial.print(i); 
            //            Serial.print(":");
            Serial.print(notesSD[a][i]);
            Serial.print("|");

        }
        Serial.println("");
    }
    Serial.println("---end array---");
    // end array printout    
}



////// OLD VERSION 
////// OLD VERSION 
////// OLD VERSION 
////// OLD VERSION 
////// OLD VERSION 
////// OLD VERSION 
////// OLD VERSION 
////// OLD VERSION 
////// OLD VERSION 

void readSDSettingsOLD (){

    // print out array 
    Serial.println("---start array - before ---");

    for (int a = 0; a< chordCount; a++){
        for(int i = 0; i< SINECOUNT; i++){
            //            Serial.print (a);
            //            Serial.print("/");
            //            Serial.print(i); 
            //            Serial.print(":");
            Serial.print(notesSD[a][i]);
            Serial.print("|");

        }
        Serial.println("");
    }
    Serial.println("---end array---");


    // end array printout 




    char character;
    int entry = 0;
    int note = 0;
    String settingValue;
    settingsFile = SD.open("chorder.txt");

    //    if (settingsFile) {
    while (settingsFile.available()) {
        character = settingsFile.read();
        Serial.print("---1:");
        Serial.println(character);
        while(character != '[' && settingsFile.available()){ // skip past anything not an open bracket 
            character = settingsFile.read();
            Serial.print("---2:");
            Serial.println(character);

        }

        while(character != ']' && settingsFile.available()){ // keep scanning until a close bracket 
            if (character != ','){
                settingValue += character;   
            }
            else{

                Serial.print("A writing ");
                Serial.print(settingValue.toInt());
                Serial.print(" to row ");
                Serial.print(entry);
                Serial.print(" column ");
                Serial.println(note);

                notesSD[entry][note] = settingValue.toInt();
                note++;
                settingValue = "";  
            }
            character = settingsFile.read();
            Serial.print("---3:");
            Serial.println(character);
        }
        if (character == ']'){

            Serial.print("B writing ");
            Serial.print(settingValue.toInt());
            Serial.print(" to row ");
            Serial.print(entry);
            Serial.print(" column ");
            Serial.println(note);

            notesSD[entry][note] = settingValue.toInt();
            settingValue = "";  
            note = 0;
            entry++;
        }

    }
    // close the file:
    settingsFile.close();
    //    } 
    //    else {
    //        // if the file didn't open, print an error:
    //        Serial.println("error opening chorder.txt");
    //    }


    // print out array 
    Serial.println("---start array - after ---");

    for (int a = 0; a< chordCount; a++){
        for(int i = 0; i< SINECOUNT; i++){
            Serial.print (a);
            Serial.print("/");
            Serial.print(i); 
            Serial.print(":");
            Serial.print(notesSD[a][i]);
            Serial.print("|");

        }
        Serial.println("");
    }
    Serial.println("---end array---");
    // end array printout    
}

////// OLD VERSION 
////// OLD VERSION 
////// OLD VERSION 
////// OLD VERSION 
////// OLD VERSION 
////// OLD VERSION 
////// OLD VERSION 
////// OLD VERSION 







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
    SD.remove("chorder.txt");
    // Create new one
    settingsFile = SD.open("chorder.txt", FILE_WRITE);
    //  // writing in the file works just like regular print()/println() function
    settingsFile.println("Some intro guff here");

    settingsFile.println("[-12,0,12,24]");
    settingsFile.println("[0,4,7,12]");
    settingsFile.println("[0,3,7,12]");
    settingsFile.println("[0,3,7,11]");
    settingsFile.println("[0,4,5,7]");
    settingsFile.println("[0,4,7,14]");
    settingsFile.println("[0,2,4,7]");
    settingsFile.println("[0,3,6,9]");
    settingsFile.println("[0,7,12,0]");
    settingsFile.println("[0,4,7,11]");
    settingsFile.println("[0,3,7,10]");
    settingsFile.println("[0,3,7,9]");
    settingsFile.println("[0,4,6,10]");
    settingsFile.println("[0,5,7,10]");
    settingsFile.println("[0,4,7,9]");

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






























