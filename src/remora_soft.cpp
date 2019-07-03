// **********************************************************************************
// Micro module Fil Pilote
// **********************************************************************************
// Copyright (C) 2014 Thibault Ducret
// Licence MIT
//
//
// **********************************************************************************

// Tout est inclus dans le fichier remora.h
#include "remora.h"

// Variables globales
// ==================
// status global de l'application
uint16_t status = 0;
unsigned long uptime = 0;
bool first_setup;
bool got_first = false;

// Nombre de deconexion cloud detectée
int my_cloud_disconnect = 0;

// Use WiFiClient class to create a connection to WEB server
WiFiClient client;

WiFiEventHandler wifiStaConnectHandler;
WiFiEventHandler wifiStaDisconnectHandler;
Ticker wifiReconnectTimer;

bool ota_blink;

bool reboot = false;


/* ======================================================================
Function: WifiHandleConn
Purpose : Handle Wifi connection / reconnection and OTA updates
Input   : setup true if we're called 1st Time from setup
Output  : state of the wifi status
Comments: -
====================================================================== */
int WifiHandleConn(boolean setup = false)
{
  int ret = WiFi.status();
  uint8_t timeout ;

  if (setup) {
    // Feed the dog
    _wdt_feed();
    Log.verbose(F("========== SDK Saved parameters Start"));
    WiFi.printDiag(DEBUG_SERIAL);
    Log.verbose(F("========== SDK Saved parameters End\r\n"));

    if (*config.ssid) {
      Log.verbose(F("Connection au Wifi : "));
      Log.verbose(config.ssid);

      // Do wa have a PSK ?
      if (*config.psk) {
        // protected network
        Log.verbose(F(" avec la clé '"));
        Log.verbose(config.psk);
        Log.verbose(F("'...\r\n"));

        WiFi.begin(config.ssid, config.psk);
      } else {
        // Open network
        Log.verbose(F("AP Ouvert\r\n"));
        WiFi.begin(config.ssid);
        Log.verbose(F("\r\n"));
      }
    }

    timeout = 25; // 25 * 200 ms = 5 sec time out

    // 200 ms loop
    while ( ((ret = WiFi.status()) != WL_CONNECTED) && timeout )
    {
       --timeout;
      _wdt_feed();
    }

    // connected ? disable AP, client mode only
    if (ret == WL_CONNECTED)
    {
      WiFi.mode(WIFI_STA);

      Log.verbose(F("connecte!\r\n"));
      Log.verbose(F("IP address   : "));
      Log.verbose(WiFi.localIP().toString().c_str());
      Log.verbose("\r\n");
      Log.verbose(F("MAC address  : "));
      Log.verbose(WiFi.macAddress().c_str());
      Log.verbose("\r\n");

    // not connected
    } else {
      Log.verbose(F("Erreur, connexion au Wifi "));
      Log.verbose(F("'\r\n"));
    }

    // Feed the dog
    _wdt_feed();

    // Set OTA parameters
     ArduinoOTA.setPort(config.ota_port);
     ArduinoOTA.setHostname(config.host);
     if (*config.ota_auth) {
       ArduinoOTA.setPassword(config.ota_auth);
     }
     ArduinoOTA.begin();

    // just in case your sketch sucks, keep update OTA Available
    // Trust me, when coding and testing it happens, this could save
    // the need to connect FTDI to reflash
    // Usefull just after 1st connexion when called from setup() before
    // launching potentially buggy main()
    for (uint8_t i=0; i<= 10; i++) {
      delay(300);
      ArduinoOTA.handle();
    }

  } // if setup

  return WiFi.status();
}

void WifiReConn(void) {
  WifiHandleConn(true);
}

/* ======================================================================
Function: onWifiStaConnect
Purpose : Connect to MQTT brocker when WiFi STA is UP
Input   : event
Output  :
Comments: Fire when the WiFi Station get ip
====================================================================== */
void onWifiStaConnect(const WiFiEventStationModeGotIP& event) {
  Log.notice(F("Connecté au WiFi STA, IP : "));
  Log.notice(WiFi.localIP().toString().c_str());
  if (config.mqtt.isActivated && !mqttClient.connected() && !first_setup) {
    connectToMqtt();
  }
}

/* ======================================================================
Function: onWifiStaDisconnect
Purpose : Suspend connection to MQTT brocker and reconnect the WiFi STA.
Input   : event
Output  :
Comments: Fire when the WiFi Station is disconnected
====================================================================== */
void onWifiStaDisconnect(const WiFiEventStationModeDisconnected& event) {
  Log.error(F("Déconecté du WiFi.\r\n"));
  wifiReconnectTimer.once(2, WifiReConn);
}

/* ======================================================================
Function: timeAgo
Purpose : format total seconds to human readable text
Input   : second
Output  : pointer to string
Comments: -
====================================================================== */
char * timeAgo(unsigned long sec)
{
  static char buff[16];

  // Clear buffer
  buff[0] = '\0';

  if (sec < 2) {
    sprintf_P(buff,PSTR("just now"));
  } else if (sec < 60) {
    sprintf_P(buff,PSTR("%d seconds ago"), sec);
  } else if (sec < 120) {
    sprintf_P(buff,PSTR("1 minute ago"));
  } else if (sec < 3600) {
    sprintf_P(buff,PSTR("%d minutes ago"), sec/60);
  } else if (sec < 7200) {
    sprintf_P(buff,PSTR("1 hour ago"));
  } else if (sec < 86400) {
    sprintf_P(buff,PSTR("%d hours ago"), sec/3660);
  } else if (sec < 172800) {
    sprintf_P(buff,PSTR("yesterday"));
  } else if (sec < 604800) {
    sprintf_P(buff,PSTR("%d days ago"), sec/86400);
  }
  return buff;
}

/* ======================================================================
Function: printTimestamp
Purpose : Logger : get timestamps as prefix
Input   : -
Output  : -
Comments: -
====================================================================== */
/*
void printTimestamp(Print* _logOutput) {
  char c[12];
  sprintf(c, "%10lu ", millis());
  _logOutput->print(c);
}
*/

/* ======================================================================
Function: printNewline
Purpose : Logger : get newline as suffix
Input   : -
Output  : -
Comments: -
====================================================================== */
/*
void printNewline(Print* _logOutput) {
  _logOutput->print('\n');
}
*/

/* ======================================================================
Function: setup
Purpose : prepare and init stuff, configuration, ..
Input   : -
Output  : -
Comments: -
====================================================================== */
void setup()
{
  #ifndef DISABLE_LOGGING
    DEBUG_SERIAL.begin(115200);
    DEBUG_SERIAL.setDebugOutput(true);
  #endif

  Log.begin(LOG_LEVEL, &DEBUG_SERIAL, false);
  //Log.setPrefix(printTimestamp); // Uncomment to get timestamps as prefix
  //Log.setSuffix(printNewline); // Uncomment to get newline as suffix

  // says main loop to do setup
  first_setup = true;

  Log.notice(F("Starting Micro pilote\r\n"));
  Log.verbose(F(" Version "));
  Log.verbose(F(REMORA_SOFT_VERSION));
  Log.notice(F("\r\n"));
}


/* ======================================================================
Function: mysetup
Purpose : prepare and init stuff, configuration, ..
Input   : -
Output  : -
Comments: -
====================================================================== */
void mysetup()
{
  Log.notice(F("Starting main setup\r\n"));

  // Our configuration is stored into EEPROM
  EEPROM.begin(sizeof(_Config));

  Log.verbose(F("Config size = %l\r\n"), sizeof(_Config));
  Log.verbose(F(" - mqtt = %l\r\n"), sizeof(_mqtt));

  // Read Configuration from EEPROM
  if (!readConfig()) {
    // Reset Configuration
    resetConfig();

    // save back
    saveConfig();

    Log.error(F("Read Configuration => Reset to default\r\n"));
  }
  else {
    Log.verbose(F("Read Configuration => Good CRC, not set!\r\n"));
  }

  #ifndef DISABLE_LOGGING
    showConfig();
  #endif

  // Connection au Wifi ou Vérification
  wifiStaConnectHandler    = WiFi.onStationModeGotIP(onWifiStaConnect);
  wifiStaDisconnectHandler = WiFi.onStationModeDisconnected(onWifiStaDisconnect);

  // Connection au Wifi ou Vérification
  wifi_station_set_hostname(config.host);
  WifiHandleConn(true);

  // OTA callbacks
  ArduinoOTA.onStart([]() {
    Log.notice(F("\r\nUpdate Started...\r\n"));

    // On affiche le début de la mise à jour OTA sur l'afficheur
    ota_blink = true;
  });

  ArduinoOTA.onEnd([]() {
    Log.notice(F("\r\nUpdate finished restarting\r\n"));
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    ota_blink = !ota_blink;
  });

  ArduinoOTA.onError([](ota_error_t error) {
    char strErr[15]; // Contient le message d'erreur

    switch (error) {
      case OTA_AUTH_ERROR: sprintf_P(strErr, PSTR("Auth Failed")); break;
      case OTA_BEGIN_ERROR: sprintf_P(strErr, PSTR("Begin Failed")); break;
      case OTA_CONNECT_ERROR: sprintf_P(strErr, PSTR("Connect Failed")); break;
      case OTA_RECEIVE_ERROR: sprintf_P(strErr, PSTR("Receive Failed")); break;
      case OTA_END_ERROR: sprintf_P(strErr, PSTR("End Failed")); break;
      default: sprintf_P(strErr, PSTR("Unknown Error")); break;
    }
    Log.error(F("Update Error[%u] : "), error);
    Log.error(strErr);
    Log.error("\r\n");
  });

  // Feed the dog
  _wdt_feed();

  // Hors gel, désactivation du fils pilote
  initFP();

 
  // On peut maintenant initialiser MQTT et subscribe au topics
  connectToMqtt();

  Log.notice(F("\nStarting main loop\r\n"));
}


/* ======================================================================
Function: loop
Purpose : boucle principale du programme
Input   : -
Output  : -
Comments: -
====================================================================== */
void loop()
{
  //static bool refreshDisplay = false;
  static bool lastcloudstate;
  static unsigned long previousMillis = 0;  // last time update
  unsigned long currentMillis = millis();
  bool currentcloudstate ;

  // our own setup
  if (first_setup) {
    mysetup();
    first_setup = false;
  }

  /* Reboot handler */
  if (reboot) {
    delay(REBOOT_DELAY);
    ESP.restart();
  }

  // Gérer notre compteur de secondes
  if ( millis()-previousMillis > 1000) {
    // Ceci arrive toute les secondes écoulées
    previousMillis = currentMillis;
    uptime++;
    //refreshDisplay = true ;
  }

  // recupération de l'état de connexion au Wifi
  currentcloudstate = WiFi.status() == WL_CONNECTED ? true : false;

  // La connexion cloud vient de chager d'état ?
  if (lastcloudstate != currentcloudstate)
  {
    // Mise à jour de l'état
    lastcloudstate=currentcloudstate;

    // on vient de se reconnecter ?
    if (!currentcloudstate)
    {
      // on compte la deconnexion led rouge
      my_cloud_disconnect++;
      Log.error(F("Perte de conexion #%d\r\n"), my_cloud_disconnect);
    }
  }

  // Connection au Wifi ou Vérification
  // Webserver
  //server.handleClient();
  ArduinoOTA.handle();
}