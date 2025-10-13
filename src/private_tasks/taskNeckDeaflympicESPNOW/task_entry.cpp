#include "globals.h"
#include "task_entry.h"
#include <espnow_manager.h>

void TaskAppInit() {
  // ESPNOW 共通初期化
  displayManager::setTitle(CATEGORY_ID_TXT, CATEGORY_ID_TXT_SIZE,
                           CHANNEL_ID_TXT, CHANNEL_ID_TXT_SIZE, GAIN_STEP_TXT,
                           GAIN_STEP_TXT_SIZE);
  setFixGain(false);
  
  // カテゴリIDを2に固定（Deaflympic専用）
  audioManager::setCategoryID(0);
  
  _display.display();
  espnowManager::init_esp_now(audioManager::PlaySndOnDataRecv);
}

void TaskAppStart() {
  xTaskCreatePinnedToCore(TaskUI_Run, "TaskUI", 4096, NULL, 23, &thp[1], 1);
}

void TaskAppLoop() {
  // ESPNOW はループで特別処理不要
}
