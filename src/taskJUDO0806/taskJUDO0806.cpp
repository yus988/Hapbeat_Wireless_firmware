#include "globals.h"

#if defined(NECKLACE_V2) && defined(JUDO0806)

// JUDO0806専用の表示処理用変数（静的確保）
static char volumeText[8];
static const int displayY = 10;      // 1行表示のY位置（上下中央）
static const int barX = 10;          // バーの開始X位置
static const int barWidth = 80;      // バー全体の幅
static const int barHeight = 12;     // バーの高さ（前と同じ）
static int padx = 3;

// ADC安定化用変数
static const int ADC_FILTER_SIZE = 4;     // 移動平均のサンプル数
static int adcBuffer[ADC_FILTER_SIZE];    // ADCサンプルバッファ
static int adcIndex = 0;                  // バッファインデックス
static int adcSum = 0;                    // サンプル合計
static int stableCounter = 0;             // 同じ値が続いた回数
static const int STABLE_COUNT = 1;        // 安定判定に必要な連続回数（高速化）

// ADC値の移動平均フィルタ
int filterADCValue(int newValue) {
  // バッファを更新
  adcSum -= adcBuffer[adcIndex];
  adcBuffer[adcIndex] = newValue;
  adcSum += newValue;
  adcIndex = (adcIndex + 1) % ADC_FILTER_SIZE;
  
  // 移動平均を返す
  return adcSum / ADC_FILTER_SIZE;
}

// ADC値の初期化
void initADCFilter(int initialValue) {
  for (int i = 0; i < ADC_FILTER_SIZE; i++) {
    adcBuffer[i] = initialValue;
  }
  adcSum = initialValue * ADC_FILTER_SIZE;
  adcIndex = 0;
}

// ボリューム表示更新関数（1行表示：レベルメータ＋数値のみ）
void updateJudoVolumeDisplay(int ampVolStep) {
  _display.clearDisplay();
  
  _display.setFont();  // 標準フォントにリセット
  _display.setTextSize(2);  // 文字サイズを大きく
  _display.setTextColor(SSD1306_WHITE);
  
  // レベル値を数値で表示（バーの右側）
  snprintf(volumeText, sizeof(volumeText), "%d", ampVolStep);
  _display.setCursor(barX + barWidth + 8, displayY-1);
  _display.print(volumeText);
  
  // レベルメータを表示
  _display.drawRect(barX, displayY, barWidth, barHeight, SSD1306_WHITE);
  
  // 内側のレベル表示（左から右に埋まる）
  int fillWidth = map(ampVolStep, 0, 23, 0, barWidth - 2);  // 枠内のサイズ
  if (fillWidth > 0) {
    _display.fillRect(barX + 1, displayY + 1, fillWidth, barHeight - 2, SSD1306_WHITE);
  }
  
  _display.display();
  _lastDisplayUpdate = millis();
  
  USBSerial.printf("JUDO0806 Mode - Volume: %d\n", ampVolStep);
}

void TaskJudoNeck() {
  // カテゴリを2で固定
  audioManager::setCategoryID(2);
  
  // 常に可変モード
  _isFixMode = false;
  audioManager::setIsFixMode(false);
  
  // 初期化
  _currAIN = analogRead(AIN_VIBVOL_PIN);
  initADCFilter(_currAIN);  // ADCフィルタ初期化
  // ディスプレイの向きに応じてADC読み取りを調整
  if (DISP_ROT == 90) {
    _ampVolStep = map(_currAIN, 0, 4095, GAIN_STEP_TXT_SIZE - 1, 0);  // 逆順
  } else {
    _ampVolStep = map(_currAIN, 0, 4095, 0, GAIN_STEP_TXT_SIZE - 1);  // 通常
  }
  setAmpStepGain(_ampVolStep, false);  // ディスプレイ更新は自前で行う
  updateJudoVolumeDisplay(_ampVolStep);
  
  int prevVolStep = _ampVolStep;
  
  while (1) {
    // analog readによるボリューム制御（フィルタリング）
    int rawADC = analogRead(AIN_VIBVOL_PIN);
    int filteredADC = filterADCValue(rawADC);
    // ディスプレイの向きに応じてADC読み取りを調整
    int newVolStep;
    if (DISP_ROT == 90) {
      newVolStep = map(filteredADC, 0, 4095, GAIN_STEP_TXT_SIZE - 1, 0);  // 逆順
    } else {
      newVolStep = map(filteredADC, 0, 4095, 0, GAIN_STEP_TXT_SIZE - 1);  // 通常
    }
    
    // 値が変化した場合の安定性チェック
    if (newVolStep == prevVolStep) {
      stableCounter++;
    } else {
      stableCounter = 0;  // 値が変わったらカウンターリセット
      prevVolStep = newVolStep;
    }
    
    // 安定した値で、かつ現在の値と違う場合のみ更新
    if (stableCounter >= STABLE_COUNT && newVolStep != _ampVolStep) {
      _ampVolStep = newVolStep;
      setAmpStepGain(_ampVolStep, false);  // ディスプレイ更新は自前で行う
      updateJudoVolumeDisplay(_ampVolStep);    // 表示更新
      stableCounter = 0;  // 更新後はカウンターリセット
    }
    
    // ボタン操作（チャンネル変更のみ）
    for (int i = 0; i < sizeof(_SW_PIN) / sizeof(_SW_PIN[0]); i++) {
      if (!digitalRead(_SW_PIN[i]) && !_isBtnPressed[i]) {
        uint8_t channel_ID = audioManager::getChannelID();
        audioManager::stopAudio();
        
        // カテゴリは固定なので、チャンネル変更のみ
        // 各ボタン毎の操作 0,1 = チャンネル上下, その他のボタンは無効
        if (i == 1 && channel_ID < CHANNEL_ID_TXT_SIZE - 1) {
          channel_ID += 1;
        } else if (i == 0 && channel_ID > 0) {
          channel_ID -= 1;
        }
        // i == 2, 3, 4 は何もしない（カテゴリ変更とモード変更は無効）
        
        _isBtnPressed[i] = true;
        
        // チャンネル変更時のみ更新（カテゴリは常に2固定）
        if (i == 0 || i == 1) {
          audioManager::setChannelID(channel_ID);
          USBSerial.printf("JUDO0806 - Channel changed to: %d\n", channel_ID);
        }
      }
      if (digitalRead(_SW_PIN[i]) && _isBtnPressed[i]) {
        _isBtnPressed[i] = false;
      }
    }
    
    // LED状態更新（常に可変モード）
    _leds[0] = COLOR_VOL_MODE;
    FastLED.show();
    
    // loop delay（高速化）
    vTaskDelay(30 / portTICK_PERIOD_MS);
  }
}

#endif

// main.cpp に出力する。用途に応じて適応するものを選択。
void TaskUI_JUDO0806(void *args) {
#if defined(NECKLACE_V2) && defined(JUDO0806)
  TaskJudoNeck();
#endif
}