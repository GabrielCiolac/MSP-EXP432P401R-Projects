/**
Lab 6
Gabriel Ciolac
101071319
*/

#include "msp.h"
#include <stdio.h>


/**
Sets initial state to RED_ON, RGB_OFF
*/
void initial_state(void){
	P1->OUT |= 0X01; //RED ON
	P2->OUT &= 0X00; //RGB off
}

/**
Debugging only
*/
void toggle_red(void){
	P1->OUT ^= 0x01;
}

void change_timerA1_period(void){
	TA1CTL &= (uint16_t) (~((1<<5) | (1<<4))); //stop the timer
	TA1CCR0 = (uint16_t)(32767 / 2); //set period to 0.5s
	TA1CTL |= (uint16_t)((1<<4) | (1<<5)); //up down mode
}

void load_RGB_state(){
	static uint8_t STATE = 0x00; //sets initial state
	if(STATE == 0x00) //if initial state
		change_timerA1_period(); //change timer period
	STATE++;//increment state
	if(STATE > 0x07) //if above RGB range
		STATE = 0x01; //reset to RED RGB
	P2->OUT &= 0x00; //clear RGB light
	P2->OUT ^= STATE; //bitwise XOR with STATE
}


/*
Toggle the timer between stopped and count up
*/
void toggle_timer(){
	static uint8_t current = 0x00;
	current ^= 0x01;
	if(current & 0x01){
		TA0CTL &= (uint16_t)(~(BIT5 | BIT4));
		return;
	}
	TA0CTL |= (uint16_t)(BIT4); //up mode (counter)
}


void TA0_N_IRQHandler(void) {
	//clear interrupt flag
	TA0CTL &= (uint16_t)(~(1<<0));
	toggle_red();
}

void TA1_N_IRQHandler(void) {
	//clear interrupt flag
	TA1CTL &= (uint16_t)(~(1<<0));
	load_RGB_state();
}

void TA1_0_IRQHandler(void) {
	//clear interrupt flag
	TA1CTL &= (uint16_t)(~(1<<0));
	load_RGB_state();
}

void configure_timers(void){
	//CONFIGURE TIMER A0
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
	
	//Configure Timer A1
	TA1CTL &= (uint16_t) (~((1<<5) | (1<<4))); //stop the timer
  TA1CTL &= (uint16_t)(~(1<<0)); //clear interrupt flag


  TA1CCR0 = (uint16_t)(32767 / 10); //set period to 0.1s

  TA1CTL |= (uint16_t) (1<<8); //select aux clk

  TA1CTL |= (uint16_t)((1<<1)); //interrupt enable
  TA1CTL |= (uint16_t)((1<<4) | (1<<5)); //up down mode

	//NVIC configuration
  NVIC_SetPriority(TA1_N_IRQn,2); //sets priority to 2
  NVIC_ClearPendingIRQ(TA1_N_IRQn);
  NVIC_EnableIRQ(TA1_N_IRQn);
	
	NVIC_SetPriority(TA1_0_IRQn,2); //sets priority to 2
  NVIC_ClearPendingIRQ(TA1_0_IRQn);
  NVIC_EnableIRQ(TA1_0_IRQn);    
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
	
	
	
	//set output resistors. Desired (PxDIR, PxREN, PxOUT) is (1,x,x)	P1->DIR |= p0; //this needs to be a bitwise OR, since P1DIR was already set above. Don't want to overwrite
	P2->DIR = p012;
	P1->DIR |= p0;
	
	//sets outputs to off
	P1->OUT |= ~p0;
	P2->OUT = ~p012;
	

	configure_timers();
	initial_state();
	
	//NVIC configuration
	NVIC_SetPriority(PORT1_IRQn, 2);
	NVIC_ClearPendingIRQ(PORT1_IRQn);
	NVIC_EnableIRQ(PORT1_IRQn);
	
	//Globally Enable interrupts in CPU
	__ASM("CPSIE I");

	
	
	//running loop
	while(1){
		__ASM("WFI"); //wait for interrupt
	}
	return 0;
}