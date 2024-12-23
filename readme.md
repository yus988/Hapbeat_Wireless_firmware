# Hapbeat Wireless Firmware

このリポジトリは、無線版 Hapbeat（Duo/Band 共通）のファームウェアです。本プロジェクトは、ESP32-S3 をベースにしており、ESP-NOW や MQTT などの通信プロトコルを活用した、デバイス間通信やインターネット接続機能を提供します。また、LittleFS を使用したファイルシステム管理や複数のセンサーデバイスへの対応も可能です。

## Quick Start (Duo Wireless & ESP-NOW の場合)
※ 事前に [git](https://git-scm.com/downloads), [VSCode](https://azure.microsoft.com/ja-jp/products/visual-studio-code) をインストールし、VSCode の拡張機能から [PlatformIO](https://platformio.org/install/ide?install=vscode) をインストールしてください。

1. リポジトリのクローン or zip ファイルダウンロード
2. Hapbeatを PC に接続し、**BOOTボタンを押下したまま電源をON**にし、認識されているか確認  
（デバイスマネージャーで不明なデバイスと表示されなければ OK。ドライバインストールは不要）
3. VSCode の下部（画像赤枠）env:DuoWL_V3-ESPNOW を選択しビルド&書き込み
   ![alt text](assets/desc_env.png)
4. PlatformIO -> upload file image でwavデータをHapbeatにアップロード  
   ![alt text](desc_fileupload.png)
5. 電源をOFF->BOOTボタンを押下しないで電源をON  
  - UIが表示されているか確認
  - （M5Stack basicの場合）ボタン押下で振動するか確認

## 必要な環境

**対応ハードウェア**:

- [Hapbeat Duo Wireless](https://hapbeat.com/products/duo-wireless/)
- [Hapbeat Band Wireless](https://hapbeat.com/products/band-wireless/)

**ソフトウェア**:

- Windows OS（Mac でも動作するとは思いますがサポート対象外です）
- [PlatformIO](https://platformio.org/) IDE

## 各種パラメータ調整

### 画面 UI の変更
[`src/adjustParams.cpp`](https://github.com/yus988/Hapbeat_Wireless_firmware/blob/main/src/adjustParams.cpp)内の変数の値を用途に応じて調整してください。

### 振動データの差し替え

ファイル名規則

## 送信データ形式説明

コンマ区切りのテキストデータ（例 "0,0,99,0,0,100,100,0"）を受信することで、以下の内容にパースしてデータの可否や再生するwavデータ、音量の選択を行います。  
**{category, channel, position, sound_id, sub_id, volume_L, volume_R, playtype}**  
- category
- channel
- position
  - 上記3つは機能的には同等です。
- sound_id
- sub_id
- volume_L, R
- playtype

### 送信データ

### 対応表

---

---

### 使用ライブラリ

このプロジェクトは以下のライブラリを使用しています。各ライブラリはそれぞれのライセンスに基づいて提供されています。

| ライブラリ名                         | ライセンス         | リンク                                                                   |
| ------------------------------------ | ------------------ | ------------------------------------------------------------------------ |
| **Adafruit SSD1306**                 | BSD ライセンス     | [GitHub](https://github.com/adafruit/Adafruit_SSD1306)                   |
| **efont Unicode Font Data**          | Apache License 2.0 | [GitHub](https://github.com/tanakamasayuki/efont-unicode-font-data)      |
| **ESP8266Audio**                     | LGPL v2.1          | [GitHub](https://github.com/earlephilhower/ESP8266Audio)                 |
| **FastLED**                          | MIT ライセンス     | [GitHub](https://github.com/FastLED/FastLED)                             |
| **MQTT (256dpi)**                    | MIT ライセンス     | [GitHub](https://github.com/256dpi/arduino-mqtt)                         |
| **Soldered MCP4018 Arduino Library** | MIT ライセンス     | [GitHub](https://github.com/SolderedElectronics/MCP4018-Arduino-Library) |

### ライセンス情報

このプロジェクトで使用されるライブラリのライセンス情報は `LICENSES` フォルダに含まれています。詳細は各ライセンスファイルを参照してください。
