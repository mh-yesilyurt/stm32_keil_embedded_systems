#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "delay.h"



GPIO_InitTypeDef GPIO_InitStructure;
void offLEDS(void);     // those functions defined
void onLEDS(void);			// at the bottom of the page


int main(void)
{
	
	// Enabling clock for PortA
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	// Configuring A2,A3 and A4 as output for LEDS
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	// Configuring PA7 as input for Button 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
   
	delayInit();
	
		int state;   //variable for "switch case"
		int a;			// creating variables for "for loops" inside "switch case"
		int b;
		int c;
		int d;


	while(1){
		
		switch(state){  // switch case can be used for "Finite State Machines"
			
			case 1:      //All LEDS are ON
				onLEDS();
				if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7)) {
				offLEDS(); state=2; 						}    // if button is released, it will switch to case 2
				break;				
					
			case 2:   	//LEDS blink as "RED->YELLOW->GREEN" order
				
				for (a=0;a<=500;a+=10) {     					// Red LED becomes ON
				GPIO_SetBits(GPIOA,GPIO_Pin_2);
				if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7)) {
				state=3; 	break;}    									// if button is pressed, it will switch to case 3
				delayMs(10);
			}
			
			for (b=0;b<=500;b+=10) {
				GPIO_ResetBits(GPIOA,GPIO_Pin_2);		 	// Red LED becomes OFF
				GPIO_SetBits(GPIOA,GPIO_Pin_3);				// Yellow LED becomes ON
				if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7)) {
				state=3; 	break;	}    									// if button is pressed, it will switch to case 3
				delayMs(10);
			}
			
			for (c=0;c<=500;c+=10) {
				GPIO_ResetBits(GPIOA,GPIO_Pin_3);			// Yellow LED becomes OFF
				GPIO_SetBits(GPIOA,GPIO_Pin_4);				// Green LED becomes ON
				if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7)) {
				state=3; 	break;	}    									// if button is pressed, it will switch to case 3
				delayMs(10);
			}	
			
			for(d=0;d<=100;d+=10){
				GPIO_ResetBits(GPIOA,GPIO_Pin_4);			// Green LED becomes OFF
				if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7)) {
				state=3; 	break;}    									// if button is pressed, it will switch to case 3
				delayMs(10);
			}
				break;				
				
			case 3:			// All LEDS are ON
				onLEDS();
				delayMs(10);
			if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7)) {
				state=0; 	break;	}   					//if button is released, it will switch to default case  
				break;	
				
				
			default:
				offLEDS();    // As default, all LEDS are OFF
				delayMs(10);
			if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7)) {
				state=1; 						}     //if button 1 pressed, it will switch to case 1
				delayMs(10);
			break;
				}
		}
}

void onLEDS(void)    //function for making all LEDS ON
{ GPIO_SetBits(GPIOA,GPIO_Pin_2);
	GPIO_SetBits(GPIOA,GPIO_Pin_3);
	GPIO_SetBits(GPIOA,GPIO_Pin_4);
}
	
void offLEDS(void)	//function for making all LEDS OFF
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);
	GPIO_ResetBits(GPIOA,GPIO_Pin_3);
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);
	
	
}
