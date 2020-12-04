// **********************************************************************************
// Remora Configuration source file
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

#include "./config.h"

// Configuration structure for whole program
_Config config;

uint16_t crc16Update(uint16_t crc, uint8_t a)
{
  int i;
  crc ^= a;
  for (i = 0; i < 8; ++i)  {
    if (crc & 1)
      crc = (crc >> 1) ^ 0xA001;
    else
      crc = (crc >> 1);
  }
  return crc;
}

/* ======================================================================
Function: eeprom_dump
Purpose : dump eeprom value to serial
Input   : -
Output  : -
Comments: -
====================================================================== */
#ifdef DEBUG
void eepromDump(uint8_t bytesPerRow)
{
  uint16_t i;
  uint16_t j=0 ;

  // default to 16 bytes per row
  if (bytesPerRow==0)
    bytesPerRow=16;

  Log.verbose("\r\n");

  // loop thru EEP address
  for (i = 0; i < sizeof(_Config); i++) {
    // First byte of the row ?
    if (j==0) {
      // Display Address
      Log.verbose(F("%04X : "), i);
    }

    // write byte in hex form
    Log.verbose(F("%04X : "), EEPROM.read(i));
    Debugf("%02X ", EEPROM.read(i));

    // Last byte of the row ?
    // start a new line
    if (++j >= bytesPerRow) {
      j=0;
      Log.verbose("\r\n");
    }
  }
}
#endif

/* ======================================================================
Function: readConfig
Purpose : fill config structure with data located into eeprom
Input   : true if we need to clear actual struc in case of error
Output  : true if config found and crc ok, false otherwise
Comments: -
====================================================================== */
bool readConfig (bool clear_on_error)
{
  uint16_t crc = ~0;
  uint8_t * pconfig = (uint8_t *) &config ;
  uint8_t data ;

  // For whole size of config structure
  for (uint16_t i = 0; i < sizeof(_Config); ++i) {
    // read data
    data = EEPROM.read(i);

    // save into struct
    *pconfig++ = data ;

    // calc CRC
    crc = crc16Update(crc, data);
  }

  // CRC Error ? OR new version
  if (crc != 0) {
    return false;
  }
  if (config.config_version < CFG_VERSION) {
    return false;
  }

  // Check the config for new elements MQTT
  /*if (config.mqtt.isActivated != true && config.mqtt.isActivated != false)
    config.mqtt.isActivated = CFG_MQTT_DEFAULT_ACTIVATED;
  if (config.mqtt.protocol[0] == '\0')
    strcpy_P(config.mqtt.protocol, CFG_MQTT_DEFAULT_PROTOCOL);
  if (config.mqtt.host[0] == '\0')
    strcpy_P(config.mqtt.host, CFG_MQTT_DEFAULT_HOST);
  if (config.mqtt.port == 0)
    config.mqtt.port = CFG_MQTT_DEFAULT_PORT;
  if (config.mqtt.user[0] == '\0')
    strcpy_P(config.mqtt.host, CFG_MQTT_DEFAULT_USER);
  if (config.mqtt.password[0] == '\0')
    strcpy_P(config.mqtt.host, CFG_MQTT_DEFAULT_PASSWORD);
  if (config.mqtt.hasAuth != true && config.mqtt.hasAuth != false)
    config.mqtt.hasAuth = CFG_MQTT_DEFAULT_AUTH;
  */
  return true ;
}

/* ======================================================================
Function: saveConfig
Purpose : save config structure values into eeprom
Input   : -
Output  : true if saved and readback ok
Comments: once saved, config is read again to check the CRC
====================================================================== */
bool saveConfig (void)
{
  uint8_t * pconfig ;
  bool ret_code;

  // Init pointer
  pconfig = (uint8_t *) &config ;

  // Init CRC
  config.crc = ~0;

  // For whole size of config structure, pre-calculate CRC
  for (uint16_t i = 0; i < sizeof (_Config) - 2; ++i) {
    config.crc = crc16Update(config.crc, *pconfig++);
  }

  // Re init pointer
  pconfig = (uint8_t *) &config ;

  // For whole size of config structure, write to EEP
  for (uint16_t i = 0; i < sizeof(_Config); ++i)
    EEPROM.write(i, *pconfig++);

  // Physically save
  EEPROM.commit();

  // Read Again to see if saved ok, but do
  // not clear if error this avoid clearing
  // default config and breaks OTA
  ret_code = readConfig(false);

  #ifdef DEBUG
    DebugF("Write config ");

    if (ret_code) {
      Debugln(F("OK!"));
    } else {
      Debugln(F("Error!"));
    }
  #endif

  // return result
  return (ret_code);
}

/* ======================================================================
Function: showConfig
Purpose : display configuration
Input   : -
Output  : -
Comments: -
====================================================================== */
#ifndef DISABLE_LOGGING
void showConfig()
{
  Log.verbose(F("== Config Version ==\r\n"));
  Log.verbose(F("%d\r\n"), config.config_version);
  Log.verbose(F("===== Wifi =====\r\nssid     : "));
  Log.verbose(config.ssid);
  Log.verbose(F("\r\npsk      : "));
  Log.verbose(config.psk);
  Log.verbose(F("\r\nhost     : "));
  Log.verbose(config.host);
  Log.verbose(F("\r\nap_psk   : "));
  Log.verbose(config.ap_psk);
  Log.verbose(F("\r\nOTA auth : "));
  Log.verbose(config.ota_auth);
  Log.verbose(F("\r\nOTA port : %d\r\n"), config.ota_port);
  _wdt_feed();

  Log.verbose(F("\r\n===== MQTT =====\r\nIsActivated : %T\r\n"), config.mqtt.isActivated);
  Log.verbose(F("host        : "));
  Log.verbose(config.mqtt.host);
  Log.verbose(F("\r\nport        : %d\r\nprotocol    : "), config.mqtt.port);
  Log.verbose(config.mqtt.protocol);
  Log.verbose(F("\r\nHasAuth     : %T\r\nuser        : "), config.mqtt.hasAuth);
  Log.verbose(config.mqtt.user);
  Log.verbose(F("\r\npassword    : "));
  Log.verbose(config.mqtt.password);
  Log.verbose("\r\n");
  _wdt_feed();
}
#endif

/* ======================================================================
Function: ResetConfig
Purpose : Set configuration to default values
Input   : -
Output  : -
Comments: -
====================================================================== */
void resetConfig(void)
{
  // enable default configuration
  memset(&config, 0, sizeof(_Config));

  config.config_version = CFG_VERSION;
  config.id = system_get_chip_id();

  // Set default Hostname
  char hostname[32] = "";
  char chipId[10] = "";
  strcat_P(hostname, DEFAULT_HOSTNAME);
  sprintf(chipId, PSTR("_%06X"), system_get_chip_id());
  strcat(hostname, chipId);
  //sprintf_P(config.host, PSTR("DEFAULT_HOSTNAME_%06X"), ESP.getChipId());
  strcpy(config.host, hostname);

  // WIfi
  strcpy_P(config.ssid, DEFAULT_WIFI_SSID);
  strcpy_P(config.psk, DEFAULT_WIFI_PASS);

  // OTA
  strcpy_P(config.ota_auth, DEFAULT_OTA_AUTH);
  config.ota_port = DEFAULT_OTA_PORT ;

  // Add other init default config here

  // MQTT
  config.mqtt.isActivated = CFG_MQTT_DEFAULT_ACTIVATED;
  strcpy_P(config.mqtt.protocol, CFG_MQTT_DEFAULT_PROTOCOL);
  strcpy_P(config.mqtt.host, CFG_MQTT_DEFAULT_HOST);
  config.mqtt.port = CFG_MQTT_DEFAULT_PORT;
  strcpy_P(config.mqtt.user, CFG_MQTT_DEFAULT_USER);
  strcpy_P(config.mqtt.password, CFG_MQTT_DEFAULT_PASSWORD);
  config.mqtt.hasAuth = CFG_MQTT_DEFAULT_AUTH;

  // save back
  saveConfig();
}

/* ======================================================================
Function: getConfigJSONData
Purpose : Return JSON string containing configuration data
Input   : Response String
Output  : -
Comments: -
====================================================================== */
void getConfJSONData(String &response)
{
  const size_t capacity = JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(7) + 300;
  StaticJsonDocument<capacity> doc;

  doc["id"]       = config.id;
  doc["hostname"] = config.host;
  doc["ssid"]     = config.ssid;
  doc["psk"]      = config.psk;
  doc["ota_auth"] = config.ota_auth;
  doc["ota_port"] = config.ota_port;

  JsonObject mqtt = doc.createNestedObject("mqtt");
  mqtt["host"]     = config.mqtt.host;
  mqtt["port"]     = config.mqtt.port;
  mqtt["protocol"] = config.mqtt.protocol;
  mqtt["hasAuth"]  = config.mqtt.hasAuth;
  mqtt["user"]     = config.mqtt.user;
  mqtt["password"] = config.mqtt.password;

  serializeJson(doc, response);
}

/* ======================================================================
Function: getSysJSONData
Purpose : Return JSON string containing system data
Input   : Response String
Output  : -
Comments: -
====================================================================== */
void getSysJSONData(String &response)
{
  const size_t capacity = JSON_OBJECT_SIZE(13) + 406;
  StaticJsonDocument<capacity> doc;
  char buffer[60];
  
  doc["id"] = config.id;

  doc["uptime"] = uptime;

  doc["sdk_version"] = system_get_sdk_version();
 
  doc["cpu_freq"]        = system_get_cpu_freq();
  doc["flash_real_size"] = ESP.getFlashChipRealSize();
  doc["firmware_size"]   = ESP.getSketchSize();
  doc["free_size"]       = ESP.getFreeSketchSpace();

  doc["ip"]   = WiFi.localIP().toString();
  doc["mac"]  = WiFi.macAddress();
  doc["ssid"] = WiFi.SSID();
  doc["rssi"] = WiFi.RSSI();

  doc["free_ram"] = system_get_free_heap_size();
  sprintf_P(buffer, PSTR("%.2f"), 100 - getLargestAvailableBlock() * 100.0 / getTotalAvailableMemory());
  doc["heap_fragmentation"] = buffer;

  serializeJson(doc, response);
}