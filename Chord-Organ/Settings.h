#ifndef Settings_h
#define Settings_h

#include <SD.h>

class Settings {
  public:
    Settings(const char* filename);
    void init(boolean hasSD);
    void read();
    void write();
	void printDebug();
	float toFloat(String settingValue);
	boolean toBoolean(String settingValue);
	boolean stacked = false;
    boolean extraWaves = false;
    boolean glide = false;
    boolean quantiseRootCV = true;
    boolean quantiseRootPot = true;

    uint8_t lowNote = 36;
    uint16_t glideTime = 50;
    uint8_t noteRange = 39;
    uint8_t numChords = 0;

    // Initialise Array with 255s, to identify unfilled elements when reading from SD card
	int16_t notes[16][8] = {
	    {255,255,255,255,255,255,255,255},
		{255,255,255,255,255,255,255,255},
		{255,255,255,255,255,255,255,255},
		{255,255,255,255,255,255,255,255},
		{255,255,255,255,255,255,255,255},
		{255,255,255,255,255,255,255,255},
		{255,255,255,255,255,255,255,255},
		{255,255,255,255,255,255,255,255},
		{255,255,255,255,255,255,255,255},
		{255,255,255,255,255,255,255,255},
		{255,255,255,255,255,255,255,255},
		{255,255,255,255,255,255,255,255},
		{255,255,255,255,255,255,255,255},
		{255,255,255,255,255,255,255,255},
		{255,255,255,255,255,255,255,255},
		{255,255,255,255,255,255,255,255}
	};
	int16_t defaultNotes[16][8] = {
	    {0,4,7,12,0,255,255,255},
	    {0,3,7,12,0,255,255,255},
	    {0,4,7,11,0,255,255,255},
	    {0,3,7,10,0,255,255,255},
	    {0,4,7,11,14,255,255,255},
	    {0,3,7,10,14,255,255,255},
	    {0,5,7,12,0,255,255,255},
	    {0,7,12,0,7,255,255,255},
	    {0,5,12,0,5,255,255,255},
	    {0,4,7,8,0,255,255,255},
	    {0,3,7,8,0,255,255,255},
	    {0,3,6,0,3,255,255,255},
	    {0,4,8,0,4,255,255,255},
	    {0,0,0,0,0,255,255,255},
	    {-12,-12,0,0,0,255,255,255},
	    {-12,0,0,12,24,255,255,255}
	};	
  private:
    const char* _filename;
    File settingsFile;
    void copyDefaults();
};

#endif
