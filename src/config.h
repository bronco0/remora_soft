// **********************************************************************************
// Remora micro pilote : Configuration include file
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// Attribution-NonCommercial-ShareAlike 4.0 International License
// http://creativecommons.org/licenses/by-nc-sa/4.0/
//
// This program works with the Remora board
// see schematic here https://github.com/thibdct/programmateur-fil-pilote-wifi
//
//
// All text above must be included in any redistribution.
//
// **********************************************************************************

#ifndef __CONFIG_H__
#define __CONFIG_H__

// Include main project include file
#include "remora.h"

#define CFG_VERSION       7

#define CFG_DEFAULT_ID    system_get_chip_id()
#define CFG_HOSTNAME_SIZE 32
#define CFG_SSID_SIZE     32
#define CFG_PSK_SIZE      64

#define DEFAULT_HOSTNAME PSTR("Micro_pilote")

// Définir ici les identifiants de
// connexion à votre réseau Wifi
// =====================================
#define DEFAULT_WIFI_SSID PSTR("TheWify")
#define DEFAULT_WIFI_PASS PSTR("TheWify1.")

// =====================================
#define CFG_OTA_PASSWORD 32
#define DEFAULT_OTA_PORT 8266
#define DEFAULT_OTA_AUTH PSTR("OTA_Remora")

// =====================================

#define CFG_MQTT_PROTOCOL_SIZE     8
#define CFG_MQTT_HOST_SIZE         64
#define CFG_MQTT_USER_SIZE         32
#define CFG_MQTT_PASSWORD_SIZE     32
#define CFG_MQTT_DEFAULT_ACTIVATED true
#define CFG_MQTT_DEFAULT_PROTOCOL  PSTR("mqtt")
#define CFG_MQTT_DEFAULT_AUTH      true
#define CFG_MQTT_DEFAULT_HOST      PSTR("192.168.2.20")
#define CFG_MQTT_DEFAULT_PORT      1883
#define CFG_MQTT_DEFAULT_USER      PSTR("micropilote")
#define CFG_MQTT_DEFAULT_PASSWORD  PSTR("MicroPilote1.")

#pragma pack(push)  // push current alignment to stack
#pragma pack(1)     // set alignment to 1 byte boundary

// Config for MQTT
// 256 Bytes
typedef struct
{
  char     host[CFG_MQTT_HOST_SIZE+1];         // FQDN or ip
  char     protocol[CFG_MQTT_PROTOCOL_SIZE+1]; // Protocol (mqtt or mqtts)
  char     user[CFG_MQTT_USER_SIZE+1];         // User
  char     password[CFG_MQTT_PASSWORD_SIZE+1]; // Password
  uint16_t port;                               // Port
  bool     isActivated;                        // Enable/Disable MQTT
  bool     hasAuth;                            // Enable/Disable Auth
  uint8_t  filler[112];                        // in case adding data in config avoiding loosing current conf by bad crc*/
} _mqtt;

// Config saved into eeprom
// 1024 bytes total including CRC
typedef struct
{
  uint32_t id;                                 // uniq id default
  char ssid[CFG_SSID_SIZE+1];                  // SSID
  char psk[CFG_PSK_SIZE+1];                    // Pre shared key
  char host[CFG_HOSTNAME_SIZE+1];              // Hostname
  char ap_psk[CFG_PSK_SIZE+1];                 // Access Point Pre shared key
  char ota_auth[CFG_OTA_PASSWORD+1];           // OTA Authentication password
  uint16_t ota_port;                           // OTA port
  uint8_t  filler[21];                         // in case adding data in config avoiding loosing current conf by bad crc
  _mqtt    mqtt;                               // MQTT configuration
  uint16_t config_version;                     // Version actuel de la conf, permet de reset la conf en incrémentant la valeur
  uint8_t  filler1[508];                       //Another filler in case we need more
  uint16_t crc;
} _Config;


// Exported variables/object instancied in main sketch
// ===================================================
extern _Config config;

#pragma pack(pop)

// Declared exported function from route.cpp
// ===================================================
bool readConfig(bool clear_on_error=true);
bool saveConfig(void);
void resetConfig(void);
void showConfig(void);
void getConfJSONData(String &);
void getSysJSONData(String &);

#endif // CONFIG_h