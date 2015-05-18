/* Jonathan Tao 2015 */
#include <msp430.h>

#include <stdint.h>
#include <stdlib.h>

#include "led_drive.h"
#include "sensor.h"



void SetVcoreUp (unsigned int level)
{
  // Open PMM registers for write
  PMMCTL0_H = PMMPW_H;              
  // Set SVS/SVM high side new level
  SVSMHCTL = SVSHE + SVSHRVL0 * level + SVMHE + SVSMHRRL0 * level;
  // Set SVM low side to new level
  SVSMLCTL = SVSLE + SVMLE + SVSMLRRL0 * level;
  // Wait till SVM is settled
  while ((PMMIFG & SVSMLDLYIFG) == 0);
  // Clear already set flags
  PMMIFG &= ~(SVMLVLRIFG + SVMLIFG);
  // Set VCore to new level
  PMMCTL0_L = PMMCOREV0 * level;
  // Wait till new level reached
  if ((PMMIFG & SVMLIFG))
    while ((PMMIFG & SVMLVLRIFG) == 0);
  // Set SVS/SVM low side to new level
  SVSMLCTL = SVSLE + SVSLRVL0 * level + SVMLE + SVSMLRRL0 * level;
  // Lock PMM registers for write access
  PMMCTL0_H = 0x00;
}



void config_clock(void)
{
	/*
	SetVcoreUp (0x01);
  SetVcoreUp (0x02);  
  SetVcoreUp (0x03);  
	*/
/*
  P2DIR |= BIT2;	// SMCLK set out to pins
  P2SEL |= BIT2;
*/
 	UCSCTL3 = SELREF_2;	// Set DCO FLL reference = REFO
  UCSCTL4 |= SELA_2;	// Set ACLK = REFO

	P5SEL |= BIT2 + BIT3; // Port select XT2
	UCSCTL6 &= ~XT2OFF; // Enable XT2

  __bis_SR_register(SCG0);	// Disable the FLL control loop
  UCSCTL0 = 0x0000;	// DCO=0, MOD=0
  UCSCTL1 = DCORSEL_6;	// Target DCO is 12Mhz = (2+1)*4MHz
  UCSCTL2 = FLLD_1 + 2;	// DCO Mult N for 25MHz; Set FLLDiv D = fDCOCLK/1

	UCSCTL3 = SELREF_5;	// use XT2 reference
  __bic_SR_register(SCG0);	// Enable the FLL control loop

	do
	{
		UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
		SFRIFG1 &= ~OFIFG;
	} while (SFRIFG1 & OFIFG);

	UCSCTL6 &= ~XT2DRIVE0;
} // enable_XT2()



void config_spi()
{
	// Config SPI Registers
	UCB0CTL1 |= UCSWRST; // DISABLE SPI

	P3SEL |= BIT2; // CLK on P3.2
	P3SEL |= BIT0; // MOSI on P3.0

	UCB0CTL0 |= UCCKPH | UCMSB | UCMST | UCSYNC;
	UCB0CTL1 |= UCSSEL_2; // SMCLK
	UCB0BR0 = 16;
	UCB0BR1 = 0;

	UCB0CTL1 &= ~UCSWRST; // ENABLE SPI


/* Configures SPI for a different module
	UCA0CTL1 = UCSWRST; // DISABLE SPI
	// Config SPI Pins
	P2SEL |= BIT7; // CLK on P2.7
	P3SEL |= BIT3; // MOSI on P3.3

	// Config SPI Registers
	UCA0CTL0 |= UCCKPH | UCMSB | UCMST | UCSYNC;
	UCA0CTL1 |= UCSSEL_2; // SMCLK
	UCA0BR0 = 0;
	UCA0BR1 = 0;
	UCA0CTL1 &= ~UCSWRST; // ENABLE SPI
*/

} // config_led()



void config_i2c()
{
	// Config I2C Registers
	UCB1CTL1 |= UCSWRST; // DISABLE I2C

	P4SEL |= BIT2; // SCL on P4.2
	P4SEL |= BIT1; // SDA on P4.1

	UCB1CTL0 |= UCMST | UCMODE_3 | UCSYNC; // Master I2C
	UCB1CTL1 |= UCSSEL_2; // SMCLK
	UCB1BR0 = 15; // 12Mhz / 30 = 400Khz
	UCB1BR1 = 0;

	UCB1CTL1 &= ~UCSWRST; // ENABLE I2C
} // config_i2c()



void config_gpio()
{
	/* IR Control Pin Mappings
		D1: P2.0
		D2: P2.2
		D3: P7.4
		D4: P3.1
		INHIBIT: P6.5
	*/
	P2OUT &= ~(BIT0 | BIT2);
	P2DIR |= BIT0 | BIT2;

	P7OUT &= ~BIT4;
	P7DIR |= BIT4;

	P3OUT &= ~BIT1;
	P3DIR |= BIT1;

/*
	P6OUT &= ~BIT5;
	P6DIR |= BIT5;
*/
	// Blinky
	P1OUT = 0;
	P1DIR |= 0x01;
} // config_gpio()







int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog ti

	config_clock();
	config_spi();
	config_i2c();
	config_gpio();


	// LED Table
	uint8_t led_table[TABLE_SIZE]; // [ 0x0E + Brightness | B | G | R ]
	off(led_table, NUM_LED);
	uint32_t i, reset_count=0;
	srand(32);

/*
	// TESTING
	uint8_t IO_A, IO_B, IO_C, IO_D;
	IO_A = IO_B = IO_C = IO_D = 0;
*/
	// SENSE Table
	uint16_t sense_table[NUM_SENSE];


/*
		UCB1I2CSA = SLAVE_ADDR_MASK; // Slave's address

	while(UCB1CTL1 & UCTXSTP);
	UCB1CTL1 |= UCTR | UCTXSTT; // Transmit and Start
	while(!(UCB1IFG & UCTXIFG));
	UCB1TXBUF = 0x00; // Inputs
	while(!(UCB1IFG & UCTXIFG));
	UCB1CTL1 |= UCTXSTP; // Stop
*/
		
//P6OUT |= BIT5; // get ready to turn off
//IR_INPUT; // high impedance

	for(;;)
	{
		P1OUT ^= 0x01; // blinky

/*
		gather(sense_table);
		random_shift(led_table, 48*5);
		//flip(led_table);
		put_data_24(led_table, 48*5);
		for(i=0;i<0xFFFF;i++) // shitty delay
			__asm__("nop\nnop\nnop\nnop\nnop\nnop\nnop");
		// inject(sense_table);

*/

		gather(sense_table);
		off(led_table, NUM_LED);
		process(led_table, sense_table);
		//random_shift(led_table, NUM_LED);
		put_data(led_table, NUM_LED);

	
		// Read Inputs
	/*	
		//while(UCB1CTL1 & UCTXSTP);
		UCB1CTL1 &= ~UCTR;
		UCB1CTL1 |= UCTXSTT; // Transmit and Start
		while(UCB1CTL1 & UCTXSTT);

		while(!(UCB1IFG & UCRXIFG));
		IO_A = UCB1RXBUF; // PORT 0 INPUT
		while(!(UCB1IFG & UCRXIFG));
		IO_B = UCB1RXBUF; // PORT 1 INPUT
		while(!(UCB1IFG & UCRXIFG));
		IO_C = UCB1RXBUF; // PORT 0 INPUT


		UCB1CTL1 |= UCTXSTP; // Stop
		while(UCB1CTL1 & UCTXSTP);
		while(!(UCB1IFG & UCRXIFG));
		IO_D = UCB1RXBUF; // PORT 1 INPUT

		if(IO_B&0x01)
			P1OUT |= 0x01;
		else
			P1OUT ^= 0x01;

*/

/*
		for(i=0;i<0xFFFF;i++) // shitty delay
			__asm__("nop\nnop\nnop\nnop\nnop\nnop\nnop");
			*/
	} // main loop

	return 0;
}
