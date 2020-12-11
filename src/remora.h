// **********************************************************************************
// Programmateur Fil Pilote et Suivi Conso
// **********************************************************************************
// Copyright (C) 2014 Thibault Ducret
// Licence MIT
//
// History : 15/01/2015 Charles-Henri Hallard (http://hallard.me)
//                      Intégration de version 1.2 de la carte electronique
//           15/09/2015 Charles-Henri Hallard : Ajout compatibilité ESP8266
//           02/12/2015 Charles-Henri Hallard : Ajout API WEB ESP8266 et Remora V1.3
//           04/01/2016 Charles-Henri Hallard : Ajout Interface WEB GUIT
//           04/03/2017 Manuel Hervo          : Ajout des connexions TCP Asynchrones
//
// **********************************************************************************
#ifndef __REMORA_H__
#define __REMORA_H__

// Définir ici le type de carte utilisé
//#define REMORA_BOARD_V12 "Remora v1.2" // Version 1.2
//#define REMORA_BOARD_V13 "Remora v1.3"  // Version 1.3
#define REMORA_BOARD_V14 "Remora v1.4" // Version 1.4
//#define REMORA_BOARD_V15 "Remora v1.5 // Version 1.5

#if defined(REMORA_BOARD_V12)
  #define REMORA_BOARD REMORA_BOARD_V12
#elif defined(REMORA_BOARD_V13)
  #define REMORA_BOARD REMORA_BOARD_V13
#elif defined(REMORA_BOARD_V14)
  #define REMORA_BOARD REMORA_BOARD_V14
#elif defined(REMORA_BOARD_V15)
  #define REMORA_BOARD REMORA_BOARD_V15
#else
  #define REMORA_BOARD "Non définie"
#endif

//  Définir ici les modules utilisés sur la carte Remora
//#define MOD_RF69         /* Module RF  */
#define MOD_OLED         /* Afficheur  */
#define MOD_TELEINFO     /* Teleinfo   */
//#define MOD_RF_OREGON    /* Reception des sondes orégon */
#define MOD_ADPS         /* Délestage */
#define MOD_MQTT         /* MQTT */
//#define MOD_EMONCMS      /* Emoncms.org */
//#define MOD_JEEDOM       /* Jeedom */
#define MOD_MICRO        /* Micro module pilote */

// Type of OLED
#ifdef MOD_OLED
  #define OLED_SH1106
  #define OLED_SSD1306
#endif

// Version logicielle remora
#define REMORA_SOFT_VERSION "2.1.0"

// Définir ici votre authentification blynk, cela
// Activera automatiquement blynk http://blynk.cc
//#define BLYNK_AUTH "YourBlynkAuthToken"

// Librairies du projet remora pour ESP8266
// Includes
#include "Arduino.h"
#include <ArduinoLog.h>
#include <ArduinoJson.h>
#include "MemoryInfo.h"
#include <EEPROM.h>
#include <FS.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#ifdef MOD_MQTT
  #include <AsyncMqttClient.h>
#endif
#include <WiFiUdp.h>
#include <Ticker.h>
#include <NeoPixelBus.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <SPI.h>
#if defined (OLED_SSD1306)
  #include <SSD1306Wire.h>
  #include <OLEDDisplayUi.h>
#endif
#if defined (OLED_SH1106)
  #include <SH1106Wire.h>
  #include <OLEDDisplayUi.h>
#endif
extern "C" {
  #include "user_interface.h"
}
#include <LibMCP23017.h>
#ifdef MOD_TELEINFO
  #include <LibLibTeleinfo.h>
#endif
#ifdef MOD_RF69
  //#include "./RFM69registers.h"
  //#include "./RFM69.h"
  #include <LibULPNode_RF_Protocol.h>
  #include <LibRH_RF69.h>
  #include <LibRadioHead.h>
  #include <LibRHReliableDatagram.h>
#endif

// Includes du projets remora
#include "./config.h"
#include "./linked_list.h"
#ifdef MOD_OLED
  #include "./display.h"
  #include "./fonts.h"
  #include "./icons.h"
#endif
#include "./i2c.h"
#ifdef MOD_RF69
  #include "./rfm.h"
#endif
#include "./pilotes.h"
#ifdef MOD_TELEINFO
  #include "./tinfo.h"
#endif
#include "./webserver.h"
#if defined(MOD_EMONCMS) || defined(MOD_JEEDOM)
  #include "./webclient.h"
#endif
#ifdef MOD_MQTT
  #include "./mqtt.h"
#endif

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
#define DEBUG_SERIAL  Serial1
#define DEBUG_INIT              /* Permet d'initialiser la connexion série pour debug */
//#define DISABLE_LOGGING

#define COLOR_RED     rgb_brightness, 0, 0
#define COLOR_ORANGE  rgb_brightness, rgb_brightness>>1, 0
#define COLOR_YELLOW  rgb_brightness, rgb_brightness, 0
#define COLOR_GREEN   0, rgb_brightness, 0
#define COLOR_CYAN    0, rgb_brightness, rgb_brightness
#define COLOR_BLUE    0, 0, rgb_brightness
#define COLOR_MAGENTA rgb_brightness, 0, rgb_brightness

// On ESP8266 we use NeopixelBus library to drive neopixel RGB LED
#define RGB_LED_PIN 0 // RGB Led driven by GPIO0
#define LedRGBOFF() { rgb_led.SetPixelColor(0,0); rgb_led.Show(); }
#define LedRGBON(x) { RgbColor color(x); rgb_led.SetPixelColor(0,color); rgb_led.Show(); }
//#define LedRGBOFF() {}
//#define LedRGBON(x) {}

// RFM69 Pin mapping
#ifdef MOD_RF69
  #define RF69_CS   15
  #define RF69_IRQ  2
#endif

// Masque de bits pour le status global de l'application
#define STATUS_MCP    0x0001 // I/O expander detecté
#ifdef MOD_OLED
  #define STATUS_OLED   0x0002 // Oled detecté
#endif
#ifdef MOD_RF69
  #define STATUS_RFM    0x0004 // RFM69  detecté
#endif
//#ifdef MOD_TELEINFO
  #define STATUS_TINFO  0x0008 // Trame téléinfo detecté
//#endif

// Variables exported to other source file
// ========================================
// define var for whole project

// status global de l'application
extern uint16_t status;
extern unsigned long uptime;
extern bool first_setup;

typedef NeoPixelBus<NeoRgbFeature, NeoEsp8266BitBang800KbpsMethod> MyPixelBus;

// ESP8266 WebServer
extern AsyncWebServer server;
// RGB LED
extern MyPixelBus rgb_led;

// define whole brigtness level for RGBLED
extern uint8_t rgb_brightness;

#ifdef MOD_EMONCMS
  extern Ticker Tick_emoncms;
#endif
#ifdef MOD_JEEDOM
  extern Ticker Tick_jeedom;
#endif
extern bool reboot;     // Flag to reboot the ESP
extern bool ota_blink;
extern bool got_first;  // Data reception flag


extern uint16_t status; // status global de l'application

// Function exported for other source file
// =======================================
char * timeAgo(unsigned long);
#ifdef MOD_EMONCMS
  void Task_emoncms();
#endif
#ifdef MOD_JEEDOM
  void Task_jeedom();
#endif

#endif // REMORA_H