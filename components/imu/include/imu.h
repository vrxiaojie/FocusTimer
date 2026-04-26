#pragma once

#include "esp_log.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lvgl.h"

/*******************************************************************************
 * ICM42670 常量定义
 ******************************************************************************/

#define ICM42670_I2C_ADDRESS         0x68 /*!< I2C address with AD0 pin low */
#define ICM42670_I2C_ADDRESS_1       0x69 /*!< I2C address with AD0 pin high */

/*******************************************************************************
 * ICM42670 寄存器地址
 ******************************************************************************/

#define ICM42670_SIGNAL_PATH_RESET   0x02 /*!< Signal path reset */
#define ICM42670_INT_CONFIG          0x06 /*!< Interrupt configuration */
#define ICM42670_PWR_MGMT0           0x1F /*!< Power management 0 */
#define ICM42670_GYRO_CONFIG0        0x20 /*!< Gyroscope configuration 0 */
#define ICM42670_ACCEL_CONFIG0       0x21 /*!< Accelerometer configuration 0 */
#define ICM42670_TEMP_CONFIG         0x22 /*!< Temperature configuration */
#define ICM42670_APEX_CONFIG0        0x25 /*!< APEX configuration 0 */
#define ICM42670_APEX_CONFIG1        0x26 /*!< APEX configuration 1 */
#define ICM42670_WOM_CONFIG          0x27 /*!< Wake on Motion configuration */
#define ICM42670_INT_SOURCE0         0x2B /*!< Interrupt source 0 */
#define ICM42670_INT_SOURCE1         0x2C /*!< Interrupt source 1 */
#define ICM42670_INTF_CONFIG0        0x35 /*!< Interface configuration 0 */
#define ICM42670_INTF_CONFIG1        0x36 /*!< Interface configuration 1 */
#define ICM42670_INT_STATUS          0x3A /*!< Interrupt status */
#define ICM42670_INT_STATUS2         0x3B /*!< Interrupt status 2 */
#define ICM42670_INT_STATUS3         0x3C /*!< Interrupt status 3 */
#define ICM42670_TEMP_DATA           0x09 /*!< Temperature data */
#define ICM42670_ACCEL_DATA          0x0B /*!< Accelerometer data */
#define ICM42670_GYRO_DATA           0x11 /*!< Gyroscope data */
#define ICM42670_WHOAMI              0x75 /*!< Who am I */
#define ICM42670_BLK_SEL_W           0x79 /*! Select MREG1, MREG2, or MREG3 bank for writing */
#define ICM42670_MADDR_W             0x7A /*! Set MREG* register address for writing */
#define ICM42670_M_W                 0x7B /*! Write MREG* register value */
#define ICM42670_BLK_SEL_R           0x7C /*! Select MREG1, MREG2, or MREG3 bank for reading */
#define ICM42670_MADDR_R             0x7D /*! Set MREG* register address for reading */
#define ICM42670_M_R                 0x7E /*! Read MREG* register value */

/*******************************************************************************
 * 类型定义
 ******************************************************************************/

typedef enum {
    ACCE_FS_16G = 0,     /*!< Accelerometer full scale range is +/- 16g */
    ACCE_FS_8G  = 1,     /*!< Accelerometer full scale range is +/- 8g */
    ACCE_FS_4G  = 2,     /*!< Accelerometer full scale range is +/- 4g */
    ACCE_FS_2G  = 3,     /*!< Accelerometer full scale range is +/- 2g */
} icm42670_acce_fs_t;

typedef enum {
    ACCE_PWR_OFF      = 0,     /*!< Accelerometer power off state */
    ACCE_PWR_ON       = 1,     /*!< Accelerometer power on state */
    ACCE_PWR_LOWPOWER = 2,     /*!< Accelerometer low-power mode */
    ACCE_PWR_LOWNOISE = 3,     /*!< Accelerometer low noise state */
} icm42670_acce_pwr_t;

typedef enum {
    ACCE_ODR_1600HZ   = 5,  /*!< Accelerometer ODR 1.6 kHz */
    ACCE_ODR_800HZ    = 6,  /*!< Accelerometer ODR 800 Hz */
    ACCE_ODR_400HZ    = 7,  /*!< Accelerometer ODR 400 Hz */
    ACCE_ODR_200HZ    = 8,  /*!< Accelerometer ODR 200 Hz */
    ACCE_ODR_100HZ    = 9,  /*!< Accelerometer ODR 100 Hz */
    ACCE_ODR_50HZ     = 10, /*!< Accelerometer ODR 50 Hz */
    ACCE_ODR_25HZ     = 11, /*!< Accelerometer ODR 25 Hz */
    ACCE_ODR_12_5HZ   = 12, /*!< Accelerometer ODR 12.5 Hz */
    ACCE_ODR_6_25HZ   = 13, /*!< Accelerometer ODR 6.25 Hz */
    ACCE_ODR_3_125HZ  = 14, /*!< Accelerometer ODR 3.125 Hz */
    ACCE_ODR_1_5625HZ = 15, /*!< Accelerometer ODR 1.5625 Hz */
} icm42670_acce_odr_t;

typedef enum {
    GYRO_FS_2000DPS = 0,     /*!< Gyroscope full scale range is +/- 2000 degree per sencond */
    GYRO_FS_1000DPS = 1,     /*!< Gyroscope full scale range is +/- 1000 degree per sencond */
    GYRO_FS_500DPS  = 2,     /*!< Gyroscope full scale range is +/- 500 degree per sencond */
    GYRO_FS_250DPS  = 3,     /*!< Gyroscope full scale range is +/- 250 degree per sencond */
} icm42670_gyro_fs_t;

typedef enum {
    GYRO_PWR_OFF      = 0,     /*!< Gyroscope power off state */
    GYRO_PWR_STANDBY  = 1,     /*!< Gyroscope power standby state */
    GYRO_PWR_LOWNOISE = 3,     /*!< Gyroscope power low noise state */
} icm42670_gyro_pwr_t;

typedef enum {
    GYRO_ODR_1600HZ = 5,  /*!< Gyroscope ODR 1.6 kHz */
    GYRO_ODR_800HZ  = 6,  /*!< Gyroscope ODR 800 Hz */
    GYRO_ODR_400HZ  = 7,  /*!< Gyroscope ODR 400 Hz */
    GYRO_ODR_200HZ  = 8,  /*!< Gyroscope ODR 200 Hz */
    GYRO_ODR_100HZ  = 9,  /*!< Gyroscope ODR 100 Hz */
    GYRO_ODR_50HZ   = 10, /*!< Gyroscope ODR 50 Hz */
    GYRO_ODR_25HZ   = 11, /*!< Gyroscope ODR 25 Hz */
    GYRO_ODR_12_5HZ = 12, /*!< Gyroscope ODR 12.5 Hz */
} icm42670_gyro_odr_t;

typedef struct {
    icm42670_acce_fs_t  acce_fs;    /*!< Accelerometer full scale range */
    icm42670_acce_odr_t acce_odr;   /*!< Accelerometer ODR selection */
    icm42670_gyro_fs_t  gyro_fs;    /*!< Gyroscope full scale range */
    icm42670_gyro_odr_t gyro_odr;   /*!< Gyroscope ODR selection */
} icm42670_cfg_t;

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} icm42670_raw_value_t;

typedef struct {
    float x;
    float y;
    float z;
} icm42670_value_t;

typedef struct {
    float roll;
    float pitch;
} complimentary_angle_t;

typedef void *icm42670_handle_t;

/*******************************************************************************
 * ICM42670 API 函数声明
 ******************************************************************************/

/**
 * @brief 创建并初始化传感器对象
 *
 * @param[in]  i2c_bus    I2C总线句柄，通过i2c_new_master_bus()获取
 * @param[in]  dev_addr   I2C设备地址，可以是ICM42670_I2C_ADDRESS或ICM42670_I2C_ADDRESS_1
 * @param[out] handle_ret 创建的ICM42670驱动对象句柄
 *
 * @return
 *     - ESP_OK 成功
 *     - ESP_ERR_NO_MEM 内存不足
 *     - ESP_ERR_NOT_FOUND 在I2C总线上未找到传感器
 *     - 其他值 来自底层I2C驱动的错误
 */
esp_err_t icm42670_create(i2c_master_bus_handle_t i2c_bus, const uint8_t dev_addr, icm42670_handle_t *handle_ret);

/**
 * @brief 删除并释放传感器对象
 *
 * @param sensor icm42670对象句柄
 */
void icm42670_delete(icm42670_handle_t sensor);

/**
 * @brief 获取ICM42670设备ID
 *
 * @param sensor icm42670对象句柄
 * @param deviceid 设备ID指针
 *
 * @return
 *     - ESP_OK 成功
 *     - ESP_FAIL 失败
 */
esp_err_t icm42670_get_deviceid(icm42670_handle_t sensor, uint8_t *deviceid);

/**
 * @brief 设置加速度计电源模式
 *
 * @param sensor icm42670对象句柄
 * @param state 加速度计电源模式
 *
 * @return
 *     - ESP_OK 成功
 *     - ESP_FAIL 失败
 */
esp_err_t icm42670_acce_set_pwr(icm42670_handle_t sensor, icm42670_acce_pwr_t state);

/**
 * @brief 设置陀螺仪电源模式
 *
 * @param sensor icm42670对象句柄
 * @param state 陀螺仪电源模式
 *
 * @return
 *     - ESP_OK 成功
 *     - ESP_FAIL 失败
 */
esp_err_t icm42670_gyro_set_pwr(icm42670_handle_t sensor, icm42670_gyro_pwr_t state);

/**
 * @brief 设置加速度计和陀螺仪满量程范围
 *
 * @param sensor icm42670对象句柄
 * @param config 加速度计和陀螺仪配置结构体
 *
 * @return
 *     - ESP_OK 成功
 *     - ESP_FAIL 失败
 */
esp_err_t icm42670_config(icm42670_handle_t sensor, const icm42670_cfg_t *config);

/**
 * @brief 获取加速度计灵敏度
 *
 * @param sensor icm42670对象句柄
 * @param sensitivity 加速度计灵敏度
 *
 * @return
 *     - ESP_OK 成功
 *     - ESP_FAIL 失败
 */
esp_err_t icm42670_get_acce_sensitivity(icm42670_handle_t sensor, float *sensitivity);

/**
 * @brief 获取陀螺仪灵敏度
 *
 * @param sensor icm42670对象句柄
 * @param sensitivity 陀螺仪灵敏度
 *
 * @return
 *     - ESP_OK 成功
 *     - ESP_FAIL 失败
 */
esp_err_t icm42670_get_gyro_sensitivity(icm42670_handle_t sensor, float *sensitivity);

/**
 * @brief 读取原始温度测量值
 *
 * @param sensor icm42670对象句柄
 * @param value 原始温度测量值（二进制补码形式）
 *
 * @return
 *     - ESP_OK 成功
 *     - ESP_FAIL 失败
 */
esp_err_t icm42670_get_temp_raw_value(icm42670_handle_t sensor, uint16_t *value);

/**
 * @brief 读取原始加速度计测量值
 *
 * @param sensor icm42670对象句柄
 * @param value 原始加速度计测量值
 *
 * @return
 *     - ESP_OK 成功
 *     - ESP_FAIL 失败
 */
esp_err_t icm42670_get_acce_raw_value(icm42670_handle_t sensor, icm42670_raw_value_t *value);

/**
 * @brief 读取原始陀螺仪测量值
 *
 * @param sensor icm42670对象句柄
 * @param value 原始陀螺仪测量值
 *
 * @return
 *     - ESP_OK 成功
 *     - ESP_FAIL 失败
 */
esp_err_t icm42670_get_gyro_raw_value(icm42670_handle_t sensor, icm42670_raw_value_t *value);

/**
 * @brief 读取加速度计测量值（单位：g）
 *
 * @param sensor icm42670对象句柄
 * @param value 加速度计测量值
 *
 * @return
 *     - ESP_OK 成功
 *     - ESP_FAIL 失败
 */
esp_err_t icm42670_get_acce_value(icm42670_handle_t sensor, icm42670_value_t *value);

/**
 * @brief 读取陀螺仪测量值（单位：dps）
 *
 * @param sensor icm42670对象句柄
 * @param value 陀螺仪测量值
 *
 * @return
 *     - ESP_OK 成功
 *     - ESP_FAIL 失败
 */
esp_err_t icm42670_get_gyro_value(icm42670_handle_t sensor, icm42670_value_t *value);

/**
 * @brief 读取温度值（单位：°C）
 *
 * @param sensor icm42670对象句柄
 * @param value 温度测量值
 *
 * @return
 *     - ESP_OK 成功
 *     - ESP_FAIL 失败
 */
esp_err_t icm42670_get_temp_value(icm42670_handle_t sensor, float *value);

/**
 * @brief 使用互补滤波器计算roll和pitch角度
 *
 * @param sensor icm42670对象句柄
 * @param acce_value 加速度计测量值
 * @param gyro_value 陀螺仪测量值
 * @param complimentary_angle 互补角度
 *
 * @return
 *     - ESP_OK 成功
 *     - ESP_FAIL 失败
 */
esp_err_t icm42670_complimentory_filter(icm42670_handle_t sensor, const icm42670_value_t *acce_value,
                                        const icm42670_value_t *gyro_value, complimentary_angle_t *complimentary_angle);

/**
 * @brief 读取寄存器
 *
 * @param sensor icm42670对象句柄
 * @param reg 寄存器地址
 * @param val 寄存器值
 *
 * @return
 *     - ESP_OK 成功
 *     - ESP_FAIL 失败
 */
esp_err_t icm42670_read_register(icm42670_handle_t sensor, uint8_t reg, uint8_t *val);

/**
 * @brief 写入寄存器
 *
 * @param sensor icm42670对象句柄
 * @param reg 寄存器地址
 * @param val 要写入的值
 *
 * @return
 *     - ESP_OK 成功
 *     - ESP_FAIL 失败
 */
esp_err_t icm42670_write_register(icm42670_handle_t sensor, uint8_t reg, uint8_t val);

/**
 * @brief 读取MREG寄存器
 *
 * @param sensor icm42670对象句柄
 * @param mreg MREG bank编号（1-3）
 * @param reg 寄存器地址
 * @param val 读取的数据
 *
 * @return
 *     - ESP_OK 成功
 *     - ESP_INVALID_ARG 无效的MREG
 *     - ESP_FAIL 失败
 */
esp_err_t icm42670_read_mreg_register(icm42670_handle_t sensor, uint8_t mreg, uint8_t reg, uint8_t *val);

/**
 * @brief 写入MREG寄存器
 *
 * @param sensor icm42670对象句柄
 * @param mreg MREG bank编号（1-3）
 * @param reg 寄存器地址
 * @param val 要写入的数据
 *
 * @return
 *     - ESP_OK 成功
 *     - ESP_INVALID_ARG 无效的MREG
 *     - ESP_FAIL 失败
 */
esp_err_t icm42670_write_mreg_register(icm42670_handle_t sensor, uint8_t mreg, uint8_t reg, uint8_t val);

/*******************************************************************************
 * IMU 高级API（简化接口）
 ******************************************************************************/

/**
 * @brief 初始化IMU (ICM42670)
 * 
 * @param i2c_port_num I2C端口序号
 * @return esp_err_t 
 *     - ESP_OK 成功
 *     - 其他值 失败
 */
esp_err_t imu_init(i2c_port_num_t i2c_port_num);

/**
 * @brief 反初始化IMU
 * 
 * @return esp_err_t 
 */
esp_err_t imu_deinit(void);

/**
 * @brief 启动翻转检测任务
 * 
 */
void imu_start_flip_detection_task(void);

/**
 * @brief 停止翻转检测任务
 * 
 */
void imu_stop_flip_detection_task(void);

/**
 * @brief 获取当前IMU句柄
 * 
 * @return icm42670_handle_t IMU句柄
 */
icm42670_handle_t imu_get_handle(void);
