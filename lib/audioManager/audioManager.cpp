
#include "audioManager.h"
#include <LittleFS.h>
#include <string>
#include <vector>

#include "FS.h"
// オーディオ再生関連 ESP8266
#include "AudioFileSourcePROGMEM.h"
#include "AudioGeneratorMP3.h"
// EEPROM
#include <EEPROM.h>
// audio libraries
#include <AudioGeneratorMP3.h>

#include "AudioFileSourceLittleFS.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2S.h"
#include "AudioOutputMixer.h"

namespace audioManager {

struct MessageData {
  uint8_t id;
  const char *message;
  MessageData(uint8_t i, const char *m) : id(i), message(m) {}
};
std::vector<MessageData> messages;  // 実際のインスタンス化

std::vector<bool> isPlayAudio(STUB_NUM, false);
// ディスプレイに表示するコールバック関数
void (*statusCallback)(const char *);

struct DataPacket {
  uint8_t category;
  uint8_t wearerId;
  uint8_t devicePos;
  uint8_t dataID;
  uint8_t subID;
  uint8_t lVol;
  uint8_t rVol;
  uint8_t playCmd;
};

int maxVol = 255;
// position identifier
// 0:neck, 1:chest, 2:abdomen
// 3:upperArm_L, 4:upperArm_R, 5:wrist_L, 6:wrist_R,
// 7:thigh_L, 8:thigh_R, 9:calf_L, 10:calf_R
uint8_t _devicePos;  // 装着場所の指定
uint8_t _gainNum;    //
// EEPROM内に保存するデバイスコンフィグを格納
struct ConfigData {
  uint8_t playCategory;
  uint8_t wearerId;  // 装着者のID。99でブロードキャスト
  bool isFixMode;
  bool isLimitEnable;
};
ConfigData _settings;

// MAX98357 gain select
uint8_t sel_A_pin;
uint8_t sel_B_pin;

AudioGeneratorWAV *_wav_gen[STUB_NUM];
AudioFileSourceLittleFS *_audioFileSrc[SOUND_FILE_NUM];
String _audioFileNames[SOUND_FILE_NUM];
int16_t *_audioRAM[SOUND_FILE_NUM];  // RAMに格納するデータのポインタ
uint8_t _audioStorageType[SOUND_FILE_NUM];  // データの格納場所を示すフラグ
AudioOutputI2S *_i2s_out;
AudioOutputMixer *_mixer;
AudioOutputMixerStub *_stub[STUB_NUM];

// グローバル変数またはクラスメンバとして以前のオーディオソースを保持する配列を宣言
AudioFileSource *_previousSources[STUB_NUM] = {nullptr};

// [cat][pos][dataID][_subID][isRight]
uint8_t _audioDataIndex[CATEGORY_NUM][POSITION_NUM][DATA_NUM][SUB_DATA_NUM][2];
size_t _audioDataSize[SOUND_FILE_NUM];
uint8_t _dataID[STUB_NUM];
uint8_t _subID[STUB_NUM];
uint8_t _volume[STUB_NUM];

// モード選択の対象にするIDを格納
int _limitIDs[10];        // 最大10個のIDを格納する配列
size_t _numLimitIDs = 0;  // 現在格納されているIDの数

void initParamsEEPROM() {
  size_t eepromSize = sizeof(ConfigData);
  EEPROM.begin(eepromSize);
  EEPROM.get(0, _settings);
  if (_settings.wearerId == 0xFF || _settings.playCategory == 0xFF ||
      _settings.isFixMode == 0xFF) {
    _settings.playCategory = 0;
    _settings.wearerId = 0;
    _settings.isFixMode = true;
    _settings.isLimitEnable = false;
  }
}

// ファイルの読み込み
void readAllSoundFiles() {
  // LittleFSマウント確認
  if (!LittleFS.begin()) {
    USBSerial.println("LittleFS Mount Failed");
    return;
  }
  uint8_t isRight;
  uint8_t cat;
  uint8_t pos;
  uint8_t dataID;
  uint8_t subID;
  File root = LittleFS.open("/");
  File _file = root.openNextFile();

  uint8_t fileIdx = 0;

  while (_file) {
    // ファイル名から種別を取得
    String fileName = _file.name();
    int underscorePos1 = fileName.indexOf('_');
    int underscorePos2 = fileName.indexOf('_', underscorePos1 + 1);
    int hyphenPos1 = fileName.indexOf('-', underscorePos2 + 1);
    int underscorePos3 = fileName.indexOf('_', hyphenPos1 + 1);
    int underscorePos4 = fileName.indexOf('_', underscorePos3 + 1);
    int underscorePos5 = fileName.indexOf('_', underscorePos4 + 1);

    cat = fileName.substring(0, underscorePos1).toInt();
    pos = fileName.substring(underscorePos1 + 1, underscorePos2).toInt();
    dataID = fileName.substring(underscorePos2 + 1, hyphenPos1).toInt();
    subID = fileName.substring(hyphenPos1 + 1, underscorePos3).toInt();
    isRight = fileName[underscorePos3 + 1] == 'R' ? 1 : 0;
    String storageType = fileName.substring(underscorePos4 + 1, underscorePos5);

    USBSerial.printf("FILE: %s, %d, %d, %d, %c, isRight: %d, Storage: %s\n",
                     fileName.c_str(), cat, pos, dataID,
                     fileName[underscorePos3 + 1], isRight,
                     storageType.c_str());

    _audioDataSize[fileIdx] = _file.size();

    // データの格納場所を決定
    if (storageType == "RAM") {
      _audioRAM[fileIdx] = new int16_t[_audioDataSize[fileIdx] /
                                       2];  // 16ビット（2バイト）ごとのデータ
      _file.read(reinterpret_cast<uint8_t *>(_audioRAM[fileIdx]),
                 _audioDataSize[fileIdx]);
      _audioStorageType[fileIdx] = RAM_STORAGE;
    } else {
      // ファイル名のみ格納
      String fullPath = "/" + fileName;
      _audioFileNames[fileIdx] = fullPath;
      _audioStorageType[fileIdx] = FS_STORAGE;
    }

    _audioDataIndex[cat][pos][dataID][subID][isRight] = fileIdx;
    fileIdx++;
    _file = root.openNextFile();
  }
}

// 引数無しの場合は全てのstubを停止
void stopAudio(uint8_t stub) {
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
  int isLR = (tStubNum % 2 == 0) ? 0 : 1;
  uint8_t pos = 0;  // pos = 0 は仮置き
  uint8_t idx = _audioDataIndex[_settings.playCategory][pos][_dataID[tStubNum]]
                               [_subID[tStubNum]][isLR];
  _volume[tStubNum] = tVol;

  _stub[tStubNum]->SetGain((float)tVol / maxVol);
  if (_wav_gen[tStubNum]->isRunning()) {
    _wav_gen[tStubNum]->stop();
  }

  // 以前のオーディオソースが存在すれば削除（ヒープメモリ解放のため必須）
  if (_previousSources[tStubNum] != nullptr) {
    delete _previousSources[tStubNum];
    _previousSources[tStubNum] = nullptr;
  }

  AudioFileSource *src = nullptr;
  if (_audioStorageType[idx] == RAM_STORAGE) {
    src = new AudioFileSourcePROGMEM(_audioRAM[idx], _audioDataSize[idx]);
    if (!_wav_gen[tStubNum]->begin(src, _stub[tStubNum])) {
      USBSerial.println("Failed to start WAV generator with RAM data");
      delete src;
      return;
    }
  } else {
    src = new AudioFileSourceLittleFS(_audioFileNames[idx].c_str());
    if (!src->isOpen()) {
      USBSerial.printf("Failed to open file: %s\n",
                       _audioFileNames[idx].c_str());
      delete src;
      return;
    }
    if (!_wav_gen[tStubNum]->begin(src, _stub[tStubNum])) {
      USBSerial.println("Failed to start WAV generator with FS data");
      delete src;
      return;
    }
  }
  // 新しいオーディオソースを保存
  _previousSources[tStubNum] = src;
  isPlayAudio[tStubNum] = true;
  // USBSerial.printf("Succeed to play with stub: %d\n", tStubNum);
}

void PlaySndOnDataRecv(const uint8_t *mac_addr, const uint8_t *data,
                       int data_len) {
  USBSerial.printf(
      "received: _category %d, _wearerId %d, _devPos %d, _dataID %d, _subID "
      "%d, Vol %d:%d, playtype %d\n",
      data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
  // USBSerial.print("Free heap: ");
  // USBSerial.println(ESP.getFreeHeap());
  // data = [_category, _wearerId, _devicePos, data_id, _subID, _L_Vol,
  // _R_Vol, playCmd] 各種条件が合致した時のみ値を保持
  if ((data[0] == _settings.playCategory || data[0] == 99) &&
      (data[1] == _settings.wearerId || data[1] == 99) &&
      (data[2] == _devicePos || data[2] == 99)) {
    USBSerial.println("prepare to playAudio");
    // 0 = oneshot(0,1), 1=loopStart(2,3), 2=stopAudio, 3=2ndline(4,5)
    // 括弧内はstub番号
    uint8_t playCmd = data[7];

    if (playCmd == 2) {
      for (int iStub = 2; iStub <= 3; ++iStub) {
        while (_wav_gen[iStub]->isRunning()) {
          stopAudio(iStub);
          delay(10);
        }
      }
      return;
    } else {
      int startIdx;
      // 再生するstubを選択
      switch (playCmd) {
        case 0:
          startIdx = 0;
          break;
        case 1:
          startIdx = 2;
          break;
        case 3:
          startIdx = 4;
      }
      // 再生するデータを選択
      for (int i = startIdx; i <= startIdx + 1; ++i) {
        _dataID[i] = data[3];
        _subID[i] = data[4];
        _volume[i] = data[5 + i % 2];
      }
      uint8_t stub = startIdx;
      stopAudio(stub);
      playAudio(stub, _volume[stub]);
      // ステレオ再生を左右のボリュームから判断
      if (_volume[stub] != _volume[stub + 1]) {
        stopAudio(stub + 1);
        playAudio(stub + 1, _volume[stub + 1]);
      }
    }
  }
}

// MQTT接続にESPNOWのコールバックを実行するためのリレー関数
void PlaySndFromMQTTcallback(char *topic, byte *payload, unsigned int length) {
  USBSerial.println();
  USBSerial.print("Message arrived in topic: ");
  USBSerial.println(topic);

  USBSerial.print("Message: ");
  for (int i = 0; i < length; i++) {
    USBSerial.print((char)payload[i]);
  }
  USBSerial.println();

  // 文字列を整数に変換する処理
  String payloadStr((char *)payload, length);
  int values[8];
  int i = 0;
  int startIndex = 0;

  for (int j = 0; j < payloadStr.length(); j++) {
    if (payloadStr[j] == ',' || j == payloadStr.length() - 1) {
      if (j == payloadStr.length() - 1) j++;
      String valueStr = payloadStr.substring(startIndex, j);
      values[i++] = valueStr.toInt();
      startIndex = j + 1;
    }
  }

  // データをESP-NOW形式に変換
  DataPacket dataPacket;
  dataPacket.category = values[0];
  dataPacket.wearerId = values[1];
  dataPacket.devicePos = values[2];
  dataPacket.dataID = values[3];
  dataPacket.subID = values[4];
  dataPacket.lVol = values[5];
  dataPacket.rVol = values[6];
  dataPacket.playCmd = values[7];

  // 制限されたIDがあるか確認
  for (size_t i = 0; i < _numLimitIDs; ++i) {
    if (_settings.isLimitEnable == true && _limitIDs[i] == dataPacket.dataID) {
      USBSerial.println("ID is restricted, aborting action.");
      return;  // 制限されたIDが見つかった場合、ここで処理を中断
    }
  }

  // dataPacket.dataIDに対応するMessageDataオブジェクトを検索
  for (int i = 0; i < messages.size(); ++i) {
    if (messages[i].id == dataPacket.dataID) {
      statusCallback(messages[i].message);
      break;  // 対応するメッセージを見つけたらループを抜ける
    }
  }

  // ダミーのMACアドレス（適宜設定）
  uint8_t dummy_mac_addr[6] = {0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x00};

  // ESP-NOWのコールバック関数を呼び出す
  PlaySndOnDataRecv(dummy_mac_addr, (uint8_t *)&dataPacket, sizeof(dataPacket));
}

void playAudioInLoop() {
  for (int iStub = 0; iStub < STUB_NUM; iStub++) {
    if (isPlayAudio[iStub]) {
      // USBSerial.printf("playing stub: ");
      // USBSerial.println(iStub);
      if ((iStub == 2 && _wav_gen[2]->isRunning()) ||
          (iStub == 3 && _wav_gen[3]->isRunning())) {  // loop _stub case
        if (_wav_gen[iStub]->isRunning()) {
          if (!_wav_gen[iStub]->loop()) {
            // stopAudio(iStub);
            playAudio(iStub, _volume[iStub]);
          }
        }
      } else {
        if (_wav_gen[iStub]->isRunning()) {
          if (!_wav_gen[iStub]->loop()) {
            stopAudio(iStub);
          }
        }
      }
    }
  }
}

// _stub 0 = 常用、stub 1 = 左右分ける場合の右、stub 2 = ループ用
void initAudioOut(int I2S_BCLK_PIN, int I2S_LRCK_PIN, int I2S_DOUT_PIN) {
  // init audio
  _i2s_out = new AudioOutputI2S();
  _i2s_out->SetPinout(I2S_BCLK_PIN, I2S_LRCK_PIN, I2S_DOUT_PIN);
  _i2s_out->SetGain(1.0);  // 出力ゲインを設定
  for (int i = 0; i < STUB_NUM; i++) {
    _wav_gen[i] = new AudioGeneratorWAV();
  }
  // _mixer に１回渡したら、あとは _stub => _mixer の順で色々渡す
  _mixer = new AudioOutputMixer(32, _i2s_out);

  for (int i = 0; i < STUB_NUM; i++) {
    _stub[i] = _mixer->NewInput();
    _stub[i]->SetRate(SAMPLING_RATE);
  }
}

// get
uint8_t getGain() { return _gainNum; }
uint8_t getPlayCategory() { return _settings.playCategory; }
uint8_t getWearerId() { return _settings.wearerId; }
uint8_t getDevicePos() { return _devicePos; }
bool getIsFixMode() { return _settings.isFixMode; }
bool getIsLimitEnable() { return _settings.isLimitEnable; }
bool getIsPlaying() {
  for (int i = 0; i < STUB_NUM; i++) {
    if (isPlayAudio[i]) {
      return true;  // 1つでもtrueがあればtrueを返す
    }
  }
  return false;  // 全てfalseの場合はfalseを返す
}

//  set
// stubNum: 0 = oneshot, 2 = loop
void setDataID(uint8_t stubNum, uint8_t dataID, uint8_t subID) {
  _dataID[stubNum] = dataID;
  _subID[stubNum] = subID;
}

void setPlayCategory(uint8_t value) {
  _settings.playCategory = value;
  EEPROM.put(offsetof(ConfigData, playCategory), _settings.playCategory);
  EEPROM.commit();
}
void setWearerId(uint8_t value) {
  _settings.wearerId = value;
  EEPROM.put(offsetof(ConfigData, wearerId), _settings.wearerId);
  EEPROM.commit();
};
void setGain(uint8_t val, uint8_t G_SEL_A = 99, uint8_t G_SEL_B = 99) {
  _gainNum = val;
  USBSerial.printf("_gainNum: ");
  USBSerial.println(val);
}
void setDevicePos(uint8_t value) { _devicePos = value; };
void setIsFixMode(bool value) {
  _settings.isFixMode = value;
  EEPROM.put(offsetof(ConfigData, isFixMode), _settings.isFixMode);
  EEPROM.commit();
};
// 有効/無効に切り替えるIDを設定
void setLimitIds(const int limitIDs[], size_t size) {
  _numLimitIDs = (size > 10) ? 10 : size;  // 最大数を超えないように制限
  for (size_t i = 0; i < _numLimitIDs; ++i) {
    _limitIDs[i] = limitIDs[i];
  }
}
void setIsLimitEnable(bool value) {
  _settings.isLimitEnable = value;
  EEPROM.put(offsetof(ConfigData, isLimitEnable), _settings.isLimitEnable);
  EEPROM.commit();
};

void setMessageData(const char *msg, uint8_t id) {
  messages.push_back(
      MessageData(id, msg));  // MessageData のコンストラクタを明示的に呼び出す
}
void setStatusCallback(void (*statusCb)(const char *)) {
  statusCallback = statusCb;
}

}  // namespace audioManager
