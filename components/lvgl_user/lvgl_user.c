#include <stdio.h>
#include <unistd.h>
#include <sys/lock.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "st7305_2p9.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_io.h"
#include "lvgl.h"
#include "spi_shared_lock.h"

#define TAG "lvgl_user"
#define LVGL_TICK_PERIOD_MS 40
#define LVGL_PALETTE_SIZE 8

// LVGL library is not thread-safe, this example will call LVGL APIs from different tasks, so use a mutex to protect it
_lock_t lvgl_api_lock;
lv_display_t *lvgl_display = NULL;
static uint8_t panel_buffer[LCD_H_RES * LCD_V_RES / 8];
static SemaphoreHandle_t s_lcd_flush_done_sem = NULL;

static bool notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t io_panel, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{
    BaseType_t high_task_wakeup = pdFALSE;
    if (s_lcd_flush_done_sem != NULL)
    {
        xSemaphoreGiveFromISR(s_lcd_flush_done_sem, &high_task_wakeup);
    }
    return high_task_wakeup == pdTRUE;
}

static void lvgl_flush_cb_partial(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t)lv_display_get_user_data(disp);
    int log_width = area->x2 - area->x1 + 1;
    int log_height = area->y2 - area->y1 + 1;
    lv_display_rotation_t rotation = lv_display_get_rotation(disp);
    lv_draw_buf_t *draw_buf = lv_display_get_buf_active(disp);
    uint32_t stride = (draw_buf) ? draw_buf->header.stride : ((log_width + 7) / 8);
    uint8_t *bitmap = px_map + LVGL_PALETTE_SIZE;

    // 将 LVGL 计算出的局部差异直接覆盖到位映射全局缓存 (Shadow Buffer)
    for (int log_y = 0; log_y < log_height; log_y++)
    {
        for (int log_x = 0; log_x < log_width; log_x++)
        {
            bool is_set = bitmap[log_y * stride + (log_x / 8)] & (1 << (7 - (log_x % 8)));

            int point_phys_x = area->x1 + log_x;
            int point_phys_y = area->y1 + log_y;

            if (rotation == LV_DISPLAY_ROTATION_90)
            {
                point_phys_x = LCD_H_RES - 1 - (area->y1 + log_y);
                point_phys_y = area->x1 + log_x;
            }
            else if (rotation == LV_DISPLAY_ROTATION_180)
            {
                point_phys_x = LCD_H_RES - 1 - (area->x1 + log_x);
                point_phys_y = LCD_V_RES - 1 - (area->y1 + log_y);
            }
            else if (rotation == LV_DISPLAY_ROTATION_270)
            {
                point_phys_x = area->y1 + log_y;
                point_phys_y = LCD_V_RES - 1 - (area->x1 + log_x);
            }

            if (point_phys_x >= 0 && point_phys_x < LCD_H_RES && point_phys_y >= 0 && point_phys_y < LCD_V_RES)
            {
                uint real_x = point_phys_x / 4;
                uint real_y = point_phys_y / 2;
                uint write_byte_index = real_y * (LCD_H_RES / 4) + real_x;
                uint one_two = (point_phys_y % 2 == 0) ? 0 : 1;
                uint line_bit_4 = point_phys_x % 4;
                uint8_t write_bit = 7 - (line_bit_4 * 2 + one_two);

                if (is_set)
                {
                    panel_buffer[write_byte_index] |= (1 << write_bit);
                }
                else
                {
                    panel_buffer[write_byte_index] &= ~(1 << write_bit);
                }
            }
        }
    }

    // 这里如果用原 draw_bitmap 局部坐标+动态分配内存，很容易因为 DMA 未结束而 free() 导致全屏崩坏或者 st7305 驱动自身的底层未对准 /12 等 bug
    // 所以配合 LVGL_PARTIAL，底层向屏幕 DMA 传输只发全屏是最完美且没有视觉耗时的方式。
    if (!spi_shared_lock_take(portMAX_DELAY))
    {
        ESP_LOGE(TAG, "Failed to take shared SPI lock");
        lv_display_flush_ready(disp);
        return;
    }

    if (s_lcd_flush_done_sem != NULL)
    {
        (void)xSemaphoreTake(s_lcd_flush_done_sem, 0);
    }

    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, LCD_H_RES, LCD_V_RES, panel_buffer);

    if (s_lcd_flush_done_sem != NULL)
    {
        if (xSemaphoreTake(s_lcd_flush_done_sem, pdMS_TO_TICKS(200)) != pdTRUE)
        {
            ESP_LOGW(TAG, "LCD flush timeout");
        }
    }

    spi_shared_lock_give();
    lv_display_flush_ready(disp);
}

static void increase_lvgl_tick(void *arg)
{
    /* Tell LVGL how many milliseconds has elapsed */
    lv_tick_inc(LVGL_TICK_PERIOD_MS);
}

static void lvgl_port_task(void *arg)
{
    ESP_LOGI(TAG, "Starting LVGL task");
    uint32_t time_till_next_ms = 0;
    while (1)
    {
        _lock_acquire(&lvgl_api_lock);
        time_till_next_ms = lv_timer_handler();
        _lock_release(&lvgl_api_lock);
        // in case of triggering a task watch dog time out
        time_till_next_ms = MAX(time_till_next_ms, 1);
        // in case of lvgl display not ready yet
        time_till_next_ms = MIN(time_till_next_ms, 500);
        vTaskDelay(pdMS_TO_TICKS(time_till_next_ms));
    }
}

void lvgl_user_init(esp_lcd_panel_handle_t panel_handle, esp_lcd_panel_io_handle_t io_handle)
{
    // 屏幕分辨率为 168x384。
    // 按前面的换算规则: 宽度168需要 (168/12)*3 = 42 Byte ；高度 384/2 = 192 个双行。
    size_t full_sz = 42 * 192;                                      // 8064 Bytes
    uint8_t *clear_buf = heap_caps_malloc(full_sz, MALLOC_CAP_DMA); // 最好放置在DMA可用内存中避免SPI报错
    if (clear_buf)
    {
        memset(clear_buf, 0x00, full_sz); // 0x00(黑) 或 0xFF(白)，根据底反色决定
        esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, 168, 384, clear_buf);
        free(clear_buf);
    }

    ESP_LOGI(TAG, "Initialize LVGL");
    lv_init();

    if (s_lcd_flush_done_sem == NULL)
    {
        s_lcd_flush_done_sem = xSemaphoreCreateBinary();
        assert(s_lcd_flush_done_sem);
    }
    // create a lvgl display
    lvgl_display = lv_display_create(LCD_H_RES, LCD_V_RES);
    // associate the i2c panel handle to the display
    lv_display_set_user_data(lvgl_display, panel_handle);
    // create draw buffer
    void *buf = NULL;
    ESP_LOGI(TAG, "Allocate separate LVGL draw buffers");
    // LVGL reserves 2 x 4 bytes in the buffer, as these are assumed to be used as a palette.
    size_t draw_buffer_sz = LCD_H_RES * LCD_V_RES / 8 + LVGL_PALETTE_SIZE;
    buf = heap_caps_calloc(1, draw_buffer_sz, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    assert(buf);

    // LVGL9 suooprt new monochromatic format.
    lv_display_set_color_format(lvgl_display, LV_COLOR_FORMAT_I1);

    // 测试来看，局部刷新在低CPU频率下的性能提升显著
    // initialize LVGL draw buffers
    lv_display_set_buffers(lvgl_display, buf, NULL, draw_buffer_sz, LV_DISPLAY_RENDER_MODE_PARTIAL);
    // set the callback which can copy the rendered image to an area of the display
    lv_display_set_flush_cb(lvgl_display, lvgl_flush_cb_partial);

    ESP_LOGI(TAG, "Apply Mono theme");
    // 对单色屏（黑底白字还是白底黑字可以按需设置 false 或 true）
    lv_theme_t *th = lv_theme_mono_init(lvgl_display, false, LV_FONT_DEFAULT);
    lv_display_set_theme(lvgl_display, th);

    ESP_LOGI(TAG, "Register io panel event callback for LVGL flush ready notification");
    const esp_lcd_panel_io_callbacks_t cbs = {
        .on_color_trans_done = notify_lvgl_flush_ready,
    };
    /* Register done callback */
    esp_lcd_panel_io_register_event_callbacks(io_handle, &cbs, lvgl_display);

    ESP_LOGI(TAG, "Use esp_timer as LVGL tick timer");
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &increase_lvgl_tick,
        .name = "lvgl_tick"};
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, LVGL_TICK_PERIOD_MS * 1000));

    ESP_LOGI(TAG, "Create LVGL task");
    xTaskCreate(lvgl_port_task, "LVGL", 4 * 1024, NULL, 2, NULL);

    // Lock the mutex due to the LVGL APIs are not thread-safe
    _lock_acquire(&lvgl_api_lock);
    // 以384为宽，168为高，横屏
    lv_display_set_rotation(lvgl_display, LV_DISPLAY_ROTATION_90);
    _lock_release(&lvgl_api_lock);
}