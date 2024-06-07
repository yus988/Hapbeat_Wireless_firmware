#include "MQTT_manager.h"

namespace MQTT_manager {
WiFiClient espClient;
PubSubClient client(espClient);
const char* clientIdPrefix = "Hapbeat_esp32_client-";

// コールバック関数
void callback(char* topic, byte* payload, unsigned int length) {
  USBSerial.print("Message arrived in topic: ");
  USBSerial.println(topic);

  USBSerial.print("Message: ");
  for (int i = 0; i < length; i++) {
    USBSerial.print((char)payload[i]);
  }
  USBSerial.println();
}

// ユニークなクライアントIDを生成する関数
String getUniqueClientId() {
  String clientId = clientIdPrefix;
  clientId += String(WiFi.macAddress());
  return clientId;
}

// MQTTブローカーへの接続関数
void reconnect() {
  // 再接続の試行をループ
  while (!client.connected()) {
    String clientId = getUniqueClientId();
    USBSerial.print("Attempting MQTT connection with client ID: ");
    USBSerial.println(clientId);

    if (client.connect(clientId.c_str())) {
      USBSerial.println("connected");
      // サブスクリプションを再設定
      if (client.subscribe("test/hapbeat")) {
        USBSerial.println("Subscribed to topic: test/hapbeat");
      } else {
        USBSerial.println("Subscription failed");
      }
    } else {
      USBSerial.print("failed, rc=");
      USBSerial.print(client.state());
      USBSerial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void initMQTTclient(void (*callback)(char*, byte*, unsigned int)) {
  WiFi.begin("haselab_pi_G", "2human2human2");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    USBSerial.println("Connecting to WiFi...");
  }
  USBSerial.println("Connected to WiFi");
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);
  reconnect();
}

void loopMQTTclient() {
  // 接続状態の確認と再接続
  if (!client.connected()) {
    reconnect();
  } else {
    // メッセージの処理と接続の維持
    client.loop();
  }
}

}  // namespace MQTT_manager
