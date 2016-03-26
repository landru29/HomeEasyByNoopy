#ifndef HomeEasyByNoopy_h
#define HomeEasyByNoopy_h

#define HE_ON         1
#define HE_OFF        0
#define HE_OPEN       1
#define HE_CLOSE      0

#define HE_GLOBAL     1
#define HE_NOT_GLOBAL 0

#define HE_DEVICE_1 0x00
#define HE_DEVICE_2 0x01
#define HE_DEVICE_3 0x02
#define HE_DEVICE_4 0x03

#define HE_DEVICE_A 0x00
#define HE_DEVICE_B 0x04
#define HE_DEVICE_C 0x08
#define HE_DEVICE_D 0x0c
#define HE_DEVICE_G 0x10

#include "Arduino.h"

typedef void (* HomeEasyByNoopyReceiver)(unsigned long controller, unsigned int device, unsigned char onOff);

class HomeEasyByNoopy
{
  public:
    HomeEasyByNoopy (int pinOut, int pinIn, HomeEasyByNoopyReceiver receiverHandler);
    
    void emit (unsigned long controller, unsigned char device, bool onOff);
    void setEmitFrameCount(int count);
    void EnableRead(unsigned char onOffState);
    unsigned char getReceiveCommand(unsigned long* controller, unsigned int* device, unsigned char* onOff);
    void setReceiver(HomeEasyByNoopyReceiver receiverHandler);
    static unsigned long getRaw();
  
  private :
    int _pinOut;
    int _count;
    
    void startEndFrame();
    void startEmit();
    static unsigned int getTimer();
    static void decodereceive_command(unsigned long _receive_command);
    static void process();
    void setListenPin(int pin);
    static void setBit(int* framePart, bool state);
    void buildFrame(int* frame, unsigned long controller, unsigned char device, bool onOff);
    void sendFrame(int* frame, int count);
    
    static int receive_pin;
    static unsigned char receive_isSignal;
    static unsigned char receive_flags[2];
    static unsigned long receive_command[2];
    static unsigned char receive_commandCursor;
    static unsigned long receive_controller;
    static unsigned int  receive_device;
    static unsigned char receive_onOff;
    static unsigned char receive_global;
    static unsigned long receive_commandFrame;
    static unsigned char receive_sreg;
    static HomeEasyByNoopyReceiver receive_handler;
    static unsigned char receive_disabled;
    
};

#endif /* HomeEasyByNoopy_h */
