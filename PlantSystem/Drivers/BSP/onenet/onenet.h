#ifndef _ONENET_H_
#define _ONENET_H_

#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdarg.h"
#include "delay.h"

#define BYTE0(dwTemp)       (*(char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))

extern char MQTT_ClientID[100];
extern char MQTT_UserName[100];
extern char MQTT_PassWord[200];

typedef struct
{
    uint8_t topic[512];
    uint16_t topic_len;
    uint8_t payload[512];
    uint16_t payload_len;
} Mqtt_RxData_Type;

#define PRODUCT_KEY "8AV8W3KkCb"
#define DEVICE_NAME "plant"
#define MQTT_TOKEN_EXPIRE_TIME "2017881776"
#define MQTT_TOKEN_SIGN "N9W%2BX9i8pHM4PCyJ%2BBr%2Fy9Dw%2Bms%3D"

#define RELY_PUBLISH_TOPIC "$sys/" PRODUCT_KEY "/" DEVICE_NAME "/thing/property/set_reply"
#define SET_TOPIC "$sys/" PRODUCT_KEY "/" DEVICE_NAME "/thing/property/set"
#define POST_TOPIC "$sys/" PRODUCT_KEY "/" DEVICE_NAME "/thing/property/post"
#define EVENT_PUBLISH_TOPIC "$sys/" PRODUCT_KEY "/" DEVICE_NAME "/thing/event/post"

void mqtt_login_init(char *ProductKey, char *DeviceName, char *TokenSign);
uint8_t mqtt_publish_data(char *topic, char *message, uint8_t qos);
uint8_t mqtt_subscribe_topic(char *topic, uint8_t qos, uint8_t whether);
void mqtt_init(void);
uint8_t mqtt_connect(char *ClientID, char *Username, char *Password);
void mqtt_send_heart(void);
void mqtt_disconnect(void);
void mqtt_send_data(uint8_t *buf, uint16_t len);
void mqtt_send_response(uint8_t *id);
uint8_t mqtt_receive_handle(uint8_t *data_received, Mqtt_RxData_Type *rx_data);

#endif
