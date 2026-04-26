/*
 * ICM42670 IMU驱动实现
 * 使用新版I2C Master驱动
 */

#include "imu.h"
#include "lvgl_user.h"
#include <string.h>
#include <math.h>
#include "esp_check.h"
#include "esp_rom_sys.h"
#include "esp_timer.h"

#define LOG_TAG "IMU"

#define I2C_MASTER_FREQ_HZ 400000
#define IMU_I2C_ADDR ICM42670_I2C_ADDRESS // 0x68

// 设备ID
#define ICM42607_ID 0x60
#define ICM42670_ID 0x67

// 互补滤波器参数
#define ALPHA 0.97f             /*!< 陀螺仪权重 */
#define RAD_TO_DEG 57.27272727f /*!< 弧度转角度 */

/* 陀螺仪灵敏度 */
#define GYRO_FS_2000_SENSITIVITY (16.4f)
#define GYRO_FS_1000_SENSITIVITY (32.8f)
#define GYRO_FS_500_SENSITIVITY (65.5f)
#define GYRO_FS_250_SENSITIVITY (131.0f)

/* 加速度计灵敏度 */
#define ACCE_FS_16G_SENSITIVITY (2048.0f)
#define ACCE_FS_8G_SENSITIVITY (4096.0f)
#define ACCE_FS_4G_SENSITIVITY (8192.0f)
#define ACCE_FS_2G_SENSITIVITY (16384.0f)

// 翻转检测阈值 (g)，当Y轴加速度超过此值时认为设备翻转
#define FLIP_THRESHOLD 0.7f
// 检测任务周期 (ms)
#define FLIP_DETECTION_PERIOD_MS 500

/*******************************************************************************
 * 类型定义
 ******************************************************************************/

typedef struct
{
    i2c_master_dev_handle_t i2c_handle;
    bool initialized_filter;
    uint64_t previous_measurement_us;
    complimentary_angle_t previous_measurement;
} icm42670_dev_t;

/*******************************************************************************
 * 静态变量
 ******************************************************************************/

static icm42670_handle_t imu_handle = NULL;
static TaskHandle_t flip_detection_task_handle = NULL;

// 当前屏幕旋转状态
static volatile lv_disp_rotation_t current_rotation = LV_DISPLAY_ROTATION_90;

/*******************************************************************************
 * 私有函数声明
 ******************************************************************************/

static esp_err_t icm42670_write(icm42670_handle_t sensor, const uint8_t reg_start_addr, const uint8_t *data_buf, const uint8_t data_len);
static esp_err_t icm42670_read(icm42670_handle_t sensor, const uint8_t reg_start_addr, uint8_t *data_buf, const uint8_t data_len);
static esp_err_t icm42670_get_raw_value(icm42670_handle_t sensor, uint8_t reg, icm42670_raw_value_t *value);

/*******************************************************************************
 * ICM42670 驱动实现
 ******************************************************************************/

esp_err_t icm42670_create(i2c_master_bus_handle_t i2c_bus, const uint8_t dev_addr, icm42670_handle_t *handle_ret)
{
    esp_err_t ret = ESP_OK;

    // 分配内存并初始化驱动对象
    icm42670_dev_t *sensor = (icm42670_dev_t *)calloc(1, sizeof(icm42670_dev_t));
    ESP_RETURN_ON_FALSE(sensor != NULL, ESP_ERR_NO_MEM, LOG_TAG, "Not enough memory");

    // 添加新的I2C设备
    const i2c_device_config_t i2c_dev_cfg = {
        .device_address = dev_addr,
        .scl_speed_hz = I2C_MASTER_FREQ_HZ,
    };
    ESP_GOTO_ON_ERROR(i2c_master_bus_add_device(i2c_bus, &i2c_dev_cfg, &sensor->i2c_handle), err, LOG_TAG,
                      "Failed to add new I2C device");
    assert(sensor->i2c_handle);

    // 检查设备是否存在
    uint8_t dev_id = 0;
    icm42670_get_deviceid(sensor, &dev_id);
    ESP_GOTO_ON_FALSE(dev_id == ICM42607_ID || dev_id == ICM42670_ID, ESP_ERR_NOT_FOUND, err, LOG_TAG,
                      "Incorrect Device ID (0x%02x).", dev_id);

    ESP_LOGI(LOG_TAG, "Found device %s, ID: 0x%02x", (dev_id == ICM42607_ID ? "ICM42607" : "ICM42670"), dev_id);
    *handle_ret = sensor;
    return ret;

err:
    icm42670_delete(sensor);
    return ret;
}

void icm42670_delete(icm42670_handle_t sensor)
{
    icm42670_dev_t *sens = (icm42670_dev_t *)sensor;

    if (sens->i2c_handle)
    {
        i2c_master_bus_rm_device(sens->i2c_handle);
    }

    free(sens);
}

esp_err_t icm42670_get_deviceid(icm42670_handle_t sensor, uint8_t *deviceid)
{
    esp_err_t ret = ESP_FAIL;

    assert(deviceid != NULL);

    for (int i = 0; (i < 5 && ret != ESP_OK); i++)
    {
        ret = icm42670_read(sensor, ICM42670_WHOAMI, deviceid, 1);
    }

    return ret;
}

esp_err_t icm42670_config(icm42670_handle_t sensor, const icm42670_cfg_t *config)
{
    uint8_t data[2];

    assert(config != NULL);

    /* 陀螺仪配置 */
    data[0] = ((config->gyro_fs & 0x03) << 5) | (config->gyro_odr & 0x0F);
    /* 加速度计配置 */
    data[1] = ((config->acce_fs & 0x03) << 5) | (config->acce_odr & 0x0F);

    return icm42670_write(sensor, ICM42670_GYRO_CONFIG0, data, sizeof(data));
}

esp_err_t icm42670_acce_set_pwr(icm42670_handle_t sensor, icm42670_acce_pwr_t state)
{
    esp_err_t ret = ESP_FAIL;
    uint8_t data;

    ret = icm42670_read(sensor, ICM42670_PWR_MGMT0, &data, 1);
    if (ret == ESP_OK)
    {
        data |= (state & 0x03);

        ret = icm42670_write(sensor, ICM42670_PWR_MGMT0, &data, sizeof(data));
    }

    return ret;
}

esp_err_t icm42670_gyro_set_pwr(icm42670_handle_t sensor, icm42670_gyro_pwr_t state)
{
    esp_err_t ret = ESP_FAIL;
    uint8_t data;

    ret = icm42670_read(sensor, ICM42670_PWR_MGMT0, &data, 1);
    if (ret == ESP_OK)
    {
        data |= ((state & 0x03) << 2);

        ret = icm42670_write(sensor, ICM42670_PWR_MGMT0, &data, sizeof(data));
    }

    return ret;
}

esp_err_t icm42670_get_acce_sensitivity(icm42670_handle_t sensor, float *sensitivity)
{
    esp_err_t ret = ESP_FAIL;
    uint8_t acce_fs;

    assert(sensitivity != NULL);

    *sensitivity = 0;

    ret = icm42670_read(sensor, ICM42670_ACCEL_CONFIG0, &acce_fs, 1);
    if (ret == ESP_OK)
    {
        acce_fs = (acce_fs >> 5) & 0x03;
        switch (acce_fs)
        {
        case ACCE_FS_16G:
            *sensitivity = ACCE_FS_16G_SENSITIVITY;
            break;
        case ACCE_FS_8G:
            *sensitivity = ACCE_FS_8G_SENSITIVITY;
            break;
        case ACCE_FS_4G:
            *sensitivity = ACCE_FS_4G_SENSITIVITY;
            break;
        case ACCE_FS_2G:
            *sensitivity = ACCE_FS_2G_SENSITIVITY;
            break;
        }
    }

    return ret;
}

esp_err_t icm42670_get_gyro_sensitivity(icm42670_handle_t sensor, float *sensitivity)
{
    esp_err_t ret = ESP_FAIL;
    uint8_t gyro_fs;

    assert(sensitivity != NULL);

    *sensitivity = 0;

    ret = icm42670_read(sensor, ICM42670_GYRO_CONFIG0, &gyro_fs, 1);
    if (ret == ESP_OK)
    {
        gyro_fs = (gyro_fs >> 5) & 0x03;
        switch (gyro_fs)
        {
        case GYRO_FS_2000DPS:
            *sensitivity = GYRO_FS_2000_SENSITIVITY;
            break;
        case GYRO_FS_1000DPS:
            *sensitivity = GYRO_FS_1000_SENSITIVITY;
            break;
        case GYRO_FS_500DPS:
            *sensitivity = GYRO_FS_500_SENSITIVITY;
            break;
        case GYRO_FS_250DPS:
            *sensitivity = GYRO_FS_250_SENSITIVITY;
            break;
        }
    }

    return ret;
}

esp_err_t icm42670_get_temp_raw_value(icm42670_handle_t sensor, uint16_t *value)
{
    esp_err_t ret = ESP_FAIL;
    uint8_t data[2];

    assert(value != NULL);

    *value = 0;

    ret = icm42670_read(sensor, ICM42670_TEMP_DATA, data, sizeof(data));
    if (ret == ESP_OK)
    {
        *value = (uint16_t)((data[0] << 8) + data[1]);
    }

    return ret;
}

esp_err_t icm42670_get_acce_raw_value(icm42670_handle_t sensor, icm42670_raw_value_t *value)
{
    return icm42670_get_raw_value(sensor, ICM42670_ACCEL_DATA, value);
}

esp_err_t icm42670_get_gyro_raw_value(icm42670_handle_t sensor, icm42670_raw_value_t *value)
{
    return icm42670_get_raw_value(sensor, ICM42670_GYRO_DATA, value);
}

esp_err_t icm42670_get_acce_value(icm42670_handle_t sensor, icm42670_value_t *value)
{
    esp_err_t ret;
    float sensitivity;
    icm42670_raw_value_t raw_value;

    assert(value != NULL);

    value->x = 0;
    value->y = 0;
    value->z = 0;

    ret = icm42670_get_acce_sensitivity(sensor, &sensitivity);
    ESP_RETURN_ON_ERROR(ret, LOG_TAG, "Get sensitivity error!");

    ret = icm42670_get_acce_raw_value(sensor, &raw_value);
    ESP_RETURN_ON_ERROR(ret, LOG_TAG, "Get raw value error!");

    value->x = raw_value.x / sensitivity;
    value->y = raw_value.y / sensitivity;
    value->z = raw_value.z / sensitivity;

    return ESP_OK;
}

esp_err_t icm42670_get_gyro_value(icm42670_handle_t sensor, icm42670_value_t *value)
{
    esp_err_t ret;
    float sensitivity;
    icm42670_raw_value_t raw_value;

    assert(value != NULL);

    value->x = 0;
    value->y = 0;
    value->z = 0;

    ret = icm42670_get_gyro_sensitivity(sensor, &sensitivity);
    ESP_RETURN_ON_ERROR(ret, LOG_TAG, "Get sensitivity error!");

    ret = icm42670_get_gyro_raw_value(sensor, &raw_value);
    ESP_RETURN_ON_ERROR(ret, LOG_TAG, "Get raw value error!");

    value->x = raw_value.x / sensitivity;
    value->y = raw_value.y / sensitivity;
    value->z = raw_value.z / sensitivity;

    return ESP_OK;
}

esp_err_t icm42670_get_temp_value(icm42670_handle_t sensor, float *value)
{
    esp_err_t ret;
    uint16_t raw_value;

    assert(value != NULL);

    *value = 0;

    ret = icm42670_get_temp_raw_value(sensor, &raw_value);
    ESP_RETURN_ON_ERROR(ret, LOG_TAG, "Get raw value error!");

    *value = ((int16_t)raw_value / 128.0f) + 25.0f;

    return ESP_OK;
}

esp_err_t icm42670_complimentory_filter(icm42670_handle_t sensor, const icm42670_value_t *const acce_value,
                                        const icm42670_value_t *const gyro_value, complimentary_angle_t *const complimentary_angle)
{
    icm42670_dev_t *sens = (icm42670_dev_t *)sensor;
    float measurement_delta;
    uint64_t current_time_us;
    float acc_roll_angle;
    float acc_pitch_angle;
    float gyro_roll_angle;
    float gyro_pitch_angle;

    acc_roll_angle = (atan2(acce_value->y,
                            sqrt(acce_value->x * acce_value->x + acce_value->z * acce_value->z)) *
                      RAD_TO_DEG);
    acc_pitch_angle = (atan2(-acce_value->x,
                             sqrt(acce_value->y * acce_value->y + acce_value->z * acce_value->z)) *
                       RAD_TO_DEG);

    if (!sens->initialized_filter)
    {
        sens->initialized_filter = true;
        sens->previous_measurement_us = esp_timer_get_time();
        sens->previous_measurement.roll = acc_roll_angle;
        sens->previous_measurement.pitch = acc_pitch_angle;
    }

    current_time_us = esp_timer_get_time();
    measurement_delta = (current_time_us - sens->previous_measurement_us) / 1000000.0f;
    sens->previous_measurement_us = current_time_us;

    gyro_roll_angle = gyro_value->x * measurement_delta;
    gyro_pitch_angle = gyro_value->y * measurement_delta;

    complimentary_angle->roll = (ALPHA * (sens->previous_measurement.roll + gyro_roll_angle)) + ((1 - ALPHA) * acc_roll_angle);
    complimentary_angle->pitch = (ALPHA * (sens->previous_measurement.pitch + gyro_pitch_angle)) + ((1 - ALPHA) * acc_pitch_angle);

    sens->previous_measurement.roll = complimentary_angle->roll;
    sens->previous_measurement.pitch = complimentary_angle->pitch;

    return ESP_OK;
}

esp_err_t icm42670_read_register(icm42670_handle_t sensor, uint8_t reg, uint8_t *val)
{
    return icm42670_read(sensor, reg, val, 1);
}

esp_err_t icm42670_write_register(icm42670_handle_t sensor, uint8_t reg, uint8_t val)
{
    return icm42670_write(sensor, reg, &val, 1);
}

esp_err_t icm42670_read_mreg_register(icm42670_handle_t sensor, uint8_t mreg, uint8_t reg, uint8_t *val)
{
    uint8_t blk_sel_r = 0;
    if (mreg == 1)
    {
        blk_sel_r = 0;
    }
    else if (mreg == 2)
    {
        blk_sel_r = 0x28;
    }
    else if (mreg == 3)
    {
        blk_sel_r = 0x50;
    }
    else
    {
        ESP_LOGE(LOG_TAG, "Invalid MREG value %d", mreg);
        return ESP_ERR_INVALID_ARG;
    }
    ESP_RETURN_ON_ERROR(icm42670_write(sensor, ICM42670_BLK_SEL_R, &blk_sel_r, 1), LOG_TAG,
                        "Failed to set BLK_SEL_R");
    ESP_RETURN_ON_ERROR(icm42670_write(sensor, ICM42670_MADDR_R, &reg, 1), LOG_TAG,
                        "Failed to set MADDR_R");

    esp_rom_delay_us(10);

    ESP_RETURN_ON_ERROR(icm42670_read(sensor, ICM42670_M_R, val, 1), LOG_TAG, "Failed to read M_R");

    esp_rom_delay_us(10);

    return ESP_OK;
}

esp_err_t icm42670_write_mreg_register(icm42670_handle_t sensor, uint8_t mreg, uint8_t reg, uint8_t val)
{
    uint8_t blk_sel_w = 0;
    if (mreg == 1)
    {
        blk_sel_w = 0;
    }
    else if (mreg == 2)
    {
        blk_sel_w = 0x28;
    }
    else if (mreg == 3)
    {
        blk_sel_w = 0x50;
    }
    else
    {
        ESP_LOGE(LOG_TAG, "Invalid MREG value %d", mreg);
        return ESP_ERR_INVALID_ARG;
    }
    ESP_RETURN_ON_ERROR(icm42670_write(sensor, ICM42670_BLK_SEL_W, &blk_sel_w, 1), LOG_TAG,
                        "Failed to set BLK_SEL_W");
    ESP_RETURN_ON_ERROR(icm42670_write(sensor, ICM42670_MADDR_W, &reg, 1), LOG_TAG,
                        "Failed to set MADDR_W");
    ESP_RETURN_ON_ERROR(icm42670_write(sensor, ICM42670_M_W, &val, 1), LOG_TAG, "Failed to set M_W");

    esp_rom_delay_us(10);

    return ESP_OK;
}

/*******************************************************************************
 * 私有函数实现
 ******************************************************************************/

static esp_err_t icm42670_get_raw_value(icm42670_handle_t sensor, uint8_t reg, icm42670_raw_value_t *value)
{
    esp_err_t ret = ESP_FAIL;
    uint8_t data[6];

    assert(value != NULL);

    value->x = 0;
    value->y = 0;
    value->z = 0;

    ret = icm42670_read(sensor, reg, data, sizeof(data));
    if (ret == ESP_OK)
    {
        value->x = (int16_t)((data[0] << 8) + data[1]);
        value->y = (int16_t)((data[2] << 8) + data[3]);
        value->z = (int16_t)((data[4] << 8) + data[5]);
    }

    return ret;
}

static esp_err_t icm42670_write(icm42670_handle_t sensor, const uint8_t reg_start_addr, const uint8_t *data_buf, const uint8_t data_len)
{
    icm42670_dev_t *sens = (icm42670_dev_t *)sensor;
    assert(sens);

    assert(data_len < 5);
    uint8_t write_buff[5] = {reg_start_addr};
    memcpy(&write_buff[1], data_buf, data_len);
    return i2c_master_transmit(sens->i2c_handle, write_buff, data_len + 1, -1);
}

static esp_err_t icm42670_read(icm42670_handle_t sensor, const uint8_t reg_start_addr, uint8_t *data_buf, const uint8_t data_len)
{
    uint8_t reg_buff[] = {reg_start_addr};
    icm42670_dev_t *sens = (icm42670_dev_t *)sensor;
    assert(sens);

    /* 写入寄存器号并读取数据 */
    return i2c_master_transmit_receive(sens->i2c_handle, reg_buff, sizeof(reg_buff), data_buf, data_len, -1);
}

/*******************************************************************************
 * IMU 高级API实现
 ******************************************************************************/

esp_err_t imu_init(i2c_port_num_t i2c_port_num)
{
    i2c_master_bus_handle_t bus_handle = NULL;
    esp_err_t ret = i2c_master_get_bus_handle(i2c_port_num, &bus_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Failed to get I2C bus handle: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = icm42670_create(bus_handle, IMU_I2C_ADDR, &imu_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Failed to create ICM42670 device: %s", esp_err_to_name(ret));
        return ret;
    }

    // 获取设备ID验证通信
    uint8_t device_id = 0;
    ret = icm42670_get_deviceid(imu_handle, &device_id);
    if (ret != ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Failed to get device ID: %s", esp_err_to_name(ret));
        icm42670_delete(imu_handle);
        imu_handle = NULL;
        return ret;
    }
    ESP_LOGI(LOG_TAG, "ICM42670 device ID: 0x%02X", device_id);

    // 配置加速度计
    icm42670_cfg_t config = {
        .acce_fs = ACCE_FS_2G,     // ±2g 量程
        .acce_odr = ACCE_ODR_50HZ, // 50Hz 采样率
        .gyro_fs = GYRO_FS_250DPS, // 陀螺仪量程（翻转检测不需要，但需要配置）
        .gyro_odr = GYRO_ODR_50HZ,
    };

    ret = icm42670_config(imu_handle, &config);
    if (ret != ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Failed to configure ICM42670: %s", esp_err_to_name(ret));
        icm42670_delete(imu_handle);
        imu_handle = NULL;
        return ret;
    }

    // 开启加速度计
    ret = icm42670_acce_set_pwr(imu_handle, ACCE_PWR_LOWNOISE);
    if (ret != ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Failed to set accelerometer power: %s", esp_err_to_name(ret));
        icm42670_delete(imu_handle);
        imu_handle = NULL;
        return ret;
    }

    // 关闭陀螺仪以节省功耗
    ret = icm42670_gyro_set_pwr(imu_handle, GYRO_PWR_OFF);
    if (ret != ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Failed to set gyroscope power: %s", esp_err_to_name(ret));
    }

    ESP_LOGI(LOG_TAG, "ICM42670 initialized successfully");
    return ESP_OK;
}

esp_err_t imu_deinit(void)
{
    imu_stop_flip_detection_task();

    if (imu_handle != NULL)
    {
        icm42670_delete(imu_handle);
        imu_handle = NULL;
        ESP_LOGI(LOG_TAG, "ICM42670 de-initialized");
    }
    return ESP_OK;
}

icm42670_handle_t imu_get_handle(void)
{
    return imu_handle;
}

/**
 * @brief 根据Y轴加速度判断屏幕旋转方向
 *
 * @param acce_y Y轴加速度值 (g)
 * @return lv_disp_rotation_t 建议的旋转方向
 */
static lv_disp_rotation_t get_rotation_from_acce_y(float acce_y)
{
    // 当Y轴加速度 > FLIP_THRESHOLD 时，设备正向朝上
    // 当Y轴加速度 < -FLIP_THRESHOLD 时，设备反向朝上（翻转180度）
    if (acce_y > FLIP_THRESHOLD)
    {
        return LV_DISPLAY_ROTATION_90; // 正常方向
    }
    else if (acce_y < -FLIP_THRESHOLD)
    {
        return LV_DISPLAY_ROTATION_270; // 翻转180度
    }
    // 在阈值范围内，保持当前方向
    return current_rotation;
}

/**
 * @brief 翻转检测任务
 *
 * @param args 任务参数（未使用）
 */
static void flip_detection_task(void *args)
{
    icm42670_value_t acce_value;
    esp_err_t ret;

    // ESP_LOGI(LOG_TAG, "Flip detection task started");

    while (1)
    {
        if (imu_handle == NULL)
        {
            vTaskDelay(pdMS_TO_TICKS(FLIP_DETECTION_PERIOD_MS));
            continue;
        }

        // 读取加速度计数据
        ret = icm42670_get_acce_value(imu_handle, &acce_value);
        if (ret != ESP_OK)
        {
            ESP_LOGW(LOG_TAG, "Failed to read accelerometer: %s", esp_err_to_name(ret));
            vTaskDelay(pdMS_TO_TICKS(FLIP_DETECTION_PERIOD_MS));
            continue;
        }

        // 根据Y轴加速度判断旋转方向
        lv_disp_rotation_t new_rotation = get_rotation_from_acce_y(acce_value.y);

        // 如果旋转方向发生变化，更新LVGL显示
        if (new_rotation != current_rotation)
        {
            // ESP_LOGI(LOG_TAG, "Flip detected! Y=%.2f g, rotation changing from %d to %d",
                     acce_value.y, current_rotation, new_rotation);

            current_rotation = new_rotation;

            // 更新LVGL显示旋转
            lvgl_user_set_rotation(current_rotation);
            ESP_LOGI(LOG_TAG, "LVGL display rotation updated to %d", current_rotation);
        }

        vTaskDelay(pdMS_TO_TICKS(FLIP_DETECTION_PERIOD_MS));
    }
}

void imu_start_flip_detection_task(void)
{
    if (flip_detection_task_handle == NULL)
    {
        xTaskCreate(flip_detection_task, "imu_flip_detection", 4096, NULL, 3, &flip_detection_task_handle);
        // ESP_LOGI(LOG_TAG, "Flip detection task created");
    }
}

void imu_stop_flip_detection_task(void)
{
    if (flip_detection_task_handle != NULL)
    {
        vTaskDelete(flip_detection_task_handle);
        flip_detection_task_handle = NULL;
        // ESP_LOGI(LOG_TAG, "Flip detection task stopped");
    }
}
