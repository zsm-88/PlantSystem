#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "PlantSystem.h"

extern char dht11_data[5];

static void LCD_ShowBootStatus(const char *line1, const char *line2, const char *line3)
{
    Lcd_Clear(WHITE);
    LCD_ShowString(8, 16, "Plant System", BLUE, WHITE);
    LCD_ShowString(8, 48, (char *)line1, BLACK, WHITE);
    LCD_ShowString(8, 72, (char *)line2, BLACK, WHITE);
    LCD_ShowString(8, 96, (char *)line3, RED, WHITE);
}

static void MQTT_ProcessIncoming(void)
{
    static uint8_t mqtt_receive_buff[512];
    static Mqtt_RxData_Type rx_data;

    if (esp8266_wait_receive() != ESP8266_EOK) {
        return;
    }

    memset(mqtt_receive_buff, 0, sizeof(mqtt_receive_buff));
    esp8266_copy_rxdata((char *)mqtt_receive_buff);

    if (mqtt_receive_handle(mqtt_receive_buff, &rx_data) == 0) {
        PlantSystem_HandleCloudPayload(rx_data.payload);
    }
}

static void MQTT_PollForMs(uint32_t wait_ms)
{
    while (wait_ms >= 50U) {
        MQTT_ProcessIncoming();
        delay_ms(50);
        wait_ms -= 50U;
    }

    if (wait_ms > 0U) {
        MQTT_ProcessIncoming();
        delay_ms((uint16_t)wait_ms);
    }
}

static void MQTT_PublishPlantData(void)
{
    static uint8_t data_send_buff[512];
    static uint32_t publish_id = 1;

    memset(data_send_buff, 0, sizeof(data_send_buff));
    snprintf((char *)data_send_buff, sizeof(data_send_buff),
            "{\"id\":\"%lu\",\"version\":\"1.0\",\"params\":{" \
            "\"temperature\":{\"value\":%d.%d}," \
            "\"air_humidity\":{\"value\":%d.%d}," \
            "\"soil_humidity\":{\"value\":%d}," \
            "\"lighting_intensity\":{\"value\":%d}," \
            "\"auto_mode\":{\"value\":%s}," \
            "\"fan\":{\"value\":%s}," \
            "\"bump\":{\"value\":%s}," \
            "\"LED\":{\"value\":%s}}}",
            (unsigned long)publish_id++,
            dht11_data[2], dht11_data[3],
            dht11_data[0], dht11_data[1],
            PlantSystem_GetSoilHumidity(),
            PlantSystem_GetLightIntensity(),
            PlantSystem_IsAutoMode() ? "true" : "false",
            PlantSystem_GetFanState() ? "true" : "false",
            PlantSystem_GetBumpState() ? "true" : "false",
            PlantSystem_GetLedState() ? "true" : "false");

    mqtt_publish_data(POST_TOPIC, (char *)data_send_buff, 0);
}

int main(void)
{
    uint8_t current_page = 1;

    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);
    delay_init(72);
    usart_init(115200);
    Lcd_Init();
    TS_Init();
    LDR_Init();
    KEY_Init();
    FAN_Init();
    led_init();
    BUMP_Init();

    LCD_ShowBootStatus("LCD Ready", "Init modules...", "Starting...");
    printf("hello world!\r\n");

    LCD_ShowBootStatus("ESP8266", "Connecting WiFi...", "Please wait");
    printf("Init ESP8266...\r\n");
    esp8266_init(115200);

    LCD_ShowBootStatus("MQTT", "Init client...", "Please wait");
    printf("Init MQTT...\r\n");
    mqtt_init();

    LCD_ShowBootStatus("MQTT", "Connecting cloud...", "Please wait");
    printf("MQTT connecting...\r\n");
    while (mqtt_connect(MQTT_ClientID, MQTT_UserName, MQTT_PassWord)) {
        LCD_ShowBootStatus("MQTT", "Connecting cloud...", "Retrying...");
        delay_ms(500);
    }

    LCD_ShowBootStatus("MQTT", "Subscribing topic...", "Please wait");
    printf("Subscribe property set topic...\r\n");
    while (mqtt_subscribe_topic(SET_TOPIC, 0, 1)) {
        LCD_ShowBootStatus("MQTT", "Subscribing topic...", "Retrying...");
        delay_ms(500);
    }

    Lcd_Clear(WHITE);

    while (1)
    {
        PlantSystem_UpdateSensors();
        uart_printf();
        MQTT_ProcessIncoming();

        if (KEY_Scan(S1) == KEY_ON)
        {
            printf("key_press\n");
            current_page += 1;
            if (current_page > 3)
                current_page = 1;
            Lcd_Clear(WHITE);
        }

        switch (current_page)
        {
            case 1: LCD_Show_Page1(); break;
            case 2: LCD_Show_Page2(); break;
            case 3: LCD_Show_Page3(); break;
        }

        MQTT_PublishPlantData();
        MQTT_ProcessIncoming();

        MQTT_PollForMs(3000);

        mqtt_send_heart();
        MQTT_ProcessIncoming();
    }
}
