#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "delay.h"



GPIO_InitTypeDef GPIO_InitStructure;

int main(void)
{
	// Enabling clock for PortC
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	// Configuring PC13 as desired LED
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	// Enabling clock for PortA
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	// i used PA3 and PA4 for this experiment
	
	// Configuring PA3 as input -- it will be Button 1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	
	// Configuring PA4 as input -- it will be Button 2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  
   
	delayInit();
	
		int state;   //variable for "switch case"
	
		int a;  // creating variables for "for loops"
		int b;
		int c;
		int d;

	while(1){
		
		switch(state){
			
			case 1:  //"we pressed button 1- condition is written inside default case- 
				       //pressing button 1 and blinking for 1000ms period
				
							// i created "for loops" for delays to precisely understand if button 2 pressed or button 1 pressed again.
				
				for(a=0;a<=1000;a+=100){
				GPIO_ResetBits(GPIOC, GPIO_Pin_13);
				delayMs(100);
				if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)){
					state=2;  }       			//if button 2 pressed it will switch to case 2
			
			else if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)){
				state=3;				
						}    									//if button 1 pressed again , it will switch to case 3
				}
				for(b=0;b<=1000;b+=100){
				GPIO_SetBits(GPIOC, GPIO_Pin_13);
				delayMs(100); 
					
				if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)){
					state=2;  }    					//if button 2 pressed it will switch to case 2
			
			else if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)){
				state=3;				
						}   									//if button 1 pressed again , it will switch to case 3
				}		
						
										
				break;
					
			case 2:   // Pressing button 2 and blinking for 500ms period
				
				for(c=0;c<=500;c+=50){
				GPIO_ResetBits(GPIOC, GPIO_Pin_13);
				delayMs(50);
					
					if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)){
				state=3;	} 							//if button 1 pressed , it will switch to case 3
				
				else if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)){
					state=1;} 						//if button 2 pressed again , it will switch to case 1
				 }
				for(d=0;d<=500;d+=50){
				GPIO_SetBits(GPIOC, GPIO_Pin_13);
				delayMs(50);
					
					if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)){
				state=3;	}  						//if button 1 pressed , it will switch to case 3
					else if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)){
						state=1; 						//if button 2 pressed again , it will switch to case 1
					}
				}
				break;
				
			case 3:   // Pressing button 1 after button 2 or 1 pressed, Led will stop blinking
				GPIO_SetBits(GPIOC, GPIO_Pin_13);
				if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)){
				state=0;	}     				//if button 1 pressed again , it will switch to default case
				break;
				
			default:
				GPIO_SetBits(GPIOC, GPIO_Pin_13);
				delayMs(50);
			if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)) {
				state=1; 						}     //if button 1 pressed , it will switch to case 1
			break;
				}
		}
}