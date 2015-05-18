/* Jonathan Tao 2015*/
#include "sensor.h"



/*
static uint8_t sense_table[NUM_SENSE]; 
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
	UCB1CTL1 |= UCTXSTP; // Stop
} // request_ex_inputs()



inline void select_ir(uint8_t row)
{

/* IR Control Pin Mappings
	D1: P2.0
	D2: P2.2
	D3: P7.4
	D4: P3.1
	INHIBIT: P6.5
*/

//IR_OUTPUT;

	if(row & 0x01)
		P2OUT |= BIT0;
	else // not 0x01
		P2OUT &= ~BIT0;

	if(row & 0x02)
		P2OUT |= BIT2;
	else // not 0x02;
		P2OUT &= ~BIT2;

	if(row & 0x04)
		P7OUT |= BIT4;
	else // not 0x04;
		P7OUT &= ~BIT4;

	if(row & 0x08)
		P3OUT |= BIT1;
	else
		P3OUT &= ~BIT1;
//IR_INPUT;
/*
uint32_t d;

		for(d=0;d<0xFF;d++) // shitty delay
			__asm__("nop\nnop\nnop\nnop\nnop\nnop\nnop");
*/

} // select_ir()



void eval_magnitude(uint8_t port, uint16_t sense_table[], uint16_t index)
{
	uint8_t offset = 0;
	uint16_t bit;

	for(bit=0x0001; bit<0x0100; bit<<=1)	
	{
		if(!(port & bit)) // if the sensor senses something (active low)
			sense_table[index + offset]++; // increment table

		offset++;
	} // for all bits
} // eval_magnitude()



void poll_A(uint8_t row, uint8_t slave_addr, uint16_t samples,
	uint16_t sense_table[])
{
	uint16_t i;
	uint8_t IO_A, IO_B;
	uint16_t index = row * 8; // sensor number: 8 sensors per row

	uint16_t bit = 0;
	uint8_t offset = 0;


	// turn on IR
	select_ir(row);	

	// zero table
	for(offset=0; offset<8; offset++)
		sense_table[index + offset] = 0;

	// Begin I2C communication
	ex_begin_inputs(slave_addr);

	// Request Data
	//while(UCB1CTL1 & UCTXSTP);
	UCB1CTL1 &= ~UCTR;
	UCB1CTL1 |= UCTXSTT; // Transmit and Start
	while(UCB1CTL1 & UCTXSTT);

	// Collect data
	for(i=0; i<samples-1; i++)
	{
		while(!(UCB1IFG & UCRXIFG));
		IO_A = UCB1RXBUF; // PORT 0 INPUT

		eval_magnitude(IO_A, sense_table, index);

		while(!(UCB1IFG & UCRXIFG));
		IO_B = UCB1RXBUF; // PORT 1 INPUT
	} // for 0 to samples - 1

	// last sample
	while(!(UCB1IFG & UCRXIFG)); // if the sensor senses something (active low)
	IO_A = UCB1RXBUF; // PORT 0 INPUT
	
	eval_magnitude(IO_A, sense_table, index);

	UCB1CTL1 |= UCTXSTP; // Stop
	while(UCB1CTL1 & UCTXSTP);
	while(!(UCB1IFG & UCRXIFG));
	IO_B = UCB1RXBUF; // PORT 1 INPUT

} // poll_A()



void poll_B(uint8_t row, uint8_t slave_addr, uint16_t samples,
	uint16_t sense_table[])
{
	uint16_t i;
	uint8_t IO_A, IO_B;
	uint16_t index = row * 8; // sensor number: 8 sensors per row

	uint16_t bit = 0;
	uint8_t offset = 0;
	uint32_t d;


	// turn on IR
	select_ir(row);	


	// zero table
	for(offset=0; offset<8; offset++)
		sense_table[index + offset] = 0;

	// Begin I2C communication
	ex_begin_inputs(slave_addr);

	// Request Data
	//while(UCB1CTL1 & UCTXSTP);
	UCB1CTL1 &= ~UCTR;
	UCB1CTL1 |= UCTXSTT; // Transmit and Start
	while(UCB1CTL1 & UCTXSTT);

	// Collect data
	for(i=0; i<samples-1; i++)
	{
		while(!(UCB1IFG & UCRXIFG));
		IO_A = UCB1RXBUF; // PORT 0 INPUT

		while(!(UCB1IFG & UCRXIFG));
		IO_B = UCB1RXBUF; // PORT 1 INPUT

		eval_magnitude(IO_B, sense_table, index);
	} // for 0 to samples - 1

	// last sample
	while(!(UCB1IFG & UCRXIFG)); // if the sensor senses something (active low)
	IO_A = UCB1RXBUF; // PORT 0 INPUT
	
	UCB1CTL1 |= UCTXSTP; // Stop
	while(UCB1CTL1 & UCTXSTP);
	while(!(UCB1IFG & UCRXIFG));
	IO_B = UCB1RXBUF; // PORT 1 INPUT

	eval_magnitude(IO_B, sense_table, index);

} // poll_B()



void gather(uint16_t sense_table[])
{
	uint8_t chip; // iterative index
	uint8_t row, slave_addr; // actual component

	row = 0;
	slave_addr = SLAVE_ADDR_MASK;

	for(chip=0; chip<8; chip++)
	{ 
		
		if(
			(slave_addr == (SLAVE_ADDR_MASK | 0x00)) ||
			(slave_addr == (SLAVE_ADDR_MASK | 0x01)) ||
			(slave_addr == (SLAVE_ADDR_MASK | 0x02)) ||
			(slave_addr == (SLAVE_ADDR_MASK | 0x03)) ||
			(slave_addr == (SLAVE_ADDR_MASK | 0x04)) ||
			(slave_addr == (SLAVE_ADDR_MASK | 0x05))

			) // for testing single panel
		{
			poll_A(row, slave_addr, NUM_SAMPLES, sense_table);
			row++;

			poll_B(row, slave_addr, NUM_SAMPLES, sense_table);
			row++;

			slave_addr++;
		}
		else
		{
			row = row +2;
			slave_addr++;
		}

	} // for 2 chips on a side
	

	//IR_OUTPUT;
} // gather()



void process(uint8_t led_table[], uint16_t sense_table[])
{
	uint16_t i, index;
	uint8_t r,g,b;
	uint8_t brightness;
	r = 0;
	g = 0xFF;
	b = 0xFF;

	for(i=0;i<NUM_SENSE;i++)
	{
		b = (sense_table[i] & 0x03)<<4;
		g = (sense_table[i] & 0x0C)<<2;
		r = (sense_table[i] & 0x30);
		index = i*12; // every three LEDs
		//brightness = 0xE0 | ((sense_table[i]) & 0x1F);
		brightness = 0xFF;

		led_table[index] = brightness;
		led_table[index+1] = b;
		led_table[index+2] = g;
		led_table[index+3] = r;

		led_table[index+4] = brightness;
		led_table[index+4+1] = b;
		led_table[index+4+2] = g;
		led_table[index+4+3] = r;

		led_table[index+8] = brightness;
		led_table[index+8+1] = b;
		led_table[index+8+2] = g;
		led_table[index+8+3] = r;
	} // for 16 leds (two rows)
} // process()



void flip(uint8_t led_table[])
{
	uint8_t row, led, index, dest;
	uint16_t super_i;
	uint8_t temp[4];

	for(row=1; row<16; row+=2)
	{
		index = row*24; // 24 leds in each row

		for(led=0; led<12; led++)
		{
			// copy #1 to temp
			super_i = index + led;
			temp[0] = led_table[super_i];
			temp[1] = led_table[super_i + 1];
			temp[2] = led_table[super_i + 2];
			temp[3] = led_table[super_i + 3];

			// copy #2 to #1 (overwrite)
			dest = index + (23 - led);
			led_table[super_i    ] = led_table[index];
			led_table[super_i + 1] = led_table[index + 1];
			led_table[super_i + 2] = led_table[index + 2];
			led_table[super_i + 3] = led_table[index + 3];

			// copy temp(#1) to #2
			led_table[index    ] = temp[0];
			led_table[index + 1] = temp[1];
			led_table[index + 2] = temp[2];
			led_table[index + 3] = temp[3];
		} // for every LED in row, flip
	} // for every other row

} //






// TEST


void inject(uint16_t sense_table[])
{
	uint8_t i;
	for(i=0; i<NUM_SENSE; i++)
		sense_table[i]++; // vary the brightness; allow overflow to wrap
} // inject()





void poll_binary(uint8_t dual_row, uint8_t slave_addr, uint16_t* sense_table)
{
	uint8_t IO_A, IO_B;
	uint16_t i, index;

	ex_begin_inputs(slave_addr);

	while(UCB1CTL1 & UCTXSTP);
	UCB1CTL1 &= ~UCTR;
	UCB1CTL1 |= UCTXSTT; // Transmit and Start
	while(UCB1CTL1 & UCTXSTT);

	while(!(UCB1IFG & UCRXIFG));
	IO_A = UCB1RXBUF; // PORT 0 INPUT

	UCB1CTL1 |= UCTXSTP; // Stop
	while(UCB1CTL1 & UCTXSTP);
	while(!(UCB1IFG & UCRXIFG));
	IO_B = UCB1RXBUF; // PORT 1 INPUT

	index = 0;
	for(i=0x0001; i<0x0100; i<<=1)	
	{
		if(IO_A & i)
			sense_table[index] = 1;
		else
			sense_table[index] = 0;

		if(IO_B & i)
			sense_table[8+index] = 1;
		else
			sense_table[8+index] = 0;

		index++;
	} // for every bit in port
} // poll_A_binary()



void gather_test(uint16_t sense_table[])
{
	memset(sense_table, 0, NUM_SENSE);
	poll_binary(0, SLAVE_ADDR_MASK, sense_table);
} // gather_binary()



void process_test(uint8_t led_table[], uint16_t sense_table[])
{
	uint16_t i, index;
	uint8_t r,g,b;
	r = 0;
	g = 0xFF;
	b = 0xFF;

	for(i=0;i<16;i++)
	{
		index = i*12;

		if(sense_table[i])
		{
			led_table[index] = 0xE0 | 5;
			led_table[index+1] = b;
			led_table[index+2] = g;
			led_table[index+3] = r;

			led_table[index+4] = 0xE0 | 5;
			led_table[index+4+1] = b;
			led_table[index+4+2] = g;
			led_table[index+4+3] = r;

			led_table[index+8] = 0xE0 | 5;
			led_table[index+8+1] = b;
			led_table[index+8+2] = g;
			led_table[index+8+3] = r;

		} // on
		else
		{
			led_table[index] = 0xE0;
			led_table[index+1] = 0;
			led_table[index+2] = 0;
			led_table[index+3] = 0;

			led_table[index+4] = 0xE0;
			led_table[index+4+1] = 0;
			led_table[index+4+2] = 0;
			led_table[index+4+3] = 0;

			led_table[index+8] = 0xE0;
			led_table[index+8+1] = 0;
			led_table[index+8+2] = 0;
			led_table[index+8+3] = 0;
		} // off
	} // for 16 leds (two rows)
} // process_binary()
