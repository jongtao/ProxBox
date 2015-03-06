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



void enable_XT2(void)
{
	SetVcoreUp (0x01);
  SetVcoreUp (0x02);  
  SetVcoreUp (0x03);  

	P5SEL |= BIT2 + BIT3; // Port select XT2

	UCSCTL6 &= ~XT2OFF; // Enable XT2
	UCSCTL3 |= SELREF_2;
	UCSCTL4 |= SELA_2; // ACLK = REFO, SMCLK = DCO, MCLK = DCO

	do
	{
		UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
		SFRIFG1 &= ~OFIFG;
	} while (SFRIFG1 & OFIFG);

	UCSCTL6 &= ~XT2DRIVE0;
} // enable_XT2()



int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog ti

	// Config Clock
	enable_XT2();
	__bis_SR_register(SCG0);
	UCSCTL1 = DCORSEL_4;
	__bis_SR_register(SCG0);
 
	UCSCTL2 = FLLD_0 + 1; // 8 Mhz Target
	UCSCTL3 = SELREF_5;
	//UCSCTL4 |= SELS_5 + SELM_5; // SMCLK = MCLK = XT2

/*
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

	UCA0CTL1 = UCSWRST; // DISABLE SPI

	// Config SPI Pins
	P3SEL |= BIT2; // CLK on P3.2
	P3SEL |= BIT0; // MOSI on P3.0

	// Config SPI Registers
	UCB0CTL0 |= UCCKPH | UCMSB | UCMST | UCSYNC;
	UCB0CTL1 |= UCSSEL_2; // SMCLK
	UCB0BR0 = 0;
	UCB0BR1 = 0;
	UCB0CTL1 &= ~UCSWRST; // ENABLE SPI



	// Config Input Pins

	// Config Control Pins

	// Config Blinky
	P1OUT = 0;
	P1DIR |= 0x01;



	srand(32);

	uint8_t led_table[TABLE_SIZE]; // [ 0x0E + Brightness | B | G | R ]
	off(led_table, 48*5);
	uint32_t i, reset_count=0;
	for(;;)
	{
		


		P1OUT ^= 0x01; // blinky

		if(P1IN & BIT5)
			off(led_table, 48*5);
		else
			randomize(led_table, 48*5);

		//random_shift(led_table, 48*5);
		//get_data_test(led_table);
		put_data_24(led_table, 48*5);


		for(i=0;i<0xFFFF;i++) // shitty delay
			__asm__("nop\nnop\nnop\nnop\nnop\nnop\nnop");


		if(reset_count == 0xFF)
		{
			off(led_table, 48*5);
			reset_count = 0;
		}

		reset_count++;
	}

	return 0;
}
