#include "AudioEngine.h"

#include <Audio.h>

void AudioEngine::init(Settings* settings, float* tuning, int waveform) {

    AudioMemory(50);

    setGlideTime(settings->glideTime);
    stackedVoices = settings->stacked;

    for(int i=0;i<NUM_OSCILLATORS;i++) {
        oscillator[i]->pulseWidth(0.5);
    }

    for(int m=0;m<4;m++) {
        mixer1.gain(m,0.25);
        mixer2.gain(m,0.25);
    }

    mixer3.gain(0,0.49);
    mixer3.gain(1,0.49);
    mixer3.gain(2,0);
    mixer3.gain(3,0);

    envelope1.attack(1);
    envelope1.decay(1);
    envelope1.sustain(1.0);
    envelope1.release(1);
    envelope1.noteOn();

    tuningTable = tuning;

    setWaveform(waveform);
}


void AudioEngine::updateNotes(int16_t* noteOffsets, float transpose) {
	if(!stackedVoices) {
		updateNotesAndAmp(noteOffsets, transpose);
	} else {
		updateNotesAndAmpStacked(noteOffsets, transpose);
	}
}

void AudioEngine::update(int waveform, float transpose) {
    if(gliding) {
    	if(glideTimer >= glideTime) {
    		gliding = false;
    	}
    }

	AudioNoInterrupts();
    if(waveform != currentWaveform) {
    	currentWaveform = waveform;
    	changeWave();
    }
    updateOscillators(transpose);
    updateMixers(waveform);
    AudioInterrupts();
}

void AudioEngine::setGlideTime(int time) {
	if(time < 5) time = 5;
	if(time > 2000) time = 2000;
	glideTime = time;
	oneOverGlideTime = 1.0 / (float) glideTime;
}

void AudioEngine::startGlide() {
	glideTimer = 0;
	gliding = true;
}

float AudioEngine::getNoteFrequency(float noteNumber, boolean quantise) {
	float newFreq;
	if(noteNumber < 0) noteNumber = 0;
	if(noteNumber > 127) noteNumber = 127;

	if(quantise) {
		newFreq = tuningTable[(int)noteNumber];
	} else {
		int floorNote = floor(noteNumber);
		float noteDelta = noteNumber - floorNote;
		if(floorNote < 127) {
			float f1 = tuningTable[floorNote];
			float f2 = tuningTable[floorNote+1];
			// Linear interpolation between notes.
			newFreq = f1 + (noteDelta * (f2 - f1));
		} else {
			newFreq = tuningTable[127];
		}

	}

	return newFreq;
}

void AudioEngine::setWaveform(int waveform) {
	currentWaveform = waveform;
    AudioNoInterrupts();
    changeWave();
    AudioInterrupts();
}

// Should be called during an AudioNoInterrupts() section
void AudioEngine::changeWave() {
    if(currentWaveform > 3) {
    	setupCustomWaveform();
    } else {
    	setWaveformType(standardWaveTypes[currentWaveform]);
    }
}

void AudioEngine::setupCustomWaveform() {
    int waveIndex = (currentWaveform - 4) % 8;

    const int16_t* wave = waveTables[waveIndex];
    for(int i=0;i<NUM_OSCILLATORS;i++) {
        oscillator[i]->arbitraryWaveform(wave, MAX_FREQ);
    }

    setWaveformType(WAVEFORM_ARBITRARY);
}

void AudioEngine::setWaveformType(short waveformType) {
    for(int i=0;i<NUM_OSCILLATORS;i++) {
        oscillator[i]->begin(1.0, getNoteFrequency(notes[i],true),waveformType);
    }
}

void AudioEngine::updateMixers(int waveIndex) {
	float waveAmp = waveformAmp[waveIndex];
    mixer1.gain(0,amp[0] * waveAmp);
    mixer1.gain(1,amp[1] * waveAmp);
    mixer1.gain(2,amp[2] * waveAmp);
    mixer1.gain(3,amp[3] * waveAmp);
    mixer2.gain(0,amp[4] * waveAmp);
    mixer2.gain(1,amp[5] * waveAmp);
    mixer2.gain(2,amp[6] * waveAmp);
    mixer2.gain(3,amp[7] * waveAmp);
}

void AudioEngine::updateOscillators(float transpose) {

	float f;
	float dt = 0.0;

	if(gliding) {
		dt = 1.0 - (glideTimer * oneOverGlideTime);
		if(dt < 0.0) {
			dt = 0.0;
			gliding = false;
		}
	} else {
		dt = 0.0;
	}

	for(int i=0;i<NUM_OSCILLATORS;i++) {
		currentNotes[i] = notes[i] - (dt * noteDeltas[i]);
		f = getNoteFrequency(currentNotes[i] + transpose, false);
		oscillator[i]->frequency(f);
	}
}

void AudioEngine::updateNotesAndAmp(int16_t* noteOffsets, float transpose) {

    int voiceCount = 0;
	for(int i = 0; i< NUM_OSCILLATORS; i++){
		if (noteOffsets[i] != 255) {
			float targetNote = noteOffsets[i] + transpose;

			notes[i] = targetNote;
			noteDeltas[i] = targetNote - currentNotes[i];

			voiceCount++;
		}
	}

    float perVoice = ampPerVoice[voiceCount-1];
    float totalAmp = 0;

	for (int i = 0; i< NUM_OSCILLATORS; i++){
		if (noteOffsets[i] != 255) {
			amp[i] = perVoice;
			totalAmp += perVoice;
		}
		else{
			amp[i] = 0.0;
		}
	}
}


void AudioEngine::updateNotesAndAmpStacked(int16_t* noteOffsets, float transpose) {
    int halfSineCount = NUM_OSCILLATORS>>1;
    int voiceCount = 0;

    for(int i=0;i < halfSineCount;i++) {
        if (noteOffsets[i] != 255) {
			float targetNote = noteOffsets[i] + transpose;

			notes[i] = targetNote;
			notes[i + halfSineCount] = targetNote * stackNoteScale;

			noteDeltas[i] = targetNote - currentNotes[i];
			noteDeltas[i + halfSineCount] = (targetNote * stackNoteScale) - currentNotes[i];

            voiceCount += 2;
        }
    }
    float perVoice = ampPerVoice[voiceCount-1];
    float totalAmp = 0;

    for (int i = 0; i < halfSineCount; i++){
        if (noteOffsets[i] != 255) {
            amp[i] = perVoice;
            amp[i + halfSineCount] = perVoice;
            totalAmp += perVoice;
        }
        else{
            amp[i] = 0.0;
        }
    }

}
