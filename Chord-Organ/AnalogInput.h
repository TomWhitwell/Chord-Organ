#ifndef AnalogInput_h
#define AnalogInput_h

#include "Arduino.h"

#define ADC_BITS 13
#define ADC_MAX_VALUE 8192

class AnalogInput {
	public:
		AnalogInput(uint pinIndex);
		// Update with hysteresis and filtering. Return changed flag
		boolean update();
		// Update using raw input value. Always returns true.
		boolean updateRaw();
		void setRange(float outLow, float outHigh);
		void setChangeTolerance(int tolerance);
		void setSmoothPower(int power);
		void useSmoothing(boolean s);
		void printDebug();
		float getRatio();
		float currentValue = 0.0;
	private:
		int pin;
		float outputLow = 0.0;
		float outputHigh = 1.0;
		float inToOutRatio = 0.0;
		float inverseRatio = 0.0;
		int inputValue = 0;
		// Set out of range to trigger a change status on first call.
		int oldInputValue = -1000;
		int valueAtLastChange = -1000;
		boolean smooth = false;

		// If the absolute input delta is greater than this then use the input value
		// otherwise the old value is filtered towards the new value
		int changeTolerance = 16;

		// If input doesn't change past 'changeTolerance' level then the
		// input delta is shifted left by 'smoothPower' bits added to the old value
		int smoothPower = 5;
};

#endif
