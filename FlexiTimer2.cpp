/*
  FlexiTimer2.h - Using timer2 with a configurable resolution
  Wim Leers <work@wimleers.com>

  Based on MsTimer2
  Javier Valencia <javiervalencia80@gmail.com>

  History:
    25/April/10 - Based on MsTimer2 V0.5 (from 29/May/09)

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <FlexiTimer2.h>

unsigned long FlexiTimer2::time_units;
void (*FlexiTimer2::func)();
volatile unsigned long FlexiTimer2::count;
volatile char FlexiTimer2::overflowing;
volatile unsigned int FlexiTimer2::tcnt2;

void FlexiTimer2::set(unsigned long ms, void (*f)()) {
    FlexiTimer2::set(ms, 0.001, f);
}


/**
 * @param resolution
 *   0.001 implies a 1 ms (1/1000s = 0.001s = 1ms) resolution. Therefore,
 *   0.0005 implies a 0.5 ms (1/2000s) resolution. And so on.
 */
void FlexiTimer2::set(unsigned long units, double resolution, void (*f)()) {
	float prescaler = 0.0;
	
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega48__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega328P__) || (__AVR_ATmega1280__)
	TIMSK2 &= ~(1<<TOIE2);
	TCCR2A &= ~((1<<WGM21) | (1<<WGM20));
	TCCR2B &= ~(1<<WGM22);
	ASSR &= ~(1<<AS2);
	TIMSK2 &= ~(1<<OCIE2A);
	
	if ((F_CPU >= 1000000UL) && (F_CPU <= 16000000UL)) {	// prescaler set to 64
		TCCR2B |= (1<<CS22);
		TCCR2B &= ~((1<<CS21) | (1<<CS20));
		prescaler = 64.0;
	} else if (F_CPU < 1000000UL) {	// prescaler set to 8
		TCCR2B |= (1<<CS21);
		TCCR2B &= ~((1<<CS22) | (1<<CS20));
		prescaler = 8.0;
	} else { // F_CPU > 16Mhz, prescaler set to 128
		TCCR2B |= ((1<<CS22) | (1<<CS20));
		TCCR2B &= ~(1<<CS21);
		prescaler = 128.0;
	}
#elif defined (__AVR_ATmega8__)
	TIMSK &= ~(1<<TOIE2);
	TCCR2 &= ~((1<<WGM21) | (1<<WGM20));
	TIMSK &= ~(1<<OCIE2);
	ASSR &= ~(1<<AS2);
	
	if ((F_CPU >= 1000000UL) && (F_CPU <= 16000000UL)) {	// prescaler set to 64
		TCCR2 |= (1<<CS22);
		TCCR2 &= ~((1<<CS21) | (1<<CS20));
		prescaler = 64.0;
	} else if (F_CPU < 1000000UL) {	// prescaler set to 8
		TCCR2 |= (1<<CS21);
		TCCR2 &= ~((1<<CS22) | (1<<CS20));
		prescaler = 8.0;
	} else { // F_CPU > 16Mhz, prescaler set to 128
		TCCR2 |= ((1<<CS22) && (1<<CS20));
		TCCR2 &= ~(1<<CS21);
		prescaler = 128.0;
	}
#elif defined (__AVR_ATmega128__)
	TIMSK &= ~(1<<TOIE2);
	TCCR2 &= ~((1<<WGM21) | (1<<WGM20));
	TIMSK &= ~(1<<OCIE2);
	
	if ((F_CPU >= 1000000UL) && (F_CPU <= 16000000UL)) {	// prescaler set to 64
		TCCR2 |= ((1<<CS21) | (1<<CS20));
		TCCR2 &= ~(1<<CS22);
		prescaler = 64.0;
	} else if (F_CPU < 1000000UL) {	// prescaler set to 8
		TCCR2 |= (1<<CS21);
		TCCR2 &= ~((1<<CS22) | (1<<CS20));
		prescaler = 8.0;
	} else { // F_CPU > 16Mhz, prescaler set to 256
		TCCR2 |= (1<<CS22);
		TCCR2 &= ~((1<<CS21) | (1<<CS20));
		prescaler = 256.0;
	}
#endif
	
	tcnt2 = 256 - (int)((float)F_CPU * resolution / prescaler);
	
	if (units == 0)
		time_units = 1;
	else
		time_units = units;
		
	func = f;
}

void FlexiTimer2::start() {
	count = 0;
	overflowing = 0;
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega48__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega328P__) || (__AVR_ATmega1280__)
	TCNT2 = tcnt2;
	TIMSK2 |= (1<<TOIE2);
#elif defined (__AVR_ATmega128__)
	TCNT2 = tcnt2;
	TIMSK |= (1<<TOIE2);
#elif defined (__AVR_ATmega8__)
	TCNT2 = tcnt2;
	TIMSK |= (1<<TOIE2);
#endif
}

void FlexiTimer2::stop() {
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega48__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega328P__) || (__AVR_ATmega1280__)
	TIMSK2 &= ~(1<<TOIE2);
#elif defined (__AVR_ATmega128__)
	TIMSK &= ~(1<<TOIE2);
#elif defined (__AVR_ATmega8__)
	TIMSK &= ~(1<<TOIE2);
#endif
}

void FlexiTimer2::_overflow() {
	count += 1;
	
	if (count >= time_units && !overflowing) {
		overflowing = 1;
		count = 0;
		(*func)();
		overflowing = 0;
	}
}

ISR(TIMER2_OVF_vect) {
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega48__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega328P__) || (__AVR_ATmega1280__)
	TCNT2 = FlexiTimer2::tcnt2;
#elif defined (__AVR_ATmega128__)
	TCNT2 = FlexiTimer2::tcnt2;
#elif defined (__AVR_ATmega8__)
	TCNT2 = FlexiTimer2::tcnt2;
#endif
	FlexiTimer2::_overflow();
}

