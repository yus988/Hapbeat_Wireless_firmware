#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

namespace espnowManager {
esp_now_peer_info_t slave;
void init_esp_now(esp_now_recv_cb_t callback) {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    USBSerial.println("ESPNow Init Success");
  } else {
    USBSerial.println("ESPNow Init Failed");
    ESP.restart();
  }
  // set long range mode
  esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_LR);
  // マルチキャスト用Slave登録
  memset(&slave, 0, sizeof(slave));
  for (int i = 0; i < 6; ++i) {
    slave.peer_addr[i] = (uint8_t)0xff;
  }
  esp_err_t addStatus = esp_now_add_peer(&slave);
  if (addStatus == ESP_OK) {  // Pair success
    USBSerial.println("Pair success");
  }
  // ESP-NOWコールバック登録
  esp_now_register_recv_cb(callback);
}
}