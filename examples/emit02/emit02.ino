#include <HomeEasyByNoopy.h>

/**
 * Open the serial console
 * Then type 
 *     <controller>+<device>   => on
 *     <controller>-<device>   => off
 * 
 * For instance "A0A406+A1"
 *    this will send on signal on button A1 of the controller A0A406
 */

// create a sender on pin 4
HomeEasyByNoopy sender(4, 0, 0);
char inputString[200];

void setup() {
  // the frame will be repeated  15 times (default is 10)
  sender.setEmitFrameCount(15);
  SERIAL_PORT_USBVIRTUAL.begin(115200);
}

char* waitForCommand() {
  bool done = false;
  unsigned int cursor = 0;
  int c;
  while (!done) {
    c = SERIAL_PORT_USBVIRTUAL.read();
    if (c != -1) {
      if (c==10) {
        done = true;
      } else {
        inputString[cursor++] = (char)c;
        inputString[cursor] = 0;
      }
    }
  }
  return inputString;
}

char hexToByte(char c) {
  if ((c >= '0') && (c <= '9')) {
      return c - '0';
    }
    if ((c >= 'a') && (c <= 'f')) {
      return c + 10 - 'a';
    }
    if ((c >= 'A') && (c <= 'F')) {
      return c + 10 - 'A';
    }
    return 0;
}

void processCommand(char* command) {
  unsigned long controller = 0;
  unsigned char onOff = 0;
  unsigned char device = 0;
  int str_i = 0;
  // Get controller
  for(; (command[str_i]) && (command[str_i] != '-')  && (command[str_i] != '+'); str_i++) {
    unsigned long digit = hexToByte(command[str_i]);
    controller = (controller << 4) + digit;
  }
  // get On / Off
  onOff = (command[str_i++] == '+');
  // getDevice
  device = ((command[str_i] == 'G') || (command[str_i] == 'g')) << 4;
  device += (hexToByte(command[str_i]) - 10) << 2;
  device += hexToByte(command[str_i+1]) - 1;
  sender.emit(controller, device, onOff);
}


void loop() {
   char* command = waitForCommand();
   processCommand(command);


}
