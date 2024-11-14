/*
 * Signal-Bue.cpp
 *
 * Licht-Steuerung für einen Bahnübergang
 *
 * used on Lichtcomputer with ATtiny13
 * Fuses for ATtiny13: L = 0x6A, H = 0xFF (default)
 * 
 * Created: 12.11.2024 15:00
 * Author : Michael
 *
 *************************************************** 
 *  Copyright (c) 2024 Michael Zimmermann <https://kruemelsoft.hier-im-netz.de/>
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

#define LED_KR1		PB0 // A Pin5: Led / Andreaskreuz'1						 = 2 Leds on both directions right beside the street
#define LED_KR2		PB1 // A Pin6: Led / Andreaskreuz'2						 = 2 Leds on both directions left beside the street
#define SEND_RM		PB2 // A Pin7: 0 = Warnlicht ist ein					 = feedback to control-panel
#define SET_ON		PB3 // E Pin2: 0 = Warnlicht einschalten			 = switch on the lights
#define LED_BUE 	PB4 // A Pin3: Led / Bü1 - Überwachnungssignal = 2 Leds on both directions right beside the track

bool bOut(false);
uint8_t iPWM(0); // 0..255 leads to 0,4352s fading time
uint8_t iPWM_Pause(0);

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
	if(bOut)
  {
	  if(!iPWM)
		  iPWM_Pause = 0;
		if(!iPWM_Pause)
			if(iPWM < 255)
				++iPWM;
		if(iPWM == 255)
			if(iPWM_Pause < 255)
  			++iPWM_Pause;
		if(iPWM_Pause == 255)
			--iPWM;
	}

	if((!bOut) && (iPWM > 0))
	  --iPWM;
}
//*************************************

int main(void)
{
	bOut = false;
	iPWM = 0;
	iPWM_Pause = 0;
	
  // set to Output
	DDRB   =  (1 << SEND_RM) | (1 << LED_BUE) | (1 << LED_KR1) | (1 << LED_KR2);
  // activate internal PullUp
	PORTB |=  (1 << SET_ON);

	OCR0A = 0;
	OCR0B = 0;

  while (1)
  {
		if(bit_is_clear(PINB, SET_ON))
		{
		  init_timer();	// enables interrupts also...
			bOut = true;
		}
		else
			bOut = false;

		// Signal feedback and Bü1
		if(bOut)
		{
			PORTB |= (1 << SEND_RM);
			if(iPWM_Pause < 255)
				PORTB |= (1 << LED_BUE);
			else
				PORTB &= ~(1 << LED_BUE);
		}
		else
			PORTB &= ~((1 << SEND_RM) | (1 << LED_BUE));
		
		// show street signal as PWM
		OCR0A = iPWM;				// PB0
		if(!bOut && !iPWM)	// PB1
		{
			stop_timer();
			OCR0B = 0;
			PORTB &= ~((1 << LED_KR1) | (1 << LED_KR2));
		}
		else
			OCR0B = 255 - iPWM;
  }
  return 0;
}
