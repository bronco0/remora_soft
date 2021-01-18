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

void mqttConfigPublish(bool error) {
  // Send config via mqtt
  if (config.mqtt.isActivated && mqttIsConnected()) {
    String message;
    if (!error) {
      message.reserve(600);
      getConfJSONData(message);
    }
    else {
      const size_t capacity = JSON_OBJECT_SIZE(2) + 20;
      StaticJsonDocument<capacity> doc;
      message.reserve(100);
      doc["id"] = config.id;
      doc["state"] = "error";
      serializeJson(doc, message);
    }
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

void mqttFpPublish() {
  if (config.mqtt.isActivated && mqttIsConnected()) {
    char message[32];
    char value[2];
    const size_t capacity = JSON_OBJECT_SIZE(2) + 50;
    StaticJsonDocument<capacity> doc;

    doc["id"] = config.id;
    value[0] = etatFP[0];
    value[1] = 0;
    doc["fp"] = value;

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
    mqttClient.subscribe(MQTT_TOPPIC_REMORA_ONLINE, 1);
  }

  // Publish online status in retained mode ( will be set to 0 by lsw when Remora disconnect after MQTT_KEEP_ALIVE as expired ).
  //char message[50] = "";
  String message;
  message.reserve(50);
  const size_t capacity = JSON_OBJECT_SIZE(2) + 50;
  StaticJsonDocument<capacity> doc;

  doc["id"]    = config.id;
  doc["state"] = true;

  serializeJson(doc, message);

  Log.verbose(F("on LSW message = "));
  Log.verbose(message.c_str());
  Log.verbose("\r\n");

  //mqttClient.setWill(MQTT_TOPIC_LSW , 1, true, "{\"id\":15387855,\"state\":\"1\"}");
  mqttClient.publish(MQTT_TOPIC_LSW , 1, true, message.c_str());
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

  // Remora online
  if (!strcmp_P(topic, PSTR(MQTT_TOPPIC_REMORA_ONLINE))) {
    const size_t capacity = JSON_OBJECT_SIZE(2) + 50;
    StaticJsonDocument<capacity> doc;

    deserializeJson(doc, payload);
    
    if (doc["state"].as<boolean>()) {
      mqttFpPublish();
    }
    else {
      setfp("1H");
    }
  }
  // Set fp
  else if (!strcmp_P(topic, PSTR(MQTT_TOPIC_FP_SET))) {
    char message[3] = "1";
    const size_t capacity = JSON_OBJECT_SIZE(NB_FILS_PILOTES+1) + (NB_FILS_PILOTES+1)*10;
    StaticJsonDocument<capacity> doc;

    deserializeJson(doc, payload);

    if (doc["id"] == config.id) {
      if (doc["fp"].isNull()) {
        strcat(message, "-");
        mqttFpPublish();
      }
      else {
        strcat(message, doc["fp"]);
      }

      Log.verbose(F("message = "));
      Log.verbose(message);
      Log.verbose("\r\n");
      setfp(message);
    }
  }
  // Set config
  else if (!strcmp_P(topic, PSTR(MQTT_TOPIC_CONFIG_SET))) {
    bool changed_ota_auth = false;
    bool changed_mqtt_param = false;
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
        changed_ota_auth = true;
      }

      if (!doc["ota_port"].isNull()) {
        config.ota_port = (doc["ota_port"].as<unsigned int>() >= 0 && doc["ota_port"].as<unsigned int>() <= 65535) ? doc["ota_port"].as<unsigned int>() : DEFAULT_OTA_PORT;
      }

      if (!doc["mqtt"]["host"].isNull()) {
        strncpy(config.mqtt.host, doc["mqtt"]["host"].as<const char *>(), CFG_MQTT_HOST_SIZE);
        changed_mqtt_param = true;
      }
      if (!doc["mqtt"]["port"].isNull()) {
        config.mqtt.port = (doc["mqtt"]["port"].as<unsigned int>() >= 0 && doc["mqtt"]["port"].as<unsigned int>() <= 65535) ? doc["mqtt"]["port"].as<unsigned int>() : CFG_MQTT_DEFAULT_PORT;
        changed_mqtt_param = true;
      }
      if (!doc["mqtt"]["protocol"].isNull()) {
        strncpy(config.mqtt.protocol, doc["mqtt"]["protocol"].as<const char *>(), CFG_MQTT_PROTOCOL_SIZE);
        changed_mqtt_param = true;
      }
      if (!doc["mqtt"]["hasAuth"].isNull()) {
        config.mqtt.hasAuth = doc["mqtt"]["hasAuth"].as<bool>();
        changed_mqtt_param = true;
      }
      if (!doc["mqtt"]["user"].isNull()) {
        strncpy(config.mqtt.user, doc["mqtt"]["user"].as<const char *>(), CFG_MQTT_USER_SIZE);
        changed_mqtt_param = true;
      }
      if (!doc["mqtt"]["password"].isNull()) {
        strncpy(config.mqtt.password, doc["mqtt"]["password"].as<const char *>(), CFG_MQTT_PASSWORD_SIZE);
        changed_mqtt_param = true;
      }

      if ( saveConfig() ) {
        mqttConfigPublish();
        if (changed_ota_auth) {
          reboot = true;
        }
        if (changed_mqtt_param) {
          if (mqttIsConnected()) {
            disconnectMqtt();
          }

          if (config.mqtt.isActivated && !mqttIsConnected()) {
            connectToMqtt();
          }
        }
      } else {
        mqttConfigPublish(true);
      }
      _wdt_feed();
      #ifndef DISABLE_LOGGING
        showConfig();
      #endif
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


  String message;
  message.reserve(200);
  const size_t capacity = JSON_OBJECT_SIZE(2) + 50;
  StaticJsonDocument<capacity> doc;

  doc["id"]    = config.id;
  doc["state"] = false;

  serializeJson(doc, message);

  Log.verbose(F("set will message = "));
  Log.verbose(message.c_str());
  Log.verbose("\r\n");

  //mqttClient.setWill(MQTT_TOPIC_LSW , 1, true, "{\"id\":15387855,\"state\":\"0\"}");
  mqttClient.setWill(MQTT_TOPIC_LSW , 1, true, message.c_str());

  #if ASYNC_TCP_SSL_ENABLED
    if (config.mqtt.protocol == "mqtts") {
      mqttClient.setSecure(true);
    }
  #endif

  mqttSysinfoTimer.attach(DELAY_PUBLISH_SYSINFO, mqttSysinfoPublish);

  Log.verbose(F("initMqtt_end\r\n"));
}