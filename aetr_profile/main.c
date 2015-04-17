#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "duty_define.h"

void TIM3_Configuration(void);
void PWM_Output_Configuration();
void pulse_val_change();
void set_pulse();

uint32_t time_now=0,old_time=0;
uint16_t pulse_val=0;
uint8_t time_10sec=0, delay_counter=0;
int start=0;

int main(void)
{
	TIM3_Configuration();
	PWM_Output_Configuration();

	//setting PWM pulses two neutral position
	pulse_val=1000;
	TIM_SetCompare1(TIM4,pulse_val);
	TIM_SetCompare2(TIM4,pulse_val);
	TIM_SetCompare3(TIM4,pulse_val);
	TIM_SetCompare4(TIM4,pulse_val);

	//delay for 15 seconds
	while(1)
	{
		time_now = TIM_GetCounter(TIM3);
		if(time_now < old_time)
		{
			if((time_now + 50000 - old_time) > 2000)
			{
				delay_counter++;
				old_time = time_now;
			}
		}
		else
		{
			if((time_now - old_time) > 2000)
			{
				delay_counter++;
				old_time = time_now;
			}
		}
		if(delay_counter>=5)
		{
			delay_counter=0;
			if(start>1)
			{
				pulse_val_change();
			}
			else if(start==0)
				start++;
		}
	}
}
void TIM3_Configuration(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);

	TIM_TimeBaseStructure.TIM_Period=50000;
	TIM_TimeBaseStructure.TIM_Prescaler= (36000-1);
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
}

void PWM_Output_Configuration()
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO , ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_PinRemapConfig(GPIO_Remap_TIM4, ENABLE);

	TIM_TimeBaseStructure.TIM_Period = 20000;
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);

	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);

	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);

	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM4, ENABLE);
	TIM_Cmd(TIM4, ENABLE);
}

void pulse_val_change()
{
	pulse_val+=50;
	if(pulse_val>2000)
	{
		pulse_val=1000;
		start=-1;
	}
	set_pulse();
}
void set_pulse()
{
	TIM_SetCompare1(TIM4,pulse_val);
	TIM_SetCompare2(TIM4,pulse_val);
	TIM_SetCompare3(TIM4,pulse_val);
	TIM_SetCompare4(TIM4,pulse_val);
}
