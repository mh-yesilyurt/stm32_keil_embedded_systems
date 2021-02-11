#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "delay.h"
#include "math.h"



GPIO_InitTypeDef GPIO_InitStructure;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
ADC_InitTypeDef ADC_InitStructure;

void GPIO_Config(void);  		// those functions defined
void ADC_Config(void);
void TIM2_Config(void);
															// at the bottom part

int digi;
int time=0; 					
float PI=3.14159265358979323846264338328;
int b1;
int b2;
int b3;
int b4;


void TIM2_IRQHandler(void)	//timer interrupt handler (empty)
{
		if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) // Periodic IRQ
	{	
			time++;
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
			
		digi = (int)(ADC_GetConversionValue(ADC1))/720;
		b1=3600*((sin(2*PI*digi*(time)/360-0.75))/2);
		b2=3600*((sin(2*PI*digi*(time)/360-0.25))/2);
		b3=3600*((sin(2*PI*digi*(time)/360+0.25))/2);
		b4=3600*((sin(2*PI*digi*(time)/360+0.75))/2);
		
		TIM_OCInitStructure.TIM_Pulse = b1;
		TIM_OC1Init(TIM2, &TIM_OCInitStructure);
		TIM_OCInitStructure.TIM_Pulse = b2;
		TIM_OC2Init(TIM2, &TIM_OCInitStructure);
		TIM_OCInitStructure.TIM_Pulse = b3;
		TIM_OC3Init(TIM2, &TIM_OCInitStructure);	
		TIM_OCInitStructure.TIM_Pulse = b4;
		TIM_OC4Init(TIM2, &TIM_OCInitStructure);	
				
		}
	}

				
				



void GPIO_Config(void)
	
{	
	
	// Set ADC clock (Maximum 14 MHz | 72/6 = 12 MHz)
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	// Enabling clock for PortA, ADC1 and Timer Interrupt
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1 | RCC_APB2Periph_AFIO, ENABLE);
	// Configuring A2,A3 and A4 as output for LEDS; // I used Pin 0 as Red, Pin 1 as Yellow, Pin 2 as Green
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	// Configuring PA4 as input for Potentiometer
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	 }

void ADC_Config(void)
{
	
	// Configuring nested vector interrupt controller for external interrupt 4
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
TIM_TimeBaseStructure.TIM_Period = 3599;		
TIM_TimeBaseStructure.TIM_Prescaler = 199;
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
TIM_TimeBaseStructure.TIM_Period = 3599;		
TIM_TimeBaseStructure.TIM_Prescaler = 199;
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
TIM_TimeBaseStructure.TIM_Period = 3599;		
TIM_TimeBaseStructure.TIM_Prescaler = 199;
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

	// Configuring timer interrupt for Pin3
TIM_TimeBaseStructure.TIM_Period = 3599;		
TIM_TimeBaseStructure.TIM_Prescaler = 199;
TIM_TimeBaseStructure.TIM_ClockDivision = 0;
TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);	
TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
TIM_OCInitStructure.TIM_Pulse = 0;
TIM_OC4Init(TIM2, &TIM_OCInitStructure);
TIM_Cmd(TIM2, ENABLE);

// Configuring nested vector interrupt controller for timer interrupt
NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
NVIC_Init(&NVIC_InitStructure);
	
}
