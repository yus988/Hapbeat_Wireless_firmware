#include "driver/i2s.h"

#ifndef AUDIO_MANAGER_H
  #define AUDIO_MANAGER_H

namespace audioManager {
void initParamsEEPROM();
void readAllSoundFiles();
void initAudioOut(int I2S_BCLK_PIN, int I2S_LRCK_PIN, int I2S_DOUT_PIN);
void PlaySndOnDataRecv(const uint8_t *mac_addr, const uint8_t *data,
                       int data_len);
void playAudioInLoop();
void stopAudio(uint8_t stubId = 99);
// get
uint8_t getPlayCategory();
uint8_t getGain();
uint8_t getWearerId();
uint8_t getDevicePos();
bool getIsFixMode();
// set
void setGain(uint8_t G_SEL_A, uint8_t G_SEL_B, uint8_t val);
void setPlayCategory(uint8_t value);
void setWearerId(uint8_t value);
void setDevicePos(uint8_t value);
void setIsFixMode(bool value);
}  // namespace audioManager
#endif
