// **********************************************************************************
// Remora Configuration include file
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
// History 2016-02-04 - First release
//
// All text above must be included in any redistribution.
//
// **********************************************************************************

#ifndef __CONFIG_H__
#define __CONFIG_H__

// Include main project include file
#include "remora.h"

#define DEFAULT_HOSTNAME PSTR("remora")

// Définir ici les identifiants de
// connexion à votre réseau Wifi
// =====================================
#define DEFAULT_WIFI_SSID PSTR("")
#define DEFAULT_WIFI_PASS PSTR("")
#define DEFAULT_WIFI_AP_PASS PSTR("Remora_WiFi")
// =====================================
#define CFG_OTA_PASSWORD 32
#define DEFAULT_OTA_PORT 8266
#define DEFAULT_OTA_AUTH PSTR("OTA_Remora")

#define CFG_HOSTNAME_SIZE 16
#define CFG_SSID_SIZE     32
#define CFG_PSK_SIZE      64

// Fils pilotes
#ifndef MOD_MICRO
  #define NB_REAL_FILS_PILOTES 7
  #define NB_VIRTUAL_FILS_PILOTES 0
#else
  #define NB_REAL_FILS_PILOTES 7
  #define NB_VIRTUAL_FILS_PILOTES 8
#endif

#define DEFAULT_RELAIS_REVERSE false

#ifdef MOD_TELEINFO
  #define CFG_COMPTEUR_MODELE_SIZE 12
  #define CFG_COMPTEUR_TIC_SIZE    10
  #define CFG_COMPTEUR_DEFAULT_MODELE PSTR("electronique")
  #define CFG_COMPTEUR_DEFAULT_TIC    PSTR("historique")
#endif

#ifdef MOD_EMONCMS
  #define CFG_EMON_HOST_SIZE    32
  #define CFG_EMON_APIKEY_SIZE  32
  #define CFG_EMON_URL_SIZE     32
  #define CFG_EMON_DEFAULT_PORT 80
  #define CFG_EMON_DEFAULT_HOST PSTR("emoncms.org")
  #define CFG_EMON_DEFAULT_URL  PSTR("/input/post.json")
#endif

#ifdef MOD_JEEDOM
  #define CFG_JDOM_HOST_SIZE         32
  #define CFG_JDOM_APIKEY_SIZE       48
  #define CFG_JDOM_URL_SIZE          64
  #define CFG_JDOM_ADCO_SIZE         12
  #define CFG_JDOM_FINGER_PRINT_SIZE 20
  #define CFG_JDOM_DEFAULT_PORT      80
  #define CFG_JDOM_DEFAULT_HOST      PSTR("jeedom.local")
  #define CFG_JDOM_DEFAULT_URL       PSTR("/jeedom/plugins/teleinfo/core/php/jeeTeleinfo.php")
  #define CFG_JDOM_DEFAULT_ADCO      PSTR("000011112222")
#endif

#ifdef MOD_MQTT
  #define CFG_MQTT_PROTOCOL_SIZE     5
  #define CFG_MQTT_HOST_SIZE         32
  #define CFG_MQTT_USER_SIZE         32
  #define CFG_MQTT_PASSWORD_SIZE     32
  #define CFG_MQTT_DEFAULT_ACTIVATED false
  #define CFG_MQTT_DEFAULT_PROTOCOL  PSTR("mqtt")
  #define CFG_MQTT_DEFAULT_AUTH      false
  #define CFG_MQTT_DEFAULT_HOST      PSTR("mqtt.local")
  #define CFG_MQTT_DEFAULT_PORT      1883
#endif

#define DEFAULT_LED_BRIGHTNESS  50                // 50%

// Bit definition for different configuration modes
#define CFG_LCD         0x0001  // Enable display
#define CFG_DEBUG       0x0002  // Enable serial debug
#define CFG_RGB_LED     0x0004  // Enable RGB LED
#define CFG_LED_AWAKE   0x0008  // Enable led blink when awake (take care, consumption)
#define CFG_LED_TX      0x0010  // Enable led blink when RF transmitting (take care, consumption)
#define CFG_FLIP_LCD    0x0020  // Flip display
#define CFG_RF_NOSEND   0x0040  // Disable this gateway to send packet (including ACK)
#define CFG_RF_ACK      0x0080  // Enable this gateway to request ACK on send
#define CFG_BAD_CRC     0x8000  // Bad CRC when reading configuration

// Web Interface Configuration Form field names
#define CFG_FORM_SSID     PSTR("ssid")
#define CFG_FORM_PSK      PSTR("psk")
#define CFG_FORM_HOST     PSTR("host")
#define CFG_FORM_AP_PSK   PSTR("ap_psk")
#define CFG_FORM_OTA_AUTH PSTR("ota_auth")
#define CFG_FORM_OTA_PORT PSTR("ota_port")

#ifdef MOD_TELEINFO
  #define CFG_FORM_COMPTEUR_MODELE PSTR("compteur_modele")
  #define CFG_FORM_COMPTEUR_TIC    PSTR("compteur_tic")
#endif

#ifdef MOD_EMONCMS
  #define CFG_FORM_EMON_HOST  PSTR("emon_host")
  #define CFG_FORM_EMON_PORT  PSTR("emon_port")
  #define CFG_FORM_EMON_URL   PSTR("emon_url")
  #define CFG_FORM_EMON_KEY   PSTR("emon_apikey")
  #define CFG_FORM_EMON_NODE  PSTR("emon_node")
  #define CFG_FORM_EMON_FREQ  PSTR("emon_freq")
#endif

#ifdef MOD_JEEDOM
  #define CFG_FORM_JDOM_HOST  PSTR("jdom_host")
  #define CFG_FORM_JDOM_PORT  PSTR("jdom_port")
  #define CFG_FORM_JDOM_URL   PSTR("jdom_url")
  #define CFG_FORM_JDOM_KEY   PSTR("jdom_apikey")
  #define CFG_FORM_JDOM_ADCO  PSTR("jdom_adco")
  #define CFG_FORM_JDOM_FREQ  PSTR("jdom_freq")
  #define CFG_FORM_JDOM_FING  PSTR("jdom_finger")
#endif

#ifdef MOD_MQTT
  #define CFG_FORM_MQTT_ACTIVATED PSTR("mqtt_isActivated")
  #define CFG_FORM_MQTT_PROTO     PSTR("mqtt_protocol")
  #define CFG_FORM_MQTT_HOST      PSTR("mqtt_host")
  #define CFG_FORM_MQTT_PORT      PSTR("mqtt_port")
  #define CFG_FORM_MQTT_AUTH      PSTR("mqtt_hasAuth")
  #define CFG_FORM_MQTT_USER      PSTR("mqtt_user")
  #define CFG_FORM_MQTT_PASS      PSTR("mqtt_password")
#endif

#define CFG_FORM_LED_BRIGHT PSTR("cfg_led_bright");

#define CFG_FORM_IP  PSTR("wifi_ip");
#define CFG_FORM_GW  PSTR("wifi_gw");
#define CFG_FORM_MSK PSTR("wifi_msk");

#define CFG_ZONE_FP_ID_SIZE       8
#define CFG_ZONE_FP_NAME_SIZE    25

#pragma pack(push)  // push current alignment to stack
#pragma pack(1)     // set alignment to 1 byte boundary


// Config for emoncms
// 128 Bytes
#ifdef MOD_EMONCMS
  typedef struct
  {
    char  host[CFG_EMON_HOST_SIZE+1];     // FQDN
    char  apikey[CFG_EMON_APIKEY_SIZE+1]; // Secret
    char  url[CFG_EMON_URL_SIZE+1];       // Post URL
    uint16_t port;                        // Protocol port (HTTP/HTTPS)
    uint16_t node;                        // optional node
    uint32_t freq;                        // refresh rate
    uint8_t filler[21];                   // in case adding data in config avoiding loosing current conf by bad crc*/
  } _emoncms;
#endif

// Config for jeedom
// 256 Bytes
#ifdef MOD_JEEDOM
  typedef struct
  {
    char     host[CFG_JDOM_HOST_SIZE+1];              // FQDN
    char     apikey[CFG_JDOM_APIKEY_SIZE+1];          // Secret
    char     url[CFG_JDOM_URL_SIZE+1];                // Post URI
    char     adco[CFG_JDOM_ADCO_SIZE+1];              // Identifiant compteur
    uint8_t  fingerprint[CFG_JDOM_FINGER_PRINT_SIZE]; // Finger print SHA1 SSL
    uint16_t port;                                    // Protocol port (HTTP/HTTPS)
    uint32_t freq;                                    // refresh rate
    uint8_t  filler[89];                              // in case adding data in config avoiding loosing current conf by bad crc*/
  } _jeedom;
#endif

// Config for MQTT
// 128 Bytes
#ifdef MOD_MQTT
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
#endif

// Config for zone fp
// 32 Bytes
typedef struct
{
  uint32_t id;                                // pins fp ou valeur system_get_chip_id
  char name[CFG_ZONE_FP_NAME_SIZE+1];         // nom afficher sur la partie web
  bool is_virtual;
  bool is_enable;
} _zone_fp;

// 512 Bytes
typedef struct
{
  bool relais_reverse;
  #ifndef MOD_MICRO
    _zone_fp fp[NB_REAL_FILS_PILOTES];
    uint8_t filler_micro[256];
  #else
    _zone_fp fp[NB_REAL_FILS_PILOTES + NB_VIRTUAL_FILS_PILOTES];
    uint8_t filler[31];
  #endif
} _zones_fp;

// Config saved into eeprom
// 1536 bytes total including CRC
typedef struct
{
  char ssid[CFG_SSID_SIZE+1];                         // SSID
  char psk[CFG_PSK_SIZE+1];                           // Pre shared key
  char host[CFG_HOSTNAME_SIZE+1];                     // Hostname
  char ap_psk[CFG_PSK_SIZE+1];                        // Access Point Pre shared key
  char ota_auth[CFG_OTA_PASSWORD+1];                  // OTA Authentication password
  uint32_t config;                                    // Bit field register
  uint16_t ota_port;                                  // OTA port
  uint8_t  led_bright;                                // RGB Led brightness 0-255
  uint16_t oled_type;                                 // Display OLED type (1306 or 1106)
  #ifdef MOD_TELEINFO
    char compteur_modele[CFG_COMPTEUR_MODELE_SIZE+1]; // Modele de compteur
    char compteur_tic[CFG_COMPTEUR_TIC_SIZE+1];       // TIC mode
    uint8_t  filler[10];
  #else
    uint8_t  filler[34];                              // in case adding data in config avoiding loosing current conf by bad crc
  #endif                                              // 256 Bytes
  #ifdef MOD_EMONCMS
    _emoncms emoncms;                                 // Emoncms configuration
  #else                                               // 128 Bytes
    uint8_t  filler_emoncms[128];
  #endif
  #ifdef MOD_JEEDOM
    _jeedom  jeedom;                                  // jeedom configuration
  #else                                               // 256 Byte
    uint8_t  filler_jeedom[256];
  #endif
  #ifdef MOD_MQTT
    _mqtt  mqtt;                                      // MQTT configuration
  #else                                               // 256 Bytes
    uint8_t  filler_mqtt[256];
  #endif
  _zones_fp zones_fp;                                 // 512 Bytes
  uint8_t  filler_end[126];                           //Another filler in case we need more
  uint16_t crc;
} _Config;


#ifdef MOD_MICRO
  // Struct system
  // 48 Bytes
  typedef struct {
    uint32_t uptime; 
    uint8    cpu_freq;
    uint32_t flash_real_size;
    uint32_t firmware_size;
    uint32_t free_size;
    int32_t  rssi;
    uint32   free_ram;
    char     heap_fragmentation[7];
    char     ip[16];
  } _mp_system;

  // Struct mqtt config
  // 107 Byte
  typedef struct {
    char     host[CFG_MQTT_HOST_SIZE+1];
    char     protocol[CFG_MQTT_PROTOCOL_SIZE+1];
    char     user[CFG_MQTT_USER_SIZE+1];
    char     password[CFG_MQTT_PASSWORD_SIZE+1];
    uint16_t port;
  } _config_mqtt;

  // Struct config
  // 166 + 107 Bytes
  typedef struct {

    char         hostname[CFG_HOSTNAME_SIZE*2+1];
    char         ssid[CFG_SSID_SIZE+1];
    char         psk[CFG_PSK_SIZE+1];
    char         ota_auth[CFG_OTA_PASSWORD+1];
    uint16_t     ota_port;
    _config_mqtt mqtt;
  } _mp_config;

  // Struct mp_info
  // 2568 Bytes
  typedef struct {
    _mp_system system[NB_VIRTUAL_FILS_PILOTES];
    _mp_config config[NB_VIRTUAL_FILS_PILOTES];
    bool isOnline[NB_VIRTUAL_FILS_PILOTES];
  } _mp_info;
#endif


// Exported variables/object instancied in main sketch
// ===================================================
extern _Config config;
extern _mp_info mp_info;

#pragma pack(pop)

// Declared exported function from route.cpp
// ===================================================
bool readConfig(bool clear_on_error=true);
bool saveConfig(void);
void resetConfig(void);
void showConfig(void);
#ifdef MOD_JEEDOM
  String getFingerPrint(void);
#endif

#endif // CONFIG_h