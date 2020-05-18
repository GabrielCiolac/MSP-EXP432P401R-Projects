/**
Lab 2
Gabriel Ciolac
101071319
*/

#include "msp.h"
#include <stdio.h>

#define DELAY_VALUE 100000

/**
Sets initial state to RED_ON, RGB_OFF
*/
void INITIAL_STATE(void){
	P1->OUT |= 0X01; //RED ON
	P2->OUT &= 0X00; //RGB off
}

/**
Debugging only
*/
void TOGGLE_RED(void){
	P1->OUT ^= 0x01;
}

/**
Toggles RED LED
*/
void LOAD_RED_STATE(uint8_t STATE){
	if(STATE & 0x08){ //if mode bit is a 1, toggle RED
		P1->OUT ^= 0x01;
		return;
	}
	P1->OUT &= ~0x01; //otherwise clear RED, NOTE TO SELF: You spent 6 hours troubleshooting this, remember in the future &= 0x00 will cause switch to be stuck closed
}

void LOAD_RGB_STATE(uint8_t STATE){
	P2->OUT &= 0x00; //clear RGB light
	
	if(STATE & 0x08) //if mode bit is 1 exit
		return;
	P2->OUT ^= STATE; //otherwise bitwise XOR with STATE
}

/**
STATE_STATE: takes switch 1 as an input

state is intially loaded as 1001.
Bits 0 through 2 are the RGB control bits
Bit 3 is the mode bit, when mode is 1 RED LED State. When mode is 0, RGB State
*/
void STATE_CONTROLLER(void){
		static 	uint8_t STATE = 0x09; //initial state of 1001 AKA RED ONLY
		
		static volatile uint8_t p4 = (1 << 4); //pins 4 
		if((P1IN & p4)){
			STATE ^= 0x08; //toggle mode bit
			LOAD_RGB_STATE(STATE);
			LOAD_RED_STATE(STATE);
			return;
		}
		if(STATE & 0x08){ //checks mode bit, if 1 toggles red
			LOAD_RED_STATE(STATE);
			return;
		}
	
		STATE += 1; //increments STATE
		if(STATE > 0x07 || STATE < 0x01) //if out of bounds, reset
			STATE = 0x01;
		LOAD_RGB_STATE(STATE); //if programs made it this far, load RGB_STATE
}





int main(void){
	volatile uint8_t p1 = (1 << 1); //pins 1 
	volatile uint8_t p4 = (1 << 4); //pins 4 
	volatile uint8_t p14 = p1 | p4; //pins 1,4 
	volatile uint8_t p0 = 0x01; //pin 0
	volatile uint8_t p012 = (p0 | p1 | (1 << 2)); //pins 0,1,2

	
	int i;
	
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; //disable watchdog timer
	
	
	//setting GPIO Function - Don't trust the hardware for defaults
	P1->SEL0 &= ~p0 | ~p14;
	P1->SEL1 &= ~p0 | ~p14;
	
	P2->SEL0 &= ~p012;
	P2->SEL1 &= ~p012;
	
	
	
	//set input pull down resistors. Desired (PxDIR, PxREN, PxOUT) is (0,1,0)

	P1->DIR |= ~p14;
	P1->REN |= p14;
	P1->OUT |= ~p14;
	
	//set output resistors. Desired (PxDIR, PxREN, PxOUT) is (1,x,x)	P1->DIR |= p0; //this needs to be a bitwise OR, since P1DIR was already set above. Don't want to overwrite
	P2->DIR = p012;
	P1->DIR |= p0;
	
	//sets outputs to off
	P1->OUT = ~p0;
	P2->OUT = ~p012;
	
	INITIAL_STATE (); //default red only
	
	
	//running loop
	while(1){
		

		while((P1IN & p1) && (P1IN & p4)){ //if p1 or p4 aren't clicked, do nothing
			__asm("");
		}
		i = DELAY_VALUE; //sets delay value, for debouncer
		while(i > 0){
		i--;
		}//count down debouncer
		
		if((P1IN & p1) && (P1IN & p4)){ //if debouncer passes, and found to now be unclicked, do nothing 
			__asm("");
			continue;
		}
		else{
			STATE_CONTROLLER();
		}
	}
}