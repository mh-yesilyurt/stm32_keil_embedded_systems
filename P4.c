#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "delay.h"



GPIO_InitTypeDef GPIO_InitStructure;
EXTI_InitTypeDef EXTI_InitStructure;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
NVIC_InitTypeDef NVIC_InitStructure;

void GPIO_Config(void);  		// those functions defined
void EXTI_Config(void);			
void TIM2_Config(void);			// at the bottom part

static uint8_t state=0;  // variable for "switch case"
int time=0;							// variable for delay and timing purposes


void EXTI1_IRQHandler(void)    //external interrupt handler function
{
	if(state==2)  // checking if interrupt occured while we are in case 2
	 {if((100-time)>20)  	 //checking if the time is bigger than 20
		{							 		//because we don't want the pedestrians to wait more.
			time=80;				// setting the time as "80" for waiting 2seconds (100-80=20=2seconds)
		         
	  }
	}
	 EXTI_ClearITPendingBit(EXTI_Line1);		// clearing the interrupt flag
			
 }

 void TIM2_IRQHandler(void)
 {if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)

	{ time++;    //increasing the time one more for every 0.1 seconds
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  // clearing the interrupt flag
	}
}
int main(void)
{
	
	GPIO_Config();
	EXTI_Config();
	TIM2_Config();
	
	while(1){

			switch(state){  // switch case can be used for "Finite State Machines"
			
				case 0:      //For Cars: Red Led ON, Yellow and Green Leds OFF ; For Pedestrians:Green Led ON, Red Led OFF
				GPIO_ResetBits(GPIOA,GPIO_Pin_3);
				GPIO_ResetBits(GPIOA,GPIO_Pin_4);
				GPIO_ResetBits(GPIOA,GPIO_Pin_6);
				GPIO_SetBits(GPIOA,GPIO_Pin_2);
				GPIO_SetBits(GPIOA,GPIO_Pin_7);
				if(time==100) // after 10 seconds
				{ time=0;
					state=1;	//Going to state 1
				  }
				break;			
				case 1:			//For Cars: Yellow Led ON, Red Led OFF ; For Pedestrians: Red Led ON, Green Led OFF
				GPIO_ResetBits(GPIOA,GPIO_Pin_2);
				GPIO_ResetBits(GPIOA,GPIO_Pin_7);
				GPIO_SetBits(GPIOA,GPIO_Pin_3);
				GPIO_SetBits(GPIOA,GPIO_Pin_6);
				if(time==20) // after 2 seconds
				{ 
					time=0;
					state=2;	//Going to state 2
				  }
				break;
			
				case 2: //For Cars: Green Led ON, Yellow Led OFF ; For Pedestrians: Red Led ON
				GPIO_ResetBits(GPIOA,GPIO_Pin_3);
				GPIO_SetBits(GPIOA,GPIO_Pin_4);
				GPIO_SetBits(GPIOA,GPIO_Pin_6);
				if(time==100) // after 10 seconds
				{ time=0;
					state=3;	//Going to state 3
				  }
				break;
			
				case 3: //For Cars: Yellow Led ON, Green Led OFF ; For Pedestrians: Red Led ON
				GPIO_ResetBits(GPIOA,GPIO_Pin_4);
				GPIO_SetBits(GPIOA,GPIO_Pin_3);
				GPIO_SetBits(GPIOA,GPIO_Pin_6);
				if(time==20) // after 2 seconds
				{ time=0;
					state=0; //Going to state 0
				  }
				break;
					
				default:
					state=0; // Going to state 0 as default
				break;
			
			
				}
				
		}
	}



void GPIO_Config(void)
	
{	// Enabling clock for PortA, External Interrupt and Timer Interrupt
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	// Configuring A2,A3 and A4 as output for LEDS; // I used Pin 2 as Red, Pin 3 as Yellow, Pin 4 as Green
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	// Configuring PA1 as input for Button 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	 }

void EXTI_Config(void)
{
// Configuring external interrupt
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1); 
	EXTI_DeInit();
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	// Configuring nested vector interrupt controller for external interrupt
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	}

void TIM2_Config(void)
{
	// Configuring timer interrupt
TIM_TimeBaseStructure.TIM_Period = 49999;		
TIM_TimeBaseStructure.TIM_Prescaler = 143;
TIM_TimeBaseStructure.TIM_ClockDivision = 0;
TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);	
TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
TIM_Cmd(TIM2, ENABLE);
	
// Configuring nested vector interrupt controller for timer interrupt
NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
NVIC_Init(&NVIC_InitStructure);
	}
