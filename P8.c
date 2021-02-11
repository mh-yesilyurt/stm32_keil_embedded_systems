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
EXTI_InitTypeDef EXTI_InitStructure;
USART_InitTypeDef USART_InitStructure;

void GPIO_Config(void);  		// those functions defined
void ADC_Config(void);
void TIM2_Config(void);			
void UART_Transmit(char *string); // at the bottom part
void EXTI_Config(void);														

double digi;
int time=0;
double a=0;
int state;
int time3=0;
double c=0;
double y=0;
double z=0;
double b3=0;


double data=0;
char adc[20];
int Sent_data=0;



// PI Controller
double PI(double r, double y)
{
 static double uk_1=0, ek_1=0;
 double ek = r - y;
 double uk = uk_1 + 2.8*ek - 2.72* ek_1 ;
 uk_1 = uk;
 ek_1 = ek;
 return uk;
}

// P Controller
double P(double r, double y)
{
 static double uk_1=0, ek_1=0;
 double ek = r - y;
 double uk = uk_1 + (2.8) * ek - (2.8) * ek_1;
 uk_1 = uk;
 ek_1 = ek;
 return uk;
}

double Gz(double uk)
{
static double yk_1=0, yk_2=0, uk_1=0, uk_2=0;
double yk = 1.822*yk_1-0.8262*yk_2+0.002134*uk_1+0.002002*uk_2;
uk_2 = uk_1;
uk_1 = uk;
yk_2 = yk_1;
yk_1 = yk;
return yk;
}


void EXTI3_IRQHandler(void)    //external interrupt handler function for button1(pin3)
{ if(EXTI_GetITStatus(EXTI_Line3)!=RESET)
	{ 
	a=!a;
	}
	EXTI_ClearITPendingBit(EXTI_Line3);
}

void TIM2_IRQHandler(void)	//timer interrupt handler (empty)
{
		if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) // Periodic IRQ
	{	time++;
		z=PI(a,y);  		//controller part  it can be P(a,y) for P type controller.
		TIM_OCInitStructure.TIM_Pulse = (z/3.3)*35999;    //PWM Signal
		TIM_OC3Init(TIM2, &TIM_OCInitStructure);
		digi = (double)(ADC_GetConversionValue(ADC1));    //ADC read
		y=(digi/4095)*3.3;    //output                   //ADC as output read
	}
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}


int main(void)
{  
	
	GPIO_Config();
	ADC_Config();
	TIM2_Config();
	EXTI_Config();
	delayInit();
	
	while(1){
			

			digi = (double)(ADC_GetConversionValue(ADC1));
		
			if(time==1)
			{								
      sprintf(adc,"%0.2f\r",y);
      UART_Transmit(adc);
			time=0;}
		
		
				
		
	}

}
				



void GPIO_Config(void)
	
{	
	
	// Set ADC clock (Maximum 14 MHz | 72/6 = 12 MHz)
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	// Enabling clock for PortA, ADC1 and Timer Interrupt
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1 |  RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);
	// Configuring A2 for the Vin
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	// Configuring PA3 as input for Button 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	// Configuring PA4 as input for ADC Read
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

void EXTI_Config(void)
{
// Configuring external interrupt
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource3); 
	EXTI_DeInit();
	EXTI_InitStructure.EXTI_Line = EXTI_Line3;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	// Configuring nested vector interrupt controller for external interrupt 4
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	

	
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
	

TIM_TimeBaseStructure.TIM_Period = 35999;		
TIM_TimeBaseStructure.TIM_Prescaler = 199;
TIM_TimeBaseStructure.TIM_ClockDivision = 0;
TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);	
TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

// Configuring timer interrupt for Pin2
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
