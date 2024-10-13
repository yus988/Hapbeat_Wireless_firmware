#include "globals.h"

// 現状はstatus=表示する文字列となっているが、
// 細かく設定したいなら、statusに応じて文とスタイルを別途定義すればよい。
// コールバック関数の引数を変えると変更箇所が多くなるので非推奨。
#if defined(BAND_V2) && defined(MQTT)
//////////////////////// コールバック関数の定義 ////////////////////////
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
  // 各種ICをON
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
    displayManager::printEfont(&_display, "WiFiへの接続を\n確認してください", 0,
                               0);
  } else {
    displayManager::printEfont(&_display, status, 0, 0);
  }
  _display.display();
  _lastDisplayUpdate = millis();  // 画面更新時刻をリセット
}

void manageBatteryStatus(bool showDisplay = false) {
  int voltage = lipo.voltage();
  int soc = (voltage - 3500) * 100 / (4100 - 3500);
  if (soc < 0) {
    soc = 0;
  } else if (soc > 100) {
    soc = 100;
  }
  uint8_t posX = 0;
  uint8_t posY = 8;
  _lastBatStatusUpdate = millis();
  std::string text;
  // 電池残量が所定の値以下になったら振動再生
  if (voltage < BAT_NOTIFY_VOL || soc < BAT_NOTIFY_SOC) {
    vibrationNotify();
    text = "充電してください";
  } else if (showDisplay) {
    std::string socStr = std::to_string(soc);  // 数値を文字列に変換
    std::string voltageStr = std::to_string(voltage);  // 数値を文字列に変換
    text = std::string(socStr) + "%" + " : " + std::string(voltageStr) +
           "mV";  // C++のstd::stringを使用して文字列を結合
  } else {
    return;
  }
  _display.ssd1306_command(SSD1306_DISPLAYON);  // ディスプレイを点灯させる
  _display.clearDisplay();
  _display.setCursor(posX, posY);  // カーソルを設定

  displayManager::printEfont(&_display, text.c_str(), posX,
                             posY);  // 文字列と座標を指定して表示
  _display.display();                // ディスプレイに表示
  _lastDisplayUpdate = millis();     // 画面更新時刻をリセット
}

// 待機中に極力電源オフ
void enableSleepMode() {
  // DISPLAY_TIMEOUT 秒が経過した場合、ディスプレイを消灯
  _display.ssd1306_command(SSD1306_DISPLAYOFF);
  // fill_solid(_leds, 1, CRGB::Black);  // すべてのLEDを黒色に設定。
  // FastLED.show();                     // LEDの色の変更を適用。
  digitalWrite(EN_VIBAMP_PIN, LOW);
}

  #include "../lib/MQTT_manager/MQTT_manager.h"
// color_sensor 用タスク
void TaskBandMQTT(void *args) {
  while (1) {
    // 所定の時間後に消灯。ただし音声再生中は実行しない
    if (millis() - _lastDisplayUpdate > DISPLAY_TIMEOUT &&
        audioManager::getIsPlaying() == false) {
      enableSleepMode();
    }
    if (millis() - _lastBatStatusUpdate > BATTERY_STATUS_INTERVAL) {
      manageBatteryStatus(false);
    }

    if (MQTT_manager::getIsWiFiConnected()) {
      // デバッグ用、電池残量表示
      // BQ27220_Cmd::printBatteryStats();
      // manageBatteryStatus();
      // ボタン操作
      for (int i = 0; i < sizeof(_SW_PIN) / sizeof(_SW_PIN[0]); i++) {
        if (!digitalRead(_SW_PIN[i]) && !_isBtnPressed[i]) {
          // ボタン押下で表示
          _display.ssd1306_command(SSD1306_DISPLAYON);
          _lastDisplayUpdate = millis();  // 画面更新時刻をリセット
          if (i == 1) {
            // モード切替
            bool isLimitEnable = audioManager::getIsLimitEnable();
            if (isLimitEnable) {
              audioManager::setIsLimitEnable(false);
              _currentColor = COLOR_FIX_MODE;
            } else {
              audioManager::setIsLimitEnable(true);
              _currentColor = COLOR_VOL_MODE;
            }
            int msgIdx = (isLimitEnable) ? 0 : 1;
            if (LIMIT_ENABLE_MSG[msgIdx] != nullptr) {
              showTextWithParams(LIMIT_ENABLE_MSG[msgIdx], 0, 8, true);
            } else {
              USBSerial.println("Error: Message pointer is null");
            }
            // vibrationNotify();
            // audioManager::playAudio(0, 30);
          } else if (i == 0) {
            USBSerial.println("Button 0");
            if (audioManager::getIsPlaying() == false) {
              manageBatteryStatus(true);
            }
            audioManager::stopAudio();
          }
          _leds[0] = _currentColor;
          FastLED.show();
          _isBtnPressed[i] = true;
        }
        if (digitalRead(_SW_PIN[i]) && _isBtnPressed[i]) {
          _isBtnPressed[i] = false;
        }
      };
      // loop delay
    }
    delay(190);
  }
}
#endif
