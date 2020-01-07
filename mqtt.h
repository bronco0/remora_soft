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

#ifdef MOD_MQTT
#define MQTT_TOPIC_BASE "remora/"

// under MQTT_TOPIC_SET  we are reading subtopics to set fp & relai
#define MQTT_TOPIC_SET  MQTT_TOPIC_BASE "set"

// under MQTT_TOPIC_FP we are send fp informations
#define MQTT_TOPIC_FP     MQTT_TOPIC_BASE "fp"

// under MQTT_TOPIC_FP we are send fp informations
#define MQTT_TOPIC_RELAIS MQTT_TOPIC_BASE "relais"

// Under MQTT_TOPIC_TINFO we are send teleinformation
#define MQTT_TOPIC_TINFO  MQTT_TOPIC_BASE "tinfo"

#define DELAY_PUBLISH_TINFO 1

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
void mqttRelaisPublish(void);

#endif
#endif
