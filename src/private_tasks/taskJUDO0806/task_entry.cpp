#include "globals.h"
#include "task_entry.h"
#include <espnow_manager.h>
#include "taskJUDO0806.h"

void TaskAppInit() {
  USBSerial.println("init Hapbeat JUDO0806 mode");
  audioManager::setCategoryID(2);
  setFixGain(false);
  const char *judoMsg = "JUDO0806 Mode Ready";
  displayManager::printEfont(&_display, judoMsg, 0, 8);
  _display.display();
  espnowManager::init_esp_now(audioManager::PlaySndOnDataRecv);
}

void TaskAppStart() {
  xTaskCreatePinnedToCore(TaskUI_JUDO0806, "TaskUI_JUDO0806", 4096, NULL, 23,
                          &thp[1], 1);
}

void TaskAppLoop() {
  // 迚ｹ縺ｫ縺ｪ縺・}


