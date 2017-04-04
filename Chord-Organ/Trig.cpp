#include "Arduino.h"
#include "Trig.h"

void Trig::init() {
	pinMode(TRIG_LED, OUTPUT);
    pinMode(TRIG_CV, OUTPUT);
}

void Trig::led(boolean high) {
    digitalWrite(TRIG_LED, high ? HIGH : LOW);
}

void Trig::update() {
	// Only if TRIG is being used as an input.
    if (!pulseHigh){
        trigCV.update();
        resetCVRose = trigCV.rose();
        if (resetCVRose) {
        	resetLedTimer = 0;
        }
        led(resetLedTimer < 20);
    } else if (pulseOutTimer > pulseTime) {
    	out(false);
    }
}

void Trig::out(boolean isHigh) {
	if(isHigh) {
//		Serial.println("TRIG OUT");
	}
	pulseHigh = isHigh;
	digitalWrite(TRIG_CV, isHigh ? HIGH : LOW);
	led(isHigh);
	if(isHigh) {
		pulseOutTimer = 0;
	}
}
