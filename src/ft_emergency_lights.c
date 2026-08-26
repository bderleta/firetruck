#include "ft.h"

static const char *TAG = "ft_emergency_lights";

static const int emergency_lights_pins[] = {MD1_L, MD1_R};
static const ledc_channel_t emergency_lights_ledc_channels[] = {LEDC_CHANNEL_4, LEDC_CHANNEL_5};
static const uint32_t emergency_lights_hpoints[] = {0, 8192};
static bool emergency_lights_active = false;

static const ledc_timer_config_t emergency_lights_timer_config = {
    .timer_num = LEDC_TIMER_2,
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .duty_resolution = LEDC_TIMER_14_BIT,
    .freq_hz = 5,
    .clk_cfg = LEDC_AUTO_CLK,
};

void emergency_lights_init() {
    ESP_ERROR_CHECK(ledc_timer_config(&emergency_lights_timer_config));
    for (size_t channel = 0; channel < sizeof(emergency_lights_pins) / sizeof(*emergency_lights_pins); channel++) {
        ledc_channel_config_t channel_config = {
            .gpio_num = emergency_lights_pins[channel],
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .channel = emergency_lights_ledc_channels[channel],
            .timer_sel = emergency_lights_timer_config.timer_num,
            .duty = 0,
            .hpoint = 0,
        };
        ESP_ERROR_CHECK(ledc_channel_config(&channel_config));
    }
    ESP_ERROR_CHECK(ledc_timer_pause(LEDC_LOW_SPEED_MODE, emergency_lights_timer_config.timer_num));
    ESP_LOGI(TAG, "Configured LEDC emergency lights timer and channels");
}

void emergency_lights_on() {
    if (!emergency_lights_active) {
        ESP_ERROR_CHECK(ledc_timer_pause(LEDC_LOW_SPEED_MODE, emergency_lights_timer_config.timer_num));
        for (size_t channel = 0; channel < sizeof(emergency_lights_pins) / sizeof(*emergency_lights_pins); channel++) {
            ESP_ERROR_CHECK(ledc_set_duty_with_hpoint(LEDC_LOW_SPEED_MODE, emergency_lights_ledc_channels[channel], 8192, emergency_lights_hpoints[channel]));
            ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, emergency_lights_ledc_channels[channel]));
        }
        ESP_ERROR_CHECK(ledc_timer_resume(LEDC_LOW_SPEED_MODE, emergency_lights_timer_config.timer_num));
        emergency_lights_active = true;
        ESP_LOGI(TAG, "Emergency lights turned on");
    }
}

void emergency_lights_off() {
    if (emergency_lights_active) {
        for (size_t channel = 0; channel < sizeof(emergency_lights_pins) / sizeof(*emergency_lights_pins); channel++) {
            ESP_ERROR_CHECK(ledc_stop(LEDC_LOW_SPEED_MODE, emergency_lights_ledc_channels[channel], 0));
            gpio_set_level(emergency_lights_pins[channel], 0);
        }
        ESP_ERROR_CHECK(ledc_timer_pause(LEDC_LOW_SPEED_MODE, emergency_lights_timer_config.timer_num));
        emergency_lights_active = false;
        ESP_LOGI(TAG, "Emergency lights turned off");
    }
}

esp_err_t emergency_lights_http_handler(httpd_req_t *req) {
    char*  buf;
    size_t buf_len;

    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        ESP_RETURN_ON_FALSE(buf, ESP_ERR_NO_MEM, TAG, "buffer alloc failed");
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            char param[CONFIG_HTTPD_MAX_URI_LEN], dec_param[CONFIG_HTTPD_MAX_URI_LEN] = {0};
            if (httpd_query_key_value(buf, "cmd", param, sizeof(param)) == ESP_OK) {
                uri_decode(dec_param, param, strnlen(param, CONFIG_HTTPD_MAX_URI_LEN));
                if (strcasecmp(dec_param, "on") == 0) {
                    emergency_lights_on();
                } else if (strcasecmp(dec_param, "off") == 0) {
                    emergency_lights_off();
                }
            }
        }
        free(buf);
    }

    httpd_resp_send(req, "OK", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}