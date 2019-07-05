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

#define CFG_DEFAULT_ID system_get_chip_id()

#define CFG_HOSTNAME_SIZE 16

#define CFG_SSID_SIZE     32
#define CFG_PSK_SIZE      64

#define CFG_MQTT_PROTOCOL_SIZE     5
#define CFG_MQTT_HOST_SIZE         32
#define CFG_MQTT_USER_SIZE         32
#define CFG_MQTT_PASSWORD_SIZE     32
#define CFG_MQTT_DEFAULT_ACTIVATED true
#define CFG_MQTT_DEFAULT_PROTOCOL  PSTR("mqtt")
#define CFG_MQTT_DEFAULT_AUTH      false
#define CFG_MQTT_DEFAULT_HOST      PSTR("mqtt_dev.lan")
#define CFG_MQTT_DEFAULT_PORT      1883

// Port pour l'OTA
#define DEFAULT_OTA_PORT     8266

#pragma pack(push)  // push current alignment to stack
#pragma pack(1)     // set alignment to 1 byte boundary

// Config for MQTT
// 128 Bytes
typedef struct
{
  char     host[CFG_MQTT_HOST_SIZE+1];         // FQDN or ip
  char     protocol[CFG_MQTT_PROTOCOL_SIZE+1]; // Protocol (mqtt or mqtts)
  char     user[CFG_MQTT_USER_SIZE+1];         // User
  char     password[CFG_MQTT_PASSWORD_SIZE+1]; // Password
  uint16_t port;                               // Port
  bool     isActivated;                        // Enable/Disable MQTT
  bool     hasAuth;                            // Enable/Disable Auth
  uint8_t  filler[19];                         // in case adding data in config avoiding loosing current conf by bad crc*/
} _mqtt;

// Config saved into eeprom
// 512 bytes total including CRC
typedef struct
{
  uint32_t id;                                 // uniq id default 
  char ssid[CFG_SSID_SIZE+1];                  // SSID
  char psk[CFG_PSK_SIZE+1];                    // Pre shared key
  char host[CFG_HOSTNAME_SIZE+1];              // Hostname
  char ap_psk[CFG_PSK_SIZE+1];                 // Access Point Pre shared key
  char ota_auth[CFG_PSK_SIZE+1];               // OTA Authentication password
  uint16_t ota_port;                           // OTA port
  uint8_t  filler[5];                          // in case adding data in config avoiding loosing current conf by bad crc
  _mqtt    mqtt;                               // MQTT configuration
  uint8_t  filler1[126];                       //Another filler in case we need more
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