#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <PubSubClient.h>
#include <WiFi.h>
#include "../../config.h"  // プロジェクトのルートからインクルード

namespace MQTT_manager {

// 初期化関数
void initMQTTclient(void (*callback)(char*, byte*, unsigned int));
void callback(char* topic, byte* payload, unsigned int length);
// ループ関数
void loopMQTTclient();

}  // namespace MQTT_manager

#endif  // MQTT_MANAGER_H
