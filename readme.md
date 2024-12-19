
# Hapbeat Wireless Firmware
このリポジトリは、無線版 Hapbeat（Duo/Band共通）のファームウェアです。本プロジェクトは、ESP32-S3をベースにしており、ESP-NOWやMQTTなどの通信プロトコルを活用した、デバイス間通信やインターネット接続機能を提供します。また、LittleFSを使用したファイルシステム管理や複数のセンサーデバイスへの対応も可能です。


python -m venv .venv
.venv\Scripts\activate
pip install scons


## 使用ライブラリ

このプロジェクトは以下のライブラリを使用しています。各ライブラリはそれぞれのライセンスに基づいて提供されています。

- **Adafruit SSD1306** (BSDライセンス)  
  <https://github.com/adafruit/Adafruit_SSD1306>
  
- **efont Unicode Font Data** (Apache License 2.0)  
  <https://github.com/tanakamasayuki/efont-unicode-font-data>
  
- **ESP8266Audio** (LGPL v2.1)  
  <https://github.com/earlephilhower/ESP8266Audio>
  
- **FastLED** (MITライセンス)  
  <https://github.com/FastLED/FastLED>
  
- **MQTT (256dpi)** (MITライセンス)  
  <https://github.com/256dpi/arduino-mqtt>
  
- **Soldered MCP4018 Arduino Library** (MITライセンス)  
  <https://github.com/SolderedElectronics/MCP4018-Arduino-Library>


## ライセンス情報

このプロジェクトで使用されるライブラリのライセンス情報は `LICENSES` フォルダに含まれています。詳細は各ライセンスファイルを参照してください。
