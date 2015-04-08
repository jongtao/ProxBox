/* Jonathan Tao 2015*/
#ifndef SENSOR_H
#define SENSOR_H

#include <msp430.h>
#include <stdint.h>

#define NUM_SAMPLES 64

uint16_t row_pin_addr[16];
uint16_t row_bit_mask[16];

void ex_set_input(uint8_t slave_addr);
void ex_begin_inputs(uint8_t slave_addr);
// add poll_A prototype
void gather(uint8_t* led_table, uint16_t num_led);



#endif // SENSOR_H
