#include "globals.h"
#include <Fonts/FreeSansBold9pt7b.h>  // 9pt太字フォント

#if defined(NECKLACE_V2) && defined(WIRED)

// WIRED専用の表示処理用変数（静的確保）
static char percentText[8];
static const int barX = 2;        // バーの開始X位置（左端）
static const int barY = 15;       // バーの開始Y位置  
static const int barWidth = 122;  // バー全体の幅（左右いっぱい）
static const int barHeight = 12;  // バーの高さ
static bool isMuted = false;      // ミュート状態管理
static bool reverseVolumeDirection = true;  // ボリューム方向切り替え
                                             // false: つまみ上で音量アップ（通常）
                                             // true:  つまみ上で音量ダウン（反転）
static int padx = 3;
static int pady = 2;
                                             
// ボリューム表示更新関数
void updateVolumeDisplay(int ampVolStep) {
  // ボリューム表示の計算（0-23段階を0-100%にマッピング）
  int volumePercent = map(ampVolStep, 0, 23, 0, 100);
  
  _display.ssd1306_command(SSD1306_DISPLAYON);
  _display.clearDisplay();
  
  // 1段目：Volume（左端）とパーセント（右端）
  _display.setFont();  // 標準フォントにリセット
  _display.setTextSize(1);
  _display.setTextColor(SSD1306_WHITE);
  
  // Volume を左端に表示
  _display.setCursor(padx, pady);
  _display.print("Volume");
  
  // パーセント を右端に表示
  snprintf(percentText, sizeof(percentText), "%d%%", volumePercent);
  int percentWidth = strlen(percentText) * 6;  // 6ピクセル/文字（標準フォント）
  _display.setCursor(128 - percentWidth - padx, pady);
  _display.print(percentText);
  
  // 2段目：レベルメータを左右いっぱいで表示
  _display.drawRect(barX, barY, barWidth, barHeight, SSD1306_WHITE);
  
  // 内側のレベル表示（左から右に埋まる）
  int fillWidth = map(volumePercent, 0, 100, 0, barWidth - 4);  // 枠内のサイズ
  if (fillWidth > 0) {
    _display.fillRect(barX + 2, barY + 2, fillWidth, barHeight - 4, SSD1306_WHITE);
  }
  
  _display.display();
  _lastDisplayUpdate = millis();
  
  USBSerial.printf("WIRED Mode - Volume: %d%% (Step: %d)\n", volumePercent, ampVolStep);
}

// ミュート時の表示関数
void updateMuteDisplay() {
  _display.ssd1306_command(SSD1306_DISPLAYON);
  _display.clearDisplay();
  
  // FreeSansBold9pt7bで2行表示
  _display.setFont(&FreeSansBold9pt7b);
  _display.setTextColor(SSD1306_WHITE);
  
  // 1行目: "MUTE: press" 
  _display.setCursor(9, 15);
  _display.print("MUTE: press");
  
  // 2行目: "to unmute"
  _display.setCursor(5, 30);
  _display.print("btn to unmute");
  
  // フォントを標準に戻す
  _display.setFont();
  
  _display.display();
  _lastDisplayUpdate = millis();
  
  USBSerial.println("WIRED Mode - MUTED");
}

void TaskWiredNeck() {
  // 初期化
  _currAIN = analogRead(AIN_VIBVOL_PIN);
  if (reverseVolumeDirection) {
    _ampVolStep = map(_currAIN, 0, 4095, GAIN_STEP_TXT_SIZE - 1, 0);
  } else {
    _ampVolStep = map(_currAIN, 0, 4095, 0, GAIN_STEP_TXT_SIZE - 1);
  }
  
  uint8_t prevAmpVolStep = 0;
  
  while (1) {
    // analog readによるボリューム制御（方向切り替え対応）
    _currAIN = analogRead(AIN_VIBVOL_PIN);
    if (reverseVolumeDirection) {
      _ampVolStep = map(_currAIN, 0, 4095, GAIN_STEP_TXT_SIZE - 1, 0);
    } else {
      _ampVolStep = map(_currAIN, 0, 4095, 0, GAIN_STEP_TXT_SIZE - 1);
    }
    
    // ミュート時とノーマル時の処理分岐
    if (!isMuted) {
      // ノーマル時：ボリューム変化を監視
      if (_ampVolStep != prevAmpVolStep) {
        setAmpStepGain(_ampVolStep, false);  // ディスプレイ更新は自前で行う
        updateVolumeDisplay(_ampVolStep);    // 表示更新（軽量化）
      }
    }
    
    prevAmpVolStep = _ampVolStep;
    
    // ボタン処理（ミュート/解除用）
    for (int i = 0; i < sizeof(_SW_PIN) / sizeof(_SW_PIN[0]); i++) {
      if (!digitalRead(_SW_PIN[i]) && !_isBtnPressed[i]) {
        _isBtnPressed[i] = true;
        
        // ミュート機能が有効な場合のみミュート/解除をトグル
        if (ENABLE_WIRED_MUTE_FUNCTION) {
          isMuted = !isMuted;
        
          if (isMuted) {
            // ミュート有効：音量0の固定モード
            _isFixMode = true;
            audioManager::setIsFixMode(true);
            setAmpStepGain(0, false);  // 音量0に設定
            updateMuteDisplay();       // ミュート表示
            _leds[0] = COLOR_FIX_MODE; // LED色変更（ミュート時）
          } else {
            // ミュート解除：可変ボリューム制御に戻る
            _isFixMode = false;
            audioManager::setIsFixMode(false);
            setAmpStepGain(_ampVolStep, false);  // 現在のアナログ値に復帰
            updateVolumeDisplay(_ampVolStep);    // ボリューム表示復帰
            _leds[0] = COLOR_VOL_MODE;           // LED色変更
          }
          FastLED.show();
          
          USBSerial.printf("Button %d pressed - Mute: %s\n", i, isMuted ? "ON" : "OFF");
        } else {
          // ミュート機能が無効の場合：ボタンが押されても何もしない
          USBSerial.printf("Button %d pressed - Mute function disabled\n", i);
        }
      }
      if (digitalRead(_SW_PIN[i]) && _isBtnPressed[i]) {
        _isBtnPressed[i] = false;
      }
    }
    
    // LED状態更新（ミュート状態に応じて色変更）
    if (!isMuted) {
      _leds[0] = COLOR_VOL_MODE;
      FastLED.show();
    }
    
    // loop delay
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

#endif

// main.cpp に出力する。用途に応じて適応するものを選択。
void TaskUI_WIRED(void *args) {
#if defined(NECKLACE_V2) && defined(WIRED)
  TaskWiredNeck();
#endif
} 