#include "MQTT_manager.h"
#include "WiFi.h"

namespace MQTT_manager {


int attemptTimes = 5;
bool mqttConnected = false;

#ifdef INTERNET
WiFiClientSecure espClient;  // インターネット用の安全なクライアント
const char* ca_cert = MQTT_CERTIFICATE;
#elif LOCAL
WiFiClient espClient;        // ローカル接続用の一般的なクライアント
#endif

MQTTClient client;
int QoS_Val = 1;  // 0, 1, 2
const char* clientIdPrefix = "Hapbeat_esp32_client-";
void (*mqttCallback)(char*, byte*, unsigned int);
void (*statusCallback)(const char*);

void messageReceived(String& topic, String& payload) {
  if (mqttCallback) {
    mqttCallback((char*)topic.c_str(), (byte*)payload.c_str(), payload.length());
  }
}

String getUniqueClientId() {
  String clientId = clientIdPrefix;
  clientId += String(WiFi.macAddress());
  return clientId;
}

void reconnect() {
  int attemptCount = 0;
  while (!client.connected()) {
    String clientId = getUniqueClientId();
    String connectionAttemptMsg = "Connecting to \nMQTT trial: " + String(attemptCount);
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
    if (attemptCount >= attemptTimes) {
      statusCallback("connection failed");
      delay(3000);
      return;
    }
    attemptCount++;
  }
}

void initMQTTclient(void (*callback)(char*, byte*, unsigned int),
                    void (*statusCb)(const char*)) {
  mqttCallback = callback;
  statusCallback = statusCb;

  #ifdef LOCAL
  if (!WiFi.config(LOCAL_IP, GATEWAY, SUBNET)) {
      Serial.printf("Failed to configure static IP");
  }
  #endif

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int attemptCount = 0;

  while (WiFi.status() != WL_CONNECTED) {
    String message = "Connecting to \nWiFi trial: " + String(attemptCount);
    statusCallback(message.c_str());
    delay(1000);
    if (attemptCount >= 10) {
      statusCallback("connection failed");
      break;
    }
    attemptCount++;
  }
  statusCallback("WiFi connected!");

  WiFi.setSleep(true);

#ifdef INTERNET
  espClient.setCACert(ca_cert);  // インターネット接続の場合のみ証明書を設定
#endif

  client.begin(MQTT_SERVER, MQTT_PORT, espClient);
  client.setCleanSession(true);
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

bool getIsWiFiConnected() { return WiFi.status() == WL_CONNECTED; }

}  // namespace MQTT_manager
