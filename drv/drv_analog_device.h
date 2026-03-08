#ifndef __DRV_ANALOG_DEVICE_H__
#define __DRV_ANALOG_DEVICE_H__
#include <stdint.h>

typedef enum _ana_device_type {
    ANA_DEV_IR,
    ANA_DEV_BAT,
    ANA_DEV_RECTIFIER,
}ana_device_type_t;

void analog_device_init(void);
int16_t get_analog_device_value(ana_device_type_t type);



#endif