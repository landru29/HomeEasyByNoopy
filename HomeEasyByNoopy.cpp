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

int                     HomeEasyByNoopy::receive_pin           = 0;
unsigned char           HomeEasyByNoopy::receive_isSignal      = 0;
unsigned char           HomeEasyByNoopy::receive_flags[2]      = {0,0};
unsigned long           HomeEasyByNoopy::receive_command[2]    = {0,0};
unsigned char           HomeEasyByNoopy::receive_commandCursor = 0;
unsigned long           HomeEasyByNoopy::receive_controller    = 0;
unsigned int            HomeEasyByNoopy::receive_device        = 0;
unsigned char           HomeEasyByNoopy::receive_onOff         = 0;
unsigned char           HomeEasyByNoopy::receive_global        = 0;
unsigned long           HomeEasyByNoopy::receive_commandFrame  = 0;
unsigned char           HomeEasyByNoopy::receive_sreg          = 0;
unsigned char           HomeEasyByNoopy::receive_disabled      = 0;
HomeEasyByNoopyReceiver HomeEasyByNoopy::receive_handler       = 0;

HomeEasyByNoopy::HomeEasyByNoopy(int pinOut, int pinIn, HomeEasyByNoopyReceiver receiverHandler) {
	_pinOut = pinOut;
	pinMode(pinOut, OUTPUT);
	setEmitFrameCount(10);
	setListenPin(pinIn);
	setReceiver(receiverHandler);
	receive_disabled = 0;
}

void HomeEasyByNoopy::setReceiver(HomeEasyByNoopyReceiver receiverHandler) {
	receive_handler = receiverHandler;
}

unsigned int HomeEasyByNoopy::getTimer() {
  unsigned char sreg;
  unsigned int value;
  sreg = SREG;
  cli();
  value = TCNT1; // read the timer
  TCNT1 = 0x00;  // reset the timer
  SREG = sreg;
  return value;
}

unsigned long HomeEasyByNoopy::getRaw() {
	return receive_commandFrame;
}

void HomeEasyByNoopy::EnableRead(unsigned char onOffState) {
  if (onOffState) {
    receive_controller = 0;
    receive_device = 0;
    receive_onOff = 0;
    receive_global = 0;
    receive_commandFrame = 0;
    receive_command[0] = 0;
    receive_command[1] = 0;
    receive_commandCursor = 0;
    receive_isSignal = 0;
    receive_flags[0] = 0;
    receive_flags[1] = 0;
    
    // Configure Timer
    TCCR1A = 0x00;
    TCCR1B = 0x03; // prescale to 64
    TCCR1C = 0x00;
    
    // Save interrupt state
	receive_sreg = SREG;
    attachInterrupt(digitalPinToInterrupt(receive_pin), HomeEasyByNoopy::process, CHANGE);
  } else {
    detachInterrupt(digitalPinToInterrupt(receive_pin));
    // Restaure interrupt state
    SREG = receive_sreg;
  }
}

void HomeEasyByNoopy::decodereceive_command(unsigned long _receive_command) {
  receive_commandFrame =  _receive_command;
  receive_controller = (_receive_command & 0xffffffc0) >> 6;
  receive_device = _receive_command & 0xf;
  receive_onOff = (_receive_command & 0x10) >> 4;
  receive_global = (_receive_command & 0x20) >> 5;
  if (receive_handler) {
	receive_handler(receive_controller, receive_device + (receive_global << 4), receive_onOff);
  }
}

void HomeEasyByNoopy::process() {
  unsigned int count;
  unsigned char sreg;
  if (receive_disabled) {
	  receive_isSignal = 0;
	  receive_commandCursor = 0;
	  return;
  }
  sreg = SREG;
  cli(); // Stops interrupts
  receive_flags[1] = digitalRead(receive_pin);
  if (receive_flags[0] != receive_flags[1]) {
    // state really changed
    count = getTimer();
    // check High level
    if (receive_flags[0] == 1) {
      receive_isSignal = (count>55) && (count<65);
    }
    // compute Low level
    if ((receive_flags[0] == 0) && (receive_isSignal)) {
      if (count>2400) {
        // Start Emit
      }
      if ((count>600) && (count<700)) {
        // Start / End of frame
        receive_commandCursor = 0;
        if ((receive_command[0]) && (receive_command[0] == receive_command[1])) {
          decodereceive_command(receive_command[0]);
        }
        receive_command[1] = receive_command[0];
        receive_command[0] = 0;
      }
      if ((count>50) && (count<100)) {
        if (!(receive_commandCursor & 1)) {
          unsigned long _bit = 1;
          receive_command[0] &= ~(_bit << (31 - receive_commandCursor/2));
        }
        receive_commandCursor++;
      }
      if ((count>300) && (count<350)) {
        if (!(receive_commandCursor & 1)) {
          unsigned long _bit = 1;
          receive_command[0] |= _bit << (31 - receive_commandCursor/2);
        }
        receive_commandCursor++;
      }

      if (receive_commandCursor>64) {
        // overflow !
        receive_commandCursor = 0;
      }
    }
  }
  receive_flags[0] = receive_flags[1];
  SREG = sreg; // enable interupts
}

void HomeEasyByNoopy::setListenPin(int pin) {
  // Configure read pin
  receive_pin = pin;
  pinMode(pin, INPUT);
}

unsigned char HomeEasyByNoopy::getReceiveCommand(unsigned long* controller, unsigned int* device, unsigned char* onOff) {
  *controller = receive_controller;
  *device = receive_device + (receive_global << 4);
  *onOff = receive_onOff;
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
  receive_disabled = 1; // Stop listening
  
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
  receive_disabled = 0; // Restore listening
  SREG = sreg; // enable interupts
}

void HomeEasyByNoopy::emit (unsigned long controller, unsigned char device, bool onOff) {
	int frame[64];
	buildFrame(frame, controller, device, onOff);
	sendFrame(frame, _count);
}
