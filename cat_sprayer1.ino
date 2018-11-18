const int pirPin = 6;
const int sprayerPin = 3;
const int LED1Pin = 2;
const int LED2Pin = 4;
const int buttonPin = 5;
const int threeVolts = 255;  // for analogWrite PWM -- should out 3V

const int pirIgnoreTime = 5;  // in seconds 
const int sprayDuration = 20;   // in seconds
const int disabledIncrement = 5; // in minutes
const int buttonBounceDelay = 250; //in milliseconds

unsigned long disableTimerStart, pirIgnoreTimerStart, sprayTimerStart;
bool spraying;
bool disabled;
bool ignoringPir;
int pirState;
int buttonState;
int LEDOnCount;
int disabledDuration;

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);  // Activate internal pull-up resistor for push button pin; LOW means button pushed
  pinMode(sprayerPin, OUTPUT);
  pinMode(LED1Pin, OUTPUT);
  pinMode(LED2Pin, OUTPUT);
  // pirPin set to INPUT by default

  spraying = false;
  disabled = false;
  ignoringPir = false;
  pirState = LOW;
  buttonState = HIGH; // Inverted logic
  LEDOnCount = 0;
  disabledDuration = 0;

}

void loop() {
  // put your main code here, to run repeatedly:

  /* Disable button code 
  // Get state of button pin
  buttonState = digitalRead(buttonPin);

  // If button pressed (inverted logic)
  if (buttonState == LOW) {
    // Delay because of potential bounce
    delay(buttonBounceDelay);
    // Reset the button state
    buttonState = HIGH;

    // Case 0: No LEDs currently on (sprayer enabled);  put in a disabled state and start disable timer
    if(LEDOnCount == 0) {
      // Set disabled state to true
      disabled = true;

      // Set LED counter to 1 (i.e. one LED on)
      LEDOnCount = 1;
      
      // Turn on LED1
      digitalWrite(LED1Pin, HIGH);

      // Start disable timer
      disableTimerStart = millis(); 

      // Set disabled duration to increment * 60 seconds
      disabledDuration = disabledIncrement * 60; 
      
      // No need to run rest of code
      return;
    }
    
    // Case 1: One LED on; set disabled state to double the disable increment (and restart timer)
    else if (LEDOnCount == 1) {
      // Set LED counter to 2 (i.e. two LEDs on)
      LEDOnCount = 2;

      // Turn on LED2
      digitalWrite(LED2Pin, HIGH);

      // Start disable timer
      disableTimerStart = millis();
      
      // Set duration to double increment size * 60 seconds
      disabledDuration = disabledIncrement * 2 * 60;

      // No need to run rest of code
      return;
    }

    // Case 2: Both LEDs on; take out of disabled state 
    else if (LEDOnCount == 2) {
      // Set counter to 0; no LEDs on
      LEDOnCount = 0;

      // Turn off both LEDs
      digitalWrite(LED1Pin, LOW);
      digitalWrite(LED2Pin, LOW);

      // Reset duration; probably unnecessary
      disabledDuration = 0;

      // Set disabled state to false
      disabled = false;
    }
  }
  
  /* END Disable button code */
 
  /* Sprayer code */
  
  // If currently spraying:
  if(spraying) {
    // If spraying timer has elapsed, stop spraying!
    if(secondsSince(sprayTimerStart) >= sprayDuration) {
      analogWrite(sprayerPin, 0);
      spraying = false;
      //DEBUG:
      Serial.print("stopped sprayer\n");
    }
    // Shouldn't need to run rest of code if spraying
    return;
  }
  
  /*
  // If currently in a disabled state:
  if (disabled) {
    //TODO
    //Need to add code that turns off LED2 when disabled timer goes above disabledIncrement value
    
    // If disable timer has elapsed, turn off LED1, set disabled to false and continue  
    if (secondsSince(disableTimerStart) >= disabledDuration) {
      // Turn off LED1
      digitalWrite(LED1Pin, LOW);
      disabled = false;
    }
    // Otherwise don't run rest of code
    else return;
  }
  */
  // If ignoring due to PIR ignore delay:
  if (ignoringPir) {
    // See if timer for ignoring has elapsed
    if (secondsSince(pirIgnoreTimerStart) >= pirIgnoreTime) ignoringPir = false;
    // Otherwise don't run rest of code
    else return;
  }
  
  // Read state of pin connected to motion sensor (PIR)
  pirState = digitalRead(pirPin);
  
  // If motion detector activated
  if(pirState == HIGH) {
    //DEBUG:
    Serial.print("motion detected\n");
    
    // Reset pir state
    pirState = LOW;
    
    // Start a timer to ignore pir pin, set ignoring state to true
    pirIgnoreTimerStart = millis();
    ignoringPir = true;
    
    // Start a timer for the sprayer, set spraying state to true, apply 3V to the spray pin
    sprayTimerStart = millis();
    spraying = true;
    analogWrite(sprayerPin, threeVolts);
  }
}

int secondsSince(unsigned long timeMS) {
  return int((millis() - timeMS) / 1000);
}
