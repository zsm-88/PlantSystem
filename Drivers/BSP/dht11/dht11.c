#include "dht11.h"
#include "delay.h"

char dht11_data[5];

static uint8_t dht11_wait_level(GPIO_PinState level, uint16_t timeout_us)
{
    while (timeout_us--) {
        if (DHT11_DQ_IN == level) {
            return 1;
        }
        delay_us(1);
    }

    return 0;
}

static uint8_t dht11_wait_level_change(GPIO_PinState level, uint16_t timeout_us)
{
    while (timeout_us--) {
        if (DHT11_DQ_IN != level) {
            return 1;
        }
        delay_us(1);
    }

    return 0;
}

/**
 * @brief Configure DHT11 data pin as input.
 */
void dht11_gpio_input(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    DHT11_CLK_ENABLE();

    GPIO_InitStructure.Pin = DHT11_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStructure);
}

/**
 * @brief Configure DHT11 data pin as push-pull output.
 */
void dht11_gpio_output(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    DHT11_CLK_ENABLE();

    GPIO_InitStructure.Pin = DHT11_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStructure);
}

/**
 * @brief Send start signal and wait for DHT11 response.
 */
static uint8_t dht11_start(void)
{
    dht11_gpio_output();
    DHT11_DQ_OUT(1);
    DHT11_DQ_OUT(0);
    HAL_Delay(20);
    DHT11_DQ_OUT(1);

    dht11_gpio_input();

    if (!dht11_wait_level_change(GPIO_PIN_SET, 100)) return 0;
    if (!dht11_wait_level(GPIO_PIN_SET, 100)) return 0;
    if (!dht11_wait_level_change(GPIO_PIN_SET, 100)) return 0;

    return 1;
}

/**
 * @brief Read one byte from DHT11.
 * @return Received 8-bit value.
 */
static uint8_t dht11_read_byte(uint8_t *value)
{
    uint8_t i;
    uint8_t temp;
    uint8_t read_data = 0;

    for (i = 0; i < 8; i++) {
        if (!dht11_wait_level(GPIO_PIN_SET, 100)) {
            return 0;
        }

        delay_us(50);

        if (DHT11_DQ_IN == 1) {
            temp = 1;
            if (!dht11_wait_level_change(GPIO_PIN_SET, 100)) {
                return 0;
            }
        } else {
            temp = 0;
        }

        read_data = read_data << 1;
        read_data |= temp;
    }

    *value = read_data;
    return 1;
}

/**
 * @brief Read 5 bytes from DHT11.
 */
void dht11_read(void)
{
    uint8_t i;
    uint8_t read_buf[5];

    if (!dht11_start()) {
        return;
    }

    dht11_gpio_input();

    for (i = 0; i < 5; i++) {
        if (!dht11_read_byte(&read_buf[i])) {
            return;
        }
    }

    for (i = 0; i < 5; i++) {
        dht11_data[i] = read_buf[i];
    }
}
