#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <Arduino.h>
#include <vector>
#include "driver/i2s.h"

// タスク固有の音声関連マクロのみ取得する
// 各 env の build_flags に対象 task ディレクトリを -I 追加してください
#if __has_include("audioManagerSettings.hpp")
  #include "audioManagerSettings.hpp"
#else
  #include "audioManagerSettings_default.hpp"
#endif

// 調整項目（値を大きくするほどRAMを圧迫するので、適切なサイズに設定してください）
// audioManagerSettings.hpp 側で必ず定義される想定。未定義ならコンパイルエラーにする
#ifndef CATEGORY_NUM
  #error "CATEGORY_NUM must be defined in adjustParams.hpp"
#endif
#ifndef SOUND_FILE_NUM
  #error "SOUND_FILE_NUM must be defined in adjustParams.hpp"
#endif
#ifndef DATA_NUM
  #error "DATA_NUM must be defined in adjustParams.hpp"
#endif
#ifndef SUB_DATA_NUM
  #error "SUB_DATA_NUM must be defined in adjustParams.hpp"
#endif
#ifndef IS_EVENT_MODE
  #error "IS_EVENT_MODE must be defined in adjustParams.hpp"
#endif
static const bool isEventMode = IS_EVENT_MODE;  // イベント時にカテゴリに限らず全て通すか

//////////////////////////////////
// 以下は基本固定
#ifndef VOLUME_MAX
  #error "VOLUME_MAX must be defined in adjustParams.hpp"
#endif

// #define STUB_NUM 4  // 同時に再生するファイルの最大数。LRで2つ必要
#ifndef STUB_NUM
  #error "STUB_NUM must be defined in adjustParams.hpp"
#endif
#ifndef POSITION_NUM
  #error "POSITION_NUM must be defined in adjustParams.hpp"
#endif

//////////////////////////////////
#define RAM_STORAGE 0
#define FS_STORAGE 1
namespace audioManager {
const int SAMPLING_RATE = 16000;  // 8000 だと明確に遅くなる
// playCmd (data[7]) の定義
// 0 = oneshot (stub0), 1 = loopStart (stub2,3), 2 = loopStop (stub2,3停止)
// 3 = oneshot(2ndline) (stub4,5), 9 = continue (ループ継続 keep-alive)
// keep-aliveのタイムアウトは LOOP_CONTINUE_TIMEOUT_MS で設定可能
void initParamsEEPROM();
void readAllSoundFiles();
void initAudioOut(int I2S_BCLK_PIN, int I2S_LRCK_PIN, int I2S_DOUT_PIN);
void PlaySndOnDataRecv(const uint8_t *mac_addr, const uint8_t *data,
                       int data_len);
void PlaySndFromMQTTcallback(char *topic, byte *payload, unsigned int length);
void playAudioInLoop();
void playAudio(uint8_t tStubNum, uint8_t tVol, bool isLoop = false);
void stopAudio(uint8_t stubId = 99);
// get
uint8_t getCategoryID();
uint8_t getGain();
uint8_t getChannelID();
uint8_t getDevicePos();
bool getIsFixMode();
bool getIsLimitEnable();
bool getIsPlaying();
// set
void setDataID(uint8_t stubNum, uint8_t dataID, uint8_t subID = 0);
void setStatusCallback(void (*statusCb)(const char *));
void setGain(uint8_t G_SEL_A, uint8_t G_SEL_B, uint8_t val);
void setCategoryID(uint8_t value);
void setChannelID(uint8_t value);
void setDevicePos(uint8_t value);
void setIsFixMode(bool value);
void setLimitIds(const int limitIDs[], size_t size);  // 更新
void setMessageData(const char *msg, uint8_t id);
void setIsLimitEnable(bool value);
void setCategorySize(uint8_t size);
void saveVolumeLevels(uint8_t *volumeLevels, uint8_t size);
void loadVolumeLevels(uint8_t *volumeLevels, uint8_t size);

}  // namespace audioManager
#endif
