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
    HomeEasyByNoopy (int pinOut);
    
    void emit (unsigned long controller, unsigned char device, bool onOff);
    void setFrameCount(int count);
  
  private :
    int _pinOut;
    int _count;
    
    void startEndFrame();
    void startEmit();
    
    static void setBit(int* framePart, bool state);
    void buildFrame(int* frame, unsigned long controller, unsigned char device, bool onOff);
    void sendFrame(int* frame, int count);
    
};

#endif /* HomeEasyByNoopy_h */
