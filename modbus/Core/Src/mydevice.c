#include "mydevice.h"
#include "modbus.h" // Ensure proto functions are declared
#include <stddef.h> // For NULL
#include <stdio.h>

// Define your registers (initialize with sample data)
uint16_t sensor_register[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; // Example values

// Modbus open function (initializes the context)
mbus_t mbus_somedevice_open(Modbus_Conf_t *pconf)
{
    // The library handles configuration via defines in modbus_conf.h
    // You can add device-specific initialization here
    return 0; // Return a valid mbus_t (e.g., 0 for context ID)
}

// Handler for reading holding registers (Modbus function 03)
mbus_status_t mbus_somedevice_read_3xxxx(mbus_t mb_context)
{
    printf("Read Handler Called\n");
    fflush(stdout);
    int address = 0;
    int quantity = 10;
    uint16_t *data = (uint16_t *)g_mbusContext[mb_context].data;
    for (int i = 0; i < quantity; i++)
    {
        data[i] = sensor_register[address + i];
    }
    return MBUS_RESPONSE_OK;
}

// Handler for writing holding registers (Modbus function 16)
mbus_status_t mbus_somedevice_write_4xxxx(mbus_t mb_context)
{
    int address;
    mbus_proto_address(mb_context, &address);
    int quantity;
    mbus_proto_quantity(mb_context, &quantity);
    uint16_t *data;
    mbus_proto_data(mb_context, &data);

    // Check if address is valid
    if (address >= 0 && address + quantity <= 10)
    {
        for (int i = 0; i < quantity; i++)
        {
            sensor_register[address + i] = data[i];
        }
        return MBUS_RESPONSE_OK;
    }
    return MBUS_RESPONSE_ILLEGAL_DATA_ADDRESS;
}