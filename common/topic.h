#ifndef __TOPIC_H__
#define __TOPIC_H__
#include <stdint.h>

#define BLE_TAG_ADDR_SIZE      6

typedef struct _ble_topic_msg {
    uint8_t addr[BLE_TAG_ADDR_SIZE];                                            /**@ 地址 */
    int8_t rssi;                                                                /**@ rssi */
}ble_topic_msg_t;

#endif
