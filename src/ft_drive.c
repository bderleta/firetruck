#include "ft.h"

static const char *TAG = "ft_drive";

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
                    gpio_set_level(DB, 1);
                    bdc_motor_enable(motor_drive);
                } else if (strcasecmp(dec_param, "disable") == 0) {
                    gpio_set_level(DB, 0);
                    bdc_motor_disable(motor_drive);
                } else if (strcasecmp(dec_param, "forward") == 0) {
                    bdc_motor_forward(motor_drive);
                } else if (strcasecmp(dec_param, "reverse") == 0) {
                    bdc_motor_reverse(motor_drive);
                } else if (strcasecmp(dec_param, "brake") == 0) {
                    bdc_motor_brake(motor_drive);
                } else if (strcasecmp(dec_param, "coast") == 0) {
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
