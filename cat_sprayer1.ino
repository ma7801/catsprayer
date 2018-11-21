const int pirPin = 6;
const int sprayerPin = 7; 
const int LED1Pin = 2;
const int LED2Pin = 4;
const int buttonPin = 5;
const int acceptingInputLEDPin = 13;

const int delayAfterEnable = 8;  // in seconds - pause value after going out of disabled state AND at power on
const int pirIgnoreTime = 7;  // in seconds 
const int sprayDuration = 1;   // in seconds
const int disabledIncrement = 300; // in seconds  

const int buttonDelay = 350; //in milliseconds

unsigned long disableTimerStart, pirIgnoreTimerStart, sprayTimerStart;
bool spraying;
bool disabled;
bool ignoringPir;
int pirState;
int buttonState;
int LEDOnCount;
int disabledDuration;

void setup() {
  //Serial.begin(9600);
  
  pinMode(sprayerPin, OUTPUT);
  pinMode(LED1Pin, OUTPUT);
  pinMode(LED2Pin, OUTPUT);
  // pirPin and buttonPin set to INPUT by default

  spraying = false;
  ignoringPir = false;
  pirState = LOW;
  buttonState = LOW; 
  LEDOnCount = 0;
  disabledDuration = 0;

  // Start off disabled (initializes enabled LED to false)
  disableSprayer(); 
  
  // Enable the sprayer (i.e. put in a "waiting for motion" state) -- function also includes the delay after power on
  enableSprayer();
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

  // If button pressed 
  if (buttonState == HIGH) {
    
    // Reset the button state
    buttonState = LOW;

    // Case 0: No LEDs currently on (sprayer enabled);  put in a disabled state and start disable timer
    if(LEDOnCount == 0) {
      // Set disabled state to true
      disableSprayer();

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

      // Re-enable sprayer
      enableSprayer();
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
      //Serial.print("stopped sprayer\n");
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
      LEDOnCount = 0;

      // Re-enable sprayer
      enableSprayer();
    }
    // Otherwise don't run rest of code
    else return;
  }
  
  // If ignoring due to PIR ignore delay:
  if (ignoringPir) {
    // See if timer for ignoring has elapsed
    if (secondsSince(pirIgnoreTimerStart) >= pirIgnoreTime) {
      ignoringPir = false;
      // Turn on the accepting input LED
      digitalWrite(acceptingInputLEDPin, HIGH);
    }
    // Otherwise don't run rest of code
    else return;
  }
  
  // Read state of pin connected to motion sensor (PIR)
  pirState = digitalRead(pirPin);
  
  // If motion detector activated
  if(pirState == HIGH) {
    //DEBUG:
    //Serial.print("motion detected\n");
    
    // Reset pir state
    pirState = LOW;
    
    // Start a timer to ignore pir pin, set ignoring state to true, turn off accepting input LED
    pirIgnoreTimerStart = millis();
    ignoringPir = true;
    digitalWrite(acceptingInputLEDPin, LOW);    
    
    // Start a timer for the sprayer, set spraying state to true, apply 3V to the spray pin
    sprayTimerStart = millis();
    spraying = true;
    digitalWrite(sprayerPin, HIGH);
  }
  
}

int secondsSince(unsigned long timeMS) {
  return int((millis() - timeMS) / 1000);
}

void enableSprayer() {
  disabled = false;
  delay(delayAfterEnable * 1000);
  digitalWrite(acceptingInputLEDPin, HIGH);
}

void disableSprayer() {
  disabled = true;
  digitalWrite(acceptingInputLEDPin, LOW);
}
