#include "ft.h"

static const char *TAG = "ft_drive";

static const int tail_lights_pin = DC;
static const ledc_channel_t tail_lights_ledc_channel = LEDC_CHANNEL_6;

static const ledc_timer_config_t tail_lights_timer_config = {
    .timer_num = LEDC_TIMER_1,
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .duty_resolution = LEDC_TIMER_14_BIT,
    .freq_hz = 500,
    .clk_cfg = LEDC_AUTO_CLK,
};

static const bdc_motor_config_t motor_drive_config = {
    .pwm_freq_hz = 25000,
    .pwma_gpio_num = MA2_L,
    .pwmb_gpio_num = MA2_R
};

static const bdc_motor_mcpwm_config_t motor_drive_mcpwm_config = {
    .group_id = 0,
    .resolution_hz = 10000000
};

bdc_motor_handle_t motor_drive;

void drive_init() {
    ESP_ERROR_CHECK(bdc_motor_new_mcpwm_device(&motor_drive_config, &motor_drive_mcpwm_config, &motor_drive));
    ESP_LOGI(TAG, "Drive motor controller ready");

    ESP_ERROR_CHECK(ledc_timer_config(&tail_lights_timer_config));
    ledc_channel_config_t channel_config = {
        .gpio_num = tail_lights_pin,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = tail_lights_ledc_channel,
        .timer_sel = tail_lights_timer_config.timer_num,
        .duty = 0,
        .hpoint = 0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&channel_config));
    ESP_ERROR_CHECK(ledc_timer_pause(LEDC_LOW_SPEED_MODE, tail_lights_timer_config.timer_num));
    ESP_LOGI(TAG, "Configured LEDC tail lights timer and channels");
}

void drive_tail_lights_on(bool brake) {
        ESP_ERROR_CHECK(ledc_timer_pause(LEDC_LOW_SPEED_MODE, tail_lights_timer_config.timer_num));
        ESP_ERROR_CHECK(ledc_set_duty_with_hpoint(LEDC_LOW_SPEED_MODE, tail_lights_ledc_channel, brake ? (16384-1) : 6144, 0));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, tail_lights_ledc_channel));
        ESP_ERROR_CHECK(ledc_timer_resume(LEDC_LOW_SPEED_MODE, tail_lights_timer_config.timer_num));
}

void drive_tail_lights_off() {
    ESP_ERROR_CHECK(ledc_stop(LEDC_LOW_SPEED_MODE, tail_lights_ledc_channel, 0));
    gpio_set_level(tail_lights_ledc_channel, 0);
    ESP_ERROR_CHECK(ledc_timer_pause(LEDC_LOW_SPEED_MODE, tail_lights_timer_config.timer_num));
}

esp_err_t drive_http_handler(httpd_req_t *req)
{
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
                if (strcasecmp(dec_param, "enable") == 0) {
                    gpio_set_level(DB, 1); /* Enable headlights */
                    drive_tail_lights_on(false); /* Enable tail lights */
                    bdc_motor_enable(motor_drive);
                } else if (strcasecmp(dec_param, "disable") == 0) {
                    gpio_set_level(DB, 0); /* Disable headlights */
                    drive_tail_lights_off(); /* Disable tail lights */
                    bdc_motor_disable(motor_drive);
                } else if (strcasecmp(dec_param, "forward") == 0) {
                    drive_tail_lights_on(false); /* Enable tail lights */
                    bdc_motor_forward(motor_drive);
                } else if (strcasecmp(dec_param, "reverse") == 0) {
                    drive_tail_lights_on(false); /* Enable tail lights */
                    bdc_motor_reverse(motor_drive);
                } else if (strcasecmp(dec_param, "brake") == 0) {
                    drive_tail_lights_on(true); /* Enable tail lights with brake */
                    bdc_motor_brake(motor_drive);
                } else if (strcasecmp(dec_param, "coast") == 0) {
                    drive_tail_lights_on(false); /* Enable tail lights */
                    bdc_motor_coast(motor_drive);
                }   
            }
            if (httpd_query_key_value(buf, "speed", param, sizeof(param)) == ESP_OK) {
                uri_decode(dec_param, param, strnlen(param, CONFIG_HTTPD_MAX_URI_LEN));
                uint32_t speed = atoll(dec_param);
                bdc_motor_set_speed(motor_drive, speed); 
            }
        }
        free(buf);
    }

    httpd_resp_send(req, "OK", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}
