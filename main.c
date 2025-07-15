// Luke Braverman
// MEMS 1049 - Mechatronics - Lab 10
// 7/15/2025

/* Pseudo Code
Setup:
Initialize clock frequency for wait function
Initialize digit sending function
Initialize SCK, MOSI, and SS as outputs and set respective modes
Set decoding mode, scan limit, and turn on display

Main:
For the month
	Write a 0 to digit 0 ? using function
		Write a 3 to digit 1 ? using function
			Wait 1 second
For the day
	Write a 1 to digit 0 ? using function
		Write a 2 to digit 1 ? using function
			Wait 1 second
For the blank display
	Set both digits blank ? using function
		Wait 2 seconds
Loop indefinitely
*/


#include <avr/io.h>
#define FREQ_CLK 1000000 // Clock frequency (default setting for the ATmega328P -- 8 MHz internal clock with divide-by-8)

//Initializing Functions
void wait(volatile int multiple);
int sendMAX7221(unsigned char, unsigned char);

int main(void)
{
	DDRB = 0b00101100; //Set pins SCK, MOSI, and SS as outputs
	SPCR = 0b01010001; //enable the SPI, set to main mode 0, SCK = Fosc/8, lead with MSB
	SPSR = 0b00000001; //setting SCK using last bit
	
	sendMAX7221(0b00001001, 0b00000000); //set decoding mode turned off for all
	sendMAX7221(0b00001011, 0b00000010); //setting scan limit to 2
	sendMAX7221(0b00001100, 0b00000001); //turn on display
	
	while(1)
	{
	// Display month as 07
	sendMAX7221(0b00000010, 0b01111110); // digit 1 (left) = 0
	sendMAX7221(0b00000001, 0b01110000); // digit 0 (right) = 7
	wait(1000);

	// Display day as 15
	sendMAX7221(0b00000010, 0b00110000); // digit 1 (left) = 1
	sendMAX7221(0b00000001, 0b01011011); // digit 0 (right) = 5
	wait(1000);

	// Clear display
	sendMAX7221(0b00000001, 0b00000000); // digit 0 blank
	sendMAX7221(0b00000010, 0b00000000); // digit 1 blank
	wait(2000);
	}
}

int sendMAX7221(unsigned char command, unsigned char data)
{
	PORTB = PORTB & 0b11111011; //clear PB2, SS bit, so transmission can start
	
	SPDR = command; //Send command
	while(!(SPSR & (1<<SPIF))); //Wait for transmission to finish
	SPDR = data; //Send data
	while(!(SPSR & (1<<SPIF))); //Wait for transmission to finish
	
	PORTB = PORTB | 0b00000100; //Set PB2 to 1, SS bit, to end transmission
	
	return 0;
}
// Clock Frequency Function

void wait(volatile int number_of_msec) {
	// This subroutine creates a delay equal to number_of_msec*T, where T is 1 msec
	// It changes depending on the frequency defined by FREQ_CLK
	char register_B_setting;
	char count_limit;
	
	// Some typical clock frequencies:
	switch(FREQ_CLK) {
		case 16000000:
		register_B_setting = 0b00000011; // this will start the timer in Normal mode with prescaler of 64 (CS02 = 0, CS01 = CS00 = 1).
		count_limit = 250; // For prescaler of 64, a count of 250 will require 1 msec
		break;
		case 8000000:
		register_B_setting =  0b00000011; // this will start the timer in Normal mode with prescaler of 64 (CS02 = 0, CS01 = CS00 = 1).
		count_limit = 125; // for prescaler of 64, a count of 125 will require 1 msec
		break;
		case 1000000:
		register_B_setting = 0b00000010; // this will start the timer in Normal mode with prescaler of 8 (CS02 = 0, CS01 = 1, CS00 = 0).
		count_limit = 125; // for prescaler of 8, a count of 125 will require 1 msec
		break;
	}
	
	while (number_of_msec > 0) {
		TCCR2A = 0x00; // clears WGM00 and WGM01 (bits 0 and 1) to ensure Timer/Counter is in normal mode.
		TCNT2 = 0;  // preload value for testing on count = 250
		TCCR2B =  register_B_setting;  // Start TIMER0 with the settings defined above
		while (TCNT2 < count_limit); // exits when count = the required limit for a 1 msec delay
		TCCR2B = 0x00; // Stop TIMER0
		number_of_msec--;
	}
}
