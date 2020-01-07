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
String lastMqttMessageFP = "";
String lastMqttMessageRelais = "";
#ifdef MOD_TELEINFO
  Ticker mqttTinfoTimer;
  String lastMqttMessageTinfo = "";
#endif
int nbRestart = 0;

void connectToMqtt() {
  DebuglnF("Connection au broker MQTT...");
  initMqtt();
  mqttClient.connect();
}

void disconnectMqtt() {
  mqttClient.disconnect(false);
  if (mqttClient.connected())
    mqttClient.disconnect(true);
}

bool mqttIsConnected() {
  return mqttClient.connected();
}

#ifdef MOD_TELEINFO
void mqttTinfoPublish(void) {
  if (status & STATUS_TINFO) {
    // Send téléinfo via mqtt
    if (config.mqtt.isActivated && mqttIsConnected()) {
      String message = "";
      getTinfoListJson(message, false);
      if (lastMqttMessageTinfo != message) {
        DebugF("message_send = ");
        Debugln(message);
        if (mqttClient.publish(MQTT_TOPIC_TINFO, 1, false, message.c_str()) == 0) {
          DebuglnF("Mqtt : Erreur publish Tinfo");
        }
        lastMqttMessageTinfo = message;
      }
    }
  }
}
#endif

void mqttFpPublish(void) {
  if (config.mqtt.isActivated && mqttIsConnected()) {
    String message = F("{\"FP\":\"");
    message.concat(etatFP);
    message.concat(F("\"}"));
    if ( lastMqttMessageFP != message ) { 
      DebugF("message send = ");
      Debugln(message);
      if ( mqttClient.publish(MQTT_TOPIC_FP, 1, false, message.c_str())  == 0 ) {
        DebuglnF("Mqtt : Erreur publish FP1");
      }
      lastMqttMessageFP = message;
    } 
  }
}

void mqttRelaisPublish(void) {
  if (config.mqtt.isActivated && mqttIsConnected()) {
    String message = F("{\"Mode\":\"");
    message.concat(fnctRelais);
    message.concat(F("\",\"Etat\":\""));
    message.concat(etatrelais);
    message.concat(F("\"}"));
    if ( lastMqttMessageRelais != message ) {
      DebugF("message_send = ");
      Debugln(message);
      if ( mqttClient.publish(MQTT_TOPIC_RELAIS, 1, false, message.c_str())  == 0 ) {
        DebuglnF("Mqtt : Erreur publish Relais1");
      }
      lastMqttMessageRelais = message;
    }
  }
}

void onMqttConnect(bool sessionPresent) {
  DebuglnF("Connecté au broker MQTT");
  if (sessionPresent)
    nbRestart = 0;

  // subscribe au topic set
  uint16_t packetIdSub = mqttClient.subscribe(MQTT_TOPIC_SET, 2);

  mqttClient.publish(MQTT_TOPIC_FP, 1, false, "{\"FP\":\"UP\"}");
  mqttClient.publish(MQTT_TOPIC_RELAIS, 1, false, "{\"RELAIS\":\"UP\"}");
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  DebuglnF("Déconnection du broker MQTT.");

  if (WiFi.isConnected() && config.mqtt.isActivated) {
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

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  DebuglnF("Subscribing at topic");
}

void onMqttUnsubscribe(uint16_t packetId) {
  DebuglnF("Unsubscribe topic");
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  DebugF("MQTT new message : topic ");
  Debugln(topic);

  String message = String(payload);

  if (len == 2 && message.startsWith("FP")) {
    mqttFpPublish();
  }
  else if (len == 6 && message.startsWith("RELAIS")) {
    mqttRelaisPublish();
  }
  else if (len == 5 && message.startsWith("FP=")) {
    message.remove(len);
    message.remove(0, 3);
    DebugF("message = ");
    Debugln(message);
    setfp(message);
  }
  else if (len == 11 && message.startsWith("FPS=")) {
    message.remove(len);
    message.remove(0, 4);
    DebugF("message = ");
    Debugln(message);
    setfp(message);
  }
  else if (len == 3 && message.startsWith("R=")) {
    message.remove(len);
    message.remove(0, 2);
    DebugF("message = ");
    Debugln(message);
    fnct_relais(message);
  }
  else {
    DebuglnF("Mqtt: Bad payload");
  }
}

void onMqttPublish(uint16_t packetId) {
  Serial.printf_P(PSTR("Publish packetId: %d\n"), packetId);
}

void initMqtt(void) {
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  if (config.mqtt.host != "" && config.mqtt.port > 0) {
    mqttClient.setServer(config.mqtt.host, config.mqtt.port);
  }
  if (config.mqtt.hasAuth && config.mqtt.user != "" && config.mqtt.password != "") {
    mqttClient.setCredentials(config.mqtt.user, config.mqtt.password);
  }
  #if ASYNC_TCP_SSL_ENABLED
    if (config.mqtt.protocol == "mqtts") {
      mqttClient.setSecure(true);
    }
  #endif

  #ifdef MOD_TELEINFO
    mqttTinfoTimer.attach(DELAY_PUBLISH_TINFO, mqttTinfoPublish);
  #endif
}
#endif // #ifdef MOD_MQTT
