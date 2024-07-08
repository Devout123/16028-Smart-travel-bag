#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "cJSON.h"
#include "hi_nv.h"
#include "sys_config.h"

#include "hal_bsp_wifi.h"
#include "hal_bsp_mqtt.h"
#include "hal_bsp_key.h"
#include "hal_bsp_sht20.h"
#include "hal_bsp_ssd1306.h"
#include "hal_bsp_nfc.h"
#include "hal_bsp_pcf8574.h"
#include "hal_bsp_nfc_to_wifi.h"

#include "mqtt_send_task.h"
#include "mqtt_recv_task.h"
#include "sensor_collect_task.h"
#include "oled_show_log.h"

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "hi_io.h"
#include "hi_gpio.h"
#include "hi_uart.h"

#include "hal_bsp_structAll.h"


#define TASK_STACK_SIZE (5 * 1024)
#define TASK_INIT_DELAY 1 // s

osThreadId_t mqtt_send_task_id;      // mqtt 发布数据任务ID
osThreadId_t mqtt_recv_task_id;      // mqtt 接收数据任务ID
osThreadId_t sensor_collect_task_id; // 传感器采集任务ID
osThreadId_t uart_task_id_recv;
osThreadId_t uart_task_id_send;


char a[64] = {0};
unsigned long long data;
int  TDS;
int  TDS_value;
int  altitude;
int  altitude_value;
int  Ultraviolet;
int  Ultraviolet_value;
int  pressure;
int  pressure_value;
int  barometer_value;

void my_uart_init(void)
{
    uint32_t ret = 0;
    // 初始化串口
    hi_io_set_func(HI_IO_NAME_GPIO_11, HI_IO_FUNC_GPIO_11_UART2_TXD);
    hi_io_set_func(HI_IO_NAME_GPIO_12, HI_IO_FUNC_GPIO_12_UART2_RXD);

    hi_uart_attribute uart_param = {
        .baud_rate = 115200,
        .data_bits = 8,
        .stop_bits = 1,
        .parity = 0,
    };
    ret = hi_uart_init(HI_UART_IDX_2, &uart_param, NULL);
    if (ret != HI_ERR_SUCCESS) {
        printf("hi uart init is faild.\r\n");
    }
}

void my_uart_send_buff(unsigned char *str, unsigned short len)
{
    hi_u32 ret = 0;
    ret = hi_uart_write(HI_UART_IDX_2, (uint8_t *)str, len);
    if (ret == HI_ERR_FAILURE) {
        printf("uart send buff is failed.\r\n");
    }
}

void my_uart_recv_buff(unsigned char *str, unsigned short msg_len)
{
    unsigned char recv_buf[32];
    hi_u32 ret = 0;
    hi_u32 len = hi_uart_read(HI_UART_IDX_2, recv_buf, sizeof(recv_buf));
    if (ret == HI_ERR_FAILURE) {
        printf("uart send buff is failed.\r\n");
    }
    memcpy(str,recv_buf,len);

   

}

void my_uart_send_task(void)
{
    char buf[32] = "hello";
    while(1){ 
        my_uart_send_buff(buf,32);
        sleep(2);
    }
}

void my_uart_recv_task(void)
{
    //char buf[32] = {0};
    while(1){
        memset(a,0,sizeof(a));
        my_uart_recv_buff(a,32);
        printf("recv_buf:%s\n",a);
        data = atoll(a);
        barometer_value = (data/1000000000000);
        altitude_value = (data/100000000)%10000;;
        TDS_value = (data/10000)%10000; 
        pressure_value = (data/100)%100;
        Ultraviolet_value = data%100;
        printf("data:%d\n",data);
        printf("a1:%d\n",barometer_value);
        printf("a2:%d\n",altitude_value);
        printf("a3:%d\n",TDS_value);
        printf("a4:%d\n",Ultraviolet_value);
        printf("a5:%d\n",pressure_value);

    }
}
//num atoi(a) 


void SF_peripheral_init(void)
{
    // 外设的初始化
    KEY_Init();                         // 按键初始化
    PCF8574_Init();                     // 初始化IO扩展芯片
    SHT20_Init();
    SSD1306_Init(); // OLED 显示屏初始化
    SSD1306_CLS();  // 清屏
    SSD1306_ShowStr(OLED_TEXT16_COLUMN_0, OLED_TEXT16_LINE_0, "Smart travel bag", TEXT_SIZE_16);
    nfc_Init();
    usleep(TASK_INIT_DELAY);
}
int nfc_connect_wifi_init(void)
{
    // 通过NFC芯片进行连接WiFi
    uint8_t ndefLen = 0;      // ndef包的长度
    uint8_t ndef_Header = 0;  // ndef消息开始标志位-用不到
    uint32_t result_code = 0; // 函数的返回值
    oled_consle_log("===start nfc===");

    // 读整个数据的包头部分，读出整个数据的长度
    if (result_code = NT3HReadHeaderNfc(&ndefLen, &ndef_Header) != true) {
        printf("NT3HReadHeaderNfc is failed. result_code = %d\r\n", result_code);
        return -1;
    }
    ndefLen += NDEF_HEADER_SIZE; // 加上头部字节
    if (ndefLen <= NDEF_HEADER_SIZE) {
        printf("ndefLen <= 2\r\n");
        return -1;
    }
    
    uint8_t *ndefBuff = (uint8_t *)malloc(ndefLen + 1);
    if (ndefBuff == NULL) {
        printf("ndefBuff malloc is Falied!\r\n");
        return -1;
    }
    if (result_code = get_NDEFDataPackage(ndefBuff, ndefLen) != HI_ERR_SUCCESS) {
        printf("get_NDEFDataPackage is failed. result_code = %d\r\n", result_code);
        return -1;
    }
    // 打印读出的数据
    printf("start print ndefBuff.\r\n");
    for (size_t i = 0; i < ndefLen; i++) {
        printf("0x%x ", ndefBuff[i]);
    }
    oled_consle_log("=== end nfc ===");
    sleep(TASK_INIT_DELAY);

    oled_consle_log("== start wifi==");
    // 连接WiFi
    while (NFC_configuresWiFiNetwork(ndefBuff) != WIFI_SUCCESS) {
        printf("nfc connect wifi is failed!\r\n");
        oled_consle_log("=== wifi no ===");
        sleep(TASK_INIT_DELAY);
        SSD1306_CLS(); // 清屏
    }
    printf("nfc connect wifi is SUCCESS\r\n");
    oled_consle_log("===wifi  yes===");
    sleep(TASK_INIT_DELAY);
    return 0;
}
int mqtt_init(void)
{
    // 连接MQTT服务器
    while (MQTTClient_connectServer(SERVER_IP_ADDR, SERVER_IP_PORT) != WIFI_SUCCESS) {
        printf("mqttClient_connectServer\r\n");
        oled_consle_log("==mqtt ser no==");
        sleep(TASK_INIT_DELAY);
        SSD1306_CLS(); // 清屏
    }
    printf("mqttClient_connectServer\r\n");
    oled_consle_log("==mqtt ser yes=");
    sleep(TASK_INIT_DELAY);

    // 初始化MQTT客户端
    while (MQTTClient_init(MQTT_CLIENT_ID, MQTT_USER_NAME, MQTT_PASS_WORD) != WIFI_SUCCESS) {
        printf("mqttClient_init\r\n");
        oled_consle_log("==mqtt cli no==");
        sleep(TASK_INIT_DELAY);
        SSD1306_CLS(); // 清屏
    }
    printf("mqttClient_init\r\n");
    oled_consle_log("==mqtt cli yes=");
    sleep(TASK_INIT_DELAY);

    // 订阅MQTT主题
    while (MQTTClient_subscribe(MQTT_TOPIC_SUB_COMMANDS) != 0) {
        printf("mqttClient_subscribe\r\n");
        oled_consle_log("==mqtt sub no==");
        sleep(TASK_INIT_DELAY);
        SSD1306_CLS(); // 清屏
    }
    printf("mqttClient_subscribe\r\n");
    oled_consle_log("=mqtt sub yes==");
    sleep(TASK_INIT_DELAY);
    SSD1306_CLS(); // 清屏

    return 0;
}
static void smartFarm_Project_example(void)
{
    printf("Enter smartFarm_Project_example()!\r\n");
    p_MQTTClient_sub_callback = &mqttClient_sub_callback;
    SF_peripheral_init();
    if (nfc_connect_wifi_init() == -1) {
        return ;
    }
    mqtt_init();

    my_uart_init();

    osThreadAttr_t options;
    options.name = "mqtt_send_task";
    options.attr_bits = 0;
    options.cb_mem = NULL;
    options.cb_size = 0;
    options.stack_mem = NULL;
    options.stack_size = TASK_STACK_SIZE;
    options.priority = osPriorityNormal;
    mqtt_send_task_id = osThreadNew((osThreadFunc_t)mqtt_send_task, NULL, &options);
    if (mqtt_send_task_id != NULL) {
        printf("ID = %d, Create mqtt_send_task_id is OK!\r\n", mqtt_send_task_id);
    }

    options.name = "mqtt_recv_task";
    mqtt_recv_task_id = osThreadNew((osThreadFunc_t)mqtt_recv_task, NULL, &options);
    if (mqtt_recv_task_id != NULL) {
        printf("ID = %d, Create mqtt_recv_task_id is OK!\r\n", mqtt_recv_task_id);
    }

    options.name = "sensor_collect_task";
    sensor_collect_task_id = osThreadNew((osThreadFunc_t)sensor_collect_task, NULL, &options);
    if (sensor_collect_task_id != NULL) {
        printf("ID = %d, Create sensor_collect_task_id is OK!\r\n", sensor_collect_task_id);
    }



    //创建子任务处理串口数据
    osThreadAttr_t uart_task_opt;
    uart_task_opt.name = "uart_send";
    uart_task_opt.stack_size = 1024*4;
    uart_task_opt.priority = osPriorityNormal;
    uart_task_id_send = osThreadNew((osThreadFunc_t)my_uart_send_task,NULL,&uart_task_opt);
    
    uart_task_opt.name = "uart_recv";
    uart_task_opt.stack_size = 1024*4;
    uart_task_opt.priority = osPriorityNormal;
    uart_task_id_recv = osThreadNew((osThreadFunc_t)my_uart_recv_task,NULL,&uart_task_opt);


}
SYS_RUN(smartFarm_Project_example);
