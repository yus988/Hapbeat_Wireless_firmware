#include "globals.h"
#include "adjustParams.h"  // グローバル adjustParams（共通定義）
#include "ui_defs.hpp"  // このタスク専用UI定義

enum class DeafMode : uint8_t { PlayAll = 0, NoScrape, VenueOnly, ArenaOnly, Test };

static DeafMode _mode = DeafMode::PlayAll;
static uint32_t _lastUserOpMs = 0;
static uint8_t _testSelId = 0;  // 0..5

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
      // 中央にラベル（最小サイズ前提）、下段左右に prev/next 矢印、右側に play→ 誘導
      const char *ja = TEST_ID_LABEL_JA[_testSelId % 6];
      const char *en = TEST_ID_LABEL_EN[_testSelId % 6];
      // おおよその中央配置（128x64, textsize=1 前提）
      displayManager::printEfont(&_display, ja, 32, 0);
      displayManager::printEfont(&_display, en, 40, 16);
      // 左下: ↙prev / 右下: next↘
      displayManager::printEfont(&_display, "\xE2\x86\x99prev", 0, 48);   // ↙
      displayManager::printEfont(&_display, "next\xE2\x86\x98", 80, 48); // ↘
      // play→ を英語行の右側に
      displayManager::printEfont(&_display, "play\xE2\x86\x92", 96, 16); // →
      break;
    }
  }
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
  _ampVolStep = map(_currAIN, 0, 4095, 0, GAIN_STEP_TXT_SIZE - 1);
  setAmpStepGain(_ampVolStep, true);
  uint8_t prevAmpVolStep = 0;
  _lastUserOpMs = millis();
  applyLimitIds();
  showModeTitle();

  while (1) {
    _currAIN = analogRead(AIN_VIBVOL_PIN);
    _ampVolStep = map(_currAIN, 0, 4095, 0, GAIN_STEP_TXT_SIZE - 1);
    static unsigned long volUiLastShown = 0;
    if (_ampVolStep != prevAmpVolStep) {
      prevAmpVolStep = _ampVolStep;
      setAmpStepGain(_ampVolStep, false);
      // 常時変動でJUDO風ボリュームUI表示
      _display.clearDisplay();
      _display.setFont();
      _display.setTextSize(VOL_UI_TEXT_SIZE);
      _display.setTextColor(SSD1306_WHITE);
      char volTxt[8]; snprintf(volTxt, sizeof(volTxt), "%d", _ampVolStep);
      _display.setCursor(VOL_UI_BAR_X + VOL_UI_BAR_W + 8, VOL_UI_BAR_Y - 1);
      _display.print(volTxt);
      _display.drawRect(VOL_UI_BAR_X, VOL_UI_BAR_Y, VOL_UI_BAR_W, VOL_UI_BAR_H, SSD1306_WHITE);
      int fillWidth = map(_ampVolStep, 0, 23, 0, VOL_UI_BAR_W - 2);
      if (fillWidth > 0) { _display.fillRect(VOL_UI_BAR_X + 1, VOL_UI_BAR_Y + 1, fillWidth, VOL_UI_BAR_H - 2, SSD1306_WHITE); }
      _display.display();
      volUiLastShown = millis();
    }
    // 一定時間操作が無ければメニュー表示へ戻す
    if (millis() - volUiLastShown > VOL_UI_TIMEOUT_MS) {
      showModeTitle();
    }

    for (int i = 0; i < sizeof(_SW_PIN) / sizeof(_SW_PIN[0]); i++) {
      if (!digitalRead(_SW_PIN[i]) && !_isBtnPressed[i]) {
        _lastUserOpMs = millis();
        uint8_t category_ID = audioManager::getCategoryID();
        uint8_t channel_ID = audioManager::getChannelID();
        // ボタン機能: 1ボタン=1アクション（モード選択）。テスト中は機能切替
        if (_mode != DeafMode::Test) {
          if (i == LOGICAL_BTN_IDX_TOP_LEFT) { _mode = DeafMode::PlayAll; applyLimitIds(); showModeTitle(); }
          else if (i == LOGICAL_BTN_IDX_TOP_RIGHT) { _mode = DeafMode::NoScrape; applyLimitIds(); showModeTitle(); }
          else if (i == LOGICAL_BTN_IDX_CENTER) { _mode = DeafMode::Test; _testSelId = 0; showModeTitle(); }
          else if (i == LOGICAL_BTN_IDX_BOTTOM_LEFT) { _mode = DeafMode::ArenaOnly; applyLimitIds(); showModeTitle(); }
          else if (i == LOGICAL_BTN_IDX_BOTTOM_RIGHT) { _mode = DeafMode::VenueOnly; applyLimitIds(); showModeTitle(); }
        } else {
          if (i == LOGICAL_BTN_IDX_BOTTOM_LEFT) { if (_testSelId > 0) _testSelId--; showModeTitle(); }
          else if (i == LOGICAL_BTN_IDX_BOTTOM_RIGHT) { if (_testSelId < 5) _testSelId++; showModeTitle(); }
          else if (i == LOGICAL_BTN_IDX_CENTER) {
          // テストモード時のみローカル再生（受信経路と同ロジックに乗せる）
            uint8_t dummy_mac_addr[6] = {0};
            uint8_t packet[8];
            packet[0] = audioManager::getCategoryID();
            packet[1] = audioManager::getChannelID();
            packet[2] = audioManager::getDevicePos();
            packet[3] = _testSelId; // dataID
            packet[4] = 0; // sub
            packet[5] = 200; // L
            packet[6] = 200; // R
            packet[7] = 0; // oneshot (テストは常に1ショット)
            audioManager::PlaySndOnDataRecv(dummy_mac_addr, packet, 8);
          }
          else if (i == LOGICAL_BTN_IDX_BOTTOM_LEFT || i == LOGICAL_BTN_IDX_BOTTOM_RIGHT) {
            // 戻る: モード選択へ（デフォルトは全て再生）
            _mode = DeafMode::PlayAll; applyLimitIds(); showModeTitle();
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






