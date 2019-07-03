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

void mqttConfigPublish(void) {
  // Send config via mqtt
  if (config.mqtt.isActivated && mqttIsConnected()) {
    String message;
    message.reserve(600);
    getConfJSONData(message);

    Log.verbose(F("message_send = "));
    Log.verbose(message.c_str());
    Log.verbose("\r\n");
    if (mqttClient.publish(MQTT_TOPIC_CONFIG_GET, 1, true, message.c_str()) == 0) {
      Log.error(F("Mqtt : Erreur publish config\r\n"));
    }
  }
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

void mqttFpPublish(uint8_t fp, bool force) {
  if (config.mqtt.isActivated && mqttIsConnected()) {
    uint8_t fp_last = NB_FILS_PILOTES;
    char message[NB_FILS_PILOTES*20 + 4];
    const size_t capacity = JSON_OBJECT_SIZE(NB_FILS_PILOTES + 1) + 50;
    StaticJsonDocument<capacity> doc;
    
    doc["id"] = config.id;
    doc["fp1"] = etatFP[0];
   
    serializeJson(doc, message);
  
    Log.verbose(F("message_send = "));
    Log.verbose(message);
    Log.verbose("\r\n");
    if ( mqttClient.publish(MQTT_TOPIC_FP_GET, 1, false, message) == 0 ) {
      Log.error(F("Mqtt : Erreur publish FP\r\n"));
    }
  }
  _wdt_feed();
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
    mqttClient.subscribe(MQTT_TOPIC_CONFIG_SET, 1);
    mqttClient.subscribe(MQTT_TOPIC_CONFIG_GET, 1);
  }

  // Publish online status in retained mode ( will be set to 0 by lsw when Remora disconnect after MQTT_KEEP_ALIVE as expired ).
  mqttClient.publish(MQTT_TOPIC_LSW,1,true,"1");
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  if (WiFi.isConnected() && config.mqtt.isActivated) {
    Log.notice(F("Connexion au broker MQTT\r\n"));
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
  if (!strcmp_P(topic, PSTR(MQTT_TOPIC_FP_GET))) {
    const size_t capacity = JSON_OBJECT_SIZE(1) + 10;
    StaticJsonDocument<capacity> doc;
    deserializeJson(doc, payload);
    if (doc["id"] == config.id) {
      mqttFpPublish(1);
    }
  }
  // Set fp
  else if (!strcmp_P(topic, PSTR(MQTT_TOPIC_FP_SET))) {
    char message[3] = "1";
    const size_t capacity = JSON_OBJECT_SIZE(NB_FILS_PILOTES+1) + (NB_FILS_PILOTES+1)*10;
    StaticJsonDocument<capacity> doc;

    deserializeJson(doc, payload);

    if (doc["id"] == config.id) {
      if (doc["fp1"].isNull()) {
        strcat(message, "-");
      }
      else {
        strcat(message, doc["fp1"]);
      }

      Log.verbose(F("message = "));
      Log.verbose(message);
      Log.verbose("\r\n");
      setfp(message);
    }
  }
  else if (!strcmp_P(topic, PSTR(MQTT_TOPIC_CONFIG_GET))) {
    const size_t capacity = JSON_OBJECT_SIZE(1) + 10;
    StaticJsonDocument<capacity> doc;
    deserializeJson(doc, payload);
    if (doc["id"] == config.id) {
      mqttConfigPublish();
    }
  }
  else if (!strcmp_P(topic, PSTR(MQTT_TOPIC_CONFIG_SET))) {
    const size_t capacity = JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(7) + 300;
    StaticJsonDocument<capacity> doc;
    deserializeJson(doc, payload);
    if (doc["id"] == config.id) {
      // Hostname
      if (!doc["hostname"].isNull()) {
        strncpy(config.host, doc["hostname"].as<const char *>(), CFG_HOSTNAME_SIZE);
      }
      
      // WifInfo
      if (!doc["ssid"].isNull()) {
        strncpy(config.ssid, doc["ssid"].as<const char *>(), CFG_SSID_SIZE);
      }
      if (!doc["psk"].isNull()) {
        strncpy(config.psk, doc["psk"].as<const char *>(), CFG_PSK_SIZE );
      }
      if (!doc["ota_auth"].isNull() && strcmp(config.ota_auth, doc["ota_auth"].as<const char *>()) != 0) {
        strncpy(config.ota_auth, doc["ota_auth"].as<const char *>(), CFG_PSK_SIZE );
        reboot = true;
      }

      if (!doc["ota_port"].isNull()) {
        //itemp = request->getParam("ota_port", true)->value().toInt();
        config.ota_port = (doc["ota_port"] >= 0 && doc["ota_port"] <= 65535) ? doc["ota_port"] : DEFAULT_OTA_PORT;
      }
    }
  }
  else {
    Log.error(F("Mqtt: Bad payload\r\n"));
  }
  _wdt_feed();
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
  //if (strcmp(config.mqtt.host, "") != 0 && config.mqtt.port > 0) {
  if (strcmp(config.mqtt.host, "") != 0 && config.mqtt.port > 0) {
    mqttClient.setServer("mqtt_dev.lan", config.mqtt.port);
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

  //mqttSysinfoTimer.attach(DELAY_PUBLISH_SYSINFO, mqttSysinfoPublish);

  Log.verbose(F("initMqtt_end\r\n"));
}