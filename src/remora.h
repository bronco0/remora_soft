// **********************************************************************************
// Micro module Fil Pilote
// **********************************************************************************
// Copyright (C) 2014 Thibault Ducret
// Licence MIT
//
//
// **********************************************************************************
#ifndef __REMORA_H__
#define __REMORA_H__

// Version logicielle remora
#define REMORA_SOFT_VERSION "0.0.1"

#define DEFAULT_HOSTNAME  PSTR("Micro_pilote")

// Définir ici les identifiants de
// connexion à votre réseau Wifi
// =====================================
#define DEFAULT_WIFI_SSID PSTR("TheWify")
#define DEFAULT_WIFI_PASS PSTR("TheWify1.")

// =====================================
#define DEFAULT_OTA_PORT  8266
#define DEFAULT_OTA_AUTH  PSTR("OTA_Remora")

// =====================================
// Includes
#include "Arduino.h"
#include <ArduinoLog.h>
#include <ArduinoJson.h>
#include "MemoryInfo.h"
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESPAsyncTCP.h>
#include <AsyncMqttClient.h>
#include <WiFiUdp.h>
#include <Ticker.h>
#include <ArduinoOTA.h>
extern "C" {
  #include "user_interface.h"
}

// Includes du projets remora
#include "./config.h"
#include "./pilotes.h"
#include "./mqtt.h"

#define _yield  yield
#define _wdt_feed ESP.wdtFeed
#define REBOOT_DELAY    100     /* Delay for rebooting once reboot flag is set */

//#define DEBUG // Décommenter cette ligne pour activer le DEBUG serial

//#define LOG_LEVEL LOG_LEVEL_SILENT    // no output
//#define LOG_LEVEL LOG_LEVEL_FATAL     // fatal errors
//#define LOG_LEVEL LOG_LEVEL_ERROR     // all errors
//#define LOG_LEVEL LOG_LEVEL_WARNING   // errors, and warnings
//#define LOG_LEVEL LOG_LEVEL_NOTICE    // errors, warnings and notices
//#define LOG_LEVEL LOG_LEVEL_TRACE     // errors, warnings, notices & traces
#define LOG_LEVEL LOG_LEVEL_VERBOSE   // all
#define DEBUG_SERIAL  Serial
//#define DEBUG_INIT              /* Permet d'initialiser la connexion série pour debug */

// Variables exported to other source file
// ========================================
// define var for whole project

// status global de l'application
extern uint16_t status;
extern unsigned long uptime;
extern bool first_setup;

extern bool reboot;     // Flag to reboot the ESP
extern bool ota_blink;
extern bool got_first;  // Data reception flag

extern uint16_t status; // status global de l'application

// Function exported for other source file
// =======================================
char * timeAgo(unsigned long);

#endif // REMORA_H