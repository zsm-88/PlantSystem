#include "onenet.h"
#include "esp8266.h"

char MQTT_ClientID[100];
char MQTT_UserName[100];
char MQTT_PassWord[200];

uint8_t *mqtt_rxbuf;
uint8_t *mqtt_txbuf;
uint16_t mqtt_rxlen;
uint16_t mqtt_txlen;
uint8_t _mqtt_txbuf[512];
uint8_t _mqtt_rxbuf[512];

typedef enum
{
    M_RESERVED1 = 0,
    M_CONNECT,
    M_CONNACK,
    M_PUBLISH,
    M_PUBACK,
    M_PUBREC,
    M_PUBREL,
    M_PUBCOMP,
    M_SUBSCRIBE,
    M_SUBACK,
    M_UNSUBSCRIBE,
    M_UNSUBACK,
    M_PINGREQ,
    M_PINGRESP,
    M_DISCONNECT,
    M_RESERVED2,
}_typdef_mqtt_message;

const uint8_t parket_connetAck[] = {0x20, 0x02, 0x00, 0x00};
const uint8_t parket_disconnet[] = {0xe0, 0x00};
const uint8_t parket_heart[] = {0xc0, 0x00};
const uint8_t parket_heart_reply[] = {0xc0, 0x00};
const uint8_t parket_subAck[] = {0x90, 0x03};

void mqtt_login_init(char *ProductKey, char *DeviceName, char *TokenSign)
{
    sprintf(MQTT_ClientID, "%s", DeviceName);
    sprintf(MQTT_UserName, "%s", ProductKey);
    sprintf(MQTT_PassWord,
            "version=2018-10-31&res=products%%2F%s%%2Fdevices%%2F%s&et=%s&method=sha1&sign=%s",
            ProductKey, DeviceName, MQTT_TOKEN_EXPIRE_TIME, TokenSign);
}

void mqtt_init(void)
{
    mqtt_login_init(PRODUCT_KEY, DEVICE_NAME, MQTT_TOKEN_SIGN);
    mqtt_rxbuf = _mqtt_rxbuf;
    mqtt_rxlen = sizeof(_mqtt_rxbuf);
    mqtt_txbuf = _mqtt_txbuf;
    mqtt_txlen = sizeof(_mqtt_txbuf);
    memset(mqtt_rxbuf, 0, mqtt_rxlen);
    memset(mqtt_txbuf, 0, mqtt_txlen);

    mqtt_disconnect();
    delay_ms(100);
    mqtt_disconnect();
    delay_ms(100);
}

uint8_t mqtt_connect(char *ClientID, char *Username, char *Password)
{
    uint8_t j;
    int ClientIDLen = strlen(ClientID);
    int UsernameLen = strlen(Username);
    int PasswordLen = strlen(Password);
    int DataLen;
    mqtt_txlen = 0;

    DataLen = 10 + (ClientIDLen + 2) + (UsernameLen + 2) + (PasswordLen + 2);

    mqtt_txbuf[mqtt_txlen++] = 0x10;
    do
    {
        uint8_t encodedByte = DataLen % 128;
        DataLen = DataLen / 128;
        if (DataLen > 0)
            encodedByte = encodedByte | 128;
        mqtt_txbuf[mqtt_txlen++] = encodedByte;
    } while (DataLen > 0);

    mqtt_txbuf[mqtt_txlen++] = 0;
    mqtt_txbuf[mqtt_txlen++] = 4;
    mqtt_txbuf[mqtt_txlen++] = 'M';
    mqtt_txbuf[mqtt_txlen++] = 'Q';
    mqtt_txbuf[mqtt_txlen++] = 'T';
    mqtt_txbuf[mqtt_txlen++] = 'T';
    mqtt_txbuf[mqtt_txlen++] = 4;
    mqtt_txbuf[mqtt_txlen++] = 0xc2;
    mqtt_txbuf[mqtt_txlen++] = 0;
    mqtt_txbuf[mqtt_txlen++] = 100;

    mqtt_txbuf[mqtt_txlen++] = BYTE1(ClientIDLen);
    mqtt_txbuf[mqtt_txlen++] = BYTE0(ClientIDLen);
    memcpy(&mqtt_txbuf[mqtt_txlen], ClientID, ClientIDLen);
    mqtt_txlen += ClientIDLen;

    if (UsernameLen > 0)
    {
        mqtt_txbuf[mqtt_txlen++] = BYTE1(UsernameLen);
        mqtt_txbuf[mqtt_txlen++] = BYTE0(UsernameLen);
        memcpy(&mqtt_txbuf[mqtt_txlen], Username, UsernameLen);
        mqtt_txlen += UsernameLen;
    }

    if (PasswordLen > 0)
    {
        mqtt_txbuf[mqtt_txlen++] = BYTE1(PasswordLen);
        mqtt_txbuf[mqtt_txlen++] = BYTE0(PasswordLen);
        memcpy(&mqtt_txbuf[mqtt_txlen], Password, PasswordLen);
        mqtt_txlen += PasswordLen;
    }

    memset(mqtt_rxbuf, 0, mqtt_rxlen);
    mqtt_send_data(mqtt_txbuf, mqtt_txlen);

    for (j = 0; j < 10; j++)
    {
        delay_ms(50);
        if (esp8266_wait_receive() == ESP8266_EOK)
            esp8266_copy_rxdata((char *)mqtt_rxbuf);

        if (mqtt_rxbuf[0] == parket_connetAck[0] &&
            mqtt_rxbuf[1] == parket_connetAck[1] &&
            mqtt_rxbuf[2] == parket_connetAck[2])
        {
            return 0;
        }
    }

    return 1;
}

uint8_t mqtt_subscribe_topic(char *topic, uint8_t qos, uint8_t whether)
{
    uint8_t j;
    mqtt_txlen = 0;
    int topiclen = strlen(topic);

    int DataLen = 2 + (topiclen + 2) + (whether ? 1 : 0);

    if (whether) mqtt_txbuf[mqtt_txlen++] = 0x82;
    else mqtt_txbuf[mqtt_txlen++] = 0xA2;

    do
    {
        uint8_t encodedByte = DataLen % 128;
        DataLen = DataLen / 128;
        if (DataLen > 0)
            encodedByte = encodedByte | 128;
        mqtt_txbuf[mqtt_txlen++] = encodedByte;
    } while (DataLen > 0);

    mqtt_txbuf[mqtt_txlen++] = 0;
    mqtt_txbuf[mqtt_txlen++] = 0x01;
    mqtt_txbuf[mqtt_txlen++] = BYTE1(topiclen);
    mqtt_txbuf[mqtt_txlen++] = BYTE0(topiclen);
    memcpy(&mqtt_txbuf[mqtt_txlen], topic, topiclen);
    mqtt_txlen += topiclen;

    if (whether)
    {
        mqtt_txbuf[mqtt_txlen++] = qos;
    }

    memset(mqtt_rxbuf, 0, mqtt_rxlen);
    mqtt_send_data(mqtt_txbuf, mqtt_txlen);

    for (j = 0; j < 10; j++)
    {
        delay_ms(50);
        if (esp8266_wait_receive() == ESP8266_EOK)
            esp8266_copy_rxdata((char *)mqtt_rxbuf);

        if (mqtt_rxbuf[0] == parket_subAck[0] && mqtt_rxbuf[1] == parket_subAck[1])
        {
            return 0;
        }
    }

    return 1;
}

uint8_t mqtt_publish_data(char *topic, char *message, uint8_t qos)
{
    int topicLength = strlen(topic);
    int messageLength = strlen(message);
    static uint16_t id = 0;
    int DataLen;
    mqtt_txlen = 0;

    if (qos) DataLen = (2 + topicLength) + 2 + messageLength;
    else DataLen = (2 + topicLength) + messageLength;

    mqtt_txbuf[mqtt_txlen++] = 0x30;

    do
    {
        uint8_t encodedByte = DataLen % 128;
        DataLen = DataLen / 128;
        if (DataLen > 0)
            encodedByte = encodedByte | 128;
        mqtt_txbuf[mqtt_txlen++] = encodedByte;
    } while (DataLen > 0);

    mqtt_txbuf[mqtt_txlen++] = BYTE1(topicLength);
    mqtt_txbuf[mqtt_txlen++] = BYTE0(topicLength);
    memcpy(&mqtt_txbuf[mqtt_txlen], topic, topicLength);
    mqtt_txlen += topicLength;

    if (qos)
    {
        mqtt_txbuf[mqtt_txlen++] = BYTE1(id);
        mqtt_txbuf[mqtt_txlen++] = BYTE0(id);
        id++;
    }

    memcpy(&mqtt_txbuf[mqtt_txlen], message, messageLength);
    mqtt_txlen += messageLength;

    mqtt_send_data(mqtt_txbuf, mqtt_txlen);
    return mqtt_txlen;
}

uint8_t mqtt_receive_handle(uint8_t *data_received, Mqtt_RxData_Type *rx_data)
{
    uint8_t *p;
    uint8_t encodeByte = 0;
    uint32_t multiplier = 1, Remaining_len = 0;
    uint8_t QS_level = 0;
    uint16_t variable_header_len;

    p = data_received;
    memset(rx_data, 0, sizeof(Mqtt_RxData_Type));

    if ((*p != 0x30) && (*p != 0x32) && (*p != 0x34))
        return 1;

    if (*p != 0x30) QS_level = 1;

    p++;
    do {
        encodeByte = *p++;
        Remaining_len += (encodeByte & 0x7F) * multiplier;
        multiplier *= 128;

        if (multiplier > 128 * 128 * 128)
            return 2;
    } while ((encodeByte & 0x80) != 0);

    rx_data->topic_len = *p++;
    rx_data->topic_len = rx_data->topic_len * 256 + *p++;
    memcpy(rx_data->topic, p, rx_data->topic_len);
    p += rx_data->topic_len;

    if (QS_level != 0)
        p += 2;

    variable_header_len = rx_data->topic_len + 2 + (QS_level != 0 ? 2 : 0);
    rx_data->payload_len = Remaining_len - variable_header_len;
    memcpy(rx_data->payload, p, rx_data->payload_len);
    if (rx_data->payload_len < sizeof(rx_data->payload)) {
        rx_data->payload[rx_data->payload_len] = '\0';
    } else {
        rx_data->payload[sizeof(rx_data->payload) - 1] = '\0';
    }

    return 0;
}

void mqtt_send_response(uint8_t *id)
{
    char buf[128] = {0};
    sprintf(buf, "{\"id\":\"%s\",\"code\":200,\"msg\":\"success\"}", id);

    mqtt_publish_data(RELY_PUBLISH_TOPIC, (char *)buf, 0);

    printf("\r\npublish_data:\r\n");
    printf("%s", (const char *)buf);
    printf("\r\n");
}

void mqtt_send_heart(void)
{
    mqtt_send_data((uint8_t *)parket_heart, sizeof(parket_heart));
}

void mqtt_disconnect(void)
{
    mqtt_send_data((uint8_t *)parket_disconnet, sizeof(parket_disconnet));
}

void mqtt_send_data(uint8_t *buf, uint16_t len)
{
    esp8266_send_data((char *)buf, len);
}
