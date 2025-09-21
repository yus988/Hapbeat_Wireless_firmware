#ifndef TASK_MQTT_H
#define TASK_MQTT_H

void TaskUI_MQTT(void *args);
void showStatusText(const char *status);
void MQTTcallback(char *topic, byte *payload, unsigned int length);

#endif  // TASK_MQTT_H


