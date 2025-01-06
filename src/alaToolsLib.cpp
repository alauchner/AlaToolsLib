#if defined(ESP8266)
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <HTTPClient.h>
#include <WiFi.h>
#endif

#include <EEPROM.h>
#include "alaToolsLib.h"

#define EEP_MAGIC 123456
#define EEP_CFGSTART 0
#define MY_TZ "CET-1CEST,M3.5.0,M10.5.0/3" // Zeitzone

WiFiClient wifiClient; // muss global sein, sonst crash
EepConfigDataT eep_config;
tm bootTime;

void WifiBegin(const String ssid, const String password)
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
            delay(500);
            ESP.restart();
        }
    }
    DEBUG("IP %s", WiFi.localIP().toString().c_str());

#if defined(ESP8266)
    configTime(MY_TZ, "fritz.box", "de.pool.ntp.org"); // --> Here is the IMPORTANT ONE LINER needed in your sketch!
#elif defined(ESP32)
    configTime(0, 0, "fritz.box", "de.pool.ntp.org"); // --> Here is the IMPORTANT ONE LINER needed in your sketch!
#endif

    while (GetNowTm().tm_year < 125) // Jahr 2025
    {
        delay(500);
        yield();
        DEBUG("NTP not OK");
    }
#if defined(ESP32)
    setenv("TZ", MY_TZ, 1);
    tzset();
#endif

    DEBUG("Lokal time is %s", tm2TimeString(GetNowTm()).c_str());
    bootTime = GetNowTm();
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
    DEBUG("Thingspeak send result: %d, data: %s", res, s.c_str());
    http.end(); // Close connection
    if (res == 200)
        return true;
    return false;
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
    return tm2TimeString(tm);
}
