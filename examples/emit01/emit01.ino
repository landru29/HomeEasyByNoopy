#include <HomeEasyByNoopy.h>

// create a sender on pin 4
HomeEasyByNoopy sender(4, 0, 0);

void setup() {
  // the frame will be repeated  15 times (default is 10)
  sender.setEmitFrameCount(15);
}

void loop() {
  // Emit the signal "ON" on controller A0A406, device A1
  sender.emit(0x0a0a406, HE_DEVICE_A + HE_DEVICE_1, HE_ON);
  
  // wait for 15 seconds before renewing the command
  delay(15000);
}
