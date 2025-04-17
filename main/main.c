#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/rmt.h"
#include "led_strip.h"


#define GPIO_DATA_PIN   17
#define LED_COUNT       12

led_strip_handle_t strip;
void led_strip_setup() {
    led_strip_config_t strip_cfg = {
        .strip_gpio_num = GPIO_DATA_PIN,
        .max_leds       = LED_COUNT,
        .led_model      = LED_MODEL_WS2812,
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
        .flags = {
            .invert_out = false,
        }
    };

    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10 * 1000 * 1000,
        .mem_block_symbols = 64,
        .flags = {
            .with_dma = false,
        }
    };

    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_cfg, &rmt_config, &strip));
}

void app_main(void) {
    led_strip_setup();
    printf("LED strip initialized\n");

    for (int i = 0; i < LED_COUNT; i++) {
        ESP_ERROR_CHECK(led_strip_set_pixel(strip, i, 255, 255, 255));
    }
    ESP_ERROR_CHECK(led_strip_refresh(strip));
}
