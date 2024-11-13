/*
 * Stellwerk-Signal-Ne1-Zp11.cpp
 *
 * Signal-Steuerung für Zp11 an Ne1-Signalen (Trapeztafel)
 *
 * used on Signal Ne1
 * Fuses for ATtiny13: L = 0x6A, H = 0xFF (default)
 * 
 * Created: 14.05.2023 12:00
 * Author : Michael
 *
 *************************************************** 
 *  Copyright (c) 2023 Michael Zimmermann <http://www.kruemelsoft.privat.t-online.de>
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

#define LED_ZP11_A  PB0 // A Pin6: Led / Signalbild Zp11 an Ne1 (lang - kurz - lang) [analog]
#define SEND_RM     PB2 // A Pin7: RM, Zp1 11
#define SET_ZP11	  PB3 // E Pin2: Zp11 an Ne1 einschalten
#define LED_ZP11_D  PB4 // A Pin3: Led / Signalbild Zp11 an Ne1 (lang - kurz - lang) [digital]

bool bOut(false);
bool bOutOn(false);
uint8_t iTakt(0); // Takt
uint8_t iPWM(0);  // 0..255 leads to 0,4352s fading time

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
	if(bOutOn)
	{
	  if(iPWM < 255)
	    ++iPWM;
	  if(iPWM == 255)
	    ++iTakt;
	}

	if(!bOutOn)
	{
	  if(iPWM > 0)
		--iPWM;
	  if(iPWM == 0)
		++iTakt;
	}
}

/***********************************************
  Takt
	1		    aufblenden
	2..6	  an (lang)
	7		    abblenden
  8..9	  aus
	10		  aufblenden
	11..12	an (kurz)
	13		  abblenden
	14..15	aus
	16		  aufblenden
	17..21	an (lang)
	22		  abblenden
          _____    __    _____
         /     \__/  \__/     \
bOutOn   ------   ---   ------
Takt     1..6           16..21 
                 10..12

  22 Takte à 0.4325s => Gesamtdauer = 9,57s	

***********************************************/

int main(void)
{
  bOut = false;
  bOutOn = false;
  iPWM = 0;
  iTakt = 0;
	
  // set to Output
  DDRB   =  ((1 << LED_ZP11_A) | (1 << SEND_RM) | (1 << LED_ZP11_D));
  // activate internal PullUp
  PORTB |=  (1 << SET_ZP11);

  OCR0A = 0;
  OCR0B = 0;

  while (1)
  {
    if(!bOut && (iTakt == 0) && bit_is_set(PINB, SET_ZP11))
  	{
		  init_timer();	// enables interrupts also...
      iTakt = 1;
      bOut = true;
	  }

	  if(((iTakt >= 1) && (iTakt <= 6))	||
	     ((iTakt >= 10) && (iTakt <= 12)) ||
	     ((iTakt >= 16) && (iTakt <= 21)))
	     bOutOn = true; // fade on and keep until off
	  else
	     bOutOn = false;  // fade off and keep until on or end
	
	  if(iTakt > 23)
	  {
      bOut = false;
	    iTakt = 0;	
	  }
	
	  // set output as analog-value using PB0 as analog out
	  OCR0A = iPWM;
		if(!bOut && !iPWM)	// PB1
		{
			stop_timer();
			PORTB &= ~((1 << LED_ZP11_A) | (1 << LED_ZP11_D));
		}
	
	  // set output as digital-value
	  if(bOutOn)
  	  PORTB |= (1 << LED_ZP11_D);
	  else
	    PORTB &= ~(1 << LED_ZP11_D);

	  // set RM
	  if(bOut)
	    PORTB |= (1 << SEND_RM);
	  else
	    PORTB &= ~(1 << SEND_RM);

  }
  return 0;
}
