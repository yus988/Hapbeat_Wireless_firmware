#include "globals.h"
#include "task_entry.h"
#include <MQTT_manager.h>

void TaskAppInit() {
  setFixGain(false);
  audioManager::setLimitIds(LIMITED_IDS, LIMITED_IDS_SIZE);
  audioManager::setStatusCallback(showStatusText);
  for (int i = 0; i < DISP_MSG_SIZE; i++) {
    const auto &msg = DISP_MSG[i];
    audioManager::setMessageData(msg.message, msg.id);
  }
  MQTT_manager::initMQTTclient(MQTTcallback, showStatusText);
  while (!MQTT_manager::getIsWiFiConnected()) {
    delay(500);
  };
}

void TaskAppStart() {
  xTaskCreatePinnedToCore(TaskUI_Run, "TaskUI", 4096, NULL, 23, &thp[1], 1);
}

void TaskAppLoop() {
  MQTT_manager::loopMQTTclient();
  delay(200);
}


