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
// Written by Charles-Henri Hallard (http://hallard.me)
//
// History 2016-02-04 - First release
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

  // CRC Error ?
  if (crc != 0) {
    // Clear config if wanted
    if (clear_on_error)
      memset(&config, 0, sizeof( _Config ));
    return false;
  }

  // Check the config for new elements Compteur
  if (config.compteur_modele[0] == '\0')
    strcpy_P(config.compteur_modele, CFG_COMPTEUR_DEFAULT_MODELE);
  if (config.compteur_tic[0] == '\0')
    strcpy_P(config.compteur_tic, CFG_COMPTEUR_DEFAULT_TIC);

  // Check the config for new elements MQTT
  #ifdef MOD_MQTT
    if (config.mqtt.isActivated != true && config.mqtt.isActivated != false)
      config.mqtt.isActivated = CFG_MQTT_DEFAULT_ACTIVATED;
    if (config.mqtt.protocol[0] == '\0')
      strcpy_P(config.mqtt.protocol, CFG_MQTT_DEFAULT_PROTOCOL);
    if (config.mqtt.host[0] == '\0')
      strcpy_P(config.mqtt.host, CFG_MQTT_DEFAULT_HOST);
    if (config.mqtt.port == 0)
      config.mqtt.port = CFG_MQTT_DEFAULT_PORT;
    if (config.mqtt.hasAuth != true && config.mqtt.hasAuth != false)
      config.mqtt.hasAuth = CFG_MQTT_DEFAULT_AUTH;
  #endif

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

  //eepromDump(32);

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

  //eepromDump(32);

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
  Log.verbose(F("\r\nOTA port : %d"), config.ota_port);
  Log.verbose(F("\r\nConfig   : "));
  if (config.config & CFG_RGB_LED) Log.verbose(F(" RGB"));
  if (config.config & CFG_DEBUG)   Log.verbose(F(" DEBUG"));
  if (config.config & CFG_LCD)     Log.verbose(F(" LCD\r\n"));
  _wdt_feed();

  Log.verbose(F("\r\n===== Compteur =====\r\nModèle   : "));
  Log.verbose(config.compteur_modele);
  Log.verbose(F("\r\nTIC      : "));
  Log.verbose(config.compteur_tic);
  Log.verbose("\r\n");
  _wdt_feed();

  #ifdef MOD_EMONCMS
    Log.verbose(F("\r\n===== Emoncms =====\r\nhost     : "));
    Log.verbose(config.emoncms.host);
    Log.verbose(F("\r\nport     : %d\r\nurl      : "), config.emoncms.port);
    Log.verbose(config.emoncms.url);
    Log.verbose(F("\r\nkey      : "));
    Log.verbose(config.emoncms.apikey);
    Log.verbose(F("\r\nnode     : %d\r\nreq     : %l\r\n"
                 ), config.emoncms.node
                  , config.emoncms.freq
                );
    _wdt_feed();
  #endif

  #ifdef MOD_JEEDOM
    Log.verbose(F("\r\n===== Jeedom =====\r\nhost     : "));
    Log.verbose(config.jeedom.host);
    Log.verbose(F("port     : %d\r\nurl      : "), config.jeedom.port);
    Log.verbose(config.jeedom.url);
    Log.verbose(F("\r\nkey      : "));
    Log.verbose(config.jeedom.apikey);
    Log.verbose(F("\r\nfinger   : "));
    for (int i=0; i < CFG_JDOM_FINGER_PRINT_SIZE; i++) {
      Log.verbose("%x ", config.jeedom.fingerprint[i]);
    }
    Log.verbose(F("\r\ncompteur : "));
    Log.verbose(config.jeedom.adco);
    Log.verbose(F("\r\nfreq     : %l\r\n"), config.jeedom.freq);
    _wdt_feed();
  #endif

  #ifdef MOD_MQTT
    Log.verbose(F("\r\n===== MQTT =====\r\nIsActivated : %T\r\n"), config.mqtt.isActivated);
    Log.verbose(F("host        : "));
    Log.verbose(config.mqtt.host);
    Log.verbose(F("\r\nport        : %d\r\nprotocol    : "), config.mqtt.port);
    Log.verbose(config.mqtt.protocol);
    Log.verbose(F("\r\nHasAuth     : %T\r\nuser        : "), config.mqtt.hasAuth);
    Log.verbose(config.mqtt.user);
    Log.verbose("\r\n");
    _wdt_feed();
  #endif

  Log.verbose(F("\r\nLED Bright: %d\r\n"), config.led_bright);
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

  // Set default Hostname
  sprintf_P(config.host, PSTR("Remora_%06X"), ESP.getChipId());
  strcpy_P(config.ota_auth, DEFAULT_OTA_AUTH);
  config.ota_port = DEFAULT_OTA_PORT ;

  // Add other init default config here

  // Compteur
  strcpy_P(config.compteur_modele, CFG_COMPTEUR_DEFAULT_MODELE);
  strcpy_P(config.compteur_tic, CFG_COMPTEUR_DEFAULT_TIC);

  // Emoncms
  #ifdef MOD_EMONCMS
    strcpy_P(config.emoncms.host, CFG_EMON_DEFAULT_HOST);
    config.emoncms.port = CFG_EMON_DEFAULT_PORT;
    strcpy_P(config.emoncms.url, CFG_EMON_DEFAULT_URL);
    config.emoncms.apikey[0] = '\0';
    config.emoncms.node = 0;
    config.emoncms.freq = 0;
  #endif

  // Jeedom
  #ifdef MOD_JEEDOM
    strcpy_P(config.jeedom.host, CFG_JDOM_DEFAULT_HOST);
    config.jeedom.port = CFG_JDOM_DEFAULT_PORT;
    strcpy_P(config.jeedom.url, CFG_JDOM_DEFAULT_URL);
    strcpy_P(config.jeedom.adco, CFG_JDOM_DEFAULT_ADCO);
    config.jeedom.apikey[0] = '\0';
    for (int i=0; i < CFG_JDOM_FINGER_PRINT_SIZE; i++) {
      config.jeedom.fingerprint[i] = 0;
    }
    config.jeedom.freq = 0;
  #endif

  // MQTT
  #ifdef MOD_MQTT
    config.mqtt.isActivated = CFG_MQTT_DEFAULT_ACTIVATED;
    strcpy_P(config.mqtt.protocol, CFG_MQTT_DEFAULT_PROTOCOL);
    strcpy_P(config.mqtt.host, CFG_MQTT_DEFAULT_HOST);
    config.mqtt.port = CFG_MQTT_DEFAULT_PORT;
    config.mqtt.hasAuth = CFG_MQTT_DEFAULT_AUTH;
    strcpy_P(config.mqtt.user, "");
    strcpy_P(config.mqtt.password, "");
  #endif

  config.led_bright = DEFAULT_LED_BRIGHTNESS;
  config.config |= CFG_RGB_LED;

  // save back
  saveConfig();
}

#ifdef MOD_JEEDOM
String getFingerPrint(void) {
  char buffer[61] = { 0 };

  sprintf_P(buffer, PSTR("%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X")
    , config.jeedom.fingerprint[0], config.jeedom.fingerprint[1], config.jeedom.fingerprint[2], config.jeedom.fingerprint[3]
    , config.jeedom.fingerprint[4], config.jeedom.fingerprint[5], config.jeedom.fingerprint[6], config.jeedom.fingerprint[7]
    , config.jeedom.fingerprint[8], config.jeedom.fingerprint[9], config.jeedom.fingerprint[10], config.jeedom.fingerprint[11]
    , config.jeedom.fingerprint[12], config.jeedom.fingerprint[13], config.jeedom.fingerprint[14], config.jeedom.fingerprint[15]
    , config.jeedom.fingerprint[16], config.jeedom.fingerprint[17], config.jeedom.fingerprint[18], config.jeedom.fingerprint[19]);
  return String(buffer);
}
#endif
