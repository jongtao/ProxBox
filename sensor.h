/* Jonathan Tao 2015*/
#ifndef SENSOR_H
#define SENSOR_H

#include <msp430.h>
#include <stdint.h>

#define NUM_SAMPLES 64

uint16_t row_pin_addr[16];
uint16_t row_bit_mask[16];

void gather(uint8_t* led_table, uint16_t num_led);

void get_data_test(uint8_t* led_table);



#endif // SENSOR_H
