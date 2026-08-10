#include "esp8266.h"
#include "string.h"
#include "delay.h"
#include <stdarg.h>

UART_HandleTypeDef g_uart_handle;
uint8_t g_uart_rx_buf[ESP8266_UART_RX_BUF_SIZE];
uint8_t g_uart_tx_buf[ESP8266_UART_TX_BUF_SIZE];
volatile uint16_t esp8266_cnt = 0;
volatile uint16_t esp8266_cntPre = 0;

void esp8266_uart_init(uint32_t baudrate)
{
    g_uart_handle.Instance = ESP8266_UART_INTERFACE;
    g_uart_handle.Init.BaudRate = baudrate;
    g_uart_handle.Init.WordLength = UART_WORDLENGTH_8B;
    g_uart_handle.Init.StopBits = UART_STOPBITS_1;
    g_uart_handle.Init.Parity = UART_PARITY_NONE;
    g_uart_handle.Init.Mode = UART_MODE_TX_RX;
    g_uart_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    g_uart_handle.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&g_uart_handle);
}

void ESP8266_UART_IRQHandler(void)
{
    if (__HAL_UART_GET_FLAG(&g_uart_handle, UART_FLAG_RXNE) != RESET) {
        if (esp8266_cnt >= sizeof(g_uart_rx_buf)) {
            esp8266_cnt = 0;
        }

        g_uart_rx_buf[esp8266_cnt++] = (uint8_t)(g_uart_handle.Instance->DR & 0x00FF);
    }
}

uint8_t esp8266_wait_receive(void)
{
    if (esp8266_cnt == 0) {
        return ESP8266_ERROR;
    }

    if (esp8266_cnt == esp8266_cntPre) {
        return ESP8266_EOK;
    }

    esp8266_cntPre = esp8266_cnt;
    return ESP8266_ERROR;
}

void esp8266_clear(void)
{
    memset(g_uart_rx_buf, 0, sizeof(g_uart_rx_buf));
    esp8266_cnt = 0;
    esp8266_cntPre = 0;
}

uint8_t esp8266_send_command(char *cmd, char *res)
{
    uint8_t timeOut = 250;

    esp8266_clear();
    HAL_UART_Transmit(&g_uart_handle, (unsigned char *)cmd, strlen((const char *)cmd), 100);

    while (timeOut--) {
        if (esp8266_wait_receive() == ESP8266_EOK) {
            if (strstr((const char *)g_uart_rx_buf, res) != NULL) {
                return ESP8266_EOK;
            }
        }
        delay_ms(10);
    }

    return ESP8266_ERROR;
}

uint8_t esp8266_restore(void)
{
    return esp8266_send_command("AT+RESTORE", "ready");
}

uint8_t esp8266_at_test(void)
{
    return esp8266_send_command("AT\r\n", "OK");
}

uint8_t esp8266_set_mode(uint8_t mode)
{
    switch (mode) {
        case ESP8266_STA_MODE:
            return esp8266_send_command("AT+CWMODE=1\r\n", "OK");

        case ESP8266_AP_MODE:
            return esp8266_send_command("AT+CWMODE=2\r\n", "OK");

        case ESP8266_STA_AP_MODE:
            return esp8266_send_command("AT+CWMODE=3\r\n", "OK");

        default:
            return ESP8266_EINVAL;
    }
}

uint8_t esp8266_sw_reset(void)
{
    return esp8266_send_command("AT+RST\r\n", "OK");
}

uint8_t esp8266_ate_config(uint8_t cfg)
{
    switch (cfg) {
        case 0:
            return esp8266_send_command("ATE0\r\n", "OK");

        case 1:
            return esp8266_send_command("ATE1\r\n", "OK");

        default:
            return ESP8266_EINVAL;
    }
}

uint8_t esp8266_join_ap(char *ssid, char *pwd)
{
    char cmd[64];

    sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pwd);
    return esp8266_send_command(cmd, "WIFI GOT IP");
}

uint8_t esp8266_open_server(void)
{
    return esp8266_send_command("AT+CIPSERVER=1,8080\r\n", "OK");
}

uint8_t esp8266_get_ip(char *buf)
{
    char *p_start;
    char *p_end;

    if (esp8266_send_command("AT+CIFSR\r\n", "STAIP") != ESP8266_EOK) {
        return ESP8266_ERROR;
    }

    p_start = strstr((const char *)g_uart_rx_buf, "\"");
    p_end = strstr(p_start + 1, "\"");
    *p_end = '\0';
    sprintf(buf, "%s", p_start + 1);

    return ESP8266_EOK;
}

uint8_t esp8266_connect_tcp_server(char *server_ip, char *server_port)
{
    char cmd[64];

    sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%s\r\n", server_ip, server_port);
    return esp8266_send_command(cmd, "CONNECT");
}

uint8_t esp8266_disconnect_tcp_server(void)
{
    return esp8266_send_command("AT+CIPCLOSE\r\n", "");
}

uint8_t esp8266_enter_unvarnished(void)
{
    uint8_t ret;

    ret = esp8266_send_command("AT+CIPMODE=1\r\n", "OK");
    HAL_Delay(300);
    ret += esp8266_send_command("AT+CIPSEND\r\n", ">");

    if (ret == ESP8266_EOK) {
        return ESP8266_EOK;
    }

    return ESP8266_ERROR;
}

uint8_t esp8266_exit_unvarnished(void)
{
    return esp8266_send_command("+++", "");
}

uint8_t esp8266_single_connection(void)
{
    return esp8266_send_command("AT+CIPMUX=0\r\n", "OK");
}

uint8_t esp8266_multi_connection(void)
{
    return esp8266_send_command("AT+CIPMUX=1\r\n", "OK");
}

uint8_t esp8266_set_ap(char *ssid, char *pwd)
{
    char cmd[64];

    sprintf(cmd, "AT+CWSAP=\"%s\",\"%s\",5,3\r\n", ssid, pwd);
    return esp8266_send_command(cmd, "OK");
}

uint8_t esp8266_init(uint32_t baudrate)
{
    char ip_buf[16];

    esp8266_uart_init(baudrate);

    printf("\r\n    Exit transparent mode\r\n");
    esp8266_exit_unvarnished();

    printf("    1. Check ESP8266 (AT)\r\n");
    while (esp8266_at_test()) {
        delay_ms(500);
    }

    printf("    2. Reset ESP8266 (AT+RST)\r\n");
    while (esp8266_sw_reset()) {
        delay_ms(500);
    }
    while (esp8266_disconnect_tcp_server()) {
        delay_ms(500);
    }

    printf("    3. Set STA mode (AT+CWMODE=1)\r\n");
    while (esp8266_set_mode(ESP8266_STA_MODE)) {
        delay_ms(500);
    }

    printf("    4. Set single connection mode (AT+CIPMUX)\r\n");
    while (esp8266_single_connection()) {
        delay_ms(500);
    }

    printf("    5. Connect WiFi, SSID: %s, PWD: %s\r\n", WIFI_SSID, WIFI_PWD);
    while (esp8266_join_ap(WIFI_SSID, WIFI_PWD)) {
        delay_ms(1000);
    }

    printf("    6. Get IP address (AT+CIFSR): ");
    while (esp8266_get_ip(ip_buf)) {
        delay_ms(500);
    }
    printf("%s\r\n\r\n", ip_buf);

    esp8266_disconnect_tcp_server();
    printf("    7. Connect cloud server (AT+CIPSTART), server_ip: %s, server_port: %s\r\n",
           TCP_SERVER_IP, TCP_SERVER_PORT);
    while (esp8266_connect_tcp_server(TCP_SERVER_IP, TCP_SERVER_PORT)) {
        delay_ms(500);
    }

    printf("    8. Enter transparent mode (AT+CIPMODE)\r\n");
    while (esp8266_enter_unvarnished()) {
        delay_ms(500);
    }
    printf("OneNET success\r\n");
    printf("ESP8266 success\r\n");

    return ESP8266_EOK;
}

void esp8266_uart_printf(char *fmt, ...)
{
    va_list ap;
    uint16_t len;

    va_start(ap, fmt);
    vsprintf((char *)g_uart_tx_buf, fmt, ap);
    va_end(ap);

    len = strlen((const char *)g_uart_tx_buf);
    HAL_UART_Transmit(&g_uart_handle, g_uart_tx_buf, len, HAL_MAX_DELAY);
}

void usart_send_string(UART_HandleTypeDef *huart, unsigned char *str, unsigned short len)
{
    HAL_UART_Transmit(huart, (uint8_t *)str, len, 0xffff);
    while (HAL_UART_GetState(huart) == HAL_UART_STATE_BUSY_TX);
}

void esp8266_send_data(char *data_send, uint16_t send_len)
{
    esp8266_clear();
    HAL_UART_Transmit(&g_uart_handle, (unsigned char *)data_send, send_len, 100);
}

uint16_t esp8266_copy_rxdata(char *data_receive)
{
    uint16_t receive_len = esp8266_cnt;

    if (receive_len > 0) {
        memcpy(data_receive, g_uart_rx_buf, receive_len);
    }

    esp8266_clear();
    return receive_len;
}
