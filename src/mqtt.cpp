// **********************************************************************************
// MQTT source file for remora project
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend but please abide with the CC-BY-SA license:
// http://creativecommons.org/licenses/by-sa/4.0/
//
// Written by bronco0 (https://github.com/bronco0/remora_soft)
//
// History : 08/01/2019 : First release
//
// All text above must be included in any redistribution.
// **********************************************************************************

#include "mqtt.h"

#ifdef MOD_MQTT

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;
Ticker mqttSysinfoTimer;
int nbRestart = 0;
char etatFP_sav[NB_FILS_PILOTES + 1] = "";

void connectToMqtt() {
  Log.notice(F("Connexion au broker MQTT.\r\n"));

  initMqtt();
  if (!mqttClient.connected()) {
    mqttClient.connect();
  }
  Log.verbose(F("connectToMqtt_end\r\n"));
}

void disconnectMqtt() {
  mqttClient.disconnect(false);
  if (mqttClient.connected()) {
    mqttClient.disconnect(true);
  }
}

bool mqttIsConnected() {
  return mqttClient.connected();
}

void mqttSysinfoPublish(void) {
  // Send sysinfo via mqtt
  if (config.mqtt.isActivated && mqttIsConnected()) {
    String message;
    message.reserve(600);
    getSysJSONData(message);

    Log.verbose(F("message_send = "));
    Log.verbose(message.c_str());
    Log.verbose("\r\n");
    if (mqttClient.publish(MQTT_TOPIC_SYSTEM, 1, true, message.c_str()) == 0) {
      Log.error(F("Mqtt : Erreur publish Sysinfo\r\n"));
    }
  }
}

#ifdef MOD_TELEINFO
  void mqttTinfoPublish(void) {
    if (status & STATUS_TINFO) {
      // Send téléinfo via mqtt
      if (config.mqtt.isActivated && mqttIsConnected()) {
        String message;
        message.reserve(300);
        getTinfoListJson(message);

        Log.verbose(F("message_send = "));
        Log.verbose(message.c_str());
        Log.verbose("\r\n");
        if (mqttClient.publish(MQTT_TOPIC_TINFO, 1, false, message.c_str()) == 0) {
          Log.error(F("Mqtt : Erreur publish Tinfo\r\n"));
        }
      }
    }
  }
#endif

#ifdef MOD_MICRO
  void mqttSendToMicroPilote(uint32_t id, char ordre){
    if (config.mqtt.isActivated && mqttIsConnected()) {
      char message[40];
      const size_t capacity = JSON_OBJECT_SIZE(2);
      StaticJsonDocument<capacity> doc;

      doc["id"] = id;
      doc["fp"] = ordre;

      serializeJson(doc, message);

      Log.verbose(F("message_send = "));
      Log.verbose(message);
      Log.verbose("\r\n");
      if ( mqttClient.publish(MQTT_TOPIC_MC_FP_SET, 1, false, message) == 0 ) {
        Log.error(F("Mqtt : Erreur publish MP\r\n"));
      }
    }
  }
#endif

void mqttFpPublish(uint8_t fp, bool force) {
  if (config.mqtt.isActivated && mqttIsConnected()) {
    uint8_t fp_last = NB_FILS_PILOTES;
    char message[NB_FILS_PILOTES * 20];
    const size_t capacity = JSON_OBJECT_SIZE(NB_FILS_PILOTES) + 10 * NB_FILS_PILOTES;
    StaticJsonDocument<capacity> doc;

    if (fp >= 0 && fp <= NB_FILS_PILOTES) {
      if (fp > 0) {
        fp_last = fp;
      }
      else {
        fp = 1;
      }

      for (; fp <= fp_last; fp++) {
        char fp_name[4] = "";
        char i_to_a[3] = "";
        // Convert i to asci
        itoa(fp, i_to_a, 10);

        // Concat "fp" whith i => fp1, fp2, fp3, ...
        strcat(fp_name, "fp");
        strcat(fp_name, i_to_a);

        if (etatFP[fp-1] != etatFP_sav[fp-1] || force) {
          char val[2];
          val[0] = etatFP[fp-1];
          val[1] = 0;
          doc[fp_name] = val;
        }
      }

      _wdt_feed();

      strcpy(etatFP_sav, etatFP);
      if (!doc.isNull())  {
        serializeJson(doc, message);
  
        Log.verbose(F("message_send = "));
        Log.verbose(message);
        Log.verbose("\r\n");
        if ( mqttClient.publish(MQTT_TOPIC_FP, 1, false, message) == 0 ) {
          Log.error(F("Mqtt : Erreur publish FP\r\n"));
        }
      }
    }
  }
}

void mqttDelestagePublish(void) {
  if (config.mqtt.isActivated && mqttIsConnected()) {
    char message[45];
    const size_t capacity = JSON_OBJECT_SIZE(3);
    StaticJsonDocument<capacity> doc;

    doc[F("status")]   = (nivDelest > 0)?1:0;
    doc[F("niveau")]  = nivDelest;
    doc[F("old_zone")] = plusAncienneZoneDelestee;

    serializeJson(doc, Serial);

    Log.verbose(F("message_send = "));
    Log.verbose(message);
    Log.verbose("\r\n");
    if ( mqttClient.publish(MQTT_TOPIC_RELAIS, 1, false, message)  == 0 ) {
      Log.error(F("Mqtt : Erreur publish Delestage\r\n"));
    }
  }
}

void mqttRelaisPublish(void) {
  if (config.mqtt.isActivated && mqttIsConnected()) {
    char message[26];
    const size_t capacity = JSON_OBJECT_SIZE(2);
    StaticJsonDocument<capacity> doc;

    doc["mode"] = fnctRelais;
    doc["status"] = etatrelais;

    serializeJson(doc, message);

    Log.verbose(F("message_send = "));
    Log.verbose(message);
    Log.verbose("\r\n");
    if (mqttClient.publish(MQTT_TOPIC_RELAIS, 1, false, message)  == 0) {
      Log.error(F("Mqtt : Erreur publish Relais\r\n"));
    }
  }
}

void onMqttConnect(bool sessionPresent) {
  Log.notice(F("Connecté au broker MQTT\r\n"));

  if (sessionPresent) {
    nbRestart = 0;
  }

  // subscribe aux topics commandes
  if (mqttClient.connected()) {
    Log.verbose(F("Subscribe topic\r\n"));
    mqttClient.subscribe(MQTT_TOPIC_FP_SET, 1);
    mqttClient.subscribe(MQTT_TOPIC_FP_GET, 1);
    mqttClient.subscribe(MQTT_TOPIC_RELAIS_SET, 1);
    mqttClient.subscribe(MQTT_TOPIC_RELAIS_GET, 1);
    mqttClient.subscribe(MQTT_TOPIC_FP_GET, 1);
    mqttClient.subscribe(MQTT_TOPIC_FP_GET, 1);
    #ifdef MOD_MICRO
      mqttClient.subscribe(MQTT_TOPIC_MC_FP_GET, 1);
      mqttClient.subscribe(MQTT_TOPIC_MC_CONFIG_GET, 1);
      mqttClient.subscribe(MQTT_TOPIC_MC_SYSTEM, 1);
      mqttClient.subscribe(MQTT_TOPIC_MC_ONLINE, 1);
    #endif
  }

  // Publish online status in retained mode ( will be set to 0 by lsw when Remora disconnect after MQTT_KEEP_ALIVE as expired ).
  mqttClient.publish(MQTT_TOPIC_LSW,1,true,"1");
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  if (WiFi.isConnected() && config.mqtt.isActivated) {
    Log.notice(F("Connexion aux brokers MQTT\r\n"));
    if (nbRestart < 2)
      mqttReconnectTimer.once(2, connectToMqtt);
    else if (nbRestart < 5)
      mqttReconnectTimer.once(10, connectToMqtt);
    else if (nbRestart < 10)
      mqttReconnectTimer.once(30, connectToMqtt);
    else
      mqttReconnectTimer.once(60, connectToMqtt);
  }
  nbRestart++;
}

/*void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  DebuglnF("Subscribing at topic");
}*/

/*void onMqttUnsubscribe(uint16_t packetId) {
  DebuglnF("Unsubscribe topic");
}*/

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  Log.verbose(F("MQTT new message : topic "));
  Log.verbose(topic);
  Log.verbose("\r\n");

    // Get fp information
  if (!strncmp_P(topic, PSTR(MQTT_TOPIC_FP_GET), 13)) {
    if (strlen(topic) > 13) {
      char fp[4] = "";
      char * pch;
      uint16_t n;
      uint8_t c = 0;

      pch = strrchr(topic,'/');

      for (n = pch-topic+1 ; n < strlen(topic); n++) {
        fp[c] = topic[n];
        c++;
      }

      _wdt_feed();

      if (strtol(fp, NULL, 10)) {
        if (strcmp(fp, "0") != 0) {
          mqttFpPublish(strtol(fp, NULL, 10), true);
        }
      }
    }
    else {
      mqttFpPublish(0, true);
    }
  }
  // Get relais information
  else if (!strncmp(topic, MQTT_TOPIC_RELAIS_GET, strlen(MQTT_TOPIC_RELAIS_GET))) {
    mqttRelaisPublish();
  }
  // Set fp
  else if (!strncmp(topic, MQTT_TOPIC_FP_SET, strlen(MQTT_TOPIC_FP_SET))) {
    char message[NB_FILS_PILOTES] = "";
    const size_t capacity = JSON_OBJECT_SIZE(NB_FILS_PILOTES) + NB_FILS_PILOTES*10;
    StaticJsonDocument<capacity> doc;

    deserializeJson(doc, payload);

    for (uint8_t i = 1; i <= NB_FILS_PILOTES; i++) {
      char fp[4] = "fp";
      char i_to_a[4] = "";
      // Convert i to asci
      itoa(i, i_to_a, 10);
      // Concat "fp" whith i => fp1, fp2, fp3, ...
      //strcat(fp, "fp");
      strcat(fp, i_to_a);
      // Concat "1" with order => 1C, 2E, 3H, ...

      if (doc[fp].isNull()) {
        strcat(message, "-");
        continue;
      }
      else {
        strcat(message, doc[fp]);
      }
    }

    _wdt_feed();

    Log.verbose(F("message = "));
    Log.verbose(message);
    Log.verbose("\r\n");
    setfp(message);
  }
  // Set relais
  else if (!strncmp(topic, MQTT_TOPIC_RELAIS_SET, strlen(MQTT_TOPIC_RELAIS_SET))) {
    const size_t capacity = JSON_OBJECT_SIZE(1) + 23;
    StaticJsonDocument<capacity> doc;

    deserializeJson(doc, payload);

    Log.verbose(F("relais mode = "));
    Log.verbose(doc["mode"].as<const char*>());
    Log.verbose("\r\n");
    fnct_relais(doc["mode"]);
  }
  #ifdef MOD_MICRO
    // Micro pilote
    // Get fp
    else if (!strncmp(topic, MQTT_TOPIC_MC_FP_GET, strlen(MQTT_TOPIC_MC_FP_GET))) {
      const size_t capacity = JSON_OBJECT_SIZE(2) + 10;
      StaticJsonDocument<capacity> doc;

      deserializeJson(doc, payload);

      Log.verbose(F("Micro pilote %lu : %c\r\n"), doc["id"].as<uint32_t>(), doc["id"].as<char>());
      
      for (int i=NB_REAL_FILS_PILOTES + 1; i >= NB_FILS_PILOTES; i++) {
        if (config.zones_fp.fp[i-1].id == doc["id"].as<uint32_t>()) {
          if (etatFP[i-1] != doc["id"].as<char>()) {
            mqttSendToMicroPilote(doc["id"].as<uint32_t>(), etatFP[i-1]);
          }
          break;
        }
      }
      _wdt_feed();
    }
    // Get config
    else if (!strncmp(topic, MQTT_TOPIC_MC_CONFIG_GET, strlen(MQTT_TOPIC_MC_CONFIG_GET))) {
      const size_t capacity = JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(7) + 515;
      StaticJsonDocument<capacity> doc;

      deserializeJson(doc, payload);

      // recup index depuis config
      for (int i=NB_REAL_FILS_PILOTES + 1; i >= NB_FILS_PILOTES; i++) {
        if (config.zones_fp.fp[i-1].id == doc["id"].as<uint32_t>()) {

          strcpy(mp_info.config[i-1].hostname, doc["hostname"].as<const char *>());
          strcpy(mp_info.config[i-1].ssid, doc["ssid"].as<const char *>());
          strcpy(mp_info.config[i-1].ota_auth, doc["ota_auth"].as<const char *>());
          mp_info.config[i-1].ota_port = doc["ota_port"].as<uint16_t>();

          JsonObject mqtt = doc["mqtt"];
          strcpy(mp_info.config[i-1].mqtt.host, mqtt["host"].as<const char *>());
          mp_info.config[i-1].mqtt.port = mqtt["port"].as<uint16_t>();
          strcpy(mp_info.config[i-1].mqtt.protocol, mqtt["protocol"].as<const char *>());
          strcpy(mp_info.config[i-1].mqtt.user, mqtt["user"].as<const char *>());
          strcpy(mp_info.config[i-1].mqtt.password, mqtt["password"].as<const char *>());

          break;
        }
      }
      _wdt_feed();
    }
    // Receive sysinfo
    else if (!strncmp(topic, MQTT_TOPIC_MC_SYSTEM, strlen(MQTT_TOPIC_MC_SYSTEM))) {
      const size_t capacity = JSON_OBJECT_SIZE(10) + 300;
      StaticJsonDocument<capacity> doc;

      deserializeJson(doc, payload);
      for (int i=NB_REAL_FILS_PILOTES + 1; i >= NB_FILS_PILOTES; i++) {
        if (config.zones_fp.fp[i-1].id == doc["id"].as<uint32_t>()) {
          mp_info.system[i-1].uptime = doc["uptime"].as<uint32_t>();
          mp_info.system[i-1].cpu_freq = doc["cpu_freq"].as<uint8>();
          mp_info.system[i-1].flash_real_size = doc["flash_real_size"].as<uint32_t>();
          mp_info.system[i-1].firmware_size = doc["firmware_size"].as<uint32_t>();
          mp_info.system[i-1].free_size = doc["free_size"].as<uint32_t>();
          mp_info.system[i-1].rssi = doc["rssi"].as<uint32_t>();
          mp_info.system[i-1].free_ram = doc["free_ram"].as<uint32_t>();
          strcpy(mp_info.system[i-1].heap_fragmentation, doc["heap_fragmentation"].as<const char *>());
          strcpy(mp_info.system[i-1].ip, doc["ip"].as<const char *>());
          break;
        }
      }
      _wdt_feed();
    }
    // LWT/ONLINE  => ex: new device
    else if (!strncmp(topic, MQTT_TOPIC_MC_ONLINE, strlen(MQTT_TOPIC_MC_ONLINE))) {
      const size_t capacity = JSON_OBJECT_SIZE(2) + 10;
      StaticJsonDocument<capacity> doc;

      deserializeJson(doc, payload);

      // recup index depuis config
      for (int i=NB_REAL_FILS_PILOTES + 1; i >= NB_FILS_PILOTES; i++) {
        if (config.zones_fp.fp[i-1].id == doc["id"].as<uint32_t>()) {
          mp_info.isOnline[i-1] = doc["state"].as<bool>();
          break;
        }
      }
      _wdt_feed();
    }
  #endif
  else {
    Log.error(F("Mqtt: Bad payload\r\n"));
  }
}

void onMqttPublish(uint16_t packetId) {
  Log.verbose(F("Publish packetId: %d\r\n"), packetId);
}

void initMqtt(void) {
  Log.verbose(F("initMqtt\r\n"));
  if (first_setup) {
    Log.verbose(F("initMqtt_first_setup\r\n"));
    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    //mqttClient.onSubscribe(onMqttSubscribe);
    //mqttClient.onUnsubscribe(onMqttUnsubscribe);
    mqttClient.onMessage(onMqttMessage);
    mqttClient.onPublish(onMqttPublish);
    Log.verbose(F("initMqtt_first_setup_end\r\n"));
  }
  if (strcmp(config.mqtt.host, "") != 0 && config.mqtt.port > 0) {
    mqttClient.setServer(config.mqtt.host, config.mqtt.port);
  }
  if (config.mqtt.hasAuth && (strcmp(config.mqtt.user, "") != 0 || strcmp(config.mqtt.password, "") != 0)) {
    mqttClient.setCredentials(config.mqtt.user, config.mqtt.password);
  }
  // Set mqttclient keep alive to 60sec, clean session to false, LSW message to  0 , & mqtt client id to hostanme
  mqttClient.setClientId(config.host);
  mqttClient.setKeepAlive(MQTT_KEEP_ALIVE);
  mqttClient.setCleanSession(true);
  mqttClient.setWill(MQTT_TOPIC_LSW , 1, true, "0");

  #if ASYNC_TCP_SSL_ENABLED
    if (config.mqtt.protocol == "mqtts") {
      mqttClient.setSecure(true);
    }
  #endif

  mqttSysinfoTimer.attach(DELAY_PUBLISH_SYSINFO, mqttSysinfoPublish);

  Log.verbose(F("initMqtt_end\r\n"));
}
#endif // MOD_MQTT