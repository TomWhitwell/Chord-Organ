#ifndef Interface_h
#define Interface_h

#include "AnalogInput.h"
#include "Settings.h"
#include <Bounce2.h>

#define CHORD_POT_PIN 9 // pin for Chord pot
#define CHORD_CV_PIN 6 // pin for Chord CV
#define ROOT_POT_PIN 7 // pin for Root Note pot
#define ROOT_CV_PIN 8 // pin for Root Note CV

#define WAVEFORM_BUTTON 8 // Reset button

// Update state flags
#define ROOT_CV_CHANGED 		1
#define ROOT_POT_CHANGED 		2
#define ROOT_NOTE_CHANGED 		4
#define CHORD_CV_CHANGED 		8
#define CHORD_POT_CHANGED 		16
#define CHORD_INDEX_CHANGED 	32
#define BUTTON_SHORT_PRESS		64
#define BUTTON_LONG_PRESS		128
#define BUTTON_VERY_LONG_PRESS	256
#define ROOT_NOTE_UPDATE		512

#define SHORT_PRESS_DURATION 10
#define LONG_PRESS_DURATION 1000
#define VERY_LONG_PRESS_DURATION 5000

class Interface {
public:
	boolean quantiseRootCV = true;
	boolean quantiseRootPot = true;

	float rootNoteCV = 36;
	float rootNotePot = 36;
	float rootNote = 36;
	int chordIndex = 0;

	Interface() :
		rootCVInput  (ROOT_CV_PIN),
		rootPotInput (ROOT_POT_PIN),
		chordCVInput (CHORD_CV_PIN),
		chordPotInput(CHORD_POT_PIN)
		{

		}

	void init(Settings* settings);
	uint16_t update();
	void setChordCount(int chordCount);

private:
	AnalogInput rootCVInput;
	AnalogInput rootPotInput;

	AnalogInput chordCVInput;
	AnalogInput chordPotInput;

	Bounce waveButtonBounce;

	elapsedMillis buttonTimer = 0;
	boolean buttonHeld = false;

	int chordCount = 0;
	int chordIndexOld = -1000;
	float rootNoteOld = 0;
	float rootNotePotOld = 0;
	float rootNoteCVOld = 0;

	uint16_t updateRootControls();
	uint16_t updateChordControls();
	uint16_t updateButton();
};

#endif
