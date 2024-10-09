// 不要になった関数のアーカイブ

// xTaskCreatePinnedToCore(TaskCurrent, "TaskCurrent", 4096, NULL, 2, &thp[2],
// 1);


// // 電流を計算するための関数
// float calculateCurrent(int adc_value) {
//   // ADCの値を電圧に変換
//   float voltage = (adc_value * V_REF) / ADC_MAX;
//   // 電圧を電流に変換
//   float current = voltage / (INA_GAIN * SHUNT_RESISTANCE);
//   return current;
// }

// void TaskCurrent(void *args) {
//   int adc_value;
//   float current;
//   int shutdownCounter[2] = {0, 0};
//   int restoreCounter = 0;  // カウンタの初期化
//   uint8_t modifiedStep;
//   while (1) {
//     // ADC値の読み取り
//     adc_value = analogRead(BAT_CURRENT_PIN);
//     // 電流値の計算
//     // current = calculateCurrent(adc_value);
//     if (current > current_thresholds[0]) {
//       shutdownCounter[0]++;
//       if (current > current_thresholds[1]) {
//         shutdownCounter[1]++;
//       }
//       if (shutdownCounter[0] >= shutdownCycles[0] ||
//           shutdownCounter[1] >= shutdownCycles[1]) {
//         // 電流値が閾値を超えた場合にアンプをミュート
//         digitalWrite(EN_VIBAMP_PIN, LOW);
//         _disableVolumeControl = true;  // 音量操作を無効化
//         _leds[0] = COLOR_DANGER_MODE;
//         FastLED.show();
//         restoreCounter = 0;  // カウンタリセット
//       }
//     } else {
//       if (restoreCounter >= restoreCycles) {
//         _disableVolumeControl = false;  // 音量操作を有効化
//         digitalWrite(EN_VIBAMP_PIN, HIGH);
//         if (_isFixMode) {
//           setFixGain(false);
//           _leds[0] = COLOR_FIX_MODE;
//         } else {
//           setAmpStepGain(_ampVolStep, false);
//           _leds[0] = COLOR_VOL_MODE;
//         }
//         FastLED.show();
//         shutdownCounter[0] = 0;  // カウンタリセット
//         shutdownCounter[1] = 1;
//       } else {
//         restoreCounter++;  // カウンタ増加
//       }
//     }
//     // 電流値をシリアルモニタに出力
//     // USBSerial.printf("Measured Current: %.6f A\n", current);
//     // 100ms待機
//     vTaskDelay(1 / portTICK_PERIOD_MS);
//   }
// }