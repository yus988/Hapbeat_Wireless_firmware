#include "globals.h"

#if defined(BAND_V2) && defined(MQTT)
void messageReceived(char *topic, byte *payload, unsigned int length) {
  USBSerial.print("Message arrived in topic: ");
  USBSerial.println(topic);
  USBSerial.print("Message: ");
  for (unsigned int i = 0; i < length; i++) {
    USBSerial.print((char)payload[i]);
  }
  USBSerial.println();
}

void MQTTcallback(char *topic, byte *payload, unsigned int length) {
  USBSerial.println("=== MQTT Message RECEIVED ===");
  USBSerial.print("Topic: ");
  USBSerial.println(topic);
  USBSerial.print("Payload length: ");
  USBSerial.println(length);
  USBSerial.print("Payload: ");
  for (unsigned int i = 0; i < length; i++) {
    USBSerial.print((char)payload[i]);
  }
  USBSerial.println();
  USBSerial.println("============================");
  digitalWrite(EN_VIBAMP_PIN, HIGH);
  audioManager::PlaySndFromMQTTcallback(topic, payload, length);
}

void vibrationNotify() {
  digitalWrite(EN_VIBAMP_PIN, HIGH);
  audioManager::setDataID(2, ID_MSG.notify);
  audioManager::stopAudio(2);
  audioManager::playAudio(2, 30);
}

void showStatusText(const char *status) {
  _display.ssd1306_command(SSD1306_DISPLAYON);
  _display.clearDisplay();
  _display.setCursor(0, 0);
  if (status == "connection failed") {
    vibrationNotify();
    displayManager::printEfont(&_display, "WiFiへの接続を\n確認してください", 0, 0);
  } else {
    displayManager::printEfont(&_display, status, 0, 0);
  }
  _display.display();
  _lastDisplayUpdate = millis();
}

void manageBatteryStatus(bool showDisplay = false) {
  int voltage = lipo.voltage();
  int soc = (voltage - 3500) * 100 / (4100 - 3500);
  if (soc < 0) { soc = 0; } else if (soc > 100) { soc = 100; }
  uint8_t posX = 0; uint8_t posY = 8; _lastBatStatusUpdate = millis();
  std::string text;
  if (voltage < BAT_NOTIFY_VOL || soc < BAT_NOTIFY_SOC) {
    vibrationNotify();
    text = "充電してください";
  } else if (showDisplay) {
    std::string socStr = std::to_string(soc);
    std::string voltageStr = std::to_string(voltage);
    text = std::string(socStr) + "%" + " : " + std::string(voltageStr) + "mV";
  } else {
    return;
  }
  _display.ssd1306_command(SSD1306_DISPLAYON);
  _display.clearDisplay();
  _display.setCursor(posX, posY);
  displayManager::printEfont(&_display, text.c_str(), posX, posY);
  _display.display();
  _lastDisplayUpdate = millis();
}

void enableSleepMode() {
  _display.ssd1306_command(SSD1306_DISPLAYOFF);
  digitalWrite(EN_VIBAMP_PIN, LOW);
}

void TaskBandMQTT() {
  while (1) {
    if (millis() - _lastDisplayUpdate > DISPLAY_TIMEOUT && audioManager::getIsPlaying() == false) {
      enableSleepMode();
    }
    if (millis() - _lastBatStatusUpdate > BATTERY_STATUS_INTERVAL) {
      manageBatteryStatus(false);
    }
    if (MQTT_manager::getIsWiFiConnected()) {
      for (int i = 0; i < sizeof(_SW_PIN) / sizeof(_SW_PIN[0]); i++) {
        if (!digitalRead(_SW_PIN[i]) && !_isBtnPressed[i]) {
          _display.ssd1306_command(SSD1306_DISPLAYON);
          _lastDisplayUpdate = millis();
          if (i == 1) {
            bool isLimitEnable = audioManager::getIsLimitEnable();
            if (isLimitEnable) { audioManager::setIsLimitEnable(false); _currentColor = COLOR_FIX_MODE; }
            else { audioManager::setIsLimitEnable(true); _currentColor = COLOR_VOL_MODE; }
            int msgIdx = (isLimitEnable) ? 0 : 1;
            if (LIMIT_ENABLE_MSG[msgIdx] != nullptr) {
              showTextWithParams(LIMIT_ENABLE_MSG[msgIdx], 0, 8, true);
            }
          } else if (i == 0) {
            if (audioManager::getIsPlaying() == false) { manageBatteryStatus(true); }
            audioManager::stopAudio();
          }
          _leds[0] = _currentColor; FastLED.show(); _isBtnPressed[i] = true;
        }
        if (digitalRead(_SW_PIN[i]) && _isBtnPressed[i]) { _isBtnPressed[i] = false; }
      };
    }
    delay(190);
  }
}
#endif

void TaskUI_MQTT(void *args) {
#if defined(NECKLACE_V2) && defined(MQTT)
  TaskNeckMQTT();
#elif defined(BAND_V2) && defined(MQTT)
  TaskBandMQTT();
#endif
}



