#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H
#include <Arduino.h>
#include <vector>
#include "driver/i2s.h"
#define STUB_NUM 4  // 同時に再生するファイルの最大数。LRで2つ必要
#define SOUND_FILE_NUM 60
#define RAM_STORAGE 0
#define FS_STORAGE 1
// 再生するカテゴリなどの最大数を定義（RAMは限られるので大きくしすぎないように）
#define CATEGORY_NUM 2
#define POSITION_NUM 1
#define DATA_NUM 20
#define SUB_DATA_NUM 6
namespace audioManager {
// int SAMPLING_RATE = 8000;
const int SAMPLING_RATE = 16000;
void initParamsEEPROM();
void readAllSoundFiles();
void initAudioOut(int I2S_BCLK_PIN, int I2S_LRCK_PIN, int I2S_DOUT_PIN);
void PlaySndOnDataRecv(const uint8_t *mac_addr, const uint8_t *data,
                       int data_len);
void PlaySndFromMQTTcallback(char *topic, byte *payload, unsigned int length);
void playAudioInLoop();
void playAudio(uint8_t tStubNum, uint8_t tVol);
void stopAudio(uint8_t stubId = 99);
// get
uint8_t getPlayCategory();
uint8_t getGain();
uint8_t getWearerId();
uint8_t getDevicePos();
bool getIsFixMode();
bool getIsLimitEnable();
bool getIsPlaying();
// set
void setDataID(uint8_t stubNum, uint8_t dataID, uint8_t subID = 0);
void setStatusCallback(void (*statusCb)(const char *));
void setGain(uint8_t G_SEL_A, uint8_t G_SEL_B, uint8_t val);
void setPlayCategory(uint8_t value);
void setWearerId(uint8_t value);
void setDevicePos(uint8_t value);
void setIsFixMode(bool value);
void setLimitIds(const int limitIDs[], size_t size);  // 更新
void setMessageData(const char *msg, uint8_t id);
void setIsLimitEnable(bool value);
}  // namespace audioManager
#endif
