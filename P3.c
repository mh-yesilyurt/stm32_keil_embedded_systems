#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "delay.h"



GPIO_InitTypeDef GPIO_InitStructure;
EXTI_InitTypeDef EXTI_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;

void GPIO_Config(void);  		// those functions defined

int state;


int main(void)
{
	
	GPIO_Config();
	delayInit();
	
	while(1){
		
		switch(state){  // switch case can be used for "Finite State Machines"
			
			case 0:      //All LEDS are ON
				GPIO_ResetBits(GPIOA,GPIO_Pin_3);
				GPIO_ResetBits(GPIOA,GPIO_Pin_4);
				GPIO_SetBits(GPIOA,GPIO_Pin_2);
				delayMs(10000);
				state=1;
				break;
			
			case 1:
				GPIO_ResetBits(GPIOA,GPIO_Pin_2);
				GPIO_SetBits(GPIOA,GPIO_Pin_3);
				delayMs(2000);
				state=2;
				break;
			
			case 2:
				GPIO_ResetBits(GPIOA,GPIO_Pin_3);
				GPIO_SetBits(GPIOA,GPIO_Pin_4);
				delayMs(10000);
				state=3;
				break;
			
			case 3:
				GPIO_ResetBits(GPIOA,GPIO_Pin_4);
				GPIO_SetBits(GPIOA,GPIO_Pin_3);
				delayMs(2000);
				state=0;
				break;
			
			default:
				state=0;
				break;
				
							
				
			
			
				}
				
		}
}

void GPIO_Config(void)
{	// Enabling clock for PortA and External Interrupt
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	// Configuring A2,A3 and A4 as output for LEDS
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	// Configuring PA1 as input for Button 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	 
}




delay.c kısmı



#include "delay.h"
 
static volatile uint32_t ticks;
 
void delayInit() 
{ 
  SystemCoreClockUpdate(); 
	SysTick_Config(SystemCoreClock / 1000000); // Overflow every 1 us 
} 

void delayDecrement(void)
{
	if (ticks != 0x00) {	 
			ticks--;
	}
	
	 }

void delayUs(uint32_t nTime) 
{ 
 	ticks = nTime;
	if((state==2) && (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1)))  //if it’s state 2 (Green led ON state) when the button pressed
	{ ticks=0; }																									//it breaks the delay
 	while(ticks); 
}
 
void delayMs(uint32_t nTime) 
{ 
	while(nTime--){
		
 		delayUs(1000); 
}
	
}
