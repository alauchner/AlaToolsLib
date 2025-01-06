#include <Arduino.h>


#include "alaTools.h"
#define SERIAL_DEBUG true

void setup()
{
  Serial.begin(115200);
  WifiBegin("ssid", "password");
  DEBUG("Hello world!"); 
}

void loop()
{

}