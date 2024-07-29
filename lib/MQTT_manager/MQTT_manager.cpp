#include "MQTT_manager.h"

#include "WiFi.h"

namespace MQTT_manager {

int attemptTimes = 5;
bool mqttConnected = false;
WiFiClientSecure espClient;
MQTTClient client;
int QoS_Val = 1;  // 0, 1, 2
const char* clientIdPrefix = "Hapbeat_esp32_client-";
void (*mqttCallback)(char*, byte*, unsigned int);
void (*statusCallback)(const char*);

void messageReceived(String& topic, String& payload) {
  if (mqttCallback) {
    mqttCallback((char*)topic.c_str(), (byte*)payload.c_str(),
                 payload.length());
  }
}

const char* ca_cert = MQTT_CERTIFICATE;

String getUniqueClientId() {
  String clientId = clientIdPrefix;
  clientId += String(WiFi.macAddress());
  return clientId;
}

void reconnect() {
  int attemptCount = 0;  // 試行回数をカウントする変数
  while (!client.connected()) {
    String clientId = getUniqueClientId();
    String connectionAttemptMsg =
        "Connecting to \nMQTT trial: " + String(attemptCount);
    if (statusCallback) {
      statusCallback(connectionAttemptMsg.c_str());
    }

    if (client.connect(clientId.c_str(), MQTT_USERNAME, MQTT_PASSWORD)) {
      mqttConnected = true;
      if (statusCallback) {
        statusCallback("connected \nsuccess");
      }
      client.subscribe(MQTT_TOPIC, QoS_Val);
      String subscribedMsg = "Subscribed topic: " + String(MQTT_TOPIC);
      if (statusCallback) {
        statusCallback(subscribedMsg.c_str());
      }
    } else {
      String failMsg = "failed, rc=" + String(client.lastError());
      String retryMsg = "try again in 1 seconds";
    }
    delay(1000);
    if (attemptCount >= attemptTimes) {  // 10回試行したが接続できなかった場合
      statusCallback("connection failed");
      delay(3000);
      return;  // 接続失敗を報告して関数から抜ける
    }
    attemptCount++;  // 試行回数をインクリメント
  }
}

void initMQTTclient(void (*callback)(char*, byte*, unsigned int),
                    void (*statusCb)(const char*)) {
  mqttCallback = callback;
  statusCallback = statusCb;

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int attemptCount = 0;  // 試行回数をカウントする変数

  while (WiFi.status() != WL_CONNECTED) {
    String message = "Connecting to \nWiFi trial: " + String(attemptCount);
    statusCallback(message.c_str());  //
    // 現在の試行回数を含めてステータスを報告
    // statusCallback("Connecting to WiFi");  //
    // 現在の試行回数を含めてステータスを報告
    delay(1000);
    if (attemptCount >= 10) {  // 10回試行したが接続できなかった場合
      statusCallback("connection failed");
      break;  // 接続失敗を報告して関数から抜ける
    }
    attemptCount++;  // 試行回数をインクリメント
  }
  statusCallback("WiFi connected!");

  // モデムスリープモードを有効に設定
  WiFi.setSleep(true);

  espClient.setCACert(ca_cert);
  client.begin(MQTT_SERVER, MQTT_PORT, espClient);
  client.setCleanSession(true);  // false で新しいセッションとして接続
  client.onMessage(messageReceived);
  reconnect();
}

void loopMQTTclient() {
  if (!client.connected()) {
    reconnect();
  } else {
    client.loop();
  }
}

// wifiの情報を返す
bool getIsWiFiConnected() { return WiFi.status() == WL_CONNECTED; }

}  // namespace MQTT_manager
