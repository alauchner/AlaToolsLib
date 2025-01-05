#include <Arduino.h>
#include <esp_task_wdt.h>


#include "alaToolsLib.h"


void setup()
{
  Serial.begin(115200);
  WifiBegin();
 
}

void loop()
{
  esp_task_wdt_reset();  //reset watchdog
}