/* Jonathan Tao 2015*/
#ifndef SENSOR_H
#define SENSOR_H

#include <msp430.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define NUM_SAMPLES 32
#define NUM_SENSE 128
#define IR_ON (P6OUT &= ~BIT5)
#define IR_OFF (P6OUT |= BIT5)
#define IR_INPUT (P6DIR &= ~BIT5)
#define IR_OUTPUT (P6DIR |= BIT5)

#define SLAVE_ADDR_MASK 0x20 // OR the last three bits with chip number

void ex_set_input(uint8_t slave_addr);
void ex_begin_inputs(uint8_t slave_addr);
void select_ir(uint8_t row);
void eval_magnitude(uint8_t port, uint16_t sense_table[], uint16_t index);
void poll_A(uint8_t row, uint8_t slave_addr, uint16_t samples,
	uint16_t sense_table[]);

void gather(uint16_t sense_table[]);
void process(uint8_t led_table[], uint16_t sense_table[]);
void flip(uint8_t led_table[]);


// TEST
void inject(uint16_t sense_table[]);

void poll_binary(uint8_t dual_row, uint8_t slave_addr, uint16_t* sense_table);
void gather_test(uint16_t sense_table[]);
void process_test(uint8_t led_table[], uint16_t sense_table[]);

#endif // SENSOR_H
