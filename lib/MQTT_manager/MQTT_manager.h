#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <PubSubClient.h>
#include <WiFi.h>
#include "../../config.h"  // プロジェクトのルートからインクルード
#include <WiFiClientSecure.h>  // WiFiClientSecureをインクルード

namespace MQTT_manager {
extern bool mqttConnected;

// 初期化関数
void initMQTTclient(void (*callback)(char*, byte*, unsigned int),
                    void (*statusCallback)(const char*));
void callback(char* topic, byte* payload, unsigned int length);
// ループ関数
void loopMQTTclient();
void reconnect();

}  // namespace MQTT_manager

#endif  // MQTT_MANAGER_H
