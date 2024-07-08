#ifndef SYS_CONFIG_H
#define SYS_CONFIG_H

#include "cmsis_os2.h"
#include "hal_bsp_structAll.h"

// DeviceSecret fs12345678
// 设备ID
#define DEVICE_ID "668a4455c2e5fa1b15909b5a_hi3861"
// MQTT客户端ID
#define MQTT_CLIENT_ID "668a4455c2e5fa1b15909b5a_hi3861_0_0_2024070707"
// MQTT用户名
#define MQTT_USER_NAME "668a4455c2e5fa1b15909b5a_hi3861"
// MQTT密码
#define MQTT_PASS_WORD "d53995d13a9be487cfafc792498224d68de5cf0fec12b28146e255f4c03869c9"
// 华为云平台的IP地址
#define SERVER_IP_ADDR "121.36.42.100"
// 华为云平台的IP端口号
#define SERVER_IP_PORT 1883
// 订阅 接收控制命令的主题
#define MQTT_TOPIC_SUB_COMMANDS "$oc/devices/%s/sys/commands/#"
// 发布 成功接收到控制命令后的主题
#define MQTT_TOPIC_PUB_COMMANDS_REQ "$oc/devices/%s/sys/commands/response/request_id=%s"
#define MALLOC_MQTT_TOPIC_PUB_COMMANDS_REQ "$oc/devices//sys/commands/response/request_id="
// 发布 设备属性数据的主题
#define MQTT_TOPIC_PUB_PROPERTIES "$oc/devices/%s/sys/properties/report"
#define MALLOC_MQTT_TOPIC_PUB_PROPERTIES "$oc/devices//sys/properties/report"

typedef struct {
    int top;  // 上边距
    int left; // 下边距
    int width; // 宽
    int hight; // 高
} margin_t;   // 边距类型

typedef struct message_data {
    unsigned char fanStatus;
    int barometer_value;    //大气压强
    int altitude_value;     //海拔
    int temperature;       // 温度值
    int humidity;          // 湿度值
    int TDS_value;         //水质
    int Ultraviolet_value; //紫外线强度
    int pressure_value;    //压力(重量)
   
    
    hi_nv_save_sensor_threshold nvFlash;
    tn_pcf8574_io_t pcf8574_io;
} msg_data_t;

extern msg_data_t sys_msg_data; // 传感器的数据

#endif
