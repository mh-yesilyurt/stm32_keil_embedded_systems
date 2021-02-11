#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "delay.h"


int main(void)
{
	RCC->APB2ENR |= (0x1 << 4); // Port C clock enable
	RCC->APB2ENR |= (0x1 << 2); // Port A clock enable

	GPIOC->CRH &= ~(0xF << 20); // Pin C13 clear
	GPIOC->CRH |= (0x2 << 20);  // Pin C13 Output config. (Push pull output 2MHz)

	
	// i used Port A3 and A4 for this experiment
	GPIOA->CRL &= ~(0xF << 16); // Port A3 clear
	GPIOA->CRL &= ~(0xF << 12); // Port A4 clear 
	GPIOA->CRL |= (0x8 << 16);  // Pin A3 input config 
	GPIOA->CRL |= (0x8 << 12); // Pin A4  input config     
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
				GPIOC->BRR |= (1 << 13);
				delayMs(100);
				if(GPIOA->IDR &= (1<<4)){
					state=2;  }       			//if button 2 pressed it will switch to case 2
			
			else if(GPIOA->IDR &= (1<<3)){
				state=3;				
						}    									//if button 1 pressed again , it will switch to case 3
				}
				for(b=0;b<=1000;b+=100){
				GPIOC->BSRR |= (1<<13);
				delayMs(100); 
				if(GPIOA->IDR &= (1<<4)){
					state=2;  }    					//if button 2 pressed it will switch to case 2
			
			else if(GPIOA->IDR &= (1<<3)){
				state=3;				
						}   									//if button 1 pressed again , it will switch to case 3
				}		
																
				break;
					
			case 2:   // Pressing button 2 and blinking for 500ms period
				for(c=0;c<=500;c+=50){
				GPIOC->BRR |= (1 << 13);
				delayMs(50);
					if(GPIOA->IDR &= (1<<3)){
				state=3;	} 							//if button 1 pressed , it will switch to case 3
					else if(GPIOA->IDR &= (1<<4)){
					state=1;				}       //if button 2 pressed again , it will switch to case 1
				}
				for(d=0;d<=500;d+=50){
				GPIOC->BSRR |= (1 << 13);
				delayMs(50);
					if(GPIOA->IDR &= (1<<3)){
				state=3;	}  						//if button 1 pressed , it will switch to case 3
					else if(GPIOA->IDR &= (1<<4)){
				state=1;}								//if button 2 pressed again , it will switch to case 1
				}
				break;
				
			case 3:   // Pressing button 1 after button 2 or 1 pressed, Led will stop blinking
				GPIOC->BSRR |= (1<<13);
				if(GPIOA->IDR &= (1<<3)){
				state=0;	}     				//if button 1 pressed again , it will switch to default case
				break;
				
			default:
				GPIOC->BSRR |= (1<<13);
				delayMs(50);
			if(GPIOA->IDR &= (1<<3)) {
				state=1; 						}     //if button 1 pressed , it will switch to case 1
			break;
				}
		}
}