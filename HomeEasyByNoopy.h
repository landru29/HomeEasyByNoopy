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

class HomeEasyByNoopy
{
  public:
    HomeEasyByNoopy (int pinOut, int pinIn);
    
    void emit (unsigned long controller, unsigned char device, bool onOff);
    void setEmitFrameCount(int count);
    void EnableRead(unsigned char onOffState);
    unsigned char getRecieveCommand(unsigned long* controller, unsigned int* device, unsigned char* onOff, unsigned char* global);
  
  private :
    int _pinOut;
    int _count;
    
    void startEndFrame();
    void startEmit();
    static unsigned int getTimer();
    static void decoderecieve_command(unsigned long _recieve_command);
    static void process();
    void setListenPin(int pin);
    static void setBit(int* framePart, bool state);
    void buildFrame(int* frame, unsigned long controller, unsigned char device, bool onOff);
    void sendFrame(int* frame, int count);
    
    static int recieve_pin;
    static unsigned char recieve_isSignal;
    static unsigned char recieve_flags[2];
    static unsigned long recieve_command[2];
    static unsigned char recieve_commandCursor;
    static unsigned long recieve_controller;
    static unsigned int  recieve_device;
    static unsigned char recieve_onOff;
    static unsigned char recieve_global;
    static unsigned long recieve_commandFrame;
    static unsigned char recieve_sreg;
    
};

#endif /* HomeEasyByNoopy_h */
