#include <HomeEasyByNoopy.h>

HomeEasyByNoopy sender(4, 0);

void setup() {
  Serial.begin(9600);
}

void loop() {
  unsigned long controller;
  unsigned int device;
  unsigned char onOff;
  unsigned char global;
  
  sender.EnableRead(true);
  delay(2000);
  sender.EnableRead(false);
  
  sender.getRecieveCommand(&controller, &device, &onOff, &global);
  
  Serial.println("##################");
  Serial.println(controller);
  Serial.println(device);
  Serial.println(onOff);
  Serial.println(global);
}
