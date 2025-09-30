#include "globals.h"
#include "task_entry.h"
#include <espnow_manager.h>

void TaskAppInit() {
  // ESPNOW 蜈ｱ騾壼・譛溷喧
  displayManager::setTitle(CATEGORY_ID_TXT, CATEGORY_ID_TXT_SIZE,
                           CHANNEL_ID_TXT, CHANNEL_ID_TXT_SIZE, GAIN_STEP_TXT,
                           GAIN_STEP_TXT_SIZE);
  setFixGain(true);
  _display.display();
  espnowManager::init_esp_now(audioManager::PlaySndOnDataRecv);
}

void TaskAppStart() {
  xTaskCreatePinnedToCore(TaskUI_Run, "TaskUI", 4096, NULL, 23, &thp[1], 1);
}

void TaskAppLoop() {
  // ESPNOW 縺ｯ繝ｫ繝ｼ繝励〒迚ｹ蛻･蜃ｦ逅・ｸ崎ｦ・}


