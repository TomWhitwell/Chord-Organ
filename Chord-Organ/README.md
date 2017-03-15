## Chord Organ Firmware V0.5

### Standard Features (No Configuration)

16 Chords with 8 notes (Voice 8 now fixed)

1v/Oct Tuning

Decoupled Root CV and Root Knob so they both use their own full range

### Extra Features (Requires config file settings)

##### 8 Extra Waves

Use `!WAVES`

The data for the waves is in the Waves.h header file and can be replaced with any 16 bit 256 point wave data.

##### Stacked Voices

Use `!STACK`

The first 4 voices will be doubled with a 0.1% frequency offset.

##### Glide with time setting

Use `!GLIDE` for default glide (50ms) or use 

`!GLIDE 80` for a custom glide time of 80ms.

Glide range is from 5 to 300 msecs.

#### Custom Note Range

The Root CV note range can be set from 12 to 72 semitones via the !RANGE setting, e.g.

`!RANGE 48`

To use 4 octaves.

### Config Generator / Firmware Binary
To generate a settings file and download the latest 
firmware binary please visit the [Config Generator Page](http://polyfather.com/chord_organ)

