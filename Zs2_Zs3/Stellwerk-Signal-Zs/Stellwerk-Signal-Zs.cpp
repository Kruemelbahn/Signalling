/*
 * Stellwerk-Signal-Zs.cpp
 *
 * Signal-Steuerung für Zs-Signale
 *
 * used on Signal Zsx
 * Fuses for ATtiny13: L = 0x6A, H = 0xFF (default)
 * 
 * Created: 10.11.2022 17:00
 * Author : Michael
 *
 *************************************************** 
 *  Copyright (c) 2022 Michael Zimmermann <http://www.kruemelsoft.privat.t-online.de>
 *  All rights reserved.
 *
 *  LICENSE
 *  -------
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************
 */ 

#define F_CPU 1200000UL  // default fuses are 9.6Mhz internal Oszi and CLKDIV 8 => 1.2Mhz
#include <avr/io.h>
#include <avr/interrupt.h>

#define LED_ZS2		PB0 // A Pin5: Led / Signalbild
#define LED_ZS3		PB1 // A Pin6: Led / Signalbild
#define SEND_RM		PB2 // A Pin7: Signal-Stellung zurückmelden
#define SET_SH0		PB3 // E Pin2: 0 = auf Sh0 stellen	
#define SET_SH1		PB4 // E Pin3: 0 = auf Sh1 stellen	

bool bOut(false);
uint8_t iPWM(0); // 0..255 leads to 0,4352s fading time

//*************************************
// initialize Timer
void init_timer()
{
	// Fast PWM Mode
	TCCR0A = (1 << COM0A1) + (1 << COM0B1) + (1 << WGM00) + (1 << WGM01);
	// Prescaler 8 -> leads to 586 Hz
	TCCR0B = (1 << CS01);
	// Trigger an interrupt on overflow
	TIMSK0 = (1 << TOIE0);
	// enable interrupts
	sei();
}

// stop Timer
void stop_timer()
{
	cli();
	TCCR0A = 0;
	TCCR0B = 0;
	TIMSK0 = 0;
}

// Timer Interrupt
ISR(TIM0_OVF_vect)
{
	if((bOut) && (iPWM < 255))
	  ++iPWM;

	if((!bOut) && (iPWM > 0))
	  --iPWM;
}
//*************************************

int main(void)
{
	bOut = false;
	iPWM = 0;
	
  // set to Output
	DDRB   =  (1 << SEND_RM) | (1 << LED_ZS2) | (1 << LED_ZS3);
  // activate internal PullUp
	PORTB |=  (1 << SET_SH0) | (1 << SET_SH1);

	OCR0A = 0;
	OCR0B = 0;

  while (1)
  {
    if(bit_is_clear(PINB, SET_SH1))
		{
			init_timer();	// enables interrupts also...
			bOut = true;
		}
    else
			if(bit_is_clear(PINB, SET_SH0))
				bOut = false;

		// Signal feedback
		if(bOut)
			PORTB |= (1 << SEND_RM);
		else
			PORTB &= ~(1 << SEND_RM);
			
		// show signal
		OCR0A = iPWM;
		OCR0B = iPWM;
		if(!bOut && !iPWM)	// PB0, PB1
		{
			stop_timer();
			PORTB &= ~((1 << LED_ZS2) | (1 << LED_ZS3));
		}
		
  }
  return 0;
}
