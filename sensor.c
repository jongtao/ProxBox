/* Jonathan Tao 2015*/
#include "sensor.h"



/*
static uint8_t sense_table[NUM_SENSE]; 
*/




/* IR Control Pin Mappings
	D1: P2.0
	D2: P2.2
	D3: P7.4
	D4: P3.1
	STROBE: P6.5
*/


inline void ir_set_line(uint8_t line_num)
{
	if(line_num & 0x01) // D1
		P2OUT |= BIT0;
	else
		P2OUT &= ~BIT0;

	
	if(line_num & 0x02) // D2
		P2OUT |= BIT2;
	else
		P2OUT &= ~BIT2;
	
	if(line_num & 0x04) // D3
		P7OUT |= BIT4;
	else
		P7OUT &= ~BIT4;

	if(line_num & 0x08) // D4
		P3OUT |= BIT1;
	else
		P3OUT &= ~BIT1;
} // ir_set_line()



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

	for(i=0;i<8;i++) // use memset
		latched[i] = 0;

	IR_ON;

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

	IR_OFF;
} // poll_A()



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
			sense_table[index*2] = 1;
		else
			sense_table[index*2] = 0;

		index++;
	} // for every bit in port
} // poll_A_binary()



void gather(uint8_t* led_table, uint16_t num_led)
{
	uint8_t dual_row, row, chip;

/*
	for(dual_row=0; dual_row<8; dual_row++)
		for(chip=0; chip<2; chip++)
		{
			row = SLAVE_ADDR_MASK | ((dual_row<<1) + chip);
			poll_binary(dual_row, SLAVE_ADDR_MASK | (dual_row<<1));
			//poll_A();
			//poll_B();
		} // for each chip
		*/

} // gather()



void gather_test(uint16_t sense_table[])
{
	memset(sense_table, 0, NUM_SENSE);
	poll_binary(0, SLAVE_ADDR_MASK, sense_table);
} // gather_binary()



void process_test(uint8_t led_table[], uint16_t sense_table[])
{
	srand(32);
	uint16_t i, index;
	uint8_t r,g,b;
	r = rand();
	g = rand();
	b = rand();
	for(i=0;i<16;i++)
	{
		index = i<<2;

		if(sense_table[index])
		{
			led_table[index] = 0xE0 | 5;
			led_table[index+1] = b;
			led_table[index+2] = g;
			led_table[index+3] = r;
		} // on
		else
		{
			led_table[index] = 0xE0;
			led_table[index+1] = 0;
			led_table[index+2] = 0;
			led_table[index+3] = 0;
		} // off
	} // for 16 leds (two rows)
} // process_binary()
