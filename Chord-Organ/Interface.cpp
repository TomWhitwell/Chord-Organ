#include "Interface.h"

#include "Arduino.h"

void Interface::init(Settings* settings) {

    analogReadRes(ADC_BITS);
    pinMode(WAVEFORM_BUTTON, INPUT);

	uint16_t bounceInterval = 5;
	waveButtonBounce.attach(WAVEFORM_BUTTON);
	waveButtonBounce.interval(bounceInterval);

    setChordCount(settings->numChords);

    quantiseRootCV = settings->quantiseRootCV;
    quantiseRootPot = settings->quantiseRootPot;

    float lowNote = settings->lowNote + 0.5;
    rootCVInput.setRange(lowNote, lowNote + settings->noteRange, settings->quantiseRootCV);
    rootPotInput.setRange(0.0,48, settings->quantiseRootPot);
}

void Interface::setChordCount(int chords) {

	chordCount = chords;
    chordCVInput.setRange(0.0,(float)chords, true);
    chordPotInput.setRange(0.0,(float)chords, true);
}

// Return value is bit map of changes / state
uint16_t Interface::update(){

	uint16_t chordChanged = updateChordControls();
	uint16_t rootChanged = updateRootControls();
	uint16_t buttonState = updateButton();

	uint16_t state = chordChanged | rootChanged | buttonState;

	if(quantiseRootCV && (state & ROOT_NOTE_CHANGED)) {
		state |= ROOT_NOTE_UPDATE;
	} else if(state & ROOT_CV_CHANGED) {
		state |= ROOT_NOTE_UPDATE;
	}

    return state;
}

uint16_t Interface::updateChordControls() {

	chordCVInput.update();
	chordPotInput.update();

	chordIndex = (int) constrain(chordCVInput.currentValue + chordPotInput.currentValue, 0, chordCount - 1);

	uint16_t chordChanged = 0;

    if (chordIndex != chordIndexOld){
        chordChanged |= CHORD_INDEX_CHANGED;
        chordIndexOld = chordIndex;
    }

    return chordChanged;
}

// return bitmap of state of changes for CV, Pot and combined Note.
uint16_t Interface::updateRootControls() {

	uint16_t change = 0;

	boolean cvChanged = rootCVInput.update();
	boolean potChanged = rootPotInput.update();

    // early out if no changes
    if(!cvChanged && !potChanged) {
    	return change;
    }

    float rootPot = rootPotInput.currentValue;
    float rootCV = rootCVInput.currentValue;

    if(cvChanged) {
    	if(quantiseRootCV) {
        	rootNoteCV = floor(rootCV);
        	if(rootNoteCV != rootNoteCVOld) {
        		change |= ROOT_CV_CHANGED;
        	}
    	} else {
    		rootNoteCV = rootCV;
    		change |= ROOT_CV_CHANGED;
    	}
    }

    if(potChanged) {
    	if(quantiseRootPot) {
        	rootNotePot = floor(rootPot);
        	if(rootNotePot != rootNotePotOld) {
        		change |= ROOT_POT_CHANGED;
        	}
    	} else {
    		rootNotePot = rootPot;
    		change |= ROOT_POT_CHANGED;
    	}
    }

	rootNote = rootNoteCV + rootNotePot;

    // Flag note changes when the note index itself changes
    if(floor(rootNote) != rootNoteOld) {
    	change |= ROOT_NOTE_CHANGED;
    	rootNoteOld = floor(rootNote);
    }

	return change;
}

uint16_t Interface::updateButton() {
	waveButtonBounce.update();
	uint16_t buttonState = 0;

	// Button pressed
	if(waveButtonBounce.rose()) {
		buttonTimer = 0;
		buttonHeld = true;
	}

    if(waveButtonBounce.fell()) {
    	// button has been held down for some time
        if (buttonTimer >= SHORT_PRESS_DURATION && buttonTimer < LONG_PRESS_DURATION){
        	buttonState |= BUTTON_SHORT_PRESS;
        } else if(buttonTimer > LONG_PRESS_DURATION) {
        	buttonState |= BUTTON_LONG_PRESS;
        }
        buttonHeld = false;
        buttonTimer = 0;
    }

    if(buttonHeld) {
    	if(buttonTimer > VERY_LONG_PRESS_DURATION) {
			buttonState |= BUTTON_VERY_LONG_PRESS;
		}
    }
    return buttonState;
}
