#include "ft.h"

static const char *TAG = "ft_ladder";

void ladder_up() {
    gpio_set_level(MC1_L, 0);
    gpio_set_level(MC1_R, 1);
}

void ladder_down() {
    gpio_set_level(MC1_L, 1);
    gpio_set_level(MC1_R, 0);
}

void ladder_off() {
    gpio_set_level(MC1_L, 0);
    gpio_set_level(MC1_R, 0);
}

esp_err_t ladder_http_handler(httpd_req_t *req)
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
                if (strcasecmp(dec_param, "up") == 0) {
                    ladder_up();
                } else if (strcasecmp(dec_param, "down") == 0) {
                    ladder_down();
                } else if (strcasecmp(dec_param, "off") == 0) {
                    ladder_off();
                }
            }
        }
        free(buf);
    }

    httpd_resp_send(req, "OK", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}