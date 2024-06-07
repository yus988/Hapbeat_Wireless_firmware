#include <esp_now.h>
#ifndef ESPNOW_MANAGER_H
  #define ESPNOW_MANAGER_H
namespace espnowManager {
void init_esp_now(esp_now_recv_cb_t callback);
}
#endif
