#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "delay.h"



GPIO_InitTypeDef GPIO_InitStructure;
EXTI_InitTypeDef EXTI_InitStructure;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;
NVIC_InitTypeDef NVIC_InitStructure;

void GPIO_Config(void);  		// those functions defined
void EXTI_Config(void);	
void TIM2_Config(void);   	// at the bottom part

int state=0;  		// variables for "switch case"                         
int count=0;			
int count2=0;			// counting,debounce
int a=0; 		
int b=0; 					//and "brightness" purposes 	


void EXTI4_IRQHandler(void)    //external interrupt handler function for button1(pin4)
{ if(EXTI_GetITStatus(EXTI_Line4)!=RESET)
	{
	a++;
	
	}
	EXTI_ClearITPendingBit(EXTI_Line4);
}

void EXTI9_5_IRQHandler(void)    //external interrupt handler function for button2(pin5)
{
	if(EXTI_GetITStatus(EXTI_Line5)!=RESET)
	{
		
		b++;

		
 }
	EXTI_ClearITPendingBit(EXTI_Line5);
}

void TIM2_IRQHandler(void)	//timer interrupt handler (empty)
{
		if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) // Periodic IRQ
	{
			TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}


void LEDSwitch(void)  //LED Switch Cases Control
{
	
           //looks a little bit complex, because of bounce issue.
					//i debounced it and handled that issue. Short explanation: 
					//i used a counter and if the state is stable more than 100 times, 
					//it is going inside this function. After that, we are checking if there is no more signal coming from button.
				if(a>=1)
				{count++;
				if(count>=100)
				{if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4))
				{
				count=0;a=0;
				if((state==3)||(state==6)||(state==9))  //if red led is on, when the button pressed;
																								//all leds will be off(it switches to default state)
				{state=0;}
				else
				{state=(state%3)+1;}									//else, it will switch to next LED.
				}
				else
				{count=0;a=0;}
				}
							
				}

}

void Brightness(void)				//Brightness Switch Cases Control
	{				
				 //looks a little bit complex, because of bounce issue.
				//i debounced it and handled that issue. Short explanation: 
				//i used a counter and if the state is stable more than 100 times, 
			 //it is going inside this function. After that, we are checking if there is no more signal coming from button.
				if(b>=1)								
				{count2++;
				if(count2>=100)
				{if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5))
				{count2=0;b=0;
				if(state==0)				//if it is default state, "brightness" control is not used, because all LEDS are off.
				{count2=0;b=0;state=0;}
				else if(state>=7)		//if it is already in the brightest state, it will switch to the least bright state.
				{state=(state%3);
				if(state==0)       // Since state=9 and 9%3=0, it was not working properly. 
				{state=3;}				//So i solved this issue by using this line of code.
				}
				else
				{state=state+3;}		//Else, it will switch to state+3, state. which is 1 more brighter state.
				}
				else
				{count2=0;b=0;}
				}
							
				}

}


int main(void)
{
	
	GPIO_Config();
	EXTI_Config();
	TIM2_Config();
	delayInit();
	LEDSwitch();
	Brightness();
	
	
	while(1){

			switch(state){  
			
		case 1: //RED LED is ON, Lowest Brightness
			
					TIM_OCInitStructure.TIM_Pulse = 0;
					TIM_OC3Init(TIM2, &TIM_OCInitStructure);
					TIM_OCInitStructure.TIM_Pulse = 500;
					TIM_OC1Init(TIM2, &TIM_OCInitStructure);
					LEDSwitch();
					Brightness();
					break;
		
				case 2: //YELLOW LED is ON, Lowest Brightness
					TIM_OCInitStructure.TIM_Pulse = 0;
					TIM_OC1Init(TIM2, &TIM_OCInitStructure);
					TIM_OCInitStructure.TIM_Pulse = 500;
					TIM_OC2Init(TIM2, &TIM_OCInitStructure);
					LEDSwitch();
					Brightness();
					break;
				
				case 3: //GREEN LED is ON, Lowest Brightness
					TIM_OCInitStructure.TIM_Pulse = 0;
					TIM_OC2Init(TIM2, &TIM_OCInitStructure);
					TIM_OCInitStructure.TIM_Pulse = 500;
					TIM_OC3Init(TIM2, &TIM_OCInitStructure);
					LEDSwitch();
					Brightness();
					break;
				
				case 4: //RED LED is ON, Medium Brightness
					TIM_OCInitStructure.TIM_Pulse = 1500;
					TIM_OC1Init(TIM2, &TIM_OCInitStructure);
					LEDSwitch();
					Brightness();
					break;
				case 5: //YELLOW LED is ON, Medium Brightness
					TIM_OCInitStructure.TIM_Pulse = 1500;
					TIM_OC2Init(TIM2, &TIM_OCInitStructure);
					LEDSwitch();
					Brightness();
					break;
				
				case 6: //GREEN LED is ON, Medium Brightness
					TIM_OCInitStructure.TIM_Pulse = 1500;
					TIM_OC3Init(TIM2, &TIM_OCInitStructure);
					LEDSwitch();
					Brightness();
					break;
				
				case 7: //RED LED is ON, Brightest
					TIM_OCInitStructure.TIM_Pulse = 3000;
					TIM_OC1Init(TIM2, &TIM_OCInitStructure);
					LEDSwitch();
					Brightness();
					break;
				
				case 8: //YELLOW LED is ON, Brightest
					TIM_OCInitStructure.TIM_Pulse = 3000;
					TIM_OC2Init(TIM2, &TIM_OCInitStructure);
					LEDSwitch();
					Brightness();
					break;
				
				case 9: //GREEN LED is ON, Brightest
					TIM_OCInitStructure.TIM_Pulse = 3000;
					TIM_OC3Init(TIM2, &TIM_OCInitStructure);
					LEDSwitch();
					Brightness();
					break;

				default: //DEFAULT STATE, ALL LEDS are OFF.
				TIM_OCInitStructure.TIM_Pulse = 0;
				TIM_OC1Init(TIM2, &TIM_OCInitStructure);
				TIM_OCInitStructure.TIM_Pulse = 0;
				TIM_OC2Init(TIM2, &TIM_OCInitStructure);
				TIM_OCInitStructure.TIM_Pulse = 0;
				TIM_OC3Init(TIM2, &TIM_OCInitStructure);
				
				LEDSwitch();
				Brightness();
				break;
		
			
				}
				
		}
	}

				
				



void GPIO_Config(void)
	
{	// Enabling clock for PortA, External Interrupt and Timer Interrupt
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	// Configuring A2,A3 and A4 as output for LEDS; // I used Pin 0 as Red, Pin 1 as Yellow, Pin 2 as Green
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	// Configuring PA1 as input for Button 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	 }

void EXTI_Config(void)
{
// Configuring external interrupt
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource4 | GPIO_PinSource5); 
	EXTI_DeInit();
	EXTI_InitStructure.EXTI_Line = EXTI_Line4 | EXTI_Line5;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	// Configuring nested vector interrupt controller for external interrupt 4
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
		// Configuring nested vector interrupt controller for external interrupt 5
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	}

void TIM2_Config(void)
{
	// Configuring timer interrupt for Pin1
TIM_TimeBaseStructure.TIM_Period = 3499;		
TIM_TimeBaseStructure.TIM_Prescaler = 200;
TIM_TimeBaseStructure.TIM_ClockDivision = 0;
TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);	
TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
TIM_OCInitStructure.TIM_Pulse = 0;
TIM_OC1Init(TIM2, &TIM_OCInitStructure);	
	
	// Configuring timer interrupt for Pin2
TIM_TimeBaseStructure.TIM_Period = 3499;		
TIM_TimeBaseStructure.TIM_Prescaler = 200;
TIM_TimeBaseStructure.TIM_ClockDivision = 0;
TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);	
TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
TIM_OCInitStructure.TIM_Pulse = 0;
TIM_OC2Init(TIM2, &TIM_OCInitStructure);

	// Configuring timer interrupt for Pin3
TIM_TimeBaseStructure.TIM_Period = 3499;		
TIM_TimeBaseStructure.TIM_Prescaler = 200;
TIM_TimeBaseStructure.TIM_ClockDivision = 0;
TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);	
TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
TIM_OCInitStructure.TIM_Pulse = 0;
TIM_OC3Init(TIM2, &TIM_OCInitStructure);
TIM_Cmd(TIM2, ENABLE);
	
}
