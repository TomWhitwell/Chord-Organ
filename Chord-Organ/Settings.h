#include <SD.h>
#ifndef Settings_h
#define Settings_h

class Settings {
  public:
    Settings(const char* filename);
    void init(boolean hasSD);
    boolean scanDirectory(File dir);
    void read(const char* fn);
    void write();
    
  	float toFloat(String settingValue);
  	boolean toBoolean(String settingValue);

    const char* FILE_TYPE = "TXT";

	  boolean stacked = false;
    boolean extraWaves = false;
    boolean glide = false;
    int glideTime = 50;
    uint8_t noteRange = 38;
    int numChords[16] = {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15};
    int16_t chordBanks[16][16][8];
    int chordFileCount = 0;
    
	int16_t defaultNotes[16][8] = {
	    {        
	        0,4,7,12,0,255,255,255}
	    ,    
	    {        
	        0,3,7,12,0,255,255,255}
	    ,    
	    {   
	        0,4,7,11,0,255,255,255}
	    ,    
	    {
	        0,3,7,10,0,255,255,255}
	    ,    
	    {
	        0,4,7,11,14,255,255,255}
	    ,    
	    {
	        0,3,7,10,14,255,255,255}
	    ,    
	    {
	        0,5,7,12,0,255,255,255}
	    ,    
	    {
	        0,7,12,0,7,255,255,255}
	    ,    
	    {
	        0,5,12,0,5,255,255,255}
	    ,    
	    {
	        0,4,7,8,0,255,255,255}
	    ,    
	    {
	        0,3,7,8,0,255,255,255}
	    ,    
	    {
	        0,3,6,0,3,255,255,255}
	    ,    
	    {
	        0,4,8,0,4,255,255,255}
	    ,    
	    {
	        0,0,0,0,0,255,255,255}
	    ,    
	    {
	        -12,-12,0,0,0,255,255,255}
	    ,    
	    {
	        -12,0,0,12,24,255,255,255}
	    ,    
	};
  
  private:
    const char* _filename;
    File settingsFile;
    void copyDefaults();
};

#endif
