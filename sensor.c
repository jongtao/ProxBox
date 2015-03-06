/* Jonathan Tao 2015*/
#include "sensor.h"

/*
	 static uint8_t led_table[TABLE_SIZE]; // [ 0x0E + Brightness | B | G | R ]

	 led_table assumed to be in row major order. Each "pixel" is 3 LEDs.
*/

uint16_t row_pin_addr[16];
/* =
{
} // row_pin_addr
*/


uint16_t row_bit_mask[16];
/* =
{
} // row_pin_addr
*/



void gather(uint8_t* led_table, uint16_t num_led)
{
	uint8_t row, sensor, sample;
	uint16_t strength[8];

	for(row=0; row<16; row++)
	{
		// turn on IR	
		strength[0] = strength[1] = strength[2] = strength[3] =
			strength[4] = strength[5] = strength[6] = strength[7] = 0;


		for(sample=0; sample<NUM_SAMPLES; sample++)
			for(sensor=0; sensor<8; sensor++)
			{
				// set sensor mux
				strength[sensor] += row_pin_addr[row] & row_bit_mask[sensor]; // count
			} // for each sensor in row
	} // for all rows
} // gather()



void get_data_test(uint8_t* led_table)
{
	// Assumes at least 3 LEDs
	uint16_t i, index = 0;

	if(P1IN & BIT5)
	{	
		for(i=0; i<3; i++)
		{
			led_table[index] = 0xE0 | 5;
			led_table[index+1] = 0xFF;// b
			led_table[index+2] = 0; // g
			led_table[index+3] = 0xFF; // r
			index+=4;
		} // for i
	} // if pin is high
	else
	{
		for(i=0; i<3; i++)
		{
			led_table[index] = 0xE0 | 5;
			led_table[index+1] = 0;// b
			led_table[index+2] = 0; // g
			led_table[index+3] = 0; // r
			index+=4;
		} // for i
	} // else pin is low
} // get_data()
