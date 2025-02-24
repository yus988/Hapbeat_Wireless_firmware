import subprocess
import serial.tools.list_ports
import os
import time

# VSCodeのUIで選択中の環境を取得（デフォルトは "DuoWL_V3-ESPNOW"）
TARGET_ENV = os.getenv('PIOENV', 'DuoWL_V3-ESPNOW')

# ESP32のポートを検出する関数
def find_esp32_ports():
    ports = list(serial.tools.list_ports.comports())
    esp32_ports = []

    for port in ports:
        if "USB" in port.description or "CP210" in port.description or "CH910" in port.description:
            esp32_ports.append(port.device)
    
    return esp32_ports

# 各デバイスにプログラムとファイルシステムを順次アップロードする関数
def upload_program_and_fs():
    devices = find_esp32_ports()

    if not devices:
        print("ESP32デバイスが見つかりませんでした。")
        return

    print(f"{len(devices)}台のESP32デバイスが見つかりました。")

    for device in devices:
        print(f"\n{device} に {TARGET_ENV} 環境でファームウェアとファイルシステムをアップロードします...")

        # 1️⃣ ファームウェア（プログラム）のアップロード
        try:
            result_fw = subprocess.run(["pio", "run", "-e", TARGET_ENV, "-t", "upload", "--upload-port", device], check=True)
            print(f"{device} へのファームウェアアップロードが完了しました。")
        except subprocess.CalledProcessError:
            print(f"{device} へのファームウェアアップロードに失敗しました。")
            continue  # ファームウェアが失敗した場合、次のデバイスへ

        time.sleep(2)  # デバイスのリセット待ち

        # 2️⃣ ファイルシステム（LittleFS/SPIFFS）のアップロード
        try:
            result_fs = subprocess.run(["pio", "run", "-e", TARGET_ENV, "-t", "uploadfs", "--upload-port", device], check=True)
            print(f"{device} へのファイルシステムアップロードが完了しました。")
        except subprocess.CalledProcessError:
            print(f"{device} へのファイルシステムアップロードに失敗しました。")

        time.sleep(2)  # 次のデバイスへの切り替え待機

if __name__ == "__main__":
    upload_program_and_fs()
