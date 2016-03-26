#include <HomeEasyByNoopy.h>

HomeEasyByNoopy sender(4, 0, 0);

void setup() {
  Serial.begin(9600);
}

void loop() {
  unsigned long controller;
  unsigned int device;
  unsigned char onOff;
  
  sender.EnableRead(true);
  delay(2000);
  sender.EnableRead(false);
  
  sender.getReceiveCommand(&controller, &device, &onOff);
  
  Serial.println("##################");
  
  String stringCtrl =  String(controller, HEX);
  Serial.println(stringCtrl);
  
  String stringDev = "";
  stringDev += (char)((device >> 2) + 'A');
  stringDev += (char)((device & 3) + '1');
  if (device & 16) {
    stringDev = "G";
  }
  Serial.println(stringDev);
  
  Serial.println(onOff);
}
