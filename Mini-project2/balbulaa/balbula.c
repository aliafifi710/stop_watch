/*
 * balbula.c
 *
 *  Created on: Feb 12, 2022
 *      Author: Ali Afifi
 */
#include <avr/io.h>
#include <util/delay.h> // for delay function
#include <avr/interrupt.h>

unsigned char SEC=0;
unsigned char MIN=0;
unsigned char HOUR=0;

ISR(INT0_vect)
{
	if(~(PIND& (1<<2)))
	{
		SEC=0;
		MIN=0;
		HOUR=0;
		//RESET THE STOPWATCH
	}


}
ISR(INT1_vect)
{
	if(PIND& (1<<3))
	{
		TCCR1B &= ~(1<<0) &~(1<<1) &~ (1<<2); //PAUSE THE STOPWATCH *NO CLOCK SOURCE*
	}

}
ISR(INT2_vect)
{
	if(~(PINB&(1<<2)))
	{
		TCCR1B |= (1<<CS12) | (1<<CS10) | (1<<WGM12); //RESUME STOPWATCH

	}

}

ISR (TIMER1_COMPA_vect)
{
	SEC ++;
	if (SEC == 60)
	{
		SEC=0;
		MIN ++;          // start counting minutes and reset seconds //
	}
	if (MIN == 60)
	{
		SEC=0;
		MIN=0;
		HOUR ++;        // start counting hours and reset both minutes and seconds //
	}
	if (HOUR == 24)
	{
		SEC=0;
		MIN=0;
		HOUR=0;         // start count from the beginning //
	}

}

void TIMER_1_init()
{
	TCNT1 = 0;                                    //Set timer1 initial count to zero
	TCCR1A |= (1<<FOC1A) ;                        // non- PWM mode
	TIMSK |= (1<<OCIE1A);                         //Enable Timer1 Compare A Interrupt
	TCCR1B = (1<<CS12) | (1<<CS10) | (1<<WGM12); //1024 PRESCALER WITH CTC MODE
	OCR1A = 970 ;                                 //set compare value


}
void INT0_init()
{
	PORTD|=(1<<2);                        //ACTIVATE PULL UP RESISTOR FOR INT0
	GICR |=(1<<6);                       // ENABLE INT 0
	DDRD &= ~(1<<2);                    //INT0 PUSH BUTTON
	MCUCR |=(1<<1);  MCUCR &= ~(1<<0); // ACTIVATE WITH FALLING EDGE


}
void INT1_init()
{
	DDRD &= ~(1<<3);         //INT1 PUSH BUTTON
	GICR |=(1<<7);           // ENABLE INT 1
	MCUCR |=(1<<3)|(1<<2);	 // ACTIVATE WITH RAISING EDGE


}
void INT2_init()
{
	DDRB &= ~(1<<2);    //INT2/ PUSH BUTTON
	PORTB|=(1<<2);      //ACTIVATE PULL UP RESISTOR FOR INT2
	GICR |=(1<<5);      // ENABLE INT 2
	MCUCSR &= ~(1<<6);  // ACTIVATE WITH FALLING EDGE

}


int main(void)
{
	SREG |=(1<<7); //ENABLE GLOBAL INTERRUPT I-BIT
	DDRC|=0X0F;    //SET THE FIRST 4 BITS OF PORTC AS OUTPUT PINS
	DDRA|=0X3F;    //SET THE FIRST 6 BITS OF PORTA AS OUTPUT PINS
	PORTC &=0XF0;  //SET THE FIRST 4 BITS OF PORTC TO LOGIC_LOW
	PORTA =0X3F;   //SET THE FIRST 6 BITS OF PORTA TO LOGIC_HIGH

	TIMER_1_init();
	INT0_init();
	INT1_init();
	INT2_init();

	while(1)
	{
		PORTA = (1<<0);    // enable PA0 (SEC1)
		PORTC = SEC % 10;   // increment SEC from 0 to 9
		_delay_ms(2);
		PORTA = (1<<1);     // enable PA1 (SEC2)
		PORTC = SEC / 10;   // increment only when number is divisible by 10
		_delay_ms(2);
		PORTA = (1<<2);      // enable PA2 (MIN1)
		PORTC = MIN % 10;
		_delay_ms(2);
		PORTA = (1<<3);       // enable PA3 (MIN2)
		PORTC = MIN / 10;
		_delay_ms(2);
		PORTA = (1<<4);        // enable PA4 (HOUR1)
		PORTC = HOUR % 10;
		_delay_ms(2);
		PORTA = (1<<5);          // enable PA5 (HOUR2)
		PORTC = HOUR / 10;
		_delay_ms(2);
	}


}


