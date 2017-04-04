#ifndef LedControl_h
#define LedControl_h

/**
 * Chord Organ LED control
 *
 * 4 LEDs across the top
 *
 */
#define LED0 6
#define LED1 5
#define LED2 4
#define LED3 3

class LedControl {

	public:
		LedControl();
		void init();
		void single(int index);
		void multi(uint8_t bits);
		void bankAndSingle(int bank, int index);
		void flash();
	private:
		boolean flashingBank = false;
		elapsedMillis bankFlashTimer = 0;
		uint32_t bankLedFlashTimes[3] = {400,100,30};

};
#endif
