#include <SD.h>
#include "Settings.h"

//#define DEBUG_CHORDS

Settings::Settings(const char* filename) {
  //create an array of filenames, detect them in the SD card
	_filename = filename;
}

void Settings::init(boolean hasSD) {
  //init chord bank values
    for (int i = 0; i < 16; i++) {
      for (int j = 0; j < 16; j++) {
        for (int k = 0; k < 8; k++) {
          chordBanks[i][j][k] = 255;
        }
      }
    }
    
	if (!hasSD) {
    // Configure defaults
    copyDefaults();
  } else {
      File root = SD.open("/");
      root.rewindDirectory();
      boolean filesFound = scanDirectory(root);
      if (filesFound == false) {
  #ifdef DEBUG_CHORDS
          Serial.println("Settings file not found, writing new settings");
  #endif
          //Write defaults
          write();
          read(_filename);
      }

      root.close();
  }
}

boolean Settings::scanDirectory(File dir) {
  boolean anyChordFilesFound = false;
  while(true) {
    File entry =  dir.openNextFile();
    if (!entry) {
      // no more files
      return anyChordFilesFound;
    }
    String fileName = (String)entry.name();
    entry.close();
    char fn[sizeof(fileName)];
    fileName.trim().toCharArray(fn, sizeof(fn));
    boolean checkExt = fileName.endsWith(".TXT");
    boolean checkName = fileName.startsWith("CHORD");

    if (checkExt && checkName) {
      
      #ifdef DEBUG_CHORDS
      Serial.print("Chord file found!: ");
      Serial.println(fileName);
      #endif
      
      anyChordFilesFound = true;
      
      if (chordFileCount > 15) {
        //stop scanning if the file number is out of bounds
        return anyChordFilesFound;
      }    
      
      #ifdef DEBUG_CHORDS
      Serial.print("Total Banks: ");
      Serial.println(chordFileCount);
      #endif
      
      //parse the current file, put it into the bank defined by chordFileCount
      read(fn);
      chordFileCount++;
    }
      
  }
  return anyChordFilesFound;
}

void Settings::copyDefaults() {
  for (int i = 0; i < 16; i++) {
      for (int j = 0; j < 16; j++) {
        for (int k = 0; k < 8; k++) {
          chordBanks[i][j][k] = defaultNotes[j][k];
        }
      }
  }

  for (int i = 0; i < 16; i++) {
    numChords[i] = 16;
  }
}

void Settings::read(const char* fn) {
    numChords[chordFileCount] = 0;

    char character;
    int note = 0;
    String settingValue;

    int NONE = 0;
    int CHORD = 1;
    int SETTING = 2;
    int state = NONE;

    //TO DO: read each file name in a loop here, then set chordBanks to the extracted notes

    settingsFile = SD.open(fn,FILE_READ);

    while (settingsFile.available()) {
        
        character = settingsFile.read();

        if (character == '[') {
            if(numChords[chordFileCount] < 16) {
                // Serial.println("Enter Chord");
                state = CHORD;
                continue;   
            }
        } else if(character == '!') {
            state = SETTING;
        }

        if(state == CHORD) {
            if (character == ',') {
            	#ifdef DEBUG_CHORDS
                Serial.print("Note ");
                Serial.println(settingValue.toInt());
                #endif
                chordBanks[chordFileCount][numChords[chordFileCount]][note] = settingValue.toInt();
                settingValue = "";   
                note++;
            } else if (character == ']') {
            	#ifdef DEBUG_CHORDS
                Serial.print("Note ");
                Serial.println(settingValue.toInt());
                #endif
                chordBanks[chordFileCount][numChords[chordFileCount]][note] = settingValue.toInt();
                settingValue = "";
                numChords[chordFileCount]++;
                note = 0;
                // Serial.println("End Chord");
                state = NONE;
            } else {
                settingValue += character;

                #ifdef DEBUG_CHORDS
                Serial.print("Character: ");
                Serial.println(character);
                Serial.print("SettingValue: ");
                Serial.println(settingValue);
                #endif  
            }

        } else if (state == SETTING) {
            if (character == '\n') {
                // 
                Serial.print("Config ");
                Serial.print(settingValue);
                Serial.println(".");

                if (settingValue.startsWith("!WAVES")) {
                    extraWaves = true;
                } else if (settingValue.startsWith("!GLIDE")) {
                    glide = true;
                    int spacePos = settingValue.indexOf(' ');
                    if (spacePos > 0) {
                        glideTime = settingValue.substring(spacePos).toInt();
                        if (glideTime < 5) glideTime = 5;
                        if (glideTime > 300) glideTime = 300;
                    }

                } else if (settingValue.startsWith("!RANGE")) {
                    int spacePos = settingValue.indexOf(' ');
                    if (spacePos > 0) {
                        noteRange = settingValue.substring(spacePos).toInt();
                        if (noteRange < 12) noteRange = 12;
                        if (noteRange > 72) noteRange = 72;
                    }
                } else if (settingValue.startsWith("!STACK")) {
                	stacked = true;
                } else {
                    Serial.print("Unknown option:");
                    Serial.print(settingValue);
                    Serial.println(":");
                }
                settingValue = "";
                state = NONE;
            } else {
                settingValue += character;
            }
        }
    }  
    settingsFile.close();
}

// converting string to Float
float Settings::toFloat(String settingValue) {
    char floatbuf[settingValue.length()];
    settingValue.toCharArray(floatbuf, sizeof(floatbuf));
    float f = atof(floatbuf);
    return f;
}

// Converting String to integer and then to boolean
// 1 = true
// 0 = false
boolean Settings::toBoolean(String settingValue) {
    if (settingValue.toInt() == 1) {
        return true;
    } 
    else {
        return false;
    }
}


void Settings::write() {
    // Delete the old One
    SD.remove(_filename);
    // Create new one
    settingsFile = SD.open(_filename, FILE_WRITE);
    //  // writing in the file works just like regular print()/println() function

    settingsFile.println("o   o o   o  o-o  o-O-o   o-o   o-O-o o  o o-O-o o   o  o-o ");
    settingsFile.println("|\\ /| |   | |       |    /        |   |  |   |   |\\  | o    ");
    settingsFile.println("| O | |   |  o-o    |   O         |   O--O   |   | \\ | |  -o ");
    settingsFile.println("|   | |   |     |   |    \\        |   |  |   |   |  \\| o   | ");
    settingsFile.println("o   o  o-o  o--o  o-O-o   o-o     o   o  o o-O-o o   o  o-o ");
    settingsFile.println("");
    settingsFile.println("  o-o o  o  o-o  o--o  o-o        o-o  o--o   o-o    O  o   o ");
    settingsFile.println(" /    |  | o   o |   | |  \\      o   o |   | o      / \\ |\\  | ");
    settingsFile.println("O     O--O |   | O-Oo  |   O     |   | O-Oo  |  -o o---o| \\ | ");
    settingsFile.println(" \\    |  | o   o |  \\  |  /      o   o |  \\  o   | |   ||  \\| ");
    settingsFile.println("  o-o o  o  o-o  o   o o-o        o-o  o   o  o-o  o   oo   o");
    settingsFile.println("");
    settingsFile.println("Edit chord shapes into the spaces below.");
    settingsFile.println("No more than 16 chords and up to 8 notes per chord.");
    settingsFile.println("Anything outside the square brackets is ignored");
    settingsFile.println("Reduce clicks on chord changes by giving all chords");
    settingsFile.println("the same number of notes.");
    settingsFile.println("");

    settingsFile.println("1  [0,4,7,12,0] Major");
    settingsFile.println("2  [0,3,7,12,0] Minor");
    settingsFile.println("3  [0,4,7,11,0] Major 7th");
    settingsFile.println("4  [0,3,7,10,0] Minor 7th");
    settingsFile.println("5  [0,4,7,11,14] Major 9th");
    settingsFile.println("6  [0,3,7,10,14] Minor 9th");
    settingsFile.println("7  [0,5,7,12,0] Suspended 4th");
    settingsFile.println("8  [0,7,12,0,7] Power 5th");
    settingsFile.println("9  [0,5,12,0,5] Power 4th");
    settingsFile.println("10 [0,4,7,8,0] Major 6th");
    settingsFile.println("11 [0,3,7,8,0] Minor 6th");
    settingsFile.println("12 [0,3,6,0,3] Diminished");
    settingsFile.println("13 [0,4,8,0,4] Augmented");
    settingsFile.println("14 [0,0,0,0,0] Root");
    settingsFile.println("15 [-12,-12,0,0,0] Sub Octave");
    settingsFile.println("16 [-12,0,0,12,24] 2 up 1 down octaves");

    // close the file:
    settingsFile.close();
    //Serial.println("Writing done.");	
}
