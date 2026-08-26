#pragma once

#include <driver/gpio.h>
#include <esp_log.h>
#include <driver/ledc.h>
#include <freertos/FreeRTOS.h>

#include "bdc_motor.h"
#include "bdc_motor_interface.h"
#include "httpd.h"

/* MA driver (MX1616H, 1.6A cont. single, 1.3A cont. double, 3A peak) */
#define MA1_R 6
#define MA1_L 7
#define MA2_R 4
#define MA2_L 5

/* MB driver (MX1616H, 1.6A cont. single, 1.3A cont. double, 3A peak) */
#define MB1_R 11
#define MB1_L 10
#define MB2_R 13
#define MB2_L 12

/* MC driver (TC118S, 1.8A cont., 2.5A peak) */
#define MC1_R 21
#define MC1_L 14

/* MD driver (MX1616H, 1.6A cont. single, 1.3A cont. double, 3A peak) */
#define MD1_R 41
#define MD1_L 42
#define MD2_R 39
#define MD2_L 40

/* DA/DD driver (IRFML8244TRPbF, 4A) */
#define DA 45
#define DD 46

/* DB/DC driver (DMN2310U, 1.2A)*/
#define DB 48
#define DC 47

/* LED pin (active LOW) */
#define LED 3

/* Custom IO pins */
#define IO1 1
#define IO2 2
#define IO8 8
#define IO9 9

void board_init();
void led_set(bool active);

void ladder_up();
void ladder_down();
void ladder_off();
esp_err_t ladder_http_handler(httpd_req_t *req);

void steer_left();
void steer_right();
void steer_off();
esp_err_t steer_http_handler(httpd_req_t *req);

void pump_on();
void pump_off();
esp_err_t pump_http_handler(httpd_req_t *req);

void turntable_init();
void turntable_left();
void turntable_right();
void turntable_off();
esp_err_t turntable_http_handler(httpd_req_t *req);

void emergency_lights_init();
void emergency_lights_on();
void emergency_lights_off();
esp_err_t emergency_lights_http_handler(httpd_req_t *req);

void drive_init();
esp_err_t drive_http_handler(httpd_req_t *req);



