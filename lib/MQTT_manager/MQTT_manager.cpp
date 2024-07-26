#include "MQTT_manager.h"

namespace MQTT_manager {
bool mqttConnected = false;
WiFiClientSecure espClient;
MQTTClient client;
int QoS_Val = 1; // 0, 1, 2
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
  while (!client.connected()) {
    String clientId = getUniqueClientId();
    USBSerial.print("Attempting MQTT connection with client ID: ");
    USBSerial.println(clientId);
    if (statusCallback) {
      statusCallback("Attempting MQTT connection...");
    }

    // Set the last parameter to false to use clean session
    if (client.connect(clientId.c_str(), MQTT_USERNAME, MQTT_PASSWORD)) {
      USBSerial.println("connected");
      mqttConnected = true;
      if (statusCallback) {
        statusCallback("connected success");
      }
      client.subscribe(MQTT_TOPIC, QoS_Val);
      USBSerial.print("Subscribed to topic: ");
      USBSerial.println(MQTT_TOPIC);
    } else {
      USBSerial.print("failed, rc=");
      USBSerial.println(client.lastError());
      USBSerial.println(" try again in 5 seconds");
      mqttConnected = false;
      if (statusCallback) {
        statusCallback("connect failed");
      }
      delay(5000);
    }
  }
}

void initMQTTclient(void (*callback)(char*, byte*, unsigned int),
                    void (*statusCb)(const char*)) {
  mqttCallback = callback;
  statusCallback = statusCb;

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    USBSerial.println("Connecting to WiFi...");
  }
  USBSerial.println("Connected to WiFi");
  espClient.setCACert(ca_cert);

  client.begin(MQTT_SERVER, MQTT_PORT, espClient);
  client.setCleanSession(true); // false で新しいセッションとして接続
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

}  // namespace MQTT_manager
