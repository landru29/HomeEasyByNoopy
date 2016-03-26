#include <HomeEasyByNoopy.h>

void myHandler(unsigned long controller, unsigned int device, unsigned char onOff) {
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

HomeEasyByNoopy sender(4, 0, myHandler);

void setup() {
  Serial.begin(9600);
  sender.EnableRead(true);
}

void loop() {
  // put other code here

}
