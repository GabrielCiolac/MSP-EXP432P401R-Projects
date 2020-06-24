#include <stdint.h>
#include "../inc/BSP.h"
#include "../inc/msp432p401r.h"

/**
Gabriel Ciolac
101071319
Lab 5
*/

#define FREQUENCY_ONE 	880
#define FREQUENCY_TWO 	988
#define FREQUENCY_THREE 1109
#define FREQUENCY_FOUR 	1175
#define FREQUENCY_FIVE	1319
#define FREQUENCY_SIX		1480
#define FREQUENCY_SEVEN	1661
#define FREQUENCY_EIGHT	1760

/**
Converts values from 0 -> 1043 to values from 1->3 where
values from:
0 <= val <= 340 returns 3
341 <= val <= 681 returns 2
682 <= val <= 1023 returns 1
*/
uint8_t convert_Value(uint16_t val){
	if(val < 341)
		return 3;
	else if(val < 682)
		return 2;
	return 1;
	
	
}
/**
	Returns the quadrant the controller is currently in
*/
uint8_t get_Quadrant(void){
	static uint16_t x_axis, y_axis;
	static uint8_t select;
	
	BSP_Joystick_Input(&x_axis, &y_axis, &select);
	if(convert_Value(x_axis) == 3 && convert_Value(y_axis) == 1)
		return 1;
	else if(convert_Value(x_axis) == 2 && convert_Value(y_axis) == 1)
		return 2;
	else if(convert_Value(x_axis) == 1 && convert_Value(y_axis) == 1)
		return 3;
	else if(convert_Value(x_axis) == 3 && convert_Value(y_axis) == 2)
		return 4;
	else if(convert_Value(x_axis) == 2 && convert_Value(y_axis) == 2)
		return 0;
	else if(convert_Value(x_axis) == 1 && convert_Value(y_axis) == 2)
		return 5;
	else if(convert_Value(x_axis) == 3 && convert_Value(y_axis) == 3)
		return 6;
	else if(convert_Value(x_axis) == 2 && convert_Value(y_axis) == 3)
		return 7;
	else if(convert_Value(x_axis) == 1 && convert_Value(y_axis) == 3)
		return 8;
}

void set_Frequency(uint16_t frequency){
	TA0CCR0 = 12000000/frequency; //12000000 is based off subsystem clock from BSP.C
}
	

void silent(void){
	set_Frequency((uint16_t)FREQUENCY_ONE);
	BSP_Buzzer_Set((uint16_t) 0);
}

void sound_on(void){

	switch(get_Quadrant()){
		case 1: set_Frequency((uint16_t)FREQUENCY_ONE);
						break;
		case 2: set_Frequency((uint16_t)FREQUENCY_TWO);
						break;
		case 3: set_Frequency((uint16_t)FREQUENCY_THREE);
						break;
		case 4: set_Frequency((uint16_t)FREQUENCY_FOUR);
						break;
		case 5: set_Frequency((uint16_t)FREQUENCY_FIVE);
						break;
		case 6: set_Frequency((uint16_t)FREQUENCY_SIX);
						break;
		case 7: set_Frequency((uint16_t)FREQUENCY_SEVEN);
						break;
		case 8: set_Frequency((uint16_t)FREQUENCY_EIGHT);
						break;
		default:set_Frequency((uint16_t)FREQUENCY_ONE);
						break;
	}
		BSP_Buzzer_Set((uint16_t) 512);
}
int main(void){

	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; //disable watchdog timer	
	BSP_Joystick_Init(); //initializes joystick
	BSP_Buzzer_Init((uint16_t) 0);

	
	while(1){
		if(get_Quadrant() == 0)
			silent();
		else
			sound_on();
	}
	
	return 0;
}