volatile unsigned char _pin;
volatile unsigned char _noopy_isSignal;
volatile unsigned char _noopy_flags[2] = {0, 0};
volatile unsigned long _noopy_command[2];
volatile unsigned char _noopy_commandCursor = 0;
volatile unsigned long _noopy_controller;
volatile unsigned int _noopy_device;
volatile unsigned char _noopy_onOff;
volatile unsigned char _noopy_global;
volatile unsigned long _noopy_commandFrame;
volatile void* _noopy_callback=0;

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

void EnableRead(unsigned char onOffState) {
  if (onOffState) {
    _noopy_controller = 0;
    _noopy_device = 0;
    _noopy_onOff = 0;
    _noopy_global = 0;
    _noopy_commandFrame = 0;
    _noopy_command[0] = 0;
    _noopy_command[1] = 0;
    _noopy_commandCursor = 0;
    _noopy_isSignal = 0;
    _noopy_flags[0] = 0;
    _noopy_flags[1] = 0;
    attachInterrupt(digitalPinToInterrupt(_pin), process, CHANGE);
  } else {
    detachInterrupt(digitalPinToInterrupt(_pin));
  }
}

void decode_noopy_command(unsigned long __noopy_command) {
  _noopy_commandFrame =  __noopy_command;
  _noopy_controller = (__noopy_command & 0xffffffc0) >> 6;
  _noopy_device = __noopy_command & 0xf;
  _noopy_onOff = (__noopy_command & 0x10) >> 4;
  _noopy_global = (__noopy_command & 0x20) >> 5;
}

void process() {
  unsigned int count;
  _noopy_flags[1] = digitalRead(_pin);
  if (_noopy_flags[0] != _noopy_flags[1]) {
    // state really changed
    count = getTimer();
    // check High level
    if (_noopy_flags[0] == 1) {
      _noopy_isSignal = (count>55) && (count<65);
    }
    // compute Low level
    if ((_noopy_flags[0] == 0) && (_noopy_isSignal)) {
      //Serial.println(count);
      if (count>2400) {
        // Start Emit
      }
      if ((count>600) && (count<700)) {
        // Start / End of frame
        _noopy_commandCursor = 0;
        if ((_noopy_command[0]) && (_noopy_command[0] == _noopy_command[1])) {
          decode_noopy_command(_noopy_command[0]);
        }
        _noopy_command[1] = _noopy_command[0];
        _noopy_command[0] = 0;
      }
      if ((count>50) && (count<100)) {
        if (!(_noopy_commandCursor & 1)) {
          unsigned long _bit = 1;
          _noopy_command[0] &= ~(_bit << (31 - _noopy_commandCursor/2));
        }
        _noopy_commandCursor++;
      }
      if ((count>300) && (count<350)) {
        if (!(_noopy_commandCursor & 1)) {
          unsigned long _bit = 1;
          _noopy_command[0] |= _bit << (31 - _noopy_commandCursor/2);
        }
        _noopy_commandCursor++;
      }

      if (_noopy_commandCursor>64) {
        // overflow !
        _noopy_commandCursor = 0;
      }
    }
  }
  _noopy_flags[0] = _noopy_flags[1];
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


unsigned char getCommand(unsigned long* controller, unsigned int* device, unsigned char* onOff, unsigned char* global) {
  *controller = _noopy_controller;
  *device = _noopy_device;
  *onOff = _noopy_onOff;
  *global = _noopy_global;
}



void onRead(unsigned long controller, unsigned int device, unsigned char onOff, unsigned char global) {
  Serial.println("##################");
  Serial.println(controller);
  Serial.println(device);
  Serial.println(onOff);
  Serial.println(global);
}

void setup() {
  setListenPin(0);
  digitalWrite(13,0);
  Serial.begin(9600);
  
}

void loop() {
  unsigned long controller;
  unsigned int device;
  unsigned char onOff;
  unsigned char global;
  
  EnableRead(true);
  delay(2000);
  EnableRead(false);
  
  getCommand(&controller, &device, &onOff, &global);
  
  Serial.println("##################");
  Serial.println(controller);
  Serial.println(device);
  Serial.println(onOff);
  Serial.println(global);
}
