/* Jonathan Tao 2015*/
#ifndef SENSOR_H
#define SENSOR_H

#include <msp430.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define NUM_SAMPLES 64
#define NUM_SENSE 128
#define IR_ON (P6OUT |= BIT5)
#define IR_OFF (P6OUT &= ~BIT5)

#define SLAVE_ADDR_MASK 0x20 // OR the last three bits with chip number

void ex_set_input(uint8_t slave_addr);
void ex_begin_inputs(uint8_t slave_addr);
void is_latched(uint8_t port, uint8_t* latched);
// add poll_A prototype
//void gather(uint8_t* led_table, uint16_t num_led);


void poll_binary(uint8_t dual_row, uint8_t slave_addr, uint16_t* sense_table);
void gather_test(uint16_t sense_table[]);
void process_test(uint8_t led_table[], uint16_t sense_table[]);

#endif // SENSOR_H
