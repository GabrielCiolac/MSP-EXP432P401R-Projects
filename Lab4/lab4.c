/**
Lab 4
Gabriel Ciolac
101071319
*/

#include "msp.h"
#include <stdio.h>


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
void STATE_CONTROLLER(uint8_t PORT){
		static 	uint8_t STATE = 0x09; //initial state of 1001 AKA RED ONLY
		
		static volatile uint8_t p4 = (1 << 4); //pins 4 
		if((PORT & 0x02)){ 
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


/*
Toggle the timer between stopped and count up
*/
void TOGGLE_TIMER(){
	static uint8_t current = 0x00;
	current ^= 0x01;
	if(current & 0x01){
		TA0CTL &= (uint16_t)(~(BIT5 | BIT4));
		return;
	}
	TA0CTL |= (uint16_t)(BIT4); //up mode (counter)
}


void PORT1_IRQHandler(void){
	if((P1IFG & 0x02) != 0){
		P1IFG &= ~0x02;
		STATE_CONTROLLER(0x02); //passes 1 to indicate port one
		return;
	}
	P1IFG &= ~0x10; //clears pin 4 interrupt
	TOGGLE_TIMER(); //toggle timer  on off
	
}

void TA0_N_IRQHandler(void) {
	//clear interrupt flag
	TA0CTL &= (uint16_t)(~(1<<0));
	STATE_CONTROLLER(0x00);
}

void CONFIGURE_TIMER(void){
	//CONFIGURE TIMER
	TA0CTL &= (uint16_t) (~((1<<5) | (1<<4))); //stop timer
	TA0CTL &= (uint16_t)(~(1<<0)); //clear interrupt flag
	TA0CCR0 = (uint16_t)(32767); // 32768 - 1
	TA0CTL |= (uint16_t) (1<<8); //select aux clk
	TA0CTL |= (uint16_t)((1<<1)); //interrupt enable
	TA0CTL |= (uint16_t)((1<<4)); //up mode (counter)
	
	//NVIC configuration
	NVIC_SetPriority(TA0_N_IRQn,2);
	NVIC_ClearPendingIRQ(TA0_N_IRQn);
	NVIC_EnableIRQ(TA0_N_IRQn);	
}

int main(void){
	volatile uint8_t p1 = (1 << 1); //pins 1 
	volatile uint8_t p4 = (1 << 4); //pins 4 
	volatile uint8_t p14 = p1 | p4; //pins 1,4 
	volatile uint8_t p0 = 0x01; //pin 0
	volatile uint8_t p012 = (p0 | p1 | (1 << 2)); //pins 0,1,2

	
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; //disable watchdog timer
	
	
	//setting GPIO Function - Don't trust the hardware for defaults
	P1->SEL0 &= ~p0 | ~p14;
	P1->SEL1 &= ~p0 | ~p14;
	
	P2->SEL0 &= ~p012;
	P2->SEL1 &= ~p012;
	
	
	
	//set input pull up resistors. Desired (PxDIR, PxREN, PxOUT) is (0,1,1)

	P1->DIR |= ~p14;
	P1->REN |= p14;
	P1->OUT |= p14;
	
	//set output resistors. Desired (PxDIR, PxREN, PxOUT) is (1,x,x)	P1->DIR |= p0; //this needs to be a bitwise OR, since P1DIR was already set above. Don't want to overwrite
	P2->DIR = p012;
	P1->DIR |= p0;
	
	//sets outputs to off
	P1->OUT |= ~p0;
	P2->OUT = ~p012;
	
	//Device interrupt configuration
	P1IES |= p14;
	P1IFG &= ~p14;
	P1IE |= p14;

	CONFIGURE_TIMER();
	
	//NVIC configuration
	NVIC_SetPriority(PORT1_IRQn, 2);
	NVIC_ClearPendingIRQ(PORT1_IRQn);
	NVIC_EnableIRQ(PORT1_IRQn);
	
	//Globally Enable interrupts in CPU
	__ASM("CPSIE I");
	
	
	INITIAL_STATE (); //default red only
	
	
	//running loop
	while(1){
		__ASM("WFI"); //wait for interrupt
	}
	return 0;
}