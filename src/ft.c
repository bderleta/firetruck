#include "ft.h"

static const char *TAG = "ft";

static const gpio_num_t driver_pins[] = {
    /* Bidirectional DC motor drivers */
    MA1_R, MA1_L, MA2_R, MA2_L, /* MA1 - turn motor; MA2 - drive motor */
    MB1_R, MB1_L, MB2_R, MB2_L, /* unused */
    MC1_R, MC1_L, /* MC1 - ladder motor */
    MD1_R, MD1_L, MD2_R, MD2_L, /* MD1 - emergency lights */
    /* Open drain drivers */
    DA, /* Water pump */
    DB, /* Headlights */
    DC, 
    DD,
    IO1, IO2, IO8, IO9
};

void board_init() {
    ESP_LOGI(TAG, "Initializing board");
    /* Init drivers */
    int driver_pins_count = sizeof(driver_pins) / sizeof(*driver_pins);
    for (int i = 0; i < driver_pins_count; i++) {
        ESP_LOGI(TAG, "Configuring GPIO%u (driver %d/%d)", driver_pins[i], i + 1, driver_pins_count);
        gpio_reset_pin(driver_pins[i]);
        gpio_set_direction(driver_pins[i], GPIO_MODE_OUTPUT);
        gpio_set_level(driver_pins[i], 0);
    }
    ESP_LOGI(TAG, "Configuring GPIO%u (LED)", LED);
    gpio_reset_pin(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    gpio_set_level(LED, 1);
    ESP_LOGI(TAG, "All GPIO ready");
    
    drive_init();
    turntable_init();
    //ladder_init();
    emergency_lights_init();
}

void led_set(bool active) {
    gpio_set_level(LED, active ? 0 : 1);
}



