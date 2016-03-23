
volatile unsigned char _pin;
volatile unsigned char isSignal;
volatile unsigned char flags[2] = {0,0};

unsigned int getTimer() {
  unsigned char sreg;
  unsigned int value;
  sreg = SREG;
  cli();
  value = TCNT1; // read the timer
  TCNT1 = 0x00; // reset the timer
  SREG = sreg;
  return value;
}

void EnableRead(unsigned char onOff) {
  if (onOff) {
    attachInterrupt(digitalPinToInterrupt(_pin), process, CHANGE);
  } else {
    detachInterrupt(digitalPinToInterrupt(_pin));
  }
}

void process() {
  unsigned int count;
  flags[1] = digitalRead(_pin);
  if (flags[0] != flags[1]) {
    // state really changed
    count = getTimer();
    if (flags[0] == 1) {
      isSignal = (count>67) && (count<70);
      digitalWrite(13, isSignal);
    }
  }
  flags[0] = flags[1];
}

void setListenPin(unsigned char pin) {
  // Configure Timer
  TCCR1A = 0x00;
  TCCR1B = 0x03; // prescale to 64
  TCCR1C = 0x00;

  // Configure read pin
  _pin = pin;
  pinMode(pin, INPUT);
}

void setup() {
  setListenPin(0);
  pinMode(13, OUTPUT);
  digitalWrite(13,0);
}

void loop() {
  // put your main code here, to run repeatedly:

}
