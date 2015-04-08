/* Jonathan Tao 2015*/
#include "sensor.h"

#define NUM_SENSE 128


/*
static uint8_t sense_table[NUM_SENSE]; 
*/


//uint16_t row_pin_addr[16];
/* =
{
} // row_pin_addr
*/


//uint16_t row_bit_mask[16];
/* =
{
} // row_pin_addr
*/



void ex_set_input(uint8_t slave_addr)
{
	// configure expander IO as inputs
	UCB1I2CSA = slave_addr; // Slave's address
	while(UCB1CTL1 & UCTXSTP);
	UCB1CTL1 |= UCTR | UCTXSTT; // Transmit and Start
	while(!(UCB1IFG & UCTXIFG)); // wait for byte to send
	UCB1TXBUF = 0x06; // Configuration
	while(!(UCB1IFG & UCTXIFG));
	UCB1TXBUF = 0xFF; // PORT 0 CONFIG
	while(!(UCB1IFG & UCTXIFG));
	UCB1TXBUF = 0xFF; // PORT 1 CONFIG
	while(!(UCB1IFG & UCTXIFG));
	UCB1CTL1 |= UCTXSTP; // Stop
} // set_ex_input()



void ex_begin_inputs(uint8_t slave_addr)
{
	// Request Inputs
	UCB1I2CSA = slave_addr; // Slave's address
	while(UCB1CTL1 & UCTXSTP);
	UCB1CTL1 |= UCTR | UCTXSTT; // Transmit and Start
	while(!(UCB1IFG & UCTXIFG));
	UCB1TXBUF = 0x00; // Inputs
	while(!(UCB1IFG & UCTXIFG));
	//UCB1CTL1 |= UCTXSTP; // Stop
} // request_ex_inputs()



inline void is_latched(uint8_t port, uint8_t* latched)
{
	/*
	if(latched[0] == 0 && !(0x01 & port))
	{
		latched[0] = 1;
	} // if latched 1
*/
} // is_latched()



void poll_A(uint8_t slave_addr, uint16_t samples, uint16_t* sense_table,
	uint16_t table_size)
{
	// TODO doesn't work yet

	uint16_t i;
	uint8_t IO_A, IO_B;
	uint8_t latched[8];

	for(i=0;i<8;i++)
		latched[i] = 0;

	// Read Inputs
	//while(UCB1CTL1 & UCTXSTP);
	UCB1CTL1 &= ~UCTR;
	UCB1CTL1 |= UCTXSTT; // Transmit and Start
	while(UCB1CTL1 & UCTXSTT);


	for(i=0; i<samples-1; i++)
	{
		while(!(UCB1IFG & UCRXIFG));
		IO_A = UCB1RXBUF; // PORT 0 INPUT
		while(!(UCB1IFG & UCRXIFG));
		IO_B = UCB1RXBUF; // PORT 1 INPUT
		while(!(UCB1IFG & UCRXIFG));
		IO_A = UCB1RXBUF; // PORT 0 INPUT
	} // for 0 to samples - 1


	// last sample
	UCB1CTL1 |= UCTXSTP; // Stop
	while(UCB1CTL1 & UCTXSTP);
	while(!(UCB1IFG & UCRXIFG));
	IO_B = UCB1RXBUF; // PORT 1 INPUT
} // poll_A()



void gather(uint8_t* led_table, uint16_t num_led)
{
	uint8_t row, chip, sensor, sample;

	for(row=0; row<16; row++)
		for(chip=0; chip<2; chip++)
		{
			//poll_A();
			//poll_B();
		} // for each chip
} // gather()
