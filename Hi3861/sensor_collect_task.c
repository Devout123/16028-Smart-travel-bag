#include "sensor_collect_task.h"
#include "hal_bsp_sht20.h"
#include "hal_bsp_pcf8574.h"
#include "hal_bsp_ssd1306.h"
#include "hal_bsp_ssd1306_bmps.h"
#include "hal_bsp_key.h"
#include "hal_bsp_mqtt.h"
#include "oled_show_log.h"
#include "sys_config.h"

msg_data_t sys_msg_data = {0}; // 传感器的数据
static uint8_t fan_gif_index = 0;
#define FAN_GIF_INDEX_MAX 3

extern int TDS_value;
extern int altitude_value;
extern int Ultraviolet_value;
extern int pressure_value;
extern int barometer_value;
margin_t bmp_number_1 = {
    .top = 16 + 8,
    .left = 8,
    .width = 16,
    .hight = 32,
}; // 数字-十位
margin_t bmp_number_2 = {
    .top = 16 + 8,
    .left = 24,
    .width = 16,
    .hight = 32,
};                                                   // 数字-个位
margin_t bmp_dian = {
    .top = 32 + 8,
    .left = 40,
    .width = 16,
    .hight = 16,
};     // 小数点
margin_t bmp_number_3 = {
    .top = 32 + 8,
    .left = 56,
    .width = 8,
    .hight = 16,
}; // 数字-小数
margin_t bmp_danwei = {
    .top = 16 + 8,
    .left = 52,
    .width = 16,
    .hight = 16,
};   // 单位
margin_t bmp_image = {
    .top = 16,
    .left = 72,
    .width = 48,
    .hight = 48,
}; // 图片

#define TASK_DELAY_TIME (100 * 1000) // us
#define COEFFICIENT_10 10
#define COEFFICIENT_100 100
#define COEFFICIENT_1000 1000

/**
 * @brief  显示页面
 * @note
 * @param  val:
 * @retval None
 */
void show_humi_page(float val)
{
    SSD1306_ShowStr(OLED_TEXT16_COLUMN_0, OLED_TEXT16_LINE_0, "Smart travel bag", TEXT_SIZE_16);

    int x = (val * COEFFICIENT_100) / COEFFICIENT_1000;
    SSD1306_DrawBMP(bmp_number_1.left, bmp_number_1.top,
                    bmp_number_1.width, bmp_number_1.hight, bmp_16X32_number[x]); // 显示数字的十位

    x = ((int)(val * COEFFICIENT_100)) / COEFFICIENT_100 % COEFFICIENT_10;
    SSD1306_DrawBMP(bmp_number_2.left, bmp_number_2.top,
                    bmp_number_1.width, bmp_number_1.hight, bmp_16X32_number[x]); // 显示数字的个位
    SSD1306_DrawBMP(bmp_dian.left, bmp_dian.top,
                    bmp_dian.width, bmp_dian.hight, bmp_16X16_dian);              // 显示小数点
    SSD1306_DrawBMP(bmp_danwei.left, bmp_danwei.top,
                    bmp_danwei.width, bmp_danwei.hight, bmp_16X16_baifenhao);     // 显示%符号

    x = ((int)(val * COEFFICIENT_100)) / COEFFICIENT_10 % COEFFICIENT_10;
    SSD1306_DrawBMP(bmp_number_3.left, bmp_number_3.top,
                    bmp_number_3.width, bmp_number_3.hight, bmp_8X16_number[x]); // 显示数字的小数位

    // 风扇动态显示
    if (sys_msg_data.fanStatus == 0) {
        SSD1306_DrawBMP(bmp_image.left, bmp_image.top,
                        bmp_image.width, bmp_image.hight, bmp_48X48_fan_gif[0]); // 静态显示
    } else {
        fan_gif_index++;
        if (fan_gif_index > FAN_GIF_INDEX_MAX)
            fan_gif_index = 0;
        SSD1306_DrawBMP(bmp_image.left, bmp_image.top,
                        bmp_image.width, bmp_image.hight, bmp_48X48_fan_gif[fan_gif_index]); // 动态显示
    }
}

/**
 * @brief  传感器采集任务
 * @note
 * @retval None
 */
void sensor_collect_task(void)
{
    float temperature = 0, humidity = 0;
    
    while (1) {
        // 采集传感器的值
        SHT20_ReadData(&temperature, &humidity);
        show_humi_page(humidity);

        sys_msg_data.temperature = (int)temperature;
        sys_msg_data.humidity = (int)humidity;
        sys_msg_data.TDS_value = (int)TDS_value;
        sys_msg_data.altitude_value = (int)altitude_value;
        sys_msg_data.Ultraviolet_value = (int)Ultraviolet_value;
        sys_msg_data.pressure_value = (int)pressure_value;
         sys_msg_data.barometer_value = (int) barometer_value;
     
        usleep(TASK_DELAY_TIME);
    }
}
