// **********************************************************************************
// MQTT header file for remora project
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
//
// **********************************************************************************

#ifndef __MQTT_H__
#define __MQTT_H__

#include "remora.h"

#define MQTT_TOPIC_BASE "micropilote/"
#define MQTT_TOPIC_REMORA_BASE "remora/"

// Topic micro pilote info
#define MQTT_TOPIC_ONLINE     MQTT_TOPIC_BASE "online"    // micropilote/online {"id": 165656, "state": true} | retain = true
#define MQTT_TOPIC_SYSTEM     MQTT_TOPIC_BASE "system"    // micropilote/system {"id": 165656, "version": "2.0.0-beta", "ip": "192.168.1.31", "mac": "cd:cf:34:45:fc:4e", "rssi": "64", "freeheap": "2342", } | retain = true

// Topic micro pilote commande
#define MQTT_TOPIC_FP_GET     MQTT_TOPIC_BASE "fp/get"      // micropilote/fp/get {"id": 165656}
#define MQTT_TOPIC_FP_SET     MQTT_TOPIC_BASE "fp/set"      // micropilote/fp/set {"id": 165656, "fp": "C"}
#define MQTT_TOPIC_CONFIG_GET MQTT_TOPIC_BASE "config/get"  // micropilote/config/get {"id": 165656, "hostname": "mod1", "ssid": "wifi", "psk": "pass", "mqtt": { "host": mqtt.lan, "port": 1883, ...}}
#define MQTT_TOPIC_CONFIG_SET MQTT_TOPIC_BASE "config/set"  // micropilote/config/set {"id": 154965, "hostname": "mod1", "ssid": "wifi", "psk": "pass", "mqtt": { "host": mqtt.lan, "port": 1883, ...}}

#define MQTT_TOPPIC_REMORA_ONLINE MQTT_TOPIC_REMORA_BASE "online"

// Last Stand Will Message to be posted when Remora disconnect( /online 0 )
#define MQTT_TOPIC_LSW MQTT_TOPIC_BASE "online"

// Set keep alive in sec. Will also be used when Remora disconnect from broker to spawn lws message after it timeouts.
#define MQTT_KEEP_ALIVE 10

#define DELAY_PUBLISH_SYSINFO 10
//#define DELAY_PUBLISH_TINFO 1

// Variables
extern AsyncMqttClient mqttClient;

// Fonctions
void connectToMqtt(void);
void onMqttConnect(bool);
void onMqttDisconnect(AsyncMqttClientDisconnectReason);
void onMqttSubscribe(uint16_t, uint8_t);
void onMqttUnsubscribe(uint16_t packetId);
void onMqttMessage(char*, char*, AsyncMqttClientMessageProperties, size_t, size_t, size_t);
void onMqttPublish(uint16_t);
void initMqtt(void);
void disconnectMqtt(void);
bool mqttIsConnected(void);
void mqttFpPublish(void);
void mqttConfigPublish(bool error = false);

#endif // MQTT_H
