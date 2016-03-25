#include "HomeEasyByNoopy.h"


extern "C" {
  // AVR LibC Includes
  #include <inttypes.h>
  #include <avr/interrupt.h>
  #include "Arduino.h"
}

const int _he_pulse_high  = 275;
const int _he_pulse_low0  = 275;
const int _he_pulse_low1  = 1225;
const int _he_pulse_frame = 2675;
const int _he_pulse_emit  = 9900;

int HomeEasyByNoopy::recieve_pin = 0;
unsigned char HomeEasyByNoopy::recieve_isSignal = 0;
unsigned char HomeEasyByNoopy::recieve_flags[2] = {0,0};
unsigned long HomeEasyByNoopy::recieve_command[2] = {0,0};
unsigned char HomeEasyByNoopy::recieve_commandCursor = 0;
unsigned long HomeEasyByNoopy::recieve_controller = 0;
unsigned int HomeEasyByNoopy::recieve_device = 0;
unsigned char HomeEasyByNoopy::recieve_onOff = 0;
unsigned char HomeEasyByNoopy::recieve_global = 0;
unsigned long HomeEasyByNoopy::recieve_commandFrame = 0;
unsigned char HomeEasyByNoopy::recieve_sreg=0;

HomeEasyByNoopy::HomeEasyByNoopy(int pinOut, int pinIn) {
	_pinOut = pinOut;
	pinMode(pinOut, OUTPUT);
	setEmitFrameCount(10);
	setListenPin(pinIn);
}

unsigned int HomeEasyByNoopy::getTimer() {
  unsigned char sreg;
  unsigned int value;
  sreg = SREG;
  cli();
  value = TCNT1; // read the timer
  TCNT1 = 0x00; // reset the timer
  SREG = sreg;
  return value;
}

void HomeEasyByNoopy::EnableRead(unsigned char onOffState) {
  if (onOffState) {
    recieve_controller = 0;
    recieve_device = 0;
    recieve_onOff = 0;
    recieve_global = 0;
    recieve_commandFrame = 0;
    recieve_command[0] = 0;
    recieve_command[1] = 0;
    recieve_commandCursor = 0;
    recieve_isSignal = 0;
    recieve_flags[0] = 0;
    recieve_flags[1] = 0;
    
    // Configure Timer
    TCCR1A = 0x00;
    TCCR1B = 0x03; // prescale to 64
    TCCR1C = 0x00;
    
    // Save interrupt state
	recieve_sreg = SREG;
    attachInterrupt(digitalPinToInterrupt(recieve_pin), HomeEasyByNoopy::process, CHANGE);
  } else {
    detachInterrupt(digitalPinToInterrupt(recieve_pin));
    // Restaure interrupt state
    SREG = recieve_sreg;
  }
}

void HomeEasyByNoopy::decoderecieve_command(unsigned long _recieve_command) {
  recieve_commandFrame =  _recieve_command;
  recieve_controller = (_recieve_command & 0xffffffc0) >> 6;
  recieve_device = _recieve_command & 0xf;
  recieve_onOff = (_recieve_command & 0x10) >> 4;
  recieve_global = (_recieve_command & 0x20) >> 5;
}

void HomeEasyByNoopy::process() {
  unsigned int count;
  unsigned char sreg;
  sreg = SREG;
  cli(); // Stops interrupts
  recieve_flags[1] = digitalRead(recieve_pin);
  if (recieve_flags[0] != recieve_flags[1]) {
    // state really changed
    count = getTimer();
    // check High level
    if (recieve_flags[0] == 1) {
      recieve_isSignal = (count>55) && (count<65);
    }
    // compute Low level
    if ((recieve_flags[0] == 0) && (recieve_isSignal)) {
      if (count>2400) {
        // Start Emit
      }
      if ((count>600) && (count<700)) {
        // Start / End of frame
        recieve_commandCursor = 0;
        if ((recieve_command[0]) && (recieve_command[0] == recieve_command[1])) {
          decoderecieve_command(recieve_command[0]);
        }
        recieve_command[1] = recieve_command[0];
        recieve_command[0] = 0;
      }
      if ((count>50) && (count<100)) {
        if (!(recieve_commandCursor & 1)) {
          unsigned long _bit = 1;
          recieve_command[0] &= ~(_bit << (31 - recieve_commandCursor/2));
        }
        recieve_commandCursor++;
      }
      if ((count>300) && (count<350)) {
        if (!(recieve_commandCursor & 1)) {
          unsigned long _bit = 1;
          recieve_command[0] |= _bit << (31 - recieve_commandCursor/2);
        }
        recieve_commandCursor++;
      }

      if (recieve_commandCursor>64) {
        // overflow !
        recieve_commandCursor = 0;
      }
    }
  }
  recieve_flags[0] = recieve_flags[1];
  SREG = sreg; // enable interupts
}

void HomeEasyByNoopy::setListenPin(int pin) {
  // Configure read pin
  recieve_pin = pin;
  pinMode(pin, INPUT);
}

unsigned char HomeEasyByNoopy::getRecieveCommand(unsigned long* controller, unsigned int* device, unsigned char* onOff, unsigned char* global) {
  *controller = recieve_controller;
  *device = recieve_device;
  *onOff = recieve_onOff;
  *global = recieve_global;
}

void HomeEasyByNoopy::setEmitFrameCount(int count) {
	_count = count;
}

void HomeEasyByNoopy::startEndFrame() {
  digitalWrite(_pinOut, HIGH);
  delayMicroseconds(_he_pulse_high);
  digitalWrite(_pinOut, LOW);
  delayMicroseconds(_he_pulse_frame);
}

void HomeEasyByNoopy::startEmit() {
  digitalWrite(_pinOut, HIGH);
  delayMicroseconds(_he_pulse_high);
  digitalWrite(_pinOut, LOW);
  delayMicroseconds(_he_pulse_emit);
}

void HomeEasyByNoopy::setBit(int* framePart, bool state) {
  if (state) {
      framePart[0] = _he_pulse_low1;
      framePart[1] = _he_pulse_low0;
    } else {
      framePart[0] = _he_pulse_low0;
      framePart[1] = _he_pulse_low1;
    }
}

void HomeEasyByNoopy::buildFrame(int* frame, unsigned long controller, unsigned char device, bool onOff) {
  int global = device >> 4;
  if (global) {
    device = 0;
  }
  for(int i=0; i<26; i++) {
    setBit(&frame[i*2], (controller >> (25 - i)) & 0x01);
  }
  setBit(&frame[26*2], global);
  setBit(&frame[27*2], onOff);
  for(int i=0; i<4; i++) {
    setBit(&frame[(28 + i)*2], (device >> (3 - i)) & 0x01);
  }
}

void HomeEasyByNoopy::sendFrame(int* frame, int count) {
  unsigned char sreg;
  sreg = SREG;
  cli(); // disable interupts
  
  while (count-- > 0) {
    startEmit();
    startEndFrame();
    for(int i=0; i<64; i++) {
      digitalWrite(_pinOut, HIGH);
      delayMicroseconds(_he_pulse_high);
      digitalWrite(_pinOut, LOW);
      delayMicroseconds(frame[i]);
    }
  }
  startEndFrame();
  SREG = sreg; // enable interupts
}

void HomeEasyByNoopy::emit (unsigned long controller, unsigned char device, bool onOff) {
	int frame[64];
	buildFrame(frame, controller, device, onOff);
	sendFrame(frame, _count);
}
