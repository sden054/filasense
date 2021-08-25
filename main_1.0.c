/*******************************************************
This program was created by the
CodeWizardAVR V3.12 Advanced
Automatic Program Generator
© Copyright 1998-2014 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : filasense
Version : 1.0
Date    : 6/2/2021
Author  : dennsk@gmail.com
Company :
Comments:


Chip type               : ATtiny13A
AVR Core Clock frequency: 1.200000 MHz
Memory model            : Tiny
External RAM size       : 0
Data Stack size         : 16
*******************************************************/

#include <tiny13a.h>

// Declare your global variables here
#define inactivityTimeout 32+16 // 32: 7 seconds  (7/0.21845)
#define BTN PINB.0
#define SIG PINB.3
#define PWR PORTB.1
#define ALM PORTB.2

volatile unsigned char inactivityTimer, running, lastState, trig;

// Timer 0 overflow interrupt service routine
interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
	unsigned char currentState;
	// Place your code here
	inactivityTimer++;
	if(BTN==0){
		// button pressed
		trig=1;
	}else{
		if(trig){
      		// button released
      		if(running){
      			running=0;
      			PWR=0;
      		}else{
      			running=1;
      			PWR=1;
      		}
			ALM=1; // reset alarm
      		inactivityTimer=0; // reset timer
            trig=0; //reset trigger
      	}
    }
    if(inactivityTimer>inactivityTimeout){ // timeout occured
    	if(running){
    		running=0;
    		PWR=0;
    		ALM=0; // set alarm
    	}
    	inactivityTimer=0; // reset timer
    }else{
    	// check filament motion (SIG-pin activity)
    	currentState=SIG;
    	if(lastState!=currentState)inactivityTimer=0; // reset timer while state changed
    	lastState=currentState; // save state for futher processing
    }

}

void main(void)
{
// Declare your local variables here

// Input/Output Ports initialization
// Port B initialization
// Function: Bit5=In Bit4=In Bit3=In Bit2=Out Bit1=Out Bit0=In
DDRB=(0<<DDB5) | (0<<DDB4) | (0<<DDB3) | (1<<DDB2) | (1<<DDB1) | (0<<DDB0);
// State: Bit5=T Bit4=T Bit3=T Bit2=0 Bit1=0 Bit0=T
//PORTB=(0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);
PORTB=(0<<PORTB5) | (1<<PORTB4) | (0<<PORTB3) | (1<<PORTB2) | (0<<PORTB1) | (1<<PORTB0);

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: 1.172 kHz
// Mode: Normal top=0xFF
// OC0A output: Disconnected
// OC0B output: Disconnected
// Timer Period: 0.21845 s
TCCR0A=(0<<COM0A1) | (0<<COM0A0) | (0<<COM0B1) | (0<<COM0B0) | (0<<WGM01) | (0<<WGM00);
TCCR0B=(0<<WGM02) | (1<<CS02) | (0<<CS01) | (1<<CS00);
TCNT0=0x00;
OCR0A=0x00;
OCR0B=0x00;

// Timer/Counter 0 Interrupt(s) initialization
TIMSK0=(0<<OCIE0B) | (0<<OCIE0A) | (1<<TOIE0);

// Global enable interrupts
#asm("sei")
/*
while (1)
      {
      // Place your code here

      }
      */
}

/*
	avrdude -p attiny13 -b 9600 -c usbasp -v -v -U flash:w:filasense.hex -U lfuse:w:0x6a:m -U hfuse:w:0xff:m
*/