#include "Arduino.h"

// Combined CV + Pot

// 2 Pins

// 13 bit + 13 bit => LOW_VAL -> HIGH_VAL

// Separate Analogue In

// 13 bit => LOW_VAL -> HIGH_VAL

// Mapping function isn't uniform, i.e. output may have a half step at top and bottom of range
#include "AnalogInput.h"

AnalogInput::AnalogInput(uint pinIndex) {

	pin = pinIndex;
}

void AnalogInput::setRange(float outLow, float outHigh) {
	// Check the range is the right way around
	if(outLow > outHigh) {
		outputLow = outHigh;
		outputHigh = outLow;
	} else {
		outputLow = outLow;
		outputHigh = outHigh;
	}

	// Use range + 1 so that the top value has equal coverage across the input range
	float range = (outputHigh - outputLow) + 1;
	inToOutRatio = (float) ADC_MAX_VALUE / range;

	// Keep the inverse ratio so we only multiply during update
	inverseRatio = 1.0 / inToOutRatio;

	currentValue = outLow;
}

void AnalogInput::setChangeTolerance(int tolerance) {
	changeTolerance = tolerance;
}

void AnalogInput::setSmoothPower(int power) {
	if(power < 1) power = 1;
	if(power > 12) power = 12;

	smoothPower = power;
}

void AnalogInput::useSmoothing(boolean s) {
	smooth = s;
}

float AnalogInput::getRatio() {
	return inToOutRatio;
}

boolean AnalogInput::update() {
	boolean changed = false;
	inputValue = analogRead(pin);
	inputValue = constrain(inputValue,0, ADC_MAX_VALUE - 1);

	if(smooth) {
		int delta = inputValue - valueAtLastChange;

		if(abs(delta) > changeTolerance) {
			changed = true;
			valueAtLastChange = oldInputValue = inputValue;
		} else {
			oldInputValue += (delta >> smoothPower);
			inputValue = oldInputValue;
		}
	} else {
		changed = true;
		valueAtLastChange = inputValue;
	}

	currentValue = (inputValue * inverseRatio) + outputLow;

	return changed;
}

void AnalogInput::printDebug() {
	Serial.print("I ");
	Serial.print(pin);
	Serial.print("\t");
	Serial.println(inputValue);
}

