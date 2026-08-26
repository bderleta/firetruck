#include "ft.h"

static const char *TAG = "ft_turntable";

static const int turntable_pins[] = {IO2, IO1, IO8, IO9};
static const ledc_channel_t turntable_ledc_channels[] = {LEDC_CHANNEL_0, LEDC_CHANNEL_1, LEDC_CHANNEL_2, LEDC_CHANNEL_3};
static bool turntable_active = false;

static const ledc_timer_config_t turntable_timer_config = {
    .timer_num = LEDC_TIMER_3,
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .duty_resolution = LEDC_TIMER_14_BIT,
    .freq_hz = 40,
    .clk_cfg = LEDC_AUTO_CLK,
};

void turntable_init() {
    /* Configuring turntable ledc */
    ESP_ERROR_CHECK(ledc_timer_config(&turntable_timer_config));
    for (size_t channel = 0; channel < sizeof(turntable_pins) / sizeof(*turntable_pins); channel++) {
        ledc_channel_config_t channel_config = {
            .gpio_num = turntable_pins[channel],
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .channel = turntable_ledc_channels[channel],
            .timer_sel = turntable_timer_config.timer_num,
            .duty = 0,
            .hpoint = 0,
        };
        ESP_ERROR_CHECK(ledc_channel_config(&channel_config));
    }
    ESP_ERROR_CHECK(ledc_timer_pause(LEDC_LOW_SPEED_MODE, turntable_timer_config.timer_num));
    ESP_LOGI(TAG, "Configured LEDC turntable timer and channels");
}

void turntable_left() {
    const uint32_t turntable_hpoints[] = {0, 4096, 8192, 12288};
    ESP_ERROR_CHECK(ledc_timer_pause(LEDC_LOW_SPEED_MODE, turntable_timer_config.timer_num));
    for (size_t channel = 0; channel < sizeof(turntable_pins) / sizeof(*turntable_pins); channel++) {
        ESP_ERROR_CHECK(ledc_set_duty_with_hpoint(LEDC_LOW_SPEED_MODE, turntable_ledc_channels[channel], 6144, turntable_hpoints[channel]));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, turntable_ledc_channels[channel]));
    }
    ESP_ERROR_CHECK(ledc_timer_resume(LEDC_LOW_SPEED_MODE, turntable_timer_config.timer_num));
    turntable_active = true;
    ESP_LOGI(TAG, "turntable turning left");
}

void turntable_right() {
const uint32_t turntable_hpoints[] = {12288, 8192, 4096, 0};
    ESP_ERROR_CHECK(ledc_timer_pause(LEDC_LOW_SPEED_MODE, turntable_timer_config.timer_num));
    for (size_t channel = 0; channel < sizeof(turntable_pins) / sizeof(*turntable_pins); channel++) {
        ESP_ERROR_CHECK(ledc_set_duty_with_hpoint(LEDC_LOW_SPEED_MODE, turntable_ledc_channels[channel], 6144, turntable_hpoints[channel]));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, turntable_ledc_channels[channel]));
    }
    ESP_ERROR_CHECK(ledc_timer_resume(LEDC_LOW_SPEED_MODE, turntable_timer_config.timer_num));
    turntable_active = true;
    ESP_LOGI(TAG, "turntable turning right");
}

void turntable_off() {
    if (turntable_active) {
        for (size_t channel = 0; channel < sizeof(turntable_pins) / sizeof(*turntable_pins); channel++) {
            ESP_ERROR_CHECK(ledc_stop(LEDC_LOW_SPEED_MODE, turntable_ledc_channels[channel], 0));
            gpio_set_level(turntable_pins[channel], 0);
        }
        ESP_ERROR_CHECK(ledc_timer_pause(LEDC_LOW_SPEED_MODE, turntable_timer_config.timer_num));
        turntable_active = false;
        ESP_LOGI(TAG, "turntable turning stopped");
    }
}

esp_err_t turntable_http_handler(httpd_req_t *req) {
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
                if (strcasecmp(dec_param, "left") == 0) {
                    turntable_left();
                } else if (strcasecmp(dec_param, "right") == 0) {
                    turntable_right();
                } else if (strcasecmp(dec_param, "off") == 0) {
                    turntable_off();
                }
            }
        }
        free(buf);
    }

    httpd_resp_send(req, "OK", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}