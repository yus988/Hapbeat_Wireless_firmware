#include "globals.h"
#include "adjustParams.h"  // グローバル adjustParams（共通定義）
#include "ui_defs.hpp"  // このタスク専用UI定義

enum class DeafMode : uint8_t { PlayAll = 0, NoScrape, VenueOnly, ArenaOnly, Test };

static DeafMode _mode = DeafMode::PlayAll;
static DeafMode _prevModeBeforeTest = DeafMode::PlayAll;  // テスト前のモードを記憶
static uint32_t _lastUserOpMs = 0;
static uint8_t _testSelId = 0;  // 0..5（電源ON中は保持）

// UI表示のヘルパ
static void showModeTitle() {
  // 画面全体をクリアしてから描画（前画面の残像を防止）
  _display.clearDisplay();
  switch (_mode) {
    case DeafMode::PlayAll:
      displayManager::printEfont(&_display, MENU_PLAYALL_JA, 0, 0);
      displayManager::printEfont(&_display, MENU_PLAYALL_EN, 0, 16);
      break;
    case DeafMode::NoScrape:
      displayManager::printEfont(&_display, MENU_NOSCRAPE_JA, 0, 0);
      displayManager::printEfont(&_display, MENU_NOSCRAPE_EN, 0, 16);
      break;
    case DeafMode::VenueOnly:
      displayManager::printEfont(&_display, MENU_VENUE_JA, 0, 0);
      displayManager::printEfont(&_display, MENU_VENUE_EN, 0, 16);
      break;
    case DeafMode::ArenaOnly:
      displayManager::printEfont(&_display, MENU_ARENA_JA, 0, 0);
      displayManager::printEfont(&_display, MENU_ARENA_EN, 0, 16);
      break;
    case DeafMode::Test: {
      const char *ja = TEST_ID_LABEL_JA[_testSelId % 6];
      const char *en = TEST_ID_LABEL_EN[_testSelId % 6];

      const int ytop=0;
      const int ybottom = 16;
      const int xtitle = 24;
      
      // 上段: 日本語ラベル（左寄せ） + play->（右寄せ）
      displayManager::printEfont(&_display, ja, xtitle, ytop);
      // "play->" は6文字半角 x 8px = 48px、右端揃え: 128 - 48 = 80
      displayManager::printEfont(&_display, "play->", 80, ytop);
      
      // 下段: ↙（左端） + 英語ラベル + ↘（右端）
      displayManager::printEfont(&_display, "\xE2\x86\x99", 0, ybottom);     // ↙ (U+2199)
      displayManager::printEfont(&_display, en, xtitle, ybottom);
      // "↘" は全角1文字 x 16px = 16px、右端揃え: 128 - 16 = 112
      displayManager::printEfont(&_display, "\xE2\x86\x98", 112, ybottom);   // ↘ (U+2198)
      break;
    }
  }
  _display.display();  // 更新を反映
}

// モードに応じたID制限の適用
static void applyLimitIds() {
  switch (_mode) {
    case DeafMode::PlayAll:
      audioManager::setLimitIds(MODE_PLAY_ALL_IDS, MODE_PLAY_ALL_IDS_SIZE);
      audioManager::setIsLimitEnable(false);
      break;
    case DeafMode::NoScrape:
      audioManager::setLimitIds(MODE_NO_SCRAPE_IDS, MODE_NO_SCRAPE_IDS_SIZE);
      audioManager::setIsLimitEnable(true);
      break;
    case DeafMode::VenueOnly:
      audioManager::setLimitIds(MODE_VENUE_ONLY_IDS, MODE_VENUE_ONLY_IDS_SIZE);
      audioManager::setIsLimitEnable(true);
      break;
    case DeafMode::ArenaOnly:
      audioManager::setLimitIds(MODE_ARENA_ONLY_IDS, MODE_ARENA_ONLY_IDS_SIZE);
      audioManager::setIsLimitEnable(true);
      break;
    case DeafMode::Test:
      audioManager::setIsLimitEnable(false);
      break; // テストはローカル再生のみ
  }
}

void TaskNeckESPNOW() {
  // 本タスクでは FixMode は使用しない
  _isFixMode = false;
  audioManager::setIsFixMode(false);
  _currAIN = analogRead(AIN_VIBVOL_PIN);
  _ampVolStep = map(_currAIN, 0, 4095, 23, 0);  // 逆向き（高電圧=低ボリューム→低ステップ）
  setAmpStepGain(_ampVolStep, true);
  
  // ちらつき防止用：前回表示した値と窓枠（ヒステリシス）
  static int lastDisplayedVolStep = _ampVolStep;
  static unsigned long volUiLastShown = 0;
  static bool volUiShown = false;
  
  _lastUserOpMs = millis();
  applyLimitIds();
  showModeTitle();

  while (1) {
    _currAIN = analogRead(AIN_VIBVOL_PIN);
    int newVolStep = map(_currAIN, 0, 4095, 23, 0);  // 逆向き（高電圧=低ボリューム→低ステップ）
    
    // ちらつき防止：窓枠を超えた変化のみ画面更新
    int delta = abs(newVolStep - lastDisplayedVolStep);
    if (delta >= VOL_UI_CHANGE_THRESHOLD) {
      _ampVolStep = newVolStep;
      lastDisplayedVolStep = newVolStep;
      setAmpStepGain(_ampVolStep, false);
      
      // JUDO風ボリュームUI表示（@taskJUDO0806 と同一）
      _display.clearDisplay();
      _display.setFont();
      _display.setTextSize(2);
      _display.setTextColor(SSD1306_WHITE);
      char volTxt[8]; 
      snprintf(volTxt, sizeof(volTxt), "%d", _ampVolStep);
      _display.setCursor(VOL_UI_BAR_X + VOL_UI_BAR_W + 8, VOL_UI_BAR_Y - 1);
      _display.print(volTxt);
      _display.drawRect(VOL_UI_BAR_X, VOL_UI_BAR_Y, VOL_UI_BAR_W, VOL_UI_BAR_H, SSD1306_WHITE);
      int fillWidth = map(_ampVolStep, 0, 23, 0, VOL_UI_BAR_W - 2);
      if (fillWidth > 0) { 
        _display.fillRect(VOL_UI_BAR_X + 1, VOL_UI_BAR_Y + 1, fillWidth, VOL_UI_BAR_H - 2, SSD1306_WHITE); 
      }
      _display.display();
      volUiLastShown = millis();
      volUiShown = true;
    } else {
      // 窓枠内の変化でもゲインは更新（音量は細かく追従）
      if (newVolStep != _ampVolStep) {
        _ampVolStep = newVolStep;
        setAmpStepGain(_ampVolStep, false);
      }
    }
    
    // 一定時間操作が無ければメニュー表示へ戻す
    if (volUiShown && millis() - volUiLastShown > VOL_UI_TIMEOUT_MS) {
      volUiShown = false;
      showModeTitle();
    }

    for (int i = 0; i < sizeof(_SW_PIN) / sizeof(_SW_PIN[0]); i++) {
      if (!digitalRead(_SW_PIN[i]) && !_isBtnPressed[i]) {
        _lastUserOpMs = millis();
        uint8_t category_ID = audioManager::getCategoryID();
        uint8_t channel_ID = audioManager::getChannelID();
        // ボタン機能: 1ボタン=1アクション（モード選択）。テスト中は機能切替
        if (_mode != DeafMode::Test) {
          if (i == LOGICAL_BTN_IDX_TOP_LEFT) { 
            _mode = DeafMode::PlayAll; 
            applyLimitIds(); 
            showModeTitle(); 
          }
          else if (i == LOGICAL_BTN_IDX_TOP_RIGHT) { 
            _mode = DeafMode::NoScrape; 
            applyLimitIds(); 
            showModeTitle(); 
          }
          else if (i == LOGICAL_BTN_IDX_CENTER) { 
            // テストモードに入る前に現在のモードを記憶
            _prevModeBeforeTest = _mode;
            _mode = DeafMode::Test; 
            // _testSelId は保持（電源ON中は記憶）
            showModeTitle(); 
          }
          else if (i == LOGICAL_BTN_IDX_BOTTOM_LEFT) { 
            _mode = DeafMode::ArenaOnly; 
            applyLimitIds(); 
            showModeTitle(); 
          }
          else if (i == LOGICAL_BTN_IDX_BOTTOM_RIGHT) { 
            _mode = DeafMode::VenueOnly; 
            applyLimitIds(); 
            showModeTitle(); 
          }
        } else {
          // テストモード中の操作
          if (i == LOGICAL_BTN_IDX_TOP_LEFT) {
            // 左上: 全て再生モードへ戻る
            _mode = DeafMode::PlayAll; 
            applyLimitIds(); 
            showModeTitle();
          }
          else if (i == LOGICAL_BTN_IDX_TOP_RIGHT) {
            // 右上: すり足無しモードへ戻る
            _mode = DeafMode::NoScrape; 
            applyLimitIds(); 
            showModeTitle();
          }
          else if (i == LOGICAL_BTN_IDX_BOTTOM_LEFT) { 
            // 左下: prev（ID減）
            if (_testSelId > 0) _testSelId--; 
            showModeTitle(); 
          }
          else if (i == LOGICAL_BTN_IDX_BOTTOM_RIGHT) { 
            // 右下: next（ID増）
            if (_testSelId < 5) _testSelId++; 
            showModeTitle(); 
          }
          else if (i == LOGICAL_BTN_IDX_CENTER) {
            // 中央: テスト再生（受信経路と同ロジックに乗せる）
            uint8_t dummy_mac_addr[6] = {0};
            uint8_t packet[8];
            packet[0] = category_ID;
            packet[1] = channel_ID;
            packet[2] = audioManager::getDevicePos();
            packet[3] = _testSelId; // dataID
            packet[4] = 0; // sub
            packet[5] = 200; // L
            packet[6] = 200; // R
            packet[7] = 0; // oneshot (テストは常に1ショット)
            audioManager::PlaySndOnDataRecv(dummy_mac_addr, packet, 8);
          }
        }
        _isBtnPressed[i] = true;
      }
      if (digitalRead(_SW_PIN[i]) && _isBtnPressed[i]) _isBtnPressed[i] = false;
    };

    // テストモードの自動復帰（60秒無操作でPlayAll）
    if (_mode == DeafMode::Test && millis() - _lastUserOpMs > MODE_TEST_IDLE_TIMEOUT_MS) {
      _mode = DeafMode::PlayAll;
      applyLimitIds();
      showModeTitle();
      _lastUserOpMs = millis();
    }

    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

void TaskUI_ESPNOW(void *args) {
#if defined(NECKLACE_V2) || defined(NECKLACE_V3)
  TaskNeckESPNOW();
#endif
}

// 統一インターフェース
void TaskUI_Run(void *args) { TaskUI_ESPNOW(args); }






