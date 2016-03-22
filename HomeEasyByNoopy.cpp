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

HomeEasyByNoopy::HomeEasyByNoopy(int pinOut) {
	_pinOut = pinOut;
	pinMode(pinOut, OUTPUT);
	setFrameCount(10);
}

void HomeEasyByNoopy::setFrameCount(int count) {
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
  sei(); // enable interupts
}

void HomeEasyByNoopy::emit (unsigned long controller, unsigned char device, bool onOff) {
	int frame[64];
	buildFrame(frame, controller, device, onOff);
	sendFrame(frame, _count);
}
