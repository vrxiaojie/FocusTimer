#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"
#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PCF85263A_I2C_ADDRESS_DEFAULT (0x51)

typedef struct {
    uint16_t year;   /* 2000-2099 */
    uint8_t month;   /* 1-12 */
    uint8_t day;     /* 1-31 */
    uint8_t weekday; /* 0-6 */
    uint8_t hour;    /* 0-23 */
    uint8_t minute;  /* 0-59 */
    uint8_t second;  /* 0-59 */
} pcf85263a_datetime_t;

typedef struct {
    uint8_t second; /* 0-59 */
    uint8_t minute; /* 0-59 */
    uint8_t hour;   /* 0-23 */
    uint8_t day;    /* 1-31 */
    uint8_t month;  /* 1-12 */

    bool match_second;
    bool match_minute;
    bool match_hour;
    bool match_day;
    bool match_month;
} pcf85263a_alarm1_t;

typedef enum {
    PCF85263A_INTA_MODE_CLK_BAT = 0,
    PCF85263A_INTA_MODE_BATTERY = 1,
    PCF85263A_INTA_MODE_INTERRUPT = 2,
    PCF85263A_INTA_MODE_HIGH_IMPEDANCE = 3,
} pcf85263a_inta_mode_t;

typedef struct pcf85263a_dev_t *pcf85263a_handle_t;

typedef enum {
    PCF85263A_FLAG_TSR1F = (1U << 0),
    PCF85263A_FLAG_TSR2F = (1U << 1),
    PCF85263A_FLAG_TSR3F = (1U << 2),
    PCF85263A_FLAG_BSF = (1U << 3),
    PCF85263A_FLAG_WDF = (1U << 4),
    PCF85263A_FLAG_A1F = (1U << 5),
    PCF85263A_FLAG_A2F = (1U << 6),
    PCF85263A_FLAG_PIF = (1U << 7),
} pcf85263a_flag_bits_t;

typedef enum {
    PCF85263A_INTA_WDIE = (1U << 0),
    PCF85263A_INTA_BSIE = (1U << 1),
    PCF85263A_INTA_TSRIE = (1U << 2),
    PCF85263A_INTA_A2IE = (1U << 3),
    PCF85263A_INTA_A1IE = (1U << 4),
    PCF85263A_INTA_OIE = (1U << 5),
    PCF85263A_INTA_PIE = (1U << 6),
    PCF85263A_INTA_ILP = (1U << 7),
} pcf85263a_inta_bits_t;

esp_err_t pcf85263a_init(i2c_port_num_t port_num);
esp_err_t pcf85263a_deinit(void);

pcf85263a_handle_t pcf85263a_get_handle(void);

esp_err_t pcf85263a_set_datetime(pcf85263a_handle_t handle, const pcf85263a_datetime_t *dt);
esp_err_t pcf85263a_get_datetime(pcf85263a_handle_t handle, pcf85263a_datetime_t *dt);

esp_err_t pcf85263a_set_alarm1(pcf85263a_handle_t handle, const pcf85263a_alarm1_t *alarm);
esp_err_t pcf85263a_enable_alarm1_interrupt(pcf85263a_handle_t handle, bool enable);
esp_err_t pcf85263a_set_inta_mask(pcf85263a_handle_t handle, uint8_t mask, bool enable);
esp_err_t pcf85263a_get_inta_mask(pcf85263a_handle_t handle, uint8_t *mask);

esp_err_t pcf85263a_get_flags(pcf85263a_handle_t handle, uint8_t *flags);
esp_err_t pcf85263a_clear_flags(pcf85263a_handle_t handle, uint8_t flag_mask);

esp_err_t pcf85263a_set_inta_mode(pcf85263a_handle_t handle, pcf85263a_inta_mode_t mode);

#ifdef __cplusplus
}
#endif
