#include "Arduino.h"

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <EEPROM.h>

#include "Settings.h"
#include "Tuning.h"
#include "Waves.h"
#include "LedControl.h"
#include "AudioEngine.h"
#include "AnalogInput.h"
#include "Interface.h"
#include "Trig.h"

//#define REQUIRE_SD_CARD

//#define DEBUG_STARTUP

// Debug Flags
//#define CHECK_CPU

// REBOOT CODES
#define RESTART_ADDR       0xE000ED0C
#define READ_RESTART()     (*(volatile uint32_t *)RESTART_ADDR)
#define WRITE_RESTART(val) ((*(volatile uint32_t *)RESTART_ADDR) = (val))

elapsedMillis cpuCheckTimer;

// Current waveform index
int waveform = 0;
int waveformPage = 0;
int waveformPages = 1;

AudioEngine audioEngine;
Settings settings("CHORDORG.TXT");
Tuning tuning("TUNING.SCL");
LedControl ledControl;
Interface interface;
Trig trig;

void setup(){
#ifdef DEBUG_STARTUP
while( !Serial );
Serial.println("Starting");
#endif // DEBUG_STARTUP

    // SD CARD SETTINGS FOR MODULE
    SPI.setMOSI(7);
    SPI.setSCK(14);

	ledControl.init();
    trig.init();

#ifdef REQUIRE_SD_CARD
    openSDCard(true);
    settings.init(true);
#else
    settings.init(openSDCard(false));
#endif


    // Read waveform settings from EEPROM
    waveform = EEPROM.read(1234);
    if (waveform < 0) waveform = 0;

    waveformPages = settings.extraWaves ? 3 : 1;
    if(settings.extraWaves) {
        waveformPage = waveform >> 2;
    } else if(waveform > 3) {
        // If we read a custom waveform index from EEPROM
        // but they are not enabled in the config then change back to sine
        waveform = 0;
    }

    interface.init(&settings);
    tuning.init();
	audioEngine.init(&settings, tuning.createNoteMap(), waveform);

    ledControl.single(waveform % 4);

    if(waveformPage > 0) {
        ledControl.flash();
    }
}

boolean openSDCard(boolean rebootIfNotFound) {
    int crashCountdown = 0;
    if (!(SD.begin(10))) {
        while (!(SD.begin(10))) {
            ledControl.single(15);
            delay(20);
            ledControl.single(crashCountdown % 4);
            delay(20);
            crashCountdown++;
            if (crashCountdown > 4) {
            	if(rebootIfNotFound) {
            		reBoot(0);
            	} else {
            		return false;
            	}
            }
        }
    }
    return true;
}

void loop(){

    uint16_t state = interface.update();

    trig.update();

	int notesUpdate = state & (ROOT_NOTE_UPDATE | CHORD_INDEX_CHANGED);
	int buttonShortPress = state & BUTTON_SHORT_PRESS;

	if(state & BUTTON_VERY_LONG_PRESS) {
		// show all LEDs
		ledControl.multi(0xF);
		reBoot(50);
	}

    if(state & BUTTON_LONG_PRESS) {
    	audioEngine.stackedVoices = !audioEngine.stackedVoices;
    	notesUpdate = true;
    }

    if (notesUpdate) {
    	audioEngine.updateNotes(settings.notes[interface.chordIndex], interface.rootNoteCV);

		// Only glide if CV is quantised
        if(settings.glide && interface.quantiseRootCV) {
			audioEngine.startGlide();
        }
    }

    if (buttonShortPress){
    	nextWaveform();
    }

    if (buttonShortPress || notesUpdate)  {
        trig.out(true);
    }

    if(state || audioEngine.gliding) {
        audioEngine.update(waveform, interface.rootNotePot);
    }

    ledControl.bankAndSingle(waveformPage, waveform);

	#ifdef CHECK_CPU
    checkCPU();
	#endif // CHECK_CPU
}

void nextWaveform() {
    waveform++;
    waveform = waveform % (4 * waveformPages);
    selectWaveform(waveform);
}

void selectWaveform(int waveform) {
    waveformPage = waveform >> 2;
    if(waveformPage > 0) {
    	ledControl.flash();
    }
    ledControl.single(waveform % 4);
    EEPROM.write(1234, waveform);
}

void reBoot(int delayTime){
    if (delayTime > 0)
        delay (delayTime);
    WRITE_RESTART(0x5FA0004);
}

void checkCPU() {
    if (cpuCheckTimer > 100) {
    	int maxCPU = AudioProcessorUsageMax();
    	Serial.print("MaxCPU=");
    	Serial.println(maxCPU);
    	cpuCheckTimer = 0;
    }
}
