#include <LittleFS.h>

#include "FS.h"
// オーディオ再生関連 ESP8266
#include "AudioFileSourcePROGMEM.h"
#include "AudioGeneratorMP3.h"
// EEPROM
#include <EEPROM.h>
// audio libraries
#include "AudioFileSourceLittleFS.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2S.h"
#include "AudioOutputMixer.h"
#include "driver/i2s.h"

#include <AudioGeneratorMP3.h>

#define STUB_NUM 4
bool isPlayAudio[] = {false, false, false, false};

namespace audioManager {
int maxVol = 255;
int samplingRate = 16000;
// int samplingRate = 8000;

// //////////////// play audio begin ///////////////////////
// position identifier
// 0:neck, 1:chest, 2:abdomen
// 3:upperArm_L, 4:upperArm_R, 5:wrist_L, 6:wrist_R,
// 7:thigh_L, 8:thigh_R, 9:calf_L, 10:calf_R
uint8_t _devicePos;  // 装着場所の指定
uint8_t _gainNum;    //
struct Data {
  uint8_t playCategory;
  uint8_t wearerId;  // 装着者のID。99でブロードキャスト
  bool isFixMode;
};
Data _data;

// uint8_t _wearerId;   // 装着者のID。99でブロードキャスト
// uint8_t _playCategory;
// uint8_t _gainNum;

// MAX98357 gain select
// https://www.analog.com/media/en/technical-documentation/data-sheets/max98357a-max98357b.pdf
uint8_t sel_A_pin;
uint8_t sel_B_pin;

// AudioGeneratorWAV *_wav_gen[STUB_NUM];
AudioGeneratorMP3 *_wav_gen[STUB_NUM];
AudioFileSourcePROGMEM *_src_prog = NULL;
AudioFileSourceLittleFS
    *_file;  // なんか要るっぽい？無くても動くが、あった方がが応答早い気がする
AudioOutputI2S *_i2s_out;
AudioOutputMixer *_mixer;
AudioOutputMixerStub *_stub[STUB_NUM];

// [cat][pos][dataID][_subID][isRight]
uint8_t _audioDataIndex[2][1][20][6][2];

uint8_t *_audioData[30];
size_t _audioDataSize[30];
uint8_t _dataID[STUB_NUM];
uint8_t _subID[STUB_NUM];
uint8_t _volume[STUB_NUM];

void initParamsEEPROM() {
  // EEPROM.begin(sizeof(_data));
  EEPROM.begin(8);
  // EEPROMからデータを読み込む
  EEPROM.get(0, _data);
  // USBSerial.println(_data.playCategory);
  // 読み込んだデータが有効な値かどうかをチェックし、無効な場合はすべての変数を0にセットする
  if (_data.wearerId == 0xFF || _data.playCategory == 0xFF ||
      _data.isFixMode == 0xFF) {
    _data.playCategory = 0;
    _data.wearerId = 0;
    _data.isFixMode = true;
  }
}

// 引数無しの場合は全てのstubを停止
void stopAudio(uint8_t stub = 99) {
  if (stub == 99) {
    for (int iStub = 0; iStub < STUB_NUM; iStub++) {
      _wav_gen[iStub]->stop();
      _stub[iStub]->stop();
      isPlayAudio[iStub] = false;
    }
  } else {
    _wav_gen[stub]->stop();
    _stub[stub]->stop();
    isPlayAudio[stub] = false;
  }
}

void playAudio(uint8_t tStubNum, uint8_t tVol) {
  int isLR = (tStubNum == 0 || 2) ? 0 : 1;
  // pos と isRight = 0 は仮置き
  uint8_t pos = 0;
  uint8_t idx = _audioDataIndex[_data.playCategory][pos][_dataID[tStubNum]]
                               [_subID[tStubNum]][isLR];
  USBSerial.printf("idx = %d\n", idx);
  _src_prog->open(_audioData[0], _audioDataSize[0]);
  _stub[tStubNum]->SetGain((float)tVol / maxVol);
  _wav_gen[tStubNum]->begin(_src_prog, _stub[tStubNum]);
  isPlayAudio[tStubNum] = true;
}

// void playAudio(uint8_t tStubNum, uint8_t tVol) {
//   int isLR = (tStubNum == 0 || 2) ? 0 : 1;
//   // pos と isRight = 0 は仮置き
//   uint8_t pos = 0;
//   uint8_t idx = _audioDataIndex[_data.playCategory][pos][_dataID[tStubNum]]
//                                [_subID[tStubNum]][isLR];
//   USBSerial.printf("idx = %d\n", idx);
//   _src_prog->open(_audioData[idx], _audioDataSize[idx]);
//   _stub[tStubNum]->SetGain((float)tVol / maxVol);
//   _wav_gen[tStubNum]->begin(_src_prog, _stub[tStubNum]);
//   isPlayAudio[tStubNum] = true;
// }

// void playSilent(uint8_t tStubNum) {
//   // pos と isRight = 0 は仮置き
//   uint8_t pos = 0;
//   uint8_t idx = _audioDataIndex[0][0][0][0][0];
//   USBSerial.printf("idx = %d\n", idx);
//   _src_prog[0]->open(_audioData[idx], _audioDataSize[idx]);
//   _stub[tStubNum]->SetGain(0);
//   _wav_gen[tStubNum]->begin(_src_prog[0], _stub[tStubNum]);
//   // isPlayAudio[tStubNum] = true;
// }

void PlaySndOnDataRecv(const uint8_t *mac_addr, const uint8_t *data,
                       int data_len) {
  USBSerial.printf(
      "recieved: _category %d, _wearerId %d, _devPos %d, _dataID %d, _subID "
      "%d, Vol %d:%d, isLoop %d\n",
      data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);

  isPlayAudio[0] = true;
  playAudio(0, _volume[0]);
  // // data = [_category, _wearerId, _devicePos, data_id, _subID, _L_Vol,
  // // _R_Vol, isLoop] 各種条件が合致した時のみ値を保持 wearerId = 0
  // の時は全受信 if ((data[0] == _data.playCategory || data[0] == 99) &&
  //     (data[1] == _data.wearerId || data[1] == 99 || _data.wearerId == 0) &&
  //     (data[2] == _devicePos || data[2] == 99)) {
  //   uint8_t isLoop = data[7];  // 0=oneshot, 1=loopStart, 2=stopAudio

  //   if (isLoop == 2) {
  //     for (int iStub = 2; iStub <= 3; ++iStub) {
  //       while (_wav_gen[iStub]->isRunning()) {
  //         stopAudio(iStub);
  //         delay(10);
  //       }
  //     }
  //     // isPlayAudio[2] = false;
  //     // isPlayAudio[3] = false;
  //     return;
  //   } else {
  //     int startIdx = (isLoop == 0) ? 0 : 2;
  //     int endIdx = (isLoop == 0) ? 2 : 4;

  //     for (int i = startIdx; i < endIdx; ++i) {
  //       _dataID[i] = data[3];
  //       _subID[i] = data[4];
  //       _volume[i] = data[5 + i % 2];
  //     }

  //     uint8_t stub = (isLoop == 0) ? 0 : 2;
  //     stopAudio(stub);
  //     playAudio(stub, _volume[stub]);
  //     // ステレオ再生を左右のボリュームから判断
  //     if (_volume[stub] != _volume[stub + 1]) {
  //       stopAudio(stub + 1);
  //       playAudio(stub + 1, _volume[stub + 1]);
  //     }
  //   }
  // }
}

void playAudioInLoop() {
  if (isPlayAudio[0]) {
    if (_wav_gen[0]->isRunning()) {
      if (!_wav_gen[0]->loop()) {
        stopAudio(0);
      }
    }
  }
}

// void playAudioInLoop() {
//   for (int iStub = 0; iStub < STUB_NUM; iStub++) {
//     if (isPlayAudio[iStub]) {
//       if ((iStub == 2 && _wav_gen[2]->isRunning()) ||
//           (iStub == 3 && _wav_gen[3]->isRunning())) {  // loop _stub case
//         if (_wav_gen[iStub]->isRunning()) {
//           if (!_wav_gen[iStub]->loop()) {
//             stopAudio(iStub);
//             playAudio(iStub, _volume[iStub]);
//           }
//         }
//       } else {
//         if (_wav_gen[iStub]->isRunning()) {
//           if (!_wav_gen[iStub]->loop()) {
//             stopAudio(iStub);
//           }
//         }
//       }
//     }
//   }
// }

 void readAllSoundFiles() {
    File file = LittleFS.open("/0_0_0-2_C_gunshot.mp3");
    if (!file || file.isDirectory()) {
        USBSerial.println("- failed to open file for reading");
        return;
    }
    _audioDataSize[0] = file.size();
    _audioData[0] = new uint8_t[_audioDataSize[0]];
    file.read(_audioData[0], _audioDataSize[0]);
 }

// ファイルの読み込み
// void readAllSoundFiles() {
//   // LittleFSマウント確認
//   if (!LittleFS.begin()) {
//     USBSerial.println("LittleFS Mount Failed");
//     return;
//   }
//   uint8_t isRight;
//   uint8_t cat;
//   uint8_t pos;
//   uint8_t dataID;
//   uint8_t subID;
//   File root = LittleFS.open("/");
//   File _file = root.openNextFile();
//   uint8_t fileIdx = 0;
//   while (_file) {
//     // ファイル名から種別を取得
//     String fileName = _file.name();
//     int underscorePos1 = fileName.indexOf('_');
//     int underscorePos2 = fileName.indexOf('_', underscorePos1 + 1);
//     int hyphenPos1 = fileName.indexOf('-', underscorePos2 + 1);
//     int underscorePos3 = fileName.indexOf('_', hyphenPos1 + 1);

//     cat = fileName.substring(0, underscorePos1).toInt();
//     pos = fileName.substring(underscorePos1 + 1, underscorePos2).toInt();
//     dataID = fileName.substring(underscorePos2 + 1, hyphenPos1).toInt();
//     subID = fileName.substring(hyphenPos1 + 1, underscorePos3).toInt();
//     isRight = fileName[underscorePos3 + 1] == 'R' ? 1 : 0;

//     USBSerial.printf("FILE: %s, %d, %d, %d, %c, isRight: %d\n",
//                      fileName.c_str(), cat, pos, dataID,
//                      fileName[underscorePos3 + 1], isRight);

//     _audioDataSize[fileIdx] = _file.size();
//     _audioData[fileIdx] = new uint8_t[_audioDataSize[fileIdx]];
//     _file.read(_audioData[fileIdx], _audioDataSize[fileIdx]);

//     // 得られたパラメータとaudioDataのインデックスを対応付け
//     _audioDataIndex[cat][pos][dataID][subID][isRight] = fileIdx;
//     fileIdx += 1;
//     _file = root.openNextFile();
//   }
// }

// _stub 0 = 常用、stub 1 = 左右分ける場合の右、stub 2 = ループ用
void initAudioOut(int I2S_BCLK_PIN, int I2S_LRCK_PIN, int I2S_DOUT_PIN) {
  // init audio
  _src_prog = new AudioFileSourcePROGMEM();
  _i2s_out = new AudioOutputI2S();
  _i2s_out->SetPinout(I2S_BCLK_PIN, I2S_LRCK_PIN, I2S_DOUT_PIN);
  for (int i = 0; i < STUB_NUM; i++) {
    // _wav_gen[i] = new AudioGeneratorWAV();
    _wav_gen[i] = new AudioGeneratorMP3();
  }
  // _mixer に１回渡したら、あとは _stub => _mixer の順で色々渡す
  _mixer = new AudioOutputMixer(32, _i2s_out);

  for (int i = 0; i < STUB_NUM; i++) {
    _stub[i] = _mixer->NewInput();
    _stub[i]->SetRate(samplingRate);
  }

#ifdef GENERAL
  // MOSFET直結のD級アンプにI2Sを入れる場合は、始めに再生することで
  playAudio(0, 0);
#endif
}

// get
uint8_t getGain() { return _gainNum; }
uint8_t getPlayCategory() { return _data.playCategory; }
uint8_t getWearerId() { return _data.wearerId; }
uint8_t getDevicePos() { return _devicePos; }
bool getIsFixMode() { return _data.isFixMode; }
//  set
void setPlayCategory(uint8_t value) {
  _data.playCategory = value;
  EEPROM.put(offsetof(Data, playCategory), _data.playCategory);
  EEPROM.commit();
}
void setWearerId(uint8_t value) {
  _data.wearerId = value;
  EEPROM.put(offsetof(Data, wearerId), _data.wearerId);
  EEPROM.commit();
};
void setGain(uint8_t val, uint8_t G_SEL_A = 99, uint8_t G_SEL_B = 99) {
  _gainNum = val;
  USBSerial.printf("_gainNum: ");
  USBSerial.println(val);
#ifndef GENERAL
  // using RS2255XN and MAX98357
  switch (val) {
    case 0:  // 6dB
      digitalWrite(G_SEL_B, LOW);
      digitalWrite(G_SEL_A, LOW);
      break;
    case 1:  // 9dB
      digitalWrite(G_SEL_B, LOW);
      digitalWrite(G_SEL_A, HIGH);
      break;
    case 2:  // 12dB
      digitalWrite(G_SEL_B, HIGH);
      digitalWrite(G_SEL_A, LOW);
      break;
    case 3:  // 15dB
      digitalWrite(G_SEL_B, HIGH);
      digitalWrite(G_SEL_A, HIGH);
      break;
    default:
      break;
  }
#endif
}
void setDevicePos(uint8_t value) { _devicePos = value; };
void setIsFixMode(bool value) {
  _data.isFixMode = value;
  EEPROM.put(offsetof(Data, isFixMode), _data.isFixMode);
  EEPROM.commit();
};
}  // namespace audioManager
