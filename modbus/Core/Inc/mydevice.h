#ifndef MYDEVICE_H
#define MYDEVICE_H

#include "modbus.h"

// Define your registers (e.g., sensor data)
extern uint16_t sensor_register[10]; // Example: 10 holding registers

// Function prototypes for Modbus handlers
mbus_t mbus_somedevice_open(Modbus_Conf_t *pconf);
mbus_status_t mbus_somedevice_read_3xxxx(mbus_t mb_context);
mbus_status_t mbus_somedevice_write_4xxxx(mbus_t mb_context);

#endif // MYDEVICE_H