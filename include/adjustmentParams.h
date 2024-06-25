// 頻繁に調整したい変数はこちらで定義

#ifndef ADJ_PARAMS_H
#define ADJ_PARAMS_H

#include <Adafruit_SSD1306.h>

#if defined(NECKLACE)
const int _fixGainStep = 41;
const char *_playCategoryTxt[] = {"ゲーム", "動画", "hoge"};
const char *_wearerIdTxt[] = {"ALL", "競技", "会場", "4", "5", "6"};
const char *_decibelTxt[] = {
    "-75",  "-40",  "-34",  "-28",  "-22",  "-16",  "-10",  "-7.5",
    "-5",   "-2.5", "0",    "1.5",  "3.0",  "4.0",  "4.4",  "4.8",
    "5.2",  "5.6",  "6.0",  "6.4",  "6.8",  "7.2",  "7.6",  "8.0",
    "8.4",  "8.8",  "9.2",  "9.6",  "10.0", "10.4", "10.8", "11.2",
    "11.6", "12.0", "12.4", "12.8", "13.2", "13.6", "14.0", "14.4",
    "14.8", "15.2", "15.6", "16.0", "16.4", "16.8", "17.2", "17.6",
    "18.0", "18.4", "18.8", "19.2", "19.6", "20.0", "20.4", "20.8",
    "21.2", "21.6", "22.0", "22.4", "22.8", "23.2", "23.6", "24.0"};
#endif

#if defined(NECKLACE_V_1_3)
const int volumeThreshold = 100;
const char *_playCategoryTxt[] = {"ゲーム", "動画", "hoge"};
const int _fixGainStep[] = {41, 28, 0};  // 各 playCategory に対応
const char *_wearerIdTxt[] = {"ALL", "競技", "会場", "4", "5", "6"};
const CRGB _colorFixMode = CRGB(10, 10, 10);
const CRGB _colorVolumeMode = CRGB(0, 0, 10);
const CRGB _colorDangerMode = CRGB(10, 0, 0);
const char *_decibelTxt[] = {
    "-75",  "-40",  "-34",  "-28",  "-22",  "-16",  "-10",  "-7.5",
    "-5",   "-2.5", "0",    "1.5",  "3.0",  "4.0",  "4.4",  "4.8",
    "5.2",  "5.6",  "6.0",  "6.4",  "6.8",  "7.2",  "7.6",  "8.0",
    "8.4",  "8.8",  "9.2",  "9.6",  "10.0", "10.4", "10.8", "11.2",
    "11.6", "12.0", "12.4", "12.8", "13.2", "13.6", "14.0", "14.4",
    "14.8", "15.2", "15.6", "16.0", "16.4", "16.8", "17.2", "17.6",
    "18.0", "18.4", "18.8", "19.2", "19.6", "20.0", "20.4", "20.8",
    "21.2", "21.6", "22.0", "22.4", "22.8", "23.2", "23.6", "24.0"};
// シャットダウン回避関連
// 新バッテリー用（最大3.8A程度）
// const float current_thresholds[2] = {5.0, 5.5};  // 電流値の閾値 (A)
// const int shutdownCycles[2] = {20, 4};           //
// シャットダウンサイクル数。大きいほど、長時間閾値越えを許容する。閾値に応じて変える
// 旧バッテリー用（最大 4.1A 程度）
const float current_thresholds[2] = {5.2, 5.5};  // 電流値の閾値 (A)
const int shutdownCycles[2] = {100, 40};
// シャットダウンサイクル数。大きいほど、長時間閾値越えを許容する。閾値に応じて変える

const int restoreCycles = 100;  // 復帰サイクル数

#endif

#if defined(GENERAL_V2)
const int _fixGainStep[] = {41, 28, 0};  // 各 playCategory に対応
const char *_playCategoryTxt[] = {"ゲーム", "動画", "hoge"};
const char *_wearerIdTxt[] = {"ALL", "競技", "会場", "4", "5", "6"};
const CRGB _colorFixMode = CRGB(10, 10, 10);
const CRGB _colorVolumeMode = CRGB(0, 0, 10);
const CRGB _colorDangerMode = CRGB(10, 0, 0);

const char *_decibelTxt[] = {
    "-75",  "-40",  "-34",  "-28",  "-22",  "-16",  "-10",  "-7.5",
    "-5",   "-2.5", "0",    "1.5",  "3.0",  "4.0",  "4.4",  "4.8",
    "5.2",  "5.6",  "6.0",  "6.4",  "6.8",  "7.2",  "7.6",  "8.0",
    "8.4",  "8.8",  "9.2",  "9.6",  "10.0", "10.4", "10.8", "11.2",
    "11.6", "12.0", "12.4", "12.8", "13.2", "13.6", "14.0", "14.4",
    "14.8", "15.2", "15.6", "16.0", "16.4", "16.8", "17.2", "17.6",
    "18.0", "18.4", "18.8", "19.2", "19.6", "20.0", "20.4", "20.8",
    "21.2", "21.6", "22.0", "22.4", "22.8", "23.2", "23.6", "24.0"};
#endif

#endif