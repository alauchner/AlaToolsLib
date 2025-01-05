// alaTools.h
#pragma once

#include <arduino.h>
#include <ArduinoOTA.h>
#include "alaDebug.h"

struct EepConfigDataT
{
	int dummy;
	int magic; // Data in EEPROM valid, am Ende, damit bei Aenderung struct ungueltig
};

bool EepLoadConfig();
void EepSaveConfig();
bool sendThingspeakSample(bool sendH, bool sendD);
tm GetNowTm();
time_t GetNowTimeT();
String tm2TimeString(tm);
String UtcTime2LocalTimeString(time_t);
void OtaInit();
void WifiBegin();

extern tm bootTime;
extern EepConfigDataT eep_config;