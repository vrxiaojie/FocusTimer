#include <stdio.h>
#include "aw32001.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "pinmap.h"

#define TAG "AW32001"

#define PWR_KEY_LONG_PRESS_MS 2000 // 长按2秒触发shipping mode

static QueueHandle_t gpio_evt_queue = NULL;
static i2c_master_dev_handle_t dev_handle = NULL;
aw32001_sys_status_t pwr_sys_status = {};

esp_err_t aw32001_init(i2c_port_num_t port_num)
{
    i2c_master_bus_handle_t bus_handle;
    i2c_master_get_bus_handle(port_num, &bus_handle);
    // 创建I2C设备句柄
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = AW32001_I2C_ADDR,
        .scl_speed_hz = 400000};

    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_config, &dev_handle));
    ESP_LOGI(TAG, "AW32001 init successfully");
    return ESP_OK;
}

esp_err_t aw32001_read_reg(uint8_t reg_addr, uint8_t *reg_val)
{
    if (dev_handle == NULL || reg_val == NULL)
    {
        ESP_LOGE(TAG, "Invalid input parameter");
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t write_buf[1] = {reg_addr};
    uint8_t read_buf[1] = {0};

    // 发送寄存器地址并读取数据
    esp_err_t err = i2c_master_transmit_receive(dev_handle, write_buf, sizeof(write_buf),
                                                read_buf, sizeof(read_buf), AW32001_I2C_TIMEOUT_MS);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Read reg 0x%02X failed, err: %s", reg_addr, esp_err_to_name(err));
        return err;
    }

    *reg_val = read_buf[0];
    ESP_LOGD(TAG, "Read reg 0x%02X: 0x%02X", reg_addr, *reg_val);
    return ESP_OK;
}

esp_err_t aw32001_write_reg(uint8_t reg_addr, uint8_t reg_val)
{
    if (dev_handle == NULL)
    {
        ESP_LOGE(TAG, "Invalid input parameter");
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t write_buf[2] = {reg_addr, reg_val};

    // 发送寄存器地址和数据
    esp_err_t err = i2c_master_transmit(dev_handle, write_buf, sizeof(write_buf), AW32001_I2C_TIMEOUT_MS);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Write reg 0x%02X (0x%02X) failed, err: %s", reg_addr, reg_val, esp_err_to_name(err));
        return err;
    }

    ESP_LOGD(TAG, "Write reg 0x%02X: 0x%02X success", reg_addr, reg_val);
    return ESP_OK;
}

esp_err_t aw32001_disable_watchdog()
{
    uint8_t reg_val;

    // 读取REG05H（充电终止/定时器控制寄存器）
    esp_err_t err = aw32001_read_reg(AW32001_REG_05H, &reg_val);
    if (err != ESP_OK)
    {
        return err;
    }

    ESP_LOGI(TAG, "Before disable watchdog, REG05H = 0x%02X", reg_val);

    // 关闭看门狗（BIT6-BIT5设为00）
    reg_val &= 0x9F; // 0x9F = 10011111，清除BIT6和BIT5
    err = aw32001_write_reg(AW32001_REG_05H, reg_val);
    if (err != ESP_OK)
    {
        return err;
    }

    // 验证配置
    err = aw32001_read_reg(AW32001_REG_05H, &reg_val);
    if (err != ESP_OK)
    {
        return err;
    }

    if ((reg_val & 0x60) == 0x00)
    { // 0x60 = 01100000，检查BIT6和BIT5是否为0
        ESP_LOGI(TAG, "Disable watchdog success, REG05H = 0x%02X", reg_val);
        return ESP_OK;
    }
    else
    {
        ESP_LOGE(TAG, "Disable watchdog failed, REG05H = 0x%02X", reg_val);
        return ESP_FAIL;
    }
}

esp_err_t aw32001_set_chg_current(uint16_t chg_current)
{
    // 检查充电电流范围
    if (chg_current < 8 || chg_current > 512)
    {
        ESP_LOGE(TAG, "Invalid charge current: %d mA (range: 8~512 mA)", chg_current);
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t reg_val;
    uint8_t ichg_code = 0;

    // 根据充电电流计算REG02H的ICHG[5:0]编码（参考数据手册表36）
    ichg_code = (chg_current - 8) / 8;

    // 读取REG02H（充电电流控制寄存器）
    esp_err_t err = aw32001_read_reg(AW32001_REG_02H, &reg_val);
    if (err != ESP_OK)
    {
        return err;
    }

    // 保留BIT7和BIT6，更新ICHG[5:0]
    reg_val = (reg_val & 0xC0) | (ichg_code & 0x3F); // 0xC0 = 11000000，0x3F = 00111111

    // 写入配置
    err = aw32001_write_reg(AW32001_REG_02H, reg_val);
    if (err != ESP_OK)
    {
        return err;
    }

    ESP_LOGI(TAG, "Set charge current to %d mA, REG02H = 0x%02X", chg_current, reg_val);
    return ESP_OK;
}

esp_err_t aw32001_get_chg_current(uint16_t *chg_current)
{
    if (chg_current == NULL)
    {
        ESP_LOGE(TAG, "Invalid input parameter");
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t reg_val;

    // 读取REG02H（充电电流控制寄存器）
    esp_err_t err = aw32001_read_reg(AW32001_REG_02H, &reg_val);
    if (err != ESP_OK)
    {
        return err;
    }

    // 提取ICHG[5:0]并计算充电电流
    uint8_t ichg_code = reg_val & 0x3F;           // 0x3F = 00111111
    *chg_current = (uint16_t)(ichg_code * 8 + 8); // 电流范围8~512mA，步长8mA

    ESP_LOGI(TAG, "Get charge current: %d mA from REG02H = 0x%02X", *chg_current, reg_val);
    return ESP_OK;
}

esp_err_t aw32001_set_chg_voltage(float chg_voltage)
{
    // 检查充电电压范围
    if (chg_voltage < 3.6 || chg_voltage > 4.545)
    {
        ESP_LOGE(TAG, "Invalid charge voltage: %.3f V (range: 3.6~4.545 V)", chg_voltage);
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t reg_val;
    uint8_t vbat_reg_code = 0;

    // 根据充电电压计算REG04H的VBAT_REG[5:0]编码（参考数据手册表38）
    float voltage_step = 0.015; // 电压步长15mV
    vbat_reg_code = (uint8_t)((chg_voltage - 3.6) / voltage_step);

    // 读取REG04H（充电电压寄存器）
    esp_err_t err = aw32001_read_reg(AW32001_REG_04H, &reg_val);
    if (err != ESP_OK)
    {
        return err;
    }

    // 保留BIT1和BIT0，更新VBAT_REG[5:0]
    reg_val = (reg_val & 0x03) | ((vbat_reg_code & 0x3F) << 2); // 0x03 = 00000011

    // 写入配置
    err = aw32001_write_reg(AW32001_REG_04H, reg_val);
    if (err != ESP_OK)
    {
        return err;
    }

    ESP_LOGI(TAG, "Set charge voltage to %.3f V, REG04H = 0x%02X", chg_voltage, reg_val);
    return ESP_OK;
}

esp_err_t aw32001_set_dischg_current(uint16_t dischg_current)
{
    // 检查放电电流范围
    if (dischg_current < 200 || dischg_current > 3200 || (dischg_current % 200) != 0)
    {
        ESP_LOGE(TAG, "Invalid discharge current: %d mA (range: 200~3200 mA, step: 200 mA)", dischg_current);
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t reg_val;
    uint8_t idschg_code = (dischg_current - 200) / 200; // 参考数据手册表37

    // 读取REG03H（放电电流控制寄存器）
    esp_err_t err = aw32001_read_reg(AW32001_REG_03H, &reg_val);
    if (err != ESP_OK)
    {
        return err;
    }

    // 保留BIT3-BIT0，更新IDSCHG[7:4]
    reg_val = (reg_val & 0x0F) | ((idschg_code & 0x0F) << 4); // 0x0F = 00001111

    // 写入配置
    err = aw32001_write_reg(AW32001_REG_03H, reg_val);
    if (err != ESP_OK)
    {
        return err;
    }

    ESP_LOGI(TAG, "Set discharge current limit to %d mA, REG03H = 0x%02X", dischg_current, reg_val);
    return ESP_OK;
}

esp_err_t aw32001_enable_charge()
{
    uint8_t reg_val;

    // 读取REG01H（上电配置寄存器）
    esp_err_t err = aw32001_read_reg(AW32001_REG_01H, &reg_val);
    if (err != ESP_OK)
    {
        return err;
    }

    // 使能充电（CEB位设为0，BIT3=0）
    reg_val &= 0xF7; // 0xF7 = 11110111，清除BIT3
    err = aw32001_write_reg(AW32001_REG_01H, reg_val);
    if (err != ESP_OK)
    {
        return err;
    }

    ESP_LOGI(TAG, "Enable charge success, REG01H = 0x%02X", reg_val);
    return ESP_OK;
}

esp_err_t aw32001_disable_charge()
{
    uint8_t reg_val;

    // 读取REG01H（上电配置寄存器）
    esp_err_t err = aw32001_read_reg(AW32001_REG_01H, &reg_val);
    if (err != ESP_OK)
    {
        return err;
    }

    // 禁用充电（CEB位设为1，BIT3=1）
    reg_val |= 0x08; // 0x08 = 00001000，设置BIT3
    err = aw32001_write_reg(AW32001_REG_01H, reg_val);
    if (err != ESP_OK)
    {
        return err;
    }

    ESP_LOGI(TAG, "Disable charge success, REG01H = 0x%02X", reg_val);
    return ESP_OK;
}

esp_err_t aw32001_read_sys_status(aw32001_sys_status_t *sys_status)
{
    if (sys_status == NULL)
    {
        ESP_LOGE(TAG, "Invalid input parameter");
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t reg_val;

    // 读取REG08H（系统状态寄存器）
    esp_err_t err = aw32001_read_reg(AW32001_REG_08H, &reg_val);
    if (err != ESP_OK)
    {
        return err;
    }

    // 解析系统状态
    sys_status->watchdog_fault = (reg_val & 0x80) ? true : false;       // BIT7：看门狗故障
    sys_status->chg_stat = (aw32001_chg_stat_t)((reg_val >> 3) & 0x03); // BIT4-BIT3：充电状态
    sys_status->ppm_stat = (reg_val & 0x04) ? true : false;             // BIT2：PPM状态
    sys_status->pg_stat = (reg_val & 0x02) ? true : false;              // BIT1：电源状态
    sys_status->therm_stat = (reg_val & 0x01) ? true : false;           // BIT0：热调节状态

    // 打印系统状态
    // ESP_LOGI(TAG, "System Status:");
    // ESP_LOGI(TAG, "  Watchdog Fault: %s", sys_status->watchdog_fault ? "Yes" : "No");
    // ESP_LOGI(TAG, "  Charge Status: %s",
    //          (sys_status->chg_stat == AW32001_CHG_STAT_NOT_CHARGING) ? "Not Charging" : (sys_status->chg_stat == AW32001_CHG_STAT_PRE_CHARGE) ? "Pre Charge"
    //                                                                                 : (sys_status->chg_stat == AW32001_CHG_STAT_FAST_CHARGE)  ? "Fast Charge"
    //                                                                                 : (sys_status->chg_stat == AW32001_CHG_STAT_CHARGE_DONE)  ? "Charge Done"
    //                                                                                                                                           : "Unknown");
    // ESP_LOGI(TAG, "  PPM Status: %s", sys_status->ppm_stat ? "IN PPM" : "No PPM");
    // ESP_LOGI(TAG, "  Power Good: %s", sys_status->pg_stat ? "Yes" : "No");
    // ESP_LOGI(TAG, "  Thermal Regulation: %s", sys_status->therm_stat ? "Yes" : "No");

    return ESP_OK;
}

esp_err_t aw32001_read_fault_status(aw32001_fault_status_t *fault_status)
{
    if (fault_status == NULL)
    {
        ESP_LOGE(TAG, "Invalid input parameter");
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t reg_val;

    // 读取REG09H（故障寄存器）
    esp_err_t err = aw32001_read_reg(AW32001_REG_09H, &reg_val);
    if (err != ESP_OK)
    {
        return err;
    }

    // 解析故障状态
    fault_status->vin_fault = (reg_val & 0x20) ? true : false;      // BIT5：输入故障
    fault_status->therm_sd = (reg_val & 0x10) ? true : false;       // BIT4：热关断
    fault_status->bat_fault = (reg_val & 0x08) ? true : false;      // BIT3：电池过压故障
    fault_status->stmr_fault = (reg_val & 0x04) ? true : false;     // BIT2：安全定时器故障
    fault_status->ntc_fault_high = (reg_val & 0x02) ? true : false; // BIT1：NTC高温故障
    fault_status->ntc_fault_low = (reg_val & 0x01) ? true : false;  // BIT0：NTC低温故障

    // 打印故障状态
    ESP_LOGI(TAG, "Fault Status:");
    ESP_LOGI(TAG, "  Input Fault (OVP/Bad Source): %s", fault_status->vin_fault ? "Yes" : "No");
    ESP_LOGI(TAG, "  Thermal Shutdown: %s", fault_status->therm_sd ? "Yes" : "No");
    ESP_LOGI(TAG, "  Battery OVP Fault: %s", fault_status->bat_fault ? "Yes" : "No");
    ESP_LOGI(TAG, "  Safety Timer Fault: %s", fault_status->stmr_fault ? "Yes" : "No");
    ESP_LOGI(TAG, "  NTC High Temp Fault: %s", fault_status->ntc_fault_high ? "Yes" : "No");
    ESP_LOGI(TAG, "  NTC Low Temp Fault: %s", fault_status->ntc_fault_low ? "Yes" : "No");

    return ESP_OK;
}

esp_err_t aw32001_enable_ntc()
{
    uint8_t reg_val;

    // 读取REG06H（主控制寄存器）
    esp_err_t err = aw32001_read_reg(AW32001_REG_06H, &reg_val);
    if (err != ESP_OK)
    {
        return err;
    }

    // 使能NTC功能（EN_NTC位设为1，BIT7=1）
    reg_val |= 0x80; // 0x80 = 10000000，设置BIT7
    err = aw32001_write_reg(AW32001_REG_06H, reg_val);
    if (err != ESP_OK)
    {
        return err;
    }

    ESP_LOGI(TAG, "Enable NTC success, REG06H = 0x%02X", reg_val);
    return ESP_OK;
}

esp_err_t aw32001_disable_ntc()
{
    uint8_t reg_val;

    // 读取REG06H（主控制寄存器）
    esp_err_t err = aw32001_read_reg(AW32001_REG_06H, &reg_val);
    if (err != ESP_OK)
    {
        return err;
    }

    // 禁用NTC功能（EN_NTC位设为0，BIT7=0）
    reg_val &= 0x7F; // 0x7F = 01111111，清除BIT7
    err = aw32001_write_reg(AW32001_REG_06H, reg_val);
    if (err != ESP_OK)
    {
        return err;
    }

    ESP_LOGI(TAG, "Disable NTC success, REG06H = 0x%02X", reg_val);
    return ESP_OK;
}

esp_err_t aw32001_enter_shipping_mode()
{
    uint8_t reg_val;
    // 步骤1：读取REG06H（主控制寄存器）
    esp_err_t err = aw32001_read_reg(AW32001_REG_06H, &reg_val);
    if (err != ESP_OK)
    {
        return err;
    }

    // 步骤2：设置FET_DIS位（BIT5=1），进入运输模式
    reg_val |= 0x20; // 0x20 = 00100000，设置BIT5
    err = aw32001_write_reg(AW32001_REG_06H, reg_val);
    if (err != ESP_OK)
    {
        return err;
    }

    // 步骤3：配置运输模式退出时间为100ms（REG0BH的BIT0=1）
    err = aw32001_read_reg(AW32001_REG_0BH, &reg_val);
    if (err != ESP_OK)
    {
        return err;
    }
    reg_val |= 0x01; // 0x01 = 00000001，设置BIT0=1（退出时间100ms）
    err = aw32001_write_reg(AW32001_REG_0BH, reg_val);
    if (err != ESP_OK)
    {
        return err;
    }

    // 步骤4：配置REG22H的BIT3=1，使能按键唤醒功能（配合REG0BH实现100ms唤醒）
    err = aw32001_read_reg(AW32001_REG_22H, &reg_val);
    if (err != ESP_OK)
    {
        return err;
    }
    reg_val |= 0x08; // 0x08 = 00001000，设置BIT3=1
    err = aw32001_write_reg(AW32001_REG_22H, reg_val);
    if (err != ESP_OK)
    {
        return err;
    }

    ESP_LOGI(TAG, "Enter shipping mode success, REG06H=0x%02X, REG0BH=0x%02X, REG22H=0x%02X",
             0x20, 0x01, reg_val);
    return ESP_OK;
}

esp_err_t aw32001_set_vsys_reg(float sys_reg_voltage)
{
    // 检查系统电压范围
    if (sys_reg_voltage < 4.2 || sys_reg_voltage > 4.95)
    {
        ESP_LOGE(TAG, "Invalid system regulator voltage: %.2f V (range: 4.20~4.95 V)", sys_reg_voltage);
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t reg_val;
    uint8_t vsys_reg_code = 0;

    // 根据系统电压计算REG07H的VSYS_REG[3:0]编码
    vsys_reg_code = (uint8_t)((sys_reg_voltage - 4.2) / 0.05); // 电压步长0.05V

    // 读取REG07H（系统电压寄存器）
    esp_err_t err = aw32001_read_reg(AW32001_REG_07H, &reg_val);
    if (err != ESP_OK)
    {
        return err;
    }

    // 保留BIT7-BIT4，更新VSYS_REG[3:0]
    reg_val = (reg_val & 0xF0) | (vsys_reg_code & 0x0F); // 0xF0 = 11110000，0x0F = 00001111

    // 写入配置
    err = aw32001_write_reg(AW32001_REG_07H, reg_val);
    if (err != ESP_OK)
    {
        return err;
    }

    ESP_LOGI(TAG, "Set system regulator voltage to %.2f V, REG0AH = 0x%02X", sys_reg_voltage, reg_val);
    return ESP_OK;
}

static void IRAM_ATTR pwr_key_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    // 禁用中断，防止重复触发（在任务中按键释放后重新使能）
    gpio_intr_disable(gpio_num);
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void pwr_key_monitor_task(void *arg)
{
    uint32_t io_num;
    for (;;)
    {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY))
        {
            // 收到上升沿中断，按键已按下，开始计时
            int press_duration_ms = 0;
            while (gpio_get_level(PWR_KEY_PIN) == 1)
            {
                vTaskDelay(pdMS_TO_TICKS(50)); // 每50ms检测一次
                press_duration_ms += 50;

                if (press_duration_ms >= PWR_KEY_LONG_PRESS_MS)
                {
                    ESP_LOGW(TAG, "Power key long press detected (%d ms), entering shipping mode...", press_duration_ms);

                    // 进入shipping mode
                    esp_err_t err = aw32001_enter_shipping_mode();
                    if (err != ESP_OK)
                    {
                        ESP_LOGE(TAG, "Failed to enter shipping mode: %s", esp_err_to_name(err));
                    }
                    else
                    {
                        ESP_LOGI(TAG, "Shipping mode entered, system will power off");
                    }

                    // 等待按键释放或系统关机
                    while (gpio_get_level(PWR_KEY_PIN) == 1)
                    {
                        vTaskDelay(pdMS_TO_TICKS(100));
                    }
                    break;
                }
            }

            // 按键释放后，重新使能中断
            gpio_intr_enable(PWR_KEY_PIN);
        }
    }
}

void aw32001_power_key_init(void)
{
    // 创建事件队列
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));

    // 配置电源按键引脚为输入，上升沿中断
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << PWR_KEY_PIN,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE, // 启用下拉，确保空闲时为低电平
        .intr_type = GPIO_INTR_POSEDGE        // 上升沿触发中断
    };
    gpio_config(&io_conf);

    // 安装GPIO中断服务（如果已安装则忽略错误）
    esp_err_t err = gpio_install_isr_service(0);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE)
    {
        ESP_LOGE(TAG, "GPIO ISR service install failed: %s", esp_err_to_name(err));
        return;
    }
    gpio_isr_handler_add(PWR_KEY_PIN, pwr_key_isr_handler, (void *)(uint32_t)PWR_KEY_PIN);

    // 创建监控任务
    xTaskCreate(pwr_key_monitor_task, "pwr_key_monitor", 2048, NULL, 5, NULL);

    ESP_LOGI(TAG, "Power key interrupt initialized, pin=%d", PWR_KEY_PIN);
}