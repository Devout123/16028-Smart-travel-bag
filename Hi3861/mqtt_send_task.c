#include "mqtt_send_task.h"
#include "sys_config.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "cJSON.h"
#include "cmsis_os2.h"

#include "hal_bsp_mqtt.h"
#include "oled_show_log.h"

#define MQTT_TOPIC_MAX 128

void publish_sensor_data(msg_data_t *msg);

char publish_topic[MQTT_TOPIC_MAX] = {0};
/**
 * @brief MQTT  发布消息任务
 */
void mqtt_send_task(void)
{
    while (1) {
        // 发布消息
        publish_sensor_data(&sys_msg_data);
        sleep(1); // 1s
    }
}

/**
 * @brief  发布传感器的信息
 * @note
 * @param  msg:
 * @retval None
 */
void publish_sensor_data(msg_data_t *msg)
{
    cJSON *root = NULL, *array = NULL, *services = NULL;
    cJSON *properties = NULL;

    // 拼接Topic
    memset_s(publish_topic, MQTT_TOPIC_MAX, 0, MQTT_TOPIC_MAX);
    if (sprintf_s(publish_topic, MQTT_TOPIC_MAX, MQTT_TOPIC_PUB_PROPERTIES, DEVICE_ID) > 0) {
        // 组JSON数据
        root = cJSON_CreateObject(); // 创建一个对象
        services = cJSON_CreateArray();
        cJSON_AddItemToObject(root, "services", services);
        array = cJSON_CreateObject();
        cJSON_AddStringToObject(array, "service_id", "attribute");
        properties = cJSON_CreateObject();
        cJSON_AddItemToObject(array, "properties", properties);
        cJSON_AddNumberToObject(properties, "barometer", msg->barometer_value); 
        cJSON_AddNumberToObject(properties, "altitude", msg->altitude_value);
        cJSON_AddNumberToObject(properties, "temperature", msg->temperature);
        cJSON_AddNumberToObject(properties, "humidity", msg->humidity);
        cJSON_AddNumberToObject(properties, "TDS", msg->TDS_value);
        cJSON_AddNumberToObject(properties, "Ultraviolet", msg->Ultraviolet_value);
        cJSON_AddNumberToObject(properties, "pressure", msg->pressure_value);
      
        cJSON_AddItemToArray(services, array);  // 将对象添加到数组中

        /* 格式化打印创建的带数组的JSON对象 */
        char *str_print = cJSON_PrintUnformatted(root);
        if (str_print != NULL) {
            // printf("%s\n", str_print);
            // 发布消息
            MQTTClient_pub(publish_topic, str_print, strlen((char *)str_print));
            cJSON_free(str_print);
            str_print = NULL;
        }

        if (root != NULL) {
            cJSON_Delete(root);
            root = NULL;
            properties = array = services = NULL;
        }
    }
}
