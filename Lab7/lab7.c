#include <msp.h>
/**
Gabriel Ciolac 101071319
SYSC 3310
Lab 7
*/
//Must always include our OS header file
#include "../inc/fate.h"
#include "../inc/BSP.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

//Functions that implement our 2 periodic tasks
//Return type and arguments must always be void
//For now, no local variables
//No calling other functions
//When execution is finished, must always call "Task_stop" with its name
//NOT return: that would probably crash our kernel
void LED_toggle(void)
{
	P1OUT ^= (uint8_t)BIT0;
	Task_stop((uint32_t)LED_toggle);
}
void LED_RGB_toggle(void)
{
	P2OUT = (uint8_t)((P2OUT & (uint8_t)0xF8) | ((P2OUT + (uint8_t)1) & ((uint8_t)7)));
	Task_stop((uint32_t)LED_RGB_toggle);
}


void initialize_TimerA1(void){
		//Configure Timer A1
	TA1CTL &= (uint16_t) (~((1<<5) | (1<<4))); //stop the timer
  TA1CTL &= (uint16_t)(~(1<<0)); //clear interrupt flag


 	TA1CCR0 = (uint16_t)(32767 * 2); //max 16 bit value
  TA1CTL |= (uint16_t) (1<<8); //select aux clk
	TA1EX0 |= (uint16_t)((0x04)); //divide by 4

	//timer mode is not initialized, this is handled by start_timer
	//NVIC configuration not needed, no interupts
}
/**
	resets timer to 0
*/
void clear_timer(void){
	TA1R = 0x00;
}

void stop_timer(void){
	TA1CTL &= (uint16_t) (~((1<<5) | (1<<4))); 
}

void start_timer(void){
	 TA1CTL |= (uint16_t)((1<<4)); //up mode
}

//writes to the LCD with a whitebackground and black text
void write_to_LCD(){
	start_timer(); //required c99 mode to be enabled
	
	static uint16_t font_color;
	font_color = BSP_LCD_Color565(0xFF,0xFF,0xFF);
	
	//reading timer value
	static char c[6];
	//static char ch[6];
	uint16_t current_timer = (uint16_t)(TA1R / (32.767 /4)); //(TA1R) / (frequency /1000) for ms 
	clear_timer();
	//sprintf(ch, "DEBUG: %u ms",(uint16_t)(TA1R / (32.767 /4)));
	//BSP_LCD_DrawString(0,5,ch,font_color);
	sprintf(c,"%u ms",current_timer); 

	BSP_LCD_DrawString(0,0,c,font_color);
	stop_timer();
	Task_stop((uint32_t)write_to_LCD);
	
}

int main()
{
	
	//configure Ports
	
	//LED p1.0 (Red)
	P1SEL0 &= (uint8_t)(~(BIT0));
	P1SEL1 &= (uint8_t)(~(BIT0));
	P1DIR |= (uint8_t)BIT0;
	P1OUT |= (uint8_t)BIT0;
	
	//LED P2.0.1.2 (RGB)
	P2SEL0 &= (uint8_t)(~(BIT0)|(BIT1)|(BIT2));
	P2SEL1 &= (uint8_t)(~((BIT0)|(BIT1)|(BIT2)));
	P2DIR |= (uint8_t)((BIT0)|(BIT1)|(BIT2));
	P2OUT |= (uint8_t)((BIT0)|(BIT1)|(BIT2));
	
	//initializes BSP display
	BSP_LCD_Init();
	initialize_TimerA1();
	
	//Initialize Task list, includes setting up idle task
	//Always the first function that must be called
	Task_list_init();
	
	
	Task_add((uint32_t)write_to_LCD,(uint32_t)200,(uint32_t)1);
	
	//This will begin scheduling our tasks 
	Task_schedule();
	
	return 0;
}
