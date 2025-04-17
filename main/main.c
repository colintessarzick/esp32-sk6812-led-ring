#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/rmt_tx.h"
#include "led_strip.h"


#define GPIO_DATA_PIN   17
#define LED_COUNT       12

led_strip_handle_t strip;
void led_ring_setup() {
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

uint8_t normalize_brightness_percentage(uint8_t brightness) {
    return brightness * 255 / 100;
}

void led_ring(uint16_t hue, uint8_t brightness_percent) {
    for (int i = 0; i < LED_COUNT; i++) {
        ESP_ERROR_CHECK(led_strip_set_pixel_hsv(strip, i, hue, 255, normalize_brightness_percentage(brightness_percent)));
    }
    ESP_ERROR_CHECK(led_strip_refresh(strip));
}

void led_ring_hue_transition(uint16_t start_hue, uint16_t end_hue, uint8_t brightness_percent, uint32_t duration_ms) {
    const uint32_t delay_ms = 25;
    uint32_t steps = duration_ms / delay_ms;

    int16_t delta = end_hue - start_hue;
    if (delta > 180) {
        delta -= 360;
    } else if (delta < -180) {
        delta += 360;
    }

    for (uint32_t step = 0; step <= steps; step++) {
        float progress = (float)step / steps;
        int current_hue = start_hue + (int)(delta * progress);

        if (current_hue < 0) {
            current_hue += 360;
        } else if (current_hue >= 360) {
            current_hue -= 360;
        }

        for (int i = 0; i < LED_COUNT; i++) {
            ESP_ERROR_CHECK(led_strip_set_pixel_hsv(strip, i, current_hue, 255, normalize_brightness_percentage(brightness_percent)));
        }
        printf("Hue: %d\n", current_hue);
        ESP_ERROR_CHECK(led_strip_refresh(strip));
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}

void blink_led_ring(uint16_t hue, uint8_t brightness_percent, uint32_t interval_ms) {
    for (int i = 0; i < LED_COUNT; i++) {
        ESP_ERROR_CHECK(led_strip_set_pixel_hsv(strip, i, hue, 255, normalize_brightness_percentage(brightness_percent)));
    }
    ESP_ERROR_CHECK(led_strip_refresh(strip));
    vTaskDelay(pdMS_TO_TICKS(interval_ms));

    for (int i = 0; i < LED_COUNT; i++) {
        ESP_ERROR_CHECK(led_strip_set_pixel_hsv(strip, i, hue, 0, 0));
    }
    ESP_ERROR_CHECK(led_strip_refresh(strip));
    vTaskDelay(pdMS_TO_TICKS(interval_ms));
}

void app_main(void) {
    led_ring_setup();
    printf("LED strip initialized\n");

    led_strip_clear(strip);
    ESP_ERROR_CHECK(led_strip_refresh(strip));
    vTaskDelay(pdMS_TO_TICKS(1000));

    led_ring_hue_transition(240, 0, 50, 30 * 1000);
    led_ring(240, 30);

    while (true) {
        blink_led_ring(180, 50, 1000);
    }
}
