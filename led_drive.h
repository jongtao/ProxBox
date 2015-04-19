/* Jonathan Tao 2015 */
#ifndef LED_DRIVE_H
#define LED_DRIVE_H

// Conditional Compilation Definitions
#define DEMO
#define EIGHT_OPTIMIZED
#define TWENTY_FOUR_OPTIMIZED

#include <msp430.h>
#include <stdint.h>

// USE GCC
//#define SPI_DELAY __asm__("nop\nnop\nnop\nnop")
#define SPI_DELAY while(UCB0STAT & UCBUSY);
#define SPI_TX UCB0TXBUF

#define NUM_LED 384
#define TABLE_SIZE NUM_LED*4


//uint8_t led_table[TABLE_SIZE]; // [ 0x0E + Brightness | B | G | R ]


void put_data(uint8_t* led_table, uint16_t num_led);


#ifdef DEMO
#include <stdlib.h>
void off(uint8_t* led_table, uint16_t num_led);
void randomize(uint8_t* led_table, uint16_t num_led);
void random_shift(uint8_t* led_table, uint16_t num_led);
#endif // DEMO


// Optimized for when there are a multiple of 8 LEDs
#ifdef EIGHT_OPTIMIZED
void put_data_8(uint8_t* led_table, uint16_t num_led);
#endif // EIGHT_OPTIMIZED


// Optimized for when there are a multiple of 24 LEDs
#ifdef TWENTY_FOUR_OPTIMIZED
void put_data_24(uint8_t* led_table, uint16_t num_led);
#endif // TWENTY_FOUR_OPTIMIZED

#endif // LED_DRIVE_H
