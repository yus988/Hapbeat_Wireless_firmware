// 頻繁に調整したい変数はこちらで定義

#ifndef ADJ_PARAMS_H
#define ADJ_PARAMS_H

#include <Adafruit_SSD1306.h>

#if defined(NECKLACE)
const int _fixGainStep = 41;
const char *_playCategoryTxt[] = {"ゲーム", "動画", "hoge"};
const char *_wearerIdTxt[] = {"ALL", "競技", "会場", "4", "5", "6"};
#endif

#if defined(NECKLACE_V_1_3)
const int _fixGainStep = 41;
  #define VOLUME_THRESHOLD 100
const char *_playCategoryTxt[] = {"ゲーム", "動画", "hoge"};
const char *_wearerIdTxt[] = {"ALL", "競技", "会場", "4", "5", "6"};
const CRGB _colorFixMode = CRGB(10, 10, 10);
const CRGB _colorVolumeMode = CRGB(0, 0, 10);
const CRGB _colorDangerMode = CRGB(10, 0, 0);

#endif

#if defined(GENERAL_V2)
const int _fixGainStep = 41;
const char *_playCategoryTxt[] = {"ゲーム", "動画", "hoge"};
const char *_wearerIdTxt[] = {"ALL", "競技", "会場", "4", "5", "6"};
const CRGB _colorFixMode = CRGB(10, 10, 10);
const CRGB _colorVolumeMode = CRGB(0, 0, 10);
const CRGB _colorDangerMode = CRGB(10, 0, 0);
#endif

#endif