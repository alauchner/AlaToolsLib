#include <HTTPClient.h>
#include <EEPROM.h>
#include <ArduinoOTA.h>
#include <TimeLib.h>
#include <WiFi.h>
#include "alaTools.h"

#define EEP_MAGIC 353761
#define EEP_CFGSTART 0
#define MY_TZ "CET-1CEST,M3.5.0,M10.5.0/3" // Zeitzone
const char *ssid = "mamalad5";
const char *password = "ottoottootto9";

WiFiClient wifiClient; // muss global sein, sonst crash
EepConfigDataT eep_config;
tm bootTime;

void WifiBegin()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    int connection_try_counter = 0;
    while (!WiFi.isConnected())
    {
        delay(1000);
        connection_try_counter++;
        if (connection_try_counter > 10)
        {
            DEBUG("\r\n**** No WIFI Connect possible, reboot ****");
            delay(1000);
            ESP.restart();
        }
    }
    DEBUG("IP %s", WiFi.localIP().toString().c_str());

    configTime(0, 0, "fritz.box", "de.pool.ntp.org"); // --> Here is the IMPORTANT ONE LINER needed in your sketch!

    while (GetNowTm().tm_year < 123) // Jahr 2023
    {
        delay(50);
        yield();
        DEBUG("NTP not OK");
    }
    setenv("TZ", MY_TZ, 1);
    tzset();
    DEBUG("Lokal time is %s", tm2TimeString(GetNowTm()).c_str());
    bootTime = GetNowTm();
    setSyncProvider(GetNowTimeT);
}

void EepSaveConfig()
{
    // Save configuration from RAM into EEPROM
    eep_config.magic = EEP_MAGIC;
    EEPROM.begin(512);
    EEPROM.put(EEP_CFGSTART, eep_config);
    EEPROM.commit(); // Only needed for ESP8266 to get data written
    EEPROM.end();    // Free RAM copy of structure
}

bool EepLoadConfig()
{
    // Loads configuration from EEPROM into RAM
    EEPROM.begin(512);
    EEPROM.get(EEP_CFGSTART, eep_config);
    EEPROM.end();
    if (eep_config.magic == EEP_MAGIC)
        return true;
    return false;
}

bool ThingspeakSend(String s, String api_key)
{
    HTTPClient http;
    String sdata = "http://api.thingspeak.com/update?api_key=" + api_key;
    sdata += s;
    http.begin(wifiClient, sdata);
    int res = http.GET();
    yield();
    DEBUG("Thingspeak send %d", res);
    http.end(); // Close connection
    if (res == 200)
        return true;
    return false;
}

bool sendThingspeakSample(bool sendH, bool sendD)
{
    String sdata = "";
    sdata += "&field1=";
    DEBUG("Thingspeak send message %s", sdata.c_str());
    return ThingspeakSend(sdata, "FE7P0VROX3FYXOKY");
}

tm GetNowTm()
{
    tm tmvar;
    time_t nowVar;
    time(&nowVar);
    localtime_r(&nowVar, &tmvar);
    return tmvar;
}
time_t GetNowTimeT()
{
    time_t n;
    time(&n);
    return n;
}

String tm2TimeString(tm tm)
{
    // format Time nach meinem Muster
    auto d = String(tm.tm_mday);
    d += ".";
    d += String(tm.tm_mon + 1);
    d += ".";
    d += String(tm.tm_year + 1900);
    auto t = String(tm.tm_hour);
    t += ":";
    if (tm.tm_min < 10) // add a zero if minute is under 10
        t += "0";
    t += String(tm.tm_min);
    t += ":";
    if (tm.tm_sec < 10) // add a zero if second is under 10
        t += "0";
    t += String(tm.tm_sec);
    return d + " " + t;
}

String UtcTime2LocalTimeString(time_t tt)
{
    tm tm;
    localtime_r(&tt, &tm);
    // format Time nach meinem Muster
    auto d = String(tm.tm_mday);
    d += ".";
    d += String(tm.tm_mon + 1);
    d += ".";
    d += String(tm.tm_year + 1900);
    auto t = String(tm.tm_hour);
    t += ":";
    if (tm.tm_min < 10) // add a zero if minute is under 10
        t += "0";
    t += String(tm.tm_min);
    t += ":";
    if (tm.tm_sec < 10) // add a zero if second is under 10
        t += "0";
    t += String(tm.tm_sec);
    return d + " " + t;
}

// Hilfsroutinen OTA Update
void OtaInit()
{
    ArduinoOTA.onStart([]()
                       { Serial.println("Start"); });
    ArduinoOTA.onEnd([]()
                     { Serial.println("\nEnd"); });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                          { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });
    ArduinoOTA.onError([](ota_error_t error)
                       {
		Serial.printf("Error[%u]: ", error);
		if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
		else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
		else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
		else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
		else if (error == OTA_END_ERROR) Serial.println("End Failed"); });
    ArduinoOTA.begin();
}