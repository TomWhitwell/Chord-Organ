#ifndef AudioEngine_h
#define AudioEngine_h

#include <Audio.h>
#include "Waves.h"
#include "Settings.h"

#define NUM_OSCILLATORS 8
#define MAX_FREQ 600


class AudioEngine {
public:
	AudioEngine() :
		patchCord1(waveform1, 0, mixer1, 0),
		patchCord2(waveform2, 0, mixer1, 1),
		patchCord3(waveform3, 0, mixer1, 2),
		patchCord4(waveform4, 0, mixer1, 3),
		patchCord5(waveform5, 0, mixer2, 0),
		patchCord6(waveform6, 0, mixer2, 1),
		patchCord7(waveform7, 0, mixer2, 2),
		patchCord8(waveform8, 0, mixer2, 3),
		patchCord9(mixer1, 0, mixer3, 0),
		patchCord10(mixer2, 0, mixer3, 1),
		patchCord11(mixer3, envelope1),
		patchCord12(envelope1, dac1)
	{
	    oscillator[0] = &waveform1;
	    oscillator[1] = &waveform2;
	    oscillator[2] = &waveform3;
	    oscillator[3] = &waveform4;
	    oscillator[4] = &waveform5;
	    oscillator[5] = &waveform6;
	    oscillator[6] = &waveform7;
	    oscillator[7] = &waveform8;

	    tuningTable = NULL;
	}

	boolean stackedVoices = false;

	boolean gliding = false;

	void init(Settings* settings, float* tuningTable, int waveform);

	void update(int waveform, float transpose);
	void updateNotes(int16_t* noteOffsets, float transpose);
	void startGlide();
	void setGlideTime(int time);

private:
	AudioSynthWaveform       waveform1;
	AudioSynthWaveform       waveform2;
	AudioSynthWaveform       waveform3;
	AudioSynthWaveform       waveform4;
	AudioSynthWaveform       waveform5;
	AudioSynthWaveform       waveform6;
	AudioSynthWaveform       waveform7;
	AudioSynthWaveform       waveform8;
	AudioMixer4              mixer1;
	AudioMixer4              mixer2;
	AudioMixer4              mixer3;
	AudioEffectEnvelope      envelope1;
	AudioOutputAnalog        dac1;
	AudioConnection          patchCord1;
	AudioConnection          patchCord2;
	AudioConnection          patchCord3;
	AudioConnection          patchCord4;
	AudioConnection          patchCord5;
	AudioConnection          patchCord6;
	AudioConnection          patchCord7;
	AudioConnection          patchCord8;
	AudioConnection          patchCord9;
	AudioConnection          patchCord10;
	AudioConnection          patchCord11;
	AudioConnection          patchCord12;
	AudioSynthWaveform* 	 oscillator[NUM_OSCILLATORS];

	short standardWaveTypes[4] = {
	    WAVEFORM_SINE,
	    WAVEFORM_SQUARE,
	    WAVEFORM_SAWTOOTH,
	    WAVEFORM_PULSE,
	};

	float waveformAmp[12] = {
	  0.8,0.6,0.8,0.6,
	  0.8,0.8,0.8,0.8,
	  0.8,0.8,0.8,0.8,
	};

	int16_t const* waveTables[8] {
	    wave1,
	    wave7,
	    wave3,
	    wave4,

	    wave8,
	    wave9,
	    wave10,
	    wave11
	};

	// Target note of each oscillator
	float notes[NUM_OSCILLATORS] = {
		36,36,36,36,36,36,36,36};

	// Distance to target note since last trigger
	float noteDeltas[NUM_OSCILLATORS] = {
		0,0,0,0,0,0,0,0};

	// Notes currently playing
	float currentNotes[NUM_OSCILLATORS] = {
		36,36,36,36,36,36,36,36};

	// Oscillator volumes
	float amp[NUM_OSCILLATORS] = {
	    0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9};

	// Volume for a single voice for each chord size
	float ampPerVoice[NUM_OSCILLATORS] = {
	  0.4,0.3,0.22,0.2,0.15,0.15,0.13,0.12};

	float* tuningTable;
	float stackNoteScale = 1.001;
	int currentWaveform = 0;

	// msecs glide time.
	uint32_t glideTime = 50;
	// keep reciprocal
	float oneOverGlideTime = 0.02;
	// Time since glide started
	elapsedMillis glideTimer = 0;

	void setWaveform(int waveform);
	void setWaveformType(short waveformtType);
	void setupCustomWaveform();
	void changeWave();

	void updateNotesAndAmp(int16_t* noteOffsets, float transpose);
	void updateNotesAndAmpStacked(int16_t* noteOffsets, float transpose);
	void updateMixers(int waveIndex);
	void updateOscillators(float transpose);

	float getNoteFrequency(float noteNumber, boolean quantise);

};

#endif
