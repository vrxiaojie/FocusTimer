#include "aw96103.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "driver/i2c_master.h"
#include "esp_log.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "pinmap.h"

static const char *TAG = "AW96103";

#define KEY_COUNT 3

// AW96103 register addresses
#define REG_SCANCTRL0 0x0000
#define REG_SCANCTRL1 0x0004
#define REG_AFECFG0_CH0 0x0010
#define REG_AFECFG0_CH1 0x0024
#define REG_AFECFG0_CH2 0x0038
#define REG_PROXTH0_CH0 0x00B8
#define REG_PROXTH0_CH1 0x00F4
#define REG_PROXTH0_CH2 0x0130
#define REG_STAT0 0x0090
#define REG_CHINTEN 0x009C
#define REG_CMD 0xF008
#define REG_IRQSRC 0xF080
#define REG_IRQEN 0xF084
#define REG_RESET 0xFF0C

QueueHandle_t touch_int_evt_queue;
static aw96103_config_t s_cfg;
static bool s_started;
static aw96103_key_event_cb_t s_key_event_cb;
static void *s_key_event_user_ctx;

void aw96103_register_key_event_cb(aw96103_key_event_cb_t cb, void *user_ctx)
{
    s_key_event_cb = cb;
    s_key_event_user_ctx = user_ctx;
}

static esp_err_t aw96103_write_reg(uint16_t reg_addr, uint32_t data)
{
    uint8_t write_buf[6];

    write_buf[0] = (reg_addr >> 8) & 0xFF;
    write_buf[1] = reg_addr & 0xFF;
    write_buf[2] = (data >> 24) & 0xFF;
    write_buf[3] = (data >> 16) & 0xFF;
    write_buf[4] = (data >> 8) & 0xFF;
    write_buf[5] = data & 0xFF;

    return i2c_master_transmit(s_cfg.handle, write_buf, sizeof(write_buf), 1000);
}

static esp_err_t aw96103_read_reg(uint16_t reg_addr, uint32_t *data)
{
    uint8_t reg_buf[2];
    uint8_t read_buf[4];

    reg_buf[0] = (reg_addr >> 8) & 0xFF;
    reg_buf[1] = reg_addr & 0xFF;

    esp_err_t err = i2c_master_transmit_receive(s_cfg.handle, reg_buf, sizeof(reg_buf), read_buf, sizeof(read_buf), 1000);

    if (err == ESP_OK)
    {
        *data = ((uint32_t)read_buf[0] << 24) |
                ((uint32_t)read_buf[1] << 16) |
                ((uint32_t)read_buf[2] << 8) |
                (uint32_t)read_buf[3];
    }

    return err;
}

static esp_err_t aw96103_chip_init(void)
{
    uint32_t val = 0;

    ESP_RETURN_ON_ERROR(aw96103_write_reg(REG_RESET, 0x00000000), TAG, "reset failed");
    vTaskDelay(pdMS_TO_TICKS(30));

    ESP_RETURN_ON_ERROR(aw96103_write_reg(REG_IRQEN, 0x00000046), TAG, "write IRQEN failed");
    ESP_RETURN_ON_ERROR(aw96103_write_reg(REG_CHINTEN, 0x0F0F0F00), TAG, "write CHINTEN failed");

    ESP_RETURN_ON_ERROR(aw96103_read_reg(REG_AFECFG0_CH0, &val), TAG, "read CH0 cfg failed");
    ESP_RETURN_ON_ERROR(aw96103_write_reg(REG_AFECFG0_CH0, (val & ~0x03U) | 0x01U), TAG, "write CH0 cfg failed");

    ESP_RETURN_ON_ERROR(aw96103_read_reg(REG_AFECFG0_CH1, &val), TAG, "read CH1 cfg failed");
    ESP_RETURN_ON_ERROR(aw96103_write_reg(REG_AFECFG0_CH1, (val & ~0x0CU) | 0x04U), TAG, "write CH1 cfg failed");

    ESP_RETURN_ON_ERROR(aw96103_read_reg(REG_AFECFG0_CH2, &val), TAG, "read CH2 cfg failed");
    ESP_RETURN_ON_ERROR(aw96103_write_reg(REG_AFECFG0_CH2, (val & ~0x30U) | 0x10U), TAG, "write CH2 cfg failed");

    ESP_RETURN_ON_ERROR(aw96103_write_reg(REG_PROXTH0_CH0, s_cfg.touch_threshold), TAG, "write CH0 threshold failed");
    ESP_RETURN_ON_ERROR(aw96103_write_reg(REG_PROXTH0_CH1, s_cfg.touch_threshold), TAG, "write CH1 threshold failed");
    ESP_RETURN_ON_ERROR(aw96103_write_reg(REG_PROXTH0_CH2, s_cfg.touch_threshold), TAG, "write CH2 threshold failed");

    ESP_RETURN_ON_ERROR(aw96103_read_reg(REG_SCANCTRL0, &val), TAG, "read SCANCTRL0 failed");
    val = (val & ~0x0FFFU) | 0x0707U;
    ESP_RETURN_ON_ERROR(aw96103_write_reg(REG_SCANCTRL0, val), TAG, "write SCANCTRL0 failed");

    // 设置扫描周期和doze mode间隔
    ESP_RETURN_ON_ERROR(aw96103_read_reg(REG_SCANCTRL1, &val), TAG, "read SCANCTRL1 failed");
    val = (val & 0xFFFF0000U) | (s_cfg.doze_mode_interval << 11) |(s_cfg.scan_period / 2);
    ESP_RETURN_ON_ERROR(aw96103_write_reg(REG_SCANCTRL1, val), TAG, "write SCANCTRL1 failed");

    ESP_RETURN_ON_ERROR(aw96103_write_reg(REG_CMD, 0x00000001), TAG, "switch to active mode failed");

    return ESP_OK;
}

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)(uintptr_t)arg;
    BaseType_t high_task_woken = pdFALSE;
    xQueueSendFromISR(touch_int_evt_queue, &gpio_num, &high_task_woken);
    if (high_task_woken == pdTRUE)
    {
        portYIELD_FROM_ISR();
    }
}

static void touch_task(void *arg)
{
    (void)arg;

    uint32_t io_num;
    uint32_t irq_src;
    uint32_t stat0;
    bool key_pressed[KEY_COUNT] = {false};

    while (1)
    {
        if (xQueueReceive(touch_int_evt_queue, &io_num, portMAX_DELAY))
        {
            (void)io_num;

            if (aw96103_read_reg(REG_IRQSRC, &irq_src) != ESP_OK)
            {
                ESP_LOGW(TAG, "read IRQSRC failed");
                continue;
            }

            if (aw96103_read_reg(REG_STAT0, &stat0) != ESP_OK)
            {
                ESP_LOGW(TAG, "read STAT0 failed");
                continue;
            }

            uint8_t touch_status = (stat0 >> 24) & 0x0F;

            for (int key = 0; key < KEY_COUNT; key++)
            {
                bool is_now_pressed = (touch_status & (1U << key)) != 0;

                if (is_now_pressed && !key_pressed[key])
                {
                    key_pressed[key] = true;
                    if (s_key_event_cb) {
                        s_key_event_cb((uint8_t)key, true, s_key_event_user_ctx);
                    }
                }

                if (!is_now_pressed && key_pressed[key])
                {
                    key_pressed[key] = false;
                    if (s_key_event_cb) {
                        s_key_event_cb((uint8_t)key, false, s_key_event_user_ctx);
                    }
                }
            }
        }
    }
}

esp_err_t aw96103_init()
{
    s_cfg.i2c_port = I2C_NUM_0;
    s_cfg.int_io = TOUCH_INT_PIN;
    s_cfg.i2c_addr = AW96103_I2C_ADDR;
    s_cfg.i2c_clk_hz = 400000;
    s_cfg.touch_threshold = 500000;
    s_cfg.task_stack_size = 4096;
    s_cfg.task_priority = 10;
    s_cfg.scan_period = 50;
    s_cfg.doze_mode_interval = 1; // 设定为1，实际进入doze mode的时间为1*4=4ms

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = s_cfg.i2c_addr,
        .scl_speed_hz = s_cfg.i2c_clk_hz,
    };
    
    i2c_master_bus_handle_t bus_handle;
    i2c_master_get_bus_handle(I2C_NUM_0, &bus_handle);
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &s_cfg.handle));

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << s_cfg.int_io),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE,
    };
    ESP_RETURN_ON_ERROR(gpio_config(&io_conf), TAG, "gpio_config failed");

    touch_int_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    if (touch_int_evt_queue == NULL)
    {
        return ESP_ERR_NO_MEM;
    }

    BaseType_t task_ok = xTaskCreate(
        touch_task,
        "aw96103_touch_task",
        s_cfg.task_stack_size,
        NULL,
        s_cfg.task_priority,
        NULL);
    if (task_ok != pdPASS)
    {
        return ESP_ERR_NO_MEM;
    }

    esp_err_t err = gpio_install_isr_service(0);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE)
    {
        return err;
    }

    ESP_RETURN_ON_ERROR(gpio_isr_handler_add(s_cfg.int_io, gpio_isr_handler, (void *)(uintptr_t)s_cfg.int_io), TAG,
                        "gpio_isr_handler_add failed");

    ESP_RETURN_ON_ERROR(aw96103_chip_init(), TAG, "aw96103 init failed");

    s_started = true;
    ESP_LOGI(TAG, "AW96103 initialized. Waiting for touch...");
    return ESP_OK;
}
