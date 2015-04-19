/* Jonathan Tao 2015 */
#include "led_drive.h"



void put_data(uint8_t* led_table, uint16_t num_led)
{
	uint16_t half_led = num_led/16 + 1 + 10; // compute end frame length
	uint16_t max_index = num_led * 4; // maximum led_table row index;
	uint16_t index; // row major order in led_table

	// Start Frame
	SPI_DELAY; //while(UCA0STAT & UCBUSY);
	SPI_TX = 0;
	SPI_DELAY;
	SPI_TX = 0;
	SPI_DELAY;
	SPI_TX = 0;
	SPI_DELAY;
	SPI_TX = 0;

	// LED Frame
	for(index=0; index<max_index; index+=4)
	{
		SPI_DELAY;
		SPI_TX = led_table[index]; // Brightness
		SPI_DELAY;
		SPI_TX = led_table[index+1]; // B
		SPI_DELAY;
		SPI_TX = led_table[index+2]; // G
		SPI_DELAY;
		SPI_TX = led_table[index+3]; // R
	} // for all leds

	// End Frame
	for(index=0; index<half_led; index++)
	{
		SPI_DELAY;
		SPI_TX = 0xFF;
	} // for end frame
} // put_data()



void off(uint8_t* led_table, uint16_t num_led)
{
	uint16_t i, index;
	for(i=0;i<num_led;i++)
	{
		index = i<<2;
		led_table[index] = 0xE0;
		led_table[index+1] = 0;
		led_table[index+2] = 0;
		led_table[index+3] = 0;
	}
} // randomize()



void randomize(uint8_t* led_table, uint16_t num_led)
{
	uint16_t i, index;
	uint8_t r,g,b;
	r = rand();
	g = rand();
	b = rand();
	for(i=0;i<num_led;i++)
	{
		index = i<<2;
		led_table[index] = 0xE0 | 5;
		led_table[index+1] = b;
		led_table[index+2] = g;
		led_table[index+3] = r;
	}
} // randomize()




void random_shift(uint8_t* led_table, uint16_t num_led)
{
	uint16_t i, index, indexx;
	uint8_t r,g,b;
	//r = rand();
	//g = rand();
	//b = rand();

	r = 0x00;
	g = 0xFF;
	b = 0xFF;

	for(i=num_led-1; i>0; i--)
	{
		index = i<<2;
		indexx = (i-1)<<2;

		led_table[index] = led_table[indexx];
		led_table[index+1] = led_table[indexx+1];
		led_table[index+2] = led_table[indexx+2];
		led_table[index+3] = led_table[indexx+3];
	}

	led_table[0] = 0xE0 | 5;
	led_table[1] = b;
	led_table[2] = g;
	led_table[3] = r;
	
} // random_shift()



#define PIXEL \
	SPI_DELAY;\
	SPI_TX =  led_table[index];\
	SPI_DELAY;\
	SPI_TX = led_table[index+1];\
	SPI_DELAY;\
	SPI_TX = led_table[index+2];\
	SPI_DELAY;\
	SPI_TX = led_table[index+3];\
	index+=4;



#ifdef EIGHT_OPTIMIZED
void put_data_8(uint8_t* led_table, uint16_t num_led)
{
	uint16_t half_led = num_led/16 + 1; // compute end frame length
	uint16_t max_index = num_led * 4; // maximum led_table row index;
	uint16_t index; // row major order in led_table

	// Start Frame
	SPI_DELAY; //while(UCA0STAT & UCBUSY);
	SPI_TX = 0;
	SPI_DELAY;
	SPI_TX = 0;
	SPI_DELAY;
	SPI_TX = 0;
	SPI_DELAY;
	SPI_TX = 0;

	// LED Frame
	for(index=0; index<max_index; )
	{
		PIXEL
		PIXEL
		PIXEL
		PIXEL
		PIXEL
		PIXEL
		PIXEL
		PIXEL
	} // for all leds

	// End Frame
	for(index=0; index<half_led; index++)
	{
		SPI_DELAY; SPI_TX = 0xFF;
	}
} // put_data()
#endif



#ifdef TWENTY_FOUR_OPTIMIZED
void put_data_24(uint8_t* led_table, uint16_t num_led)
{
	uint16_t half_led = num_led/16 + 1; // compute end frame length
	uint16_t max_index = num_led * 4; // maximum led_table row index;
	uint16_t index; // row major order in led_table

	// Start Frame
	SPI_DELAY; //while(UCA0STAT & UCBUSY);
	SPI_TX = 0;
	SPI_DELAY;
	SPI_TX = 0;
	SPI_DELAY;
	SPI_TX = 0;
	SPI_DELAY;
	SPI_TX = 0;

	// LED Frame
	for(index=0; index<max_index; )
	{
		PIXEL
		PIXEL
		PIXEL
		PIXEL
		PIXEL
		PIXEL
		PIXEL
		PIXEL
		PIXEL
		PIXEL
		PIXEL
		PIXEL
		PIXEL
		PIXEL
		PIXEL
		PIXEL
		PIXEL
		PIXEL
		PIXEL
		PIXEL
		PIXEL
		PIXEL
		PIXEL
		PIXEL
	} // for all leds

	// End Frame
	for(index=0; index<half_led; index++)
	{
		SPI_DELAY; SPI_TX = 0xFF;
	} // for end frame
} // put_data()
#endif

