#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <stdio.h>
#include <bme680_reg.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

int main(void)
{
    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);
    while(1)
    {
        const float conversion_factor = 3.3f / (1 << 12);
        uint16_t result = adc_read();
        printf("Raw value: 0x%03x, voltage: %f V\n", result, result * conversion_factor);
        sleep_ms(2000);
    }
}