#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <MQTT.h>
#include "../../config.h"

namespace MQTT_manager {
extern bool mqttConnected;

void initMQTTclient(void (*statusCallback)(const char*));
void loopMQTTclient();

}  // namespace MQTT_manager

#endif  // MQTT_MANAGER_H
