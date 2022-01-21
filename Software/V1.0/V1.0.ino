int NC = 9;
int NO = 10;
int PWM_input = A4;
int P3_out = 5;
int BTN_LED = 3;
int RLY = 4;
bool buttonPressed;
bool validTimeToChange;
unsigned long long int lastTimeButtonPressed;
double currBrightness;
unsigned long long int lastTimeFaded;
bool firstTimeOn;
unsigned long long int lastTimeBrightnessApplied;
unsigned long long int waitTime;
unsigned long duration;
int pwmMeasureTracker;

unsigned long long int runTimeCounter;



//LED fading parameters
#define fadeInterval 20 //ms between fades
//#define brightnessChunk 5 //how much we increment the led brightness per fade (out of 255 / the max brightness). Lower number has smoother fades, higher means faster fades
#define onOffWait 1000 //the time in ms that the fade stays on / off when it reaches the top of bottom of the fade curve

int maxBrightness = 255; //The brigness of leds when at top of fade cycle. a number from 0-255. 0 is off, 255 is full on.
int offBrightness = 15; //The brigness of leds when button is turned off. a number from 0-255. 0 is off, 255 is full on.
int minBrightness = offBrightness + 50; //The brigness of leds when at bottom of fade cycle. a number from 0-255. 0 is off, 255 is full on.


//state change parameters
#define minChangeTime 50 //The minimum number of ms that the relay for channel 1 stays on and off. (to prevent turning on
// and off like 10 times a second, which is bad for the electronics that are connected and the relay.

enum stateEnum {
  on,
  off
};
stateEnum state;

enum ledStateEnum {
  fadeOn,
  fadeOff,
  waitOff,
  waitOn
};

ledStateEnum LEDfade;

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  Serial.begin(9600);
  duration = 0;
  pwmMeasureTracker = 4; //measure immediately
  lastTimeButtonPressed = millis();
  validTimeToChange = true;
  state = off;
  currBrightness = offBrightness; //start wit led being dim, aka in the turned off mode
  lastTimeFaded = millis();
  firstTimeOn = true;
  runTimeCounter = millis();
  lastTimeBrightnessApplied = millis();
  waitTime = millis();

  pinMode(PWM_input, INPUT);
  pinMode(NC, INPUT_PULLUP);
  pinMode(NO, INPUT_PULLUP);
  pinMode(P3_out, OUTPUT);
  pinMode(BTN_LED, OUTPUT);
  pinMode(RLY, OUTPUT);

  digitalWrite(P3_out, LOW);
  digitalWrite(BTN_LED, LOW);
  digitalWrite(RLY, LOW);
  
}

// the loop function runs over and over again forever
void loop() {
  //printParams();
  
  analogWrite(BTN_LED, currBrightness); //always write the current brightness to the led

  stateLogic(); //determine when button is on/off

  determineBrightness(); //set the leg and p3 gauge brightness based off the car's brightness


  if (state == on) {
    pulseLED(); //Pulse led in the button when on
    digitalWrite(RLY, HIGH); //turn on the relay, and therefore heated wipers
  }

  if (state == off) {
    //make LED min brightness (should match the car's ambient lighting)
    currBrightness = offBrightness;
    digitalWrite(RLY, LOW); //turn off the relay
  }

}



void determineBrightness() {
  //we don't need to do this super frequently, and measuring the pulse may take up to
  // 27ms. So instead we do this once every 5 loop cycles to allow other stuff time.

  //skip measuring every 4/5 cycles
  if (pwmMeasureTracker < 4) {
    pwmMeasureTracker++;
    return;
  }

  duration = pulseIn(PWM_input, HIGH, 27000);

  if (duration == 0) {
    maxBrightness = 255;
    offBrightness = 0;
    minBrightness = 0;
    digitalWrite(P3_out, LOW);
  }

  else {
    int offset = map(duration, 1400, 10000, 15, 150);
    maxBrightness = map(duration, 1400, 10000, 4, 25) + offset; //The brigness of leds when at top of fade cycle. a number from 0-255. 0 is off, 255 is full on.
    minBrightness = map(duration, 1400, 10000, 4, 25); //The brigness of leds when button is turned off. a number from 0-255. 0 is off, 255 is full on.
    offBrightness = map(duration, 1400, 10000, 4, 25); //The brigness of leds when at bottom of fade cycle. a number from 0-255. 0 is off, 255 is full on.
    digitalWrite(P3_out, HIGH);
  }

  pwmMeasureTracker = 0;

}


void printParams() {
  //this is a function largely for debugging that allows us to print various aspects of
  //what we want to see when running the program
  char buffer[100];


  //Print the runtime per loop
  sprintf(buffer, "ms per loop: %i\n", (millis() - runTimeCounter));
  Serial.print(buffer);
  runTimeCounter = millis();
  


  //Current Brightness
  Serial.print("Current Brightness: ");
  Serial.println(currBrightness);


  // //print the state of the LEDfade variable Note: this doesnt work yet
  // if (LEDfade == waitOff) {
  //   Serial.println("LEDfade: waitOff");
  // }
  // else if (LEDfade == waitOn) {
  //   Serial.println("LEDfade: waitOn");
  // }
  // else if (LEDfade == fadeOff) {
  //   Serial.println("LEDfade: fadeOff");
  // }
  // else if (LEDfade == fadeOn) {
  //   Serial.println("LEDfade: fadeOn");
  // }
}


void pulseLED() {
  //pulse the led in the button when the button is turned on. Various parameters regarding this pulse are 
  //included as #DEFINE's in the header

  //the way the fade loop goes is:
  // waitOn -> fadeOff -> waitOff -> fadeOn (repeat)

  //we want the pulsing to happen at the same speed regardless of the absolute change
  //in brightness

  //we want:
  //fade on/off = 500ms each
  //wait at each end of fade = 1000ms each (onoffwait variable)

  double absChange = maxBrightness - minBrightness;

  //assuming each cycle is 10ms, we want to determine how much to increase the fade
  //each cycle to acheive the led fade on in the desired 500ms. (50 cycles)
  //Note: at current settings, the minimum absolute change = 21. 

  double brightnessChunk = absChange / 30;






  //if the button was just turned on, start the fade as fading down from max brightness
  if (firstTimeOn) {
    firstTimeOn = false;
    LEDfade = waitOn;
    waitTime = millis();
    currBrightness = maxBrightness;
  }


  //fade wait logic
  if ((millis() - waitTime) > onOffWait) {
    if (LEDfade == waitOff) {
      //wait, then change state to fade on
      LEDfade = fadeOn;
    }
    else if (LEDfade == waitOn) {
      //wait, then change state to fade off
      LEDfade = fadeOff;
    }
  }

  //Fading logic
  if ((millis() - lastTimeFaded) > fadeInterval) {
    //fade off
    if (LEDfade == fadeOff) {
      //fade down
      currBrightness -= brightnessChunk;
      lastTimeFaded = millis();

      //evaluate if we need to switch to fading up
      if (currBrightness < (minBrightness + brightnessChunk)) {
        currBrightness = minBrightness;
        LEDfade = waitOff;
        waitTime = millis();
      }
    }

    //fade on
    else if (LEDfade == fadeOn) {
      //fade up
      currBrightness += brightnessChunk;
      lastTimeFaded = millis();

      //evaluate if we need to switch to fading down
      if (currBrightness > (maxBrightness - brightnessChunk)) {
        LEDfade = waitOn;
        waitTime = millis();
      }
    }
  }
  
}


void stateLogic() {
  //determine the mode of the system (on/off) based off the sequence of pushes of the momentary button

  //Update condition variables
  buttonPressed = !(digitalRead(NO) == LOW);
  validTimeToChange = (millis() - lastTimeButtonPressed) > minChangeTime;

  if (buttonPressed && validTimeToChange) {
    //Switch the state
    if (state == on) {
      state = off;
    }
    else if (state == off) {
      state = on;
      firstTimeOn = true;
    }
  }

  //If button is pressed, reset the lastTimeButtonPressed
  if (buttonPressed){
    lastTimeButtonPressed = millis();
  }

}


