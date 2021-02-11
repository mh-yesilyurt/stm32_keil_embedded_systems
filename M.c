#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include <stdio.h>
#include "delay.h"


GPIO_InitTypeDef GPIO_InitStructure;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
ADC_InitTypeDef ADC_InitStructure;
USART_InitTypeDef USART_InitStructure;

void GPIO_Config(void);  		// those functions defined
void ADC_Config(void);
void TIM2_Config(void);			
void UART_Transmit(char *string); // at the bottom part
															

int digi;
float time=0;
float time2=0;
float PI=3.14159265358979323846264338328;    //pi value defined
int state;
int b1=0;
float b2=0;
int b3=0;

static int data=0;
char adc[20];
int Sent_data=0;



void TIM2_IRQHandler(void)	//timer interrupt handler (empty)
{
		if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) // Periodic IRQ
	{	
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6))
		{time++;}
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7))
		{time2++;}
				  //time value increases for every timer interrupt
	}
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}



int main(void)
{
	
	GPIO_Config();
	ADC_Config();
	TIM2_Config();
	delayInit();
	
	while(1){
		
			digi = (int)(ADC_GetConversionValue(ADC1))*5.29; 
			b3=digi+300;
			b2=b1/216;
			
			data=b3;
      sprintf(adc,"%d",data);
      UART_Transmit(adc);	
			delayMs(10);
		
		switch(state){
			case 1:
				if(time==25)
				{if(b3>b1+720)
				{b1=b1+720;}
				time=0;
				}
				
				if(b3<b1)
				{b1=b3;}
				
				if(time2==25)
				{b1=b1-720;
				time2=0;
				}
				
				if(b1<720)
				{state=0;
				TIM_OCInitStructure.TIM_Pulse = 0;
				TIM_OC1Init(TIM2, &TIM_OCInitStructure);
				break;}
				
				
				if(b1>=7200)
				{state=2;}
				

				
				TIM_OCInitStructure.TIM_Pulse = b1;
				TIM_OC1Init(TIM2, &TIM_OCInitStructure);
					
				
				break;
			
			case 2:
				
				if(time==25)  //0.25 SN
				{if(b3>b1+720)
				{b1=b1+720;}
				time=0;
				}
				if(b3<b1)
				{b1=b3;}
				
				if(time2==25)
				{b1=b1-720;
				time2=0;
				}
				
				if(b1<=7200)
				{state=1;	
				TIM_OCInitStructure.TIM_Pulse = 0;
				TIM_OC2Init(TIM2, &TIM_OCInitStructure);
				break;}
				
				if(b1>=14400)
				{state=3;}
				
				
				
				TIM_OCInitStructure.TIM_Pulse = b1-7200;
				TIM_OC2Init(TIM2, &TIM_OCInitStructure);
						
				break;
				
			case 3:
				if(time==25)  //0.25 SN
				{{if(b3>b1+720)
				{b1=b1+720;}
				}
				if(b1>=21600)
				{b1=21600;
				time=0;
				if(b1>=21600)
				{b1=21600;}}
				time=0;}
				
				if(b3<b1)
				{b1=b3;}
				
				
				if(time2==25)
				{b1=b1-720;
				time2=0;
				}
				
				if(b1<14400)
				{state=2;
				TIM_OCInitStructure.TIM_Pulse =0;
				TIM_OC3Init(TIM2, &TIM_OCInitStructure);
				break;}
				
				if(b1>=21600)
				{b1=21600;}
				
				TIM_OCInitStructure.TIM_Pulse = b1-14400;
				TIM_OC3Init(TIM2, &TIM_OCInitStructure);
				
				break;
				
			default:
				TIM_OCInitStructure.TIM_Pulse = 0;
				TIM_OC1Init(TIM2, &TIM_OCInitStructure);
				TIM_OCInitStructure.TIM_Pulse = 0;
				TIM_OC2Init(TIM2, &TIM_OCInitStructure);
				TIM_OCInitStructure.TIM_Pulse = 0;
				TIM_OC3Init(TIM2, &TIM_OCInitStructure);
				if(time==25)  //0.25 SN
				{state=1;time=0;b1=720;}
				break;

				
		}
	}

}
				



void GPIO_Config(void)
	
{	
	
	// Set ADC clock (Maximum 14 MHz | 72/6 = 12 MHz)
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	// Enabling clock for PortA, ADC1 and Timer Interrupt
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1 |  RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);
	// Configuring A0,A1 and A2 as output for LEDS; // I used Pin 0 as Green, Pin 1 as Yellow, Pin 2 as Red
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	// Configuring PA6 and PA7 as input for Buttons 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	// Configuring PA4 as input for Potentiometer
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// Configue UART RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	// Configue UART TX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	// USART settings
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART1, &USART_InitStructure);
	// Enable data receive interrupt & USART1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	 }

	 void UART_Transmit(char *string)
{
	while(*string)
	{
		while(!(USART1->SR & 0x00000040));
		USART_SendData(USART1,*string);
		*string++;
	}
}

void ADC_Config(void)
{
	
	// Configuring ADC 
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel=1;
	ADC_Init(ADC1,&ADC_InitStructure);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_4,1,ADC_SampleTime_7Cycles5);
	ADC_Cmd(ADC1,ENABLE);
	
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	}

void TIM2_Config(void)
{
	// Configuring timer interrupt for Pin0
TIM_TimeBaseStructure.TIM_Period = 35999;		
TIM_TimeBaseStructure.TIM_Prescaler = 19;
TIM_TimeBaseStructure.TIM_ClockDivision = 0;
TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);	
TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
TIM_OCInitStructure.TIM_Pulse = 0;
TIM_OC1Init(TIM2, &TIM_OCInitStructure);	
	
	// Configuring timer interrupt for Pin1
TIM_TimeBaseStructure.TIM_Period = 35999;		
TIM_TimeBaseStructure.TIM_Prescaler = 19;
TIM_TimeBaseStructure.TIM_ClockDivision = 0;
TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);	
TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
TIM_OCInitStructure.TIM_Pulse = 0;
TIM_OC2Init(TIM2, &TIM_OCInitStructure);

	// Configuring timer interrupt for Pin2
TIM_TimeBaseStructure.TIM_Period = 35999;		
TIM_TimeBaseStructure.TIM_Prescaler = 19;
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


// Configuring nested vector interrupt controller for timer interrupt
NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
NVIC_Init(&NVIC_InitStructure);
	
}
