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
const int volumeThreshold = 100;
const char *_playCategoryTxt[] = {"ゲーム", "動画", "hoge"};
const char *_wearerIdTxt[] = {"ALL", "競技", "会場", "4", "5", "6"};
const CRGB _colorFixMode = CRGB(10, 10, 10);
const CRGB _colorVolumeMode = CRGB(0, 0, 10);
const CRGB _colorDangerMode = CRGB(10, 0, 0);

// シャットダウン回避関連
// 新バッテリー用（最大3.8A程度）
// const float current_thresholds[2] = {5.0, 5.5};  // 電流値の閾値 (A)
// const int shutdownCycles[2] = {20, 4}; // シャットダウンサイクル数。大きいほど、長時間閾値越えを許容する。閾値に応じて変える
// const int restoreCycles = 100;  // 復帰サイクル数
// 旧バッテリー用（最大 4.1A 程度）
const float current_thresholds[2] = {5.2, 5.5};  // 電流値の閾値 (A)
const int shutdownCycles[2] = {200, 40}; // シャットダウンサイクル数。大きいほど、長時間閾値越えを許容する。閾値に応じて変える
const int restoreCycles = 100;  // 復帰サイクル数

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