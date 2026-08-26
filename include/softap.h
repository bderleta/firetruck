#pragma once

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#define ESP_WIFI_SSID "ToyControl"
#define ESP_WIFI_CHANNEL 6
#define ESP_WIFI_PASS "12345678"
#define ESP_WIFI_MAX_STA_CONN 8
#define ESP_WIFI_GTK_REKEY_INTERVAL 0

void wifi_init(void);
void wifi_init_softap(void);