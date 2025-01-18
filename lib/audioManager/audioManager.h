#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <Arduino.h>
#include <vector>
#include "driver/i2s.h"

// 調整項目（値を大きくするほどRAMを圧迫するので、適切なサイズに設定してください）
#define CATEGORY_NUM 3     // カテゴリの最大数
#define SOUND_FILE_NUM 60  // 読み込むファイルの最大数（＝/data 内のファイル数）
#define DATA_NUM 25        // 各カテゴリのデータ最大数
#define SUB_DATA_NUM 6     // sub_id の最大数

//////////////////////////////////
// 以下は基本固定
#define VOLUME_MAX 23     //ボリュームの最大値

// #define STUB_NUM 4  // 同時に再生するファイルの最大数。LRで2つ必要
#define STUB_NUM 6  // FPS/迷路デモするなら 6 必要（playCmd=3, oneshot_bgmのため）
#define RAM_STORAGE 0
#define FS_STORAGE 1
#define POSITION_NUM 1
namespace audioManager {
const int SAMPLING_RATE = 16000; //8000 だと明確に遅くなる
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
