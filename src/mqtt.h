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

// Topic info
#define MQTT_TOPIC_ONLINE    MQTT_TOPIC_BASE "online"    // remora/online 1 | retain = true
#define MQTT_TOPIC_SYSTEM    MQTT_TOPIC_BASE "system"    // remora/system {"version": "2.0.0-beta", "board": "Remora v1.5", "ip": "192.168.1.31", "mac": "cd:cf:34:45:fc:4e", "rssi": "64", "freeheap": "2342", } | retain = true
#define MQTT_TOPIC_RELAIS    MQTT_TOPIC_BASE "relais"    // remora/relais {"mode": "0", "status": "2"} | retain = false
#define MQTT_TOPIC_FP        MQTT_TOPIC_BASE "fp"        // remora/fp {"fp3": "H", "fp4": "H", "fp5": "C", "fp7": "E"} | retain = false
#define MQTT_TOPIC_DELESTAGE MQTT_TOPIC_BASE "delestage" // remora/delestage {"etat": "1", "delestage": "0", "relestage": "0"} | retain = false
#define MQTT_TOPIC_TINFO     MQTT_TOPIC_BASE "teleinfo"  // remora/teleinfo {"etat": "1", "adco": "03428067", "optarif": "HC", "isousc": "15", "hchc": "000261373", "hchp": "000132345", "ptec": "HC", etc...} | retain = false


// Topic commande
#define MQTT_TOPIC_FP_GET     MQTT_TOPIC_BASE "fp/get/#"   // remora/fp/get || remora/fp/get/1
#define MQTT_TOPIC_FP_SET     MQTT_TOPIC_BASE "fp/set"     // remora/fp/set {"fp1": "C", "fp2": "H"}
#define MQTT_TOPIC_RELAIS_GET MQTT_TOPIC_BASE "relais/get" // remora/relais/get
#define MQTT_TOPIC_RELAIS_SET MQTT_TOPIC_BASE "relais/set" // remora/relais/set {"mode": "1", "status": "1"}

// Last Stand Will Message to be posted when Remora disconnect( /online 0 )
#define MQTT_TOPIC_LSW MQTT_TOPIC_BASE "online"

// Set keep alive in sec. Will also be used when Remora disconnect from broker to spawn lws message after it timeouts.
#define MQTT_KEEP_ALIVE 10

#define DELAY_PUBLISH_SYSINFO 5
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
void mqttFpPublish(uint8_t fp = 0, bool force = false);
#ifdef MOD_ADPS
  void mqttDelestagePublish(void);
#endif
void mqttRelaisPublish(void);
#ifdef MOD_TELEINFO
  void mqttTinfoPublish(void);
#endif

#endif // MOD_MQTT
#endif // MQTT_H