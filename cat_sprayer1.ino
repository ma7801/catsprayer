const int pirPin = 6;
const int sprayerPin = 7; 
const int LED1Pin = 2;
const int LED2Pin = 4;
const int buttonPin = 5;

const int pirIgnoreTime = 8;  // in seconds 
const int sprayDuration = 1;   // in seconds
const int disabledIncrement = 10; // in seconds  ***Low time for testing -- production time will be more like 300 seconds

const int buttonDelay = 350; //in milliseconds

unsigned long disableTimerStart, pirIgnoreTimerStart, sprayTimerStart;
bool spraying;
bool disabled;
bool ignoringPir;
int pirState;
int buttonState;
int LEDOnCount;
int disabledDuration;

int programStart;

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
  
  programStart = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  //DEBUG:
  /*if(!(millis() - programStart % 1000)) {
    Serial.print("running...\n");
  }*/

  /* Disable button code */
  // Get state of button pin
  buttonState = digitalRead(buttonPin);

  // If button pressed (inverted logic)
  if (buttonState == LOW) {
    
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

      // Set disabled duration to increment
      disabledDuration = disabledIncrement; 
      
    }
    
    // Case 1: One LED on; set disabled state to double the disable increment (and restart timer)
    else if (LEDOnCount == 1) {
      // Set LED counter to 2 (i.e. two LEDs on)
      LEDOnCount = 2;

      // Turn on LED2
      digitalWrite(LED2Pin, HIGH);

      // Start disable timer
      disableTimerStart = millis();
      
      // Set duration to double increment size
      disabledDuration = disabledIncrement * 2;

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

    // Delay so that a button press is not counted more than once in buttonDelay seconds
    delay(buttonDelay);

    // No need to run rest of code
    return;
  }
  
  /* END Disable button code */
 
  /* Sprayer code */
  
  // If currently spraying:
  if(spraying) {
    // If spraying timer has elapsed, stop spraying!
    if(secondsSince(sprayTimerStart) >= sprayDuration) {
      digitalWrite(sprayerPin, LOW);
      spraying = false;
      //DEBUG:
      Serial.print("stopped sprayer\n");
    }
    // Shouldn't need to run rest of code if spraying
    return;
  }
  
  
  // If currently in a disabled state:
  if (disabled) {
    //TODO
    //Need to add code that turns off LED2 when disabled timer goes above disabledIncrement value
    // If LED2 is on (longer duration), check to see if half the duration has elapsed and turn off LED2
    //  (disabledDuration - disabledIncrement = half the duration)
    if (LEDOnCount == 2 && secondsSince(disableTimerStart) >= disabledDuration - disabledIncrement) {
      digitalWrite(LED2Pin, LOW);
      LEDOnCount = 1;
    }
    
    // If disable timer has elapsed, turn off LED1, set disabled to false and continue  
    else if (secondsSince(disableTimerStart) >= disabledDuration) {
      // Turn off LED1
      digitalWrite(LED1Pin, LOW);
      disabled = false;
      LEDOnCount = 0;
    }
    // Otherwise don't run rest of code
    else return;
  }
  
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
    digitalWrite(sprayerPin, HIGH);
  }
  
}

int secondsSince(unsigned long timeMS) {
  return int((millis() - timeMS) / 1000);
}
