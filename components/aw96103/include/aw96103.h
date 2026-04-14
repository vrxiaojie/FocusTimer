#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"

#define AW96103_I2C_ADDR 0x12 // default
#define AW96103_I2C_ADDR_GND 0x13
#define AW96103_I2C_ADDR_VCC 0x14

typedef struct
{
    i2c_master_dev_handle_t handle;
    i2c_port_t i2c_port;
    gpio_num_t int_io;
    uint8_t i2c_addr;
    uint32_t i2c_clk_hz;
    uint32_t touch_threshold;
    uint32_t long_press_threshold_ms;
    uint32_t task_stack_size;
    UBaseType_t task_priority;
} aw96103_config_t;

extern QueueHandle_t touch_int_evt_queue;

typedef void (*aw96103_key_event_cb_t)(uint8_t key_index, bool pressed, void *user_ctx);

esp_err_t aw96103_init();
void aw96103_register_key_event_cb(aw96103_key_event_cb_t cb, void *user_ctx);
