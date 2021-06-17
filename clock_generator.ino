// 9ms seems to be the shortest time in which the digital out rises over the required 3.3V
int TIMEDELTA = 9;   // resolution in ms
bool DEBUG = true;
int OPIN1 = 7;
int OPIN2 = 8;
int OPIN3 = 4;
int OPIN_SYNC = 9;
int IPIN_GATEMODE = 2;
int IPIN_RESET = 3;
int IPIN_TEMPO = 2;

int STEP = 20;   // time in TIMEDELTA * ms it takes for the clock to advance
int STEP_MIN = 2;
int STEP_MAX = 50;
int clock0 = 0;
int clock1 = 0;
int clock2 = 0;
int division1 = 8;
int division2 = 4;

bool gatemodebutton_pressed = false;
bool gatemode = false;
bool resetbutton_pressed = false;


void setup() {
  // put your setup code here, to run once:
  pinMode(OPIN1, OUTPUT);
  pinMode(OPIN2, OUTPUT);
  pinMode(OPIN3, OUTPUT);
  pinMode(OPIN_SYNC, OUTPUT);
  pinMode(IPIN_GATEMODE, INPUT);
  pinMode(IPIN_RESET, INPUT);
  if (DEBUG) Serial.begin(9600);
}

void loop() {
  // check if the gatemode button is being released
  int buttonState_gate = digitalRead(IPIN_GATEMODE);
  if (buttonState_gate == HIGH) {
    gatemodebutton_pressed = true;
  } else {
    if (gatemodebutton_pressed) {
      // the button was pressed and is now released
      gatemode = !gatemode;
      gatemodebutton_pressed = false;
    }
  }
  // check if the reset button is being released
  int buttonState_reset = digitalRead(IPIN_RESET);
  if (buttonState_reset == HIGH) {
    resetbutton_pressed = true;
  } else {
    if (resetbutton_pressed) {
      // the button was pressed and is now released
      resetbutton_pressed = false;
      // reset the clocks
      clock0 = 0;
      clock1 = 0;
      clock2 = 0;
    }
  }
  // set tempo according to knob
  int tempoval = analogRead(IPIN_TEMPO);
  STEP = map(tempoval, 0, 1023, STEP_MIN, STEP_MAX);
  if (DEBUG) Serial.println(tempoval);

  if (clock0 == 0) {
    digitalWrite(OPIN_SYNC, HIGH);
  }
  if (clock1 == 0) {
    digitalWrite(OPIN1, HIGH);
  }
  else if (gatemode && clock1 > STEP * division1 / 2) {
    digitalWrite(OPIN1, LOW);
  }
  if (clock2 == 0) {
    digitalWrite(OPIN2, HIGH);
  }
  else if (gatemode && clock2 > STEP * division2 / 2) {
    digitalWrite(OPIN2, LOW);
  }
  if (clock1 == 0 || clock2 == 0) {
    digitalWrite(OPIN3, HIGH);
  }
  else if (gatemode 
    && ((clock1 > STEP * division1 / 2 && clock2 > STEP * division2 / 2)
    || clock1 == STEP * division1 - 1  // turn the gate off just before a new gate on
    || clock2 == STEP * division2 - 1)
  ) {
    digitalWrite(OPIN3, LOW);
  }
  
  delay(TIMEDELTA);
  
  if (!gatemode) {  // if gatemode is off, turn off signal immediately
    digitalWrite(OPIN1, LOW);
    digitalWrite(OPIN2, LOW);
    digitalWrite(OPIN3, LOW);
  }
  digitalWrite(OPIN_SYNC, LOW);

  // move clocks forward
  clock0 = (clock0 + 1) % STEP;
  clock1 = (clock1 + 1) % (STEP * division1);
  clock2 = (clock2 + 1) % (STEP * division2);
}
