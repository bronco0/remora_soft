// **********************************************************************************
// Remora WEB Server, route web function
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// Attribution-NonCommercial-ShareAlike 4.0 International License
// http://creativecommons.org/licenses/by-nc-sa/4.0/
//
// This program works with the Remora board
// see schematic here https://github.com/thibdct/programmateur-fil-pilote-wifi
//
// Written by Charles-Henri Hallard (http://hallard.me)
//
// History 2015-06-14 - First release
//         2015-11-31 - Added Remora API
//         2016-01-04 - Added Web Interface part
//
// All text above must be included in any redistribution.
//
// **********************************************************************************

#ifndef ROUTE_H
#define ROUTE_H

// Include main project include file
#include "remora.h"
#include <AsyncJson.h>

// Exported variables/object instanciated in main sketch
// =====================================================
extern char response[];
extern uint16_t response_idx;

const char FP_SSID[]        PROGMEM = "ssid";
const char FP_RSSI[]        PROGMEM = "rssi";
const char FP_ENCRYPTION[]  PROGMEM = "enc";
const char FP_CHANNEL[]     PROGMEM = "chan";
const char FP_STATUS[]      PROGMEM = "status";
const char FP_OK[]          PROGMEM = "OK";

// declared exported function from webserver.cpp
// ===================================================
void handleFormConfig(AsyncWebServerRequest *request);
void handleFactoryReset(AsyncWebServerRequest *request);
void handleReset(AsyncWebServerRequest *request);
void tinfoJSON(AsyncWebServerRequest *request);
void tinfoJSONTable(AsyncWebServerRequest *request);
String sysJSONTable(AsyncWebServerRequest *request);
String confJSONTable(AsyncWebServerRequest *request);
void spiffsJSONTable(AsyncWebServerRequest *request);
void wifiScanJSON(AsyncWebServerRequest *request);
void handleNotFound(AsyncWebServerRequest *request);
void handle_fw_upload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);

#endif