#include "globals.h"
#include "task_entry.h"

void TaskAppInit() {
  USBSerial.println("init Hapbeat WIRED mode");
  const char *wiredMsg = "WIRED Mode Ready";
  displayManager::printEfont(&_display, wiredMsg, 0, 8);
  _display.display();
  setFixGain(false);
}

void TaskAppStart() {
  xTaskCreatePinnedToCore(TaskUI_Run, "TaskUI", 4096, NULL, 23, &thp[1], 1);
}

void TaskAppLoop() {
  // ESPNOW はループで特別処理不要}
}

