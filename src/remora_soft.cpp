// **********************************************************************************
// Programmateur Fil Pilote et Suivi Conso
// **********************************************************************************
// Copyright (C) 2014 Thibault Ducret
// Licence MIT
//
// History : 15/01/2015 Charles-Henri Hallard (http://hallard.me)
//                      Intégration de version 1.2 de la carte electronique
//           13/04/2015 Theju
//                      Modification des variables cloud teleinfo
//                      (passage en 1 seul appel) et liberation de variables
//           15/09/2015 Charles-Henri Hallard : Ajout compatibilité ESP8266
//           02/12/2015 Charles-Henri Hallard : Ajout API WEB ESP8266 et Remora V1.3
//           04/01/2016 Charles-Henri Hallard : Ajout Interface WEB GUIT
//           04/03/2017 Manuel Hervo          : Ajout des connexions TCP Asynchrones
//
// **********************************************************************************

// Tout est inclus dans le fichier remora.h
// Pour activer des modules spécifiques ou
// changer différentes configurations il
// faut le faire dans le fichier remora.h
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

// ESP8266 WebServer
AsyncWebServer server(80);
// Use WiFiClient class to create a connection to WEB server
WiFiClient client;
// RGB LED (1 LED)
MyPixelBus rgb_led(1, RGB_LED_PIN);

// define whole brigtness level for RGBLED
uint8_t rgb_brightness = 127;

#ifdef MOD_EMONCMS
  Ticker Tick_emoncms;
  volatile boolean task_emoncms = false;
#endif
#ifdef MOD_JEEDOM
  Ticker Tick_jeedom;
  volatile boolean task_jeedom = false;
#endif

WiFiEventHandler wifiStaConnectHandler;
WiFiEventHandler wifiStaDisconnectHandler;
Ticker wifiReconnectTimer;

bool ota_blink;

bool reboot = false;

// ====================================================
// Following are dedicated to ESP8266 Platform
// Wifi management and OTA updates
// ====================================================

#ifdef MOD_EMONCMS
/* ======================================================================
Function: Task_emoncms
Purpose : callback of emoncms ticker
Input   :
Output  : -
Comments: Like an Interrupt, need to be short, we set flag for main loop
====================================================================== */

void Task_emoncms()
{
  task_emoncms = true;
}
#endif
#ifdef MOD_JEEDOM
/* ======================================================================
Function: Task_jeedom
Purpose : callback of jeedom ticker
Input   :
Output  : -
Comments: Like an Interrupt, need to be short, we set flag for main loop
====================================================================== */

void Task_jeedom()
{
  task_jeedom = true;
}
#endif

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

    #if defined (DEFAULT_WIFI_SSID) && defined (DEFAULT_WIFI_PASS)
      Log.verbose(F("Connection au Wifi : "));
      Log.verbose(F(DEFAULT_WIFI_SSID));
      Log.verbose(F(" avec la clé '"));
      Log.verbose(F(DEFAULT_WIFI_PASS));
      Log.verbose(F("'...\r\n"));

      WiFi.begin(DEFAULT_WIFI_SSID, DEFAULT_WIFI_PASS);
    #else
      if (*config.ssid) {
        Log.verbose(F("Connection à: "));
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
        }
      }
    #endif

    timeout = 25; // 25 * 200 ms = 5 sec time out

    // 200 ms loop
    while ( ((ret = WiFi.status()) != WL_CONNECTED) && timeout )
    {
      // Orange LED
      LedRGBON(COLOR_ORANGE);
      delay(50);
      LedRGBOFF();
      delay(150);
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

    // not connected ? start AP
    } else {
      Log.verbose(F("Erreur, passage en point d'acces "));
      Log.verbose(F(DEFAULT_HOSTNAME));

      // protected network
      Log.verbose(F(" avec la clé '"));
      if (*config.ap_psk) {
        Log.verbose(config.ap_psk);
      } else {
        Log.verbose(F(DEFAULT_WIFI_AP_PASS));
      }
      Log.verbose(F("'\r\n"));

      if (*config.ap_psk) {
        WiFi.softAP(DEFAULT_HOSTNAME, config.ap_psk);
      } else {
        WiFi.softAP(DEFAULT_HOSTNAME, DEFAULT_WIFI_AP_PASS);
      }
      WiFi.mode(WIFI_AP_STA);

      Log.verbose(F("IP address   : "));
      Log.verbose(WiFi.softAPIP().toString().c_str());
      Log.verbose("\r\n");
      Log.verbose(F("MAC address  : "));
      Log.verbose(WiFi.softAPmacAddress().c_str());
      Log.verbose("\r\n");
    }

    // Feed the dog
    _wdt_feed();

    // Set OTA parameters
     ArduinoOTA.setPort(DEFAULT_OTA_PORT);
     ArduinoOTA.setHostname(DEFAULT_HOSTNAME);
     if (*config.ota_auth) {
       ArduinoOTA.setPassword(config.ota_auth);
     }/* else {
       ArduinoOTA.setPassword(DEFAULT_OTA_PASS);
     }*/
     ArduinoOTA.begin();

    // just in case your sketch sucks, keep update OTA Available
    // Trust me, when coding and testing it happens, this could save
    // the need to connect FTDI to reflash
    // Usefull just after 1st connexion when called from setup() before
    // launching potentially buggy main()
    for (uint8_t i=0; i<= 10; i++) {
      LedRGBON(COLOR_MAGENTA);
      delay(100);
      LedRGBOFF();
      delay(200);
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
  #ifdef MOD_MQTT
    if (config.mqtt.isActivated && !mqttClient.connected() && !first_setup)
      connectToMqtt();
  #endif
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
void printTimestamp(Print* _logOutput) {
  char c[12];
  sprintf(c, "%10lu ", millis());
  _logOutput->print(c);
}


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
  #if (defined DEBUG_INIT || !defined MOD_TELEINFO)
    DEBUG_SERIAL.begin(115200);
    DEBUG_SERIAL.setDebugOutput(true);
  #endif

  Log.begin(LOG_LEVEL, &DEBUG_SERIAL, false);
  //Log.setPrefix(printTimestamp); // Uncomment to get timestamps as prefix
  //Log.setSuffix(printNewline); // Uncomment to get newline as suffix

  // says main loop to do setup
  first_setup = true;

  Log.notice(F("Starting Remora\r\n"));
  Log.verbose(F(" Version "));
  Log.verbose(F(REMORA_SOFT_VERSION));
  Log.verbose(F("\r\nCompiled with : "));

  #if defined (REMORA_BOARD_V12)
    Log.verbose(F("BOARD V1.2 MCP23017 "));
  #elif defined (REMORA_BOARD_V13)
    Log.verbose(F("BOARD V1.3 MCP23017 "));
  #elif defined (REMORA_BOARD_V14)
    Log.verbose(F("BOARD V1.4 MCP23017 "));
  #elif defined (REMORA_BOARD_V15)
    Log.verbose(F("BOARD V1.5 MCP23017 "));
  #else
    Log.verbose(F("BOARD unknow "));
  #endif

  #ifdef MOD_OLED
    Log.verbose(F("OLED "));
  #endif
  #ifdef MOD_TELEINFO
    Log.verbose(F("TELEINFO "));
  #endif
  #ifdef MOD_RF69
    Log.verbose(F("RFM69 "));
  #endif
  #ifdef BLYNK_AUTH
    Log.verbose(F("BLYNK "));
  #endif
  #ifdef MOD_ADPS
    Log.verbose(F("ADPS "));
  #endif
  #ifdef MOD_EMONCMS
    Log.verbose(F("EMONCMS "));
  #endif
  #ifdef MOF_JEEDOM
    Log.verbose(F("JEEDOM "));
  #endif
  #ifdef MOD_MQTT
    Log.verbose(F("MQTT "));
  #endif
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

  #ifdef MOD_TELEINFO
    // Init de la téléinformation
    Serial.begin(1200, SERIAL_7E1);
  #endif

  // Clear our global flags
  config.config = 0;

  // Our configuration is stored into EEPROM
  EEPROM.begin(sizeof(_Config));
  //EEPROM.begin(1024);

  Log.verbose(F("Config size = %l\r\n"), sizeof(_Config));
  #ifdef MOD_EMONCMS
    Log.verbose(F(" - emoncms = %l\r\n"), sizeof(_emoncms));
  #endif
  #ifdef MOD_JEEDOM
    Log.verbose(F(" - jeedom = %l\r\n"), sizeof(_jeedom));
  #endif
  #ifdef MOD_MQTT
    Log.verbose(F(" - mqtt = %l\r\n"), sizeof(_mqtt));
  #endif


  // Check File system init
  if (SPIFFS.begin()) {
      // Serious problem
      Log.error(F("SPIFFS Mount failed\r\n"));
  }
  else {
    Log.notice(F("SPIFFS Mount succesfull\r\n"));

    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Log.verbose(F("FS File: "));
      Log.verbose(fileName.c_str());
      Log.verbose(F("size: %l\r\n"), fileSize);
      _wdt_feed();
    }
  }

  // Read Configuration from EEP
  if (!readConfig()) {
    // Reset Configuration
    resetConfig();

    // save back
    saveConfig();

    // Indicate the error in global flags
    config.config |= CFG_BAD_CRC;


    Log.error(F("Read Configuration => Reset to default\r\n"));
  }
  else {
    Log.verbose(F("Read Configuration => Good CRC, not set!\r\n"));
  }

  #ifndef DISABLE_LOGGING
    showConfig();
  #endif

  rgb_brightness = config.led_bright;
  Log.verbose(F("RGB Brightness : %d\r\n"), rgb_brightness);

  // Connection au Wifi ou Vérification
  #ifdef MOD_MQTT
    wifiStaConnectHandler    = WiFi.onStationModeGotIP(onWifiStaConnect);
    wifiStaDisconnectHandler = WiFi.onStationModeDisconnected(onWifiStaDisconnect);
  #endif

  // Init de la téléinformation
  #ifdef MOD_TELEINFO
    if (strcmp_P(config.compteur_tic, PSTR("standard")) == 0) {
      Log.verbose(F("TIC standard : Serial 9600 bps\r\n"));
      Serial.begin(9600, SERIAL_7E1);
    }
    else {
      Log.verbose(F("Tic historique : Serial 1200 bps\r\n"));
      Serial.begin(1200, SERIAL_7E1);
    }
  #endif

  // Connection au Wifi ou Vérification
  wifi_station_set_hostname(config.host);
  WifiHandleConn(true);

  // OTA callbacks
  ArduinoOTA.onStart([]() {
    if (ArduinoOTA.getCommand() == U_SPIFFS) {
      Log.verbose(F("OTA : upload SPIFFS\r\n"));
      SPIFFS.end(); // Arret du SPIFFS, sinon plantage de la mise à jour
    }
    LedRGBON(COLOR_MAGENTA);

    Log.notice(F("\r\nUpdate Started...\r\n"));

    // On affiche le début de la mise à jour OTA sur l'afficheur
    #ifdef MOD_OLED
      if (status & STATUS_OLED && config.config & CFG_LCD) {
        ui->disableAutoTransition();
        display->clear();
        display->setFont(Roboto_Condensed_Bold_Bold_16);
        display->setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
        display->drawString(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2 - 16, F("OTA Update"));
        display->display();
      }
    #endif
      ota_blink = true;
  });

  ArduinoOTA.onEnd([]() {
    LedRGBOFF();

    Log.notice(F("\r\nUpdate finished restarting\r\n"));

    // On affiche le message de fin sur l'afficheur
    #ifdef MOD_OLED
      if (status & STATUS_OLED && config.config & CFG_LCD) {
        ui->disableAutoTransition();
        display->clear();
        display->setFont(Roboto_Condensed_Bold_Bold_16);
        display->setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
        display->drawString(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2 - 16, F("Restart"));
        display->display();
      }
    #endif
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    if (ota_blink) {
      LedRGBON(COLOR_MAGENTA);
    } else {
      LedRGBOFF();
    }
    ota_blink = !ota_blink;
    // On affiche la progression sur l'afficheur
    #ifdef MOD_OLED
      if (status & STATUS_OLED && config.config & CFG_LCD) {
        ui->disableAutoTransition();
        display->clear();
        display->setTextAlignment(TEXT_ALIGN_CENTER);
        display->setFont(Roboto_Condensed_Bold_Bold_16);
        display->drawString(DISPLAY_WIDTH/2, 10, F("OTA Update"));
        display->drawProgressBar(2, 28, 124, 10, progress / (total / 100));
        display->display();
      }
    #endif
  });

  ArduinoOTA.onError([](ota_error_t error) {
    char strErr[15]; // Contient le message d'erreur

    LedRGBON(COLOR_RED);

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

    // On affiche l'erreur sur l'afficheur
    #ifdef MOD_OLED
      if (status & STATUS_OLED && config.config & CFG_LCD) {
        ui->disableAutoTransition();
        display->clear();
        display->setFont(Roboto_Condensed_Bold_Bold_16);
        display->setTextAlignment(TEXT_ALIGN_CENTER);
        display->drawString(DISPLAY_WIDTH/2, 10, F("OTA Error"));
        display->setFont(Roboto_Condensed_Bold_14);
        display->drawString(DISPLAY_WIDTH/2, 30, strErr);
        display->display();
      }
    #endif
  });

  // handler for uptime
  server.on(PSTR("/uptime"), HTTP_GET, [](AsyncWebServerRequest *request) {
    String response = "";
    const size_t capacity = JSON_OBJECT_SIZE(1) + 20;
    StaticJsonDocument<capacity> doc;
    doc["uptime"] = uptime;
    serializeJson(doc, response);
    request->send(200, "text/json", response);
  });

  server.on(PSTR("/config_form.json"), HTTP_POST, handleFormConfig);
  server.on(PSTR("/factory_reset"),handleFactoryReset );
  server.on(PSTR("/reset"), handleReset);
  server.on(PSTR("/tinfo"), tinfoJSON);
  server.on(PSTR("/tinfo.json"), tinfoJSONTable);
  server.on(PSTR("/system.json"), sysJSONTable);
  server.on(PSTR("/config.json"), confJSONTable);
  server.on(PSTR("/spiffs.json"), spiffsJSONTable);
  server.on(PSTR("/wifiscan.json"), wifiScanJSON);

  // handler for the hearbeat
  server.on(PSTR("/hb.htm"), HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(200, "text/html", R"(OK)");
    response->addHeader(F("Connection"), F("close"));
    response->addHeader(F("Access-Control-Allow-Origin"), F("*"));
    request->send(response);
  });

  // handler for the /update form POST (once file upload finishes)
  server.on(PSTR("/update"), HTTP_POST, [](AsyncWebServerRequest *request) {
    reboot = !Update.hasError();
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", reboot ? "OK" : "FAIL");
    response->addHeader("Connection","close");
    request->send(response);
  }, handle_fw_upload);

  server.onNotFound(handleNotFound);

  // serves all SPIFFS Web file with 24hr max-age control
  // to avoid multiple requests to ESP
  server.serveStatic(PSTR("/font"), SPIFFS, PSTR("/font"), PSTR("max-age=86400"));
  server.serveStatic(PSTR("/js"),   SPIFFS, PSTR("/js")  , PSTR("max-age=86400"));
  server.serveStatic(PSTR("/css"),  SPIFFS, PSTR("/css") , PSTR("max-age=86400"));
  server.begin();

  Log.notice(F("HTTP server started\r\n"));

  #ifdef BLYNK_AUTH
    Blynk.config(BLYNK_AUTH);
  #endif

  // Init bus I2C
  i2c_init();
  i2c_scan();

  // Init des fils pilotes
  if (pilotes_setup()) {
    status |= STATUS_MCP ;
  }

  #ifdef MOD_OLED
    // Initialisation de l'afficheur
    if (initDisplay()) {
      Log.verbose(F("Initializing Display => OK\r\n"));

      status |= STATUS_OLED; // Statut OLED ajouté
      // On lance l'initialisation des frames
      initDisplayUI();
    }
    else {
      Log.error(F("Initializing Display => Fail\r\n"));
    }
  #endif

  #ifdef MOD_RF69
    // Initialisation RFM69 Module
    if (rfm_setup())
      status |= STATUS_RFM; // Statut RFM ajouté
  #endif

  // Feed the dog
  _wdt_feed();

  #ifdef MOD_TELEINFO
    // Initialiser la téléinfo et attente d'une trame valide
    // Le status est mis à jour dans les callback de la teleinfo
    tinfo_setup(true);
    #ifdef MOD_EMONCMS
      // Initialise la mise à jour de Emoncms si la config est définie
      if (strlen(config.emoncms.host) > 0 && strlen(config.emoncms.url) > 0
        && strlen(config.emoncms.apikey) > 0 && (config.emoncms.freq > 0 && config.emoncms.freq < 86400)) {
        // Jeedom Update if needed
        Tick_emoncms.detach();
        Tick_emoncms.attach(config.emoncms.freq, Task_emoncms);
      }
    #endif
    #ifdef MOD_JEEDOM
      // Initialise la mise à jour de Jeedom si la config est définie
      if (strlen(config.jeedom.host) > 0 && strlen(config.jeedom.url) > 0
        && strlen(config.jeedom.apikey) > 0 && (config.jeedom.freq > 0 && config.jeedom.freq < 86400)) {
        // Jeedom Update if needed
        Tick_jeedom.detach();
        Tick_jeedom.attach(config.jeedom.freq, Task_jeedom);
      }
    #endif
  #endif

  // Led verte durant le test
  LedRGBON(COLOR_GREEN);

  // nous avons fini, led Jaune
  LedRGBON(COLOR_YELLOW);

  // Hors gel, désactivation des fils pilotes
  initFP();

  // On etteint la LED embarqué du core
  LedRGBOFF();

  #ifdef MOD_MQTT
    // On peut maintenant initialiser MQTT et subscribe au MQTT_TOPIC_SET
    connectToMqtt();
  #endif

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
    #ifdef BLYNK_AUTH
      if ( Blynk.connected() ) {
        String up    = String(uptime) + "s";
        String papp  = String(mypApp) + "W";
        String iinst = String(myiInst)+ "A";
        Blynk.virtualWrite(V0, up, papp, iinst, mypApp);
        _yield();
      }
    #endif
  } else {
    #ifdef BLYNK_AUTH
      Blynk.run(); // Initiates Blynk
    #endif
  }

  #ifdef MOD_TELEINFO
    // Vérification de la reception d'une 1ere trame téléinfo
    tinfo_loop();
    _yield();
  #endif

  #ifdef MOD_RF69
    // Vérification de la reception d'une trame RF
    if (status & STATUS_RFM)
      rfm_loop();
      _yield();
  #endif

  #ifdef MOD_OLED
    if (status & STATUS_OLED && config.config & CFG_LCD) {
      int remainingTimeBudget = ui->update();
      if (remainingTimeBudget > 0) {
        // You can do some work here
        // Don't do stuff if you are below your
        // time budget.
        // delay(remainingTimeBudget);
      }
    }
  #endif

  // recupération de l'état de connexion au Wifi
  currentcloudstate = WiFi.status() == WL_CONNECTED ? true : false;

  // La connexion cloud vient de chager d'état ?
  if (lastcloudstate != currentcloudstate)
  {
    // Mise à jour de l'état
    lastcloudstate=currentcloudstate;

    // on vient de se reconnecter ?
    if (currentcloudstate)
    {
      // led verte
      LedRGBON(COLOR_GREEN);
    }
    else
    {
      // on compte la deconnexion led rouge
      my_cloud_disconnect++;
      Log.error(F("Perte de conexion #%d\r\n"), my_cloud_disconnect);
      LedRGBON(COLOR_RED);
    }
  }

  // Connection au Wifi ou Vérification
  // Webserver
  //server.handleClient();
  ArduinoOTA.handle();

  #ifdef MOD_EMONCMS
    if (task_emoncms) {
      if (!emoncmsPost()) {
        Log.error(F("Erreur push Emoncms\r\n"));
      }
    task_emoncms=false;
  }
  #endif
  #ifdef MOD_JEEDOM
    if (task_jeedom) {
      if (!jeedomPost()) {
        Log.error(F("Erreur push Jeedom\r\n"));
      }
      task_jeedom=false;
    }
  #endif
}
