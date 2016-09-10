
/* 
 DESCRIPTION
 ====================
 Simple example of the Bounce library that switches the debug LED when a button is pressed.
 */
// Include the Bounce2 library found here :
// https://github.com/thomasfredericks/Bounce-Arduino-Wiring
#include <Bounce2.h>

#define BUTTON_PIN 9
#define LED_PIN 6
#define LED0 6
#define LED1 5
#define LED2 4
#define LED3 3

// Instantiate a Bounce object
Bounce debouncer = Bounce(); 

void setup() {

  // Setup the button with an internal pull-up :
  pinMode(BUTTON_PIN,INPUT_PULLUP);

  // After setting up the button, setup the Bounce instance :
  debouncer.attach(BUTTON_PIN);
  debouncer.interval(5); // interval in ms

  //Setup the LED :
  pinMode(LED_PIN,OUTPUT);
    pinMode(LED0,OUTPUT);
    pinMode(LED1,OUTPUT);
    pinMode(LED2,OUTPUT);
    pinMode(LED3,OUTPUT);
}

void loop() {
  // Update the Bounce instance :
  debouncer.update();

    digitalWrite(LED0, debouncer.read() );
    digitalWrite(LED1, debouncer.fell() );
    digitalWrite(LED2, debouncer.rose() );
delay(10);

}


