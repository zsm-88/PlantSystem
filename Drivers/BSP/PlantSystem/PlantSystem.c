#include "PlantSystem.h"
#include <string.h>

extern char dht11_data[5];

uint16_t current_humi = 0;
uint16_t current_light = 0;

static uint8_t remote_control_mode = 0;
static uint8_t flag_S2 = 0;
static uint8_t flag_S3 = 0;
static uint8_t flag_S4 = 0;
static uint8_t fan_statu;
static uint8_t bump_statu;
static uint8_t led_statu;
static uint8_t temp_value = 25;
static uint8_t humi_vlaue = 10;
static uint8_t light_vlaue = 200;
static uint8_t count = 0;

void PlantSystem_UpdateSensors(void)
{
    static uint32_t last_dht11_tick = 0;
    uint32_t now = HAL_GetTick();

    if (last_dht11_tick == 0U || (now - last_dht11_tick) >= 2000U) {
        dht11_read();
        last_dht11_tick = HAL_GetTick();
    }

    current_humi = TS_GetData();
    current_light = LDR_LuxData();
}

uint16_t PlantSystem_GetSoilHumidity(void)
{
    return current_humi;
}

uint16_t PlantSystem_GetLightIntensity(void)
{
    return current_light;
}

uint8_t PlantSystem_GetFanState(void)
{
    return HAL_GPIO_ReadPin(FAN_PORT, FAN_PIN) == GPIO_PIN_SET;
}

uint8_t PlantSystem_GetBumpState(void)
{
    return HAL_GPIO_ReadPin(BUMP_PORT, BUMP_PIN) == GPIO_PIN_SET;
}

uint8_t PlantSystem_GetLedState(void)
{
    return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) == GPIO_PIN_SET;
}

uint8_t PlantSystem_IsAutoMode(void)
{
    return remote_control_mode == 0;
}

static uint8_t json_get_bool(const char *payload, const char *key, uint8_t *value)
{
    const char *p = strstr(payload, key);
    const char *end;
    const char *true_pos;
    const char *false_pos;
    const char *one_pos;
    const char *zero_pos;

    if (p == NULL) return 0;

    end = strchr(p, ',');
    if (end == NULL) end = strchr(p, '}');
    if (end == NULL) end = p + strlen(p);

    true_pos = strstr(p, "true");
    false_pos = strstr(p, "false");
    one_pos = strstr(p, ":1");
    zero_pos = strstr(p, ":0");

    if ((true_pos != NULL && true_pos < end) || (one_pos != NULL && one_pos < end)) {
        *value = 1;
        return 1;
    }

    if ((false_pos != NULL && false_pos < end) || (zero_pos != NULL && zero_pos < end)) {
        *value = 0;
        return 1;
    }

    return 0;
}

static void json_get_id(const char *payload, char *id, uint8_t id_size)
{
    const char *p = strstr(payload, "\"id\"");
    const char *start;
    const char *end;
    uint8_t len;

    if (id_size == 0) return;
    id[0] = '\0';

    if (p == NULL) return;
    p = strchr(p, ':');
    if (p == NULL) return;
    start = strchr(p, '\"');
    if (start == NULL) return;
    start++;
    end = strchr(start, '\"');
    if (end == NULL) return;

    len = (uint8_t)(end - start);
    if (len >= id_size) len = id_size - 1;
    memcpy(id, start, len);
    id[len] = '\0';
}

void PlantSystem_HandleCloudPayload(uint8_t *payload)
{
    uint8_t value;
    uint8_t handled = 0;
    uint8_t auto_mode = 0;
    char id[32];
    char *payload_str = (char *)payload;

    if (payload == NULL) return;

    if (json_get_bool(payload_str, "\"auto_mode\"", &auto_mode) && auto_mode) {
        remote_control_mode = 0;
        flag_S2 = 0;
        flag_S3 = 0;
        flag_S4 = 0;
        handled = 1;
    }

    if (json_get_bool(payload_str, "\"fan\"", &value)) {
        if (value) FAN_ON; else FAN_OFF;
        remote_control_mode = 1;
        handled = 1;
    }

    if (json_get_bool(payload_str, "\"bump\"", &value)) {
        if (value) BUMP_ON; else BUMP_OFF;
        remote_control_mode = 1;
        handled = 1;
    }

    if (json_get_bool(payload_str, "\"LED\"", &value)) {
        if (value) led1_on(); else led1_off();
        remote_control_mode = 1;
        handled = 1;
    }

    if (handled) {
        json_get_id(payload_str, id, sizeof(id));
        if (id[0] == '\0') strcpy(id, "0");
        mqtt_send_response((uint8_t *)id);
    }
}

void LCD_ShowDHT11Data(void)
{
    char buffer[32];

    if(dht11_data[4] == (dht11_data[0] + dht11_data[1] + dht11_data[2] + dht11_data[3]))
    {
        sprintf(buffer, ":%d.%d C", dht11_data[2], dht11_data[3]);
        LCD_ShowString(65, 60, buffer, BLACK, WHITE);

        sprintf(buffer, ":%d.%d %%", dht11_data[0], dht11_data[1]);
        LCD_ShowString(65, 80, buffer, BLACK, WHITE);
    }
    else
    {
        LCD_ShowString(1, 60, "DHT11 Error!    ", RED, WHITE);
        LCD_ShowString(1, 80, "Check Sensor    ", RED, WHITE);
    }
}

void LCD_Show_Page1(void)
{
    uint8_t str[] = {0,1,2,3};
    uint8_t str1[] = {4,5,6,7,8,9};
    uint8_t str2[] = {0,1,2,3,4,5,6,7};
    uint8_t str3[] = {0,1,4,5};
    uint8_t str4[] = {0,1,6,7};
    uint8_t str5[] = {2,3,6,7};
    uint8_t str6[] = {14,15,16,17};
    char buffer[32];

    LCD_Show_Chinese_String(1,1,str,4,RED,WHITE);
    LCD_Show_Chinese_String(32,20,str1,6,RED,WHITE);
    LCD_Show_Chinese_String2(1,40,str2,8,BLUE,WHITE);
    LCD_Show_Chinese_String3(1,60,str3,4,BLACK,WHITE);
    LCD_Show_Chinese_String3(1,80,str4,4,BLACK,WHITE);
    LCD_Show_Chinese_String3(1,100,str5,4,BLACK,WHITE);
    LCD_Show_Chinese_String3(1,120,str6,4,BLACK,WHITE);

    LCD_ShowDHT11Data();

    sprintf(buffer, ":%3d  %%", current_humi);
    LCD_ShowString(65, 100, buffer, BLACK, WHITE);

    sprintf(buffer, ": %3d %%", current_light);
    LCD_ShowString(65, 120, buffer, BLACK, WHITE);

    Fan_Bump_LED_status();
}

void LCD_Show_Page2(void)
{
    uint8_t str[] = {8,9};
    uint8_t str1[] = {10,11};
    uint8_t str2[] = {18,19,20};
    uint8_t str3[] = {21,22,23,24,25,26};
    uint8_t str4[] = {27,28,29,30};

    LCD_Show_Chinese_String3(1,1,str3,6,BLACK,WHITE);
    LCD_Show_Chinese_String3(1,20,str,2,BLACK,WHITE);
    LCD_Show_Chinese_String3(1,40,str1,2,BLACK,WHITE);
    LCD_Show_Chinese_String3(1,60,str2,3,BLACK,WHITE);
    LCD_Show_Chinese_String3(1,80,str4,4,BLACK,WHITE);
    LCD_Show_Chinese_String3(1,100,str,2,BLACK,WHITE);
    LCD_Show_Chinese_String3(1,120,str1,2,BLACK,WHITE);
    LCD_Show_Chinese_String3(1,140,str2,3,BLACK,WHITE);
    LCD_ShowChar(33,20,':', BLACK,WHITE);
    LCD_ShowChar(33,40,':', BLACK,WHITE);
    LCD_ShowChar(49,60,':', BLACK,WHITE);
    LCD_ShowChar(33,100,':', BLACK,WHITE);
    LCD_ShowChar(33,120,':', BLACK,WHITE);
    LCD_ShowChar(49,140,':', BLACK,WHITE);

    fan_statu = HAL_GPIO_ReadPin(FAN_PORT,FAN_PIN);
    if(fan_statu==1) LCD_Show_Single_Chinese3(40,20,12,RED,WHITE);
    else LCD_Show_Single_Chinese3(40,20,13,RED,WHITE);

    bump_statu = HAL_GPIO_ReadPin(BUMP_PORT,BUMP_PIN);
    if(bump_statu==1) LCD_Show_Single_Chinese3(40,40,12,RED,WHITE);
    else LCD_Show_Single_Chinese3(40,40,13,RED,WHITE);

    led_statu = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_8);
    if(led_statu==1) LCD_Show_Single_Chinese3(55,60,12,RED,WHITE);
    else LCD_Show_Single_Chinese3(55,60,13,RED,WHITE);

    if(KEY_Scan(S2) == KEY_ON)
    {
        flag_S2 += 1;
        if(flag_S2 > 1) flag_S2 = 0;
        if(flag_S2 == 1)
        {
            remote_control_mode = 1;
        }
        else
        {
            remote_control_mode = 0;
        }
    }

    if(flag_S2==1)
    {
        LCD_Show_Chinese_String3(1,80,str4,4,RED,BLACK);
        if(KEY_Scan(S3) == KEY_ON)
        {
            flag_S3 += 1;
            if(flag_S3 > 3) flag_S3 = 0;
        }
        if(KEY_Scan(S4) == KEY_ON)
        {
            flag_S4 += 1;
            if(flag_S4 > 1) flag_S4 = 0;
        }

        if(flag_S3==1)
        {
            LCD_Show_Chinese_String3(1,100,str,2,RED,BLACK);
            if(flag_S4==0)
            {
                FAN_OFF;
                LCD_Show_Single_Chinese3(40,100,13,RED,WHITE);
            }
            else if(flag_S4==1)
            {
                FAN_ON;
                LCD_Show_Single_Chinese3(40,100,12,RED,WHITE);
            }
        }
        else if(flag_S3==2)
        {
            LCD_Show_Chinese_String3(1,120,str1,2,RED,BLACK);
            if(flag_S4==0)
            {
                BUMP_OFF;
                LCD_Show_Single_Chinese3(40,120,13,RED,WHITE);
            }
            else if(flag_S4==1)
            {
                BUMP_ON;
                LCD_Show_Single_Chinese3(40,120,12,RED,WHITE);
            }
        }
        else if(flag_S3==3)
        {
            LCD_Show_Chinese_String3(1,140,str2,3,RED,BLACK);
            if(flag_S4==0)
            {
                led1_off();
                LCD_Show_Single_Chinese3(60,140,13,RED,WHITE);
            }
            else if(flag_S4==1)
            {
                led1_on();
                LCD_Show_Single_Chinese3(60,140,12,RED,WHITE);
            }
        }
    }
    else if(flag_S2==0)
    {
        Fan_Bump_LED_status();
    }
}

void LCD_Show_Page3(void)
{
    char buffer1[32];
    char buffer2[32];
    char buffer3[32];

    LCD_ShowString(10, 10, "Set Threshold", BLUE, WHITE);

    LCD_ShowString(10, 40, "temp:", BLACK, WHITE);
    LCD_ShowString(10, 60, "humi:", BLACK, WHITE);
    LCD_ShowString(10, 80, "light:", BLACK, WHITE);

    sprintf(buffer1, "%3d C", temp_value);
    LCD_ShowString(60, 40, buffer1, RED, WHITE);

    sprintf(buffer2, "%3d %%", humi_vlaue);
    LCD_ShowString(60, 60, buffer2, RED, WHITE);

    sprintf(buffer3, "%3d %%", light_vlaue);
    LCD_ShowString(60, 80, buffer3, RED, WHITE);

    if(KEY_Scan(S2) == KEY_ON)
    {
        count++;
        if(count>3)count=0;
    }
    if(count == 1)
    {
        LCD_ShowString(60, 40, buffer1, RED, BLACK);
        if(KEY_Scan(S3) == KEY_ON) temp_value++;
        else if(KEY_Scan(S4) == KEY_ON) temp_value--;
    }
    else if(count == 2)
    {
        LCD_ShowString(60, 60, buffer2, RED, BLACK);
        if(KEY_Scan(S3) == KEY_ON) humi_vlaue++;
        else if(KEY_Scan(S4) == KEY_ON) humi_vlaue--;
    }
    else if(count == 3)
    {
        LCD_ShowString(60, 80, buffer3, RED, BLACK);
        if(KEY_Scan(S3) == KEY_ON) light_vlaue++;
        else if(KEY_Scan(S4) == KEY_ON) light_vlaue--;
    }
    Fan_Bump_LED_status();
}

void Fan_Bump_LED_status(void)
{
    float current_temp;

    if (remote_control_mode) return;

    current_temp = dht11_data[2] + dht11_data[3] * 0.1f;

    if(current_temp>temp_value) FAN_ON;
    else FAN_OFF;

    if(current_humi<humi_vlaue) BUMP_ON;
    else BUMP_OFF;

    if(current_light<light_vlaue) led1_on();
    else led1_off();
}

void uart_printf(void)
{
    printf("temperature: %d.%d C, air_humidity: %d.%d %%\r\n",
           dht11_data[2], dht11_data[3],
           dht11_data[0], dht11_data[1]);
    printf("soil_humidity: %d\r\n",current_humi);
    printf("lighting_intensity: %d\r\n",current_light);
    printf("----------------------------------------------\n");

    printf("temp_value: %d\r\n",temp_value);
    printf("humi_vlaue: %d\r\n",humi_vlaue);
    printf("light_vlaue: %d\r\n",light_vlaue);
    printf("remote_control_mode: %d\r\n",remote_control_mode);
    printf("----------------------------------------------\n");

    printf("fan: %s\n", PlantSystem_GetFanState() ? "open" : "close");
    printf("bump: %s\n", PlantSystem_GetBumpState() ? "open" : "close");
    printf("LED: %s\n", PlantSystem_GetLedState() ? "open" : "close");
}
