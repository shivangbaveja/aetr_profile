#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "duty_define.h"

void TIM3_Configuration(void);
void PWM_Output_Configuration();
void aer_update();
void throttle_update();

uint32_t time_now=0,old_time=0,time_1min=0,throttle_phase=0,aer_phase=0,delay_counter=0,finish=0;

int main(void)
{
	TIM3_Configuration();
	PWM_Output_Configuration();

	//setting PWM pulses two neutral position
	TIM_SetCompare1(TIM4,1500);
	TIM_SetCompare2(TIM4,1500);
	TIM_SetCompare3(TIM4,1000);
	TIM_SetCompare4(TIM4,1500);

	finish=0;

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
		if(delay_counter>=10)
		break;
	}

	throttle_update();

    while(1)
    {
    	time_now = TIM_GetCounter(TIM3);
    	if(time_now < old_time)
		{
			if((time_now + 50000 - old_time) > 2000)
			{
				old_time = time_now;
				aer_update();		//change the aileron, elevator, rudder pulse and set aer_pase to zero after the flight profile is over
				aer_phase++;
				time_1min++;
				if(time_1min>=60)
				{
					throttle_phase++;
					throttle_update();			// this function will update the throttle pulse value every 60 second and when the whole flight profile is over it will make the throttle_phase counter zero
					time_1min=0;
				}
			}
		}
		else
		{
			if((time_now - old_time) > 2000)
			{
				old_time = time_now;
				aer_update();		//change the aileron, elevator, rudder pulse and set aer_pase to zero after the flight profile is over
				aer_phase++;
				time_1min++;
				if(time_1min>=60)
				{
					throttle_phase++;
					throttle_update();			// this function will update the throttle pulse value every 60 second and when the whole flight profile is over it will make the throttle_phase counter zero
					time_1min=0;
				}
			}
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

void throttle_update()
{
	if(finish!=1)
	{
		switch(throttle_phase)
		{
		case 0:
			TIM_SetCompare3(TIM4,TAKE_OFF_POWER_UP);
			break;
		case 1:
			TIM_SetCompare3(TIM4,MAINTAIN_ALT);
			break;
		case 2:
			TIM_SetCompare3(TIM4,CLIMB);
			break;
		case 3:
			TIM_SetCompare3(TIM4,MAINTAIN_ALT);
			break;
		case 4:
			TIM_SetCompare3(TIM4,DESCENT);
			break;
		case 5:
			TIM_SetCompare3(TIM4,MAINTAIN_ALT);
			break;
		case 6:
			TIM_SetCompare3(TIM4,CLIMB);
			break;
		case 7:
			TIM_SetCompare3(TIM4,MAINTAIN_ALT);
			break;
		case 8:
			TIM_SetCompare3(TIM4,DESCENT);
			break;
		case 9:
			TIM_SetCompare3(TIM4,MAINTAIN_ALT);
			break;
		case 10:
			finish=1;
			TIM_SetCompare1(TIM4,1500);
			TIM_SetCompare2(TIM4,1500);
			TIM_SetCompare3(TIM4,1000);
			TIM_SetCompare4(TIM4,1500);
			break;
		default:
			break;
		}
	}
}

void aer_update()
{
	if(finish!=1)
	{
		switch(aer_phase)
		{
		case 0:
			TIM_SetCompare1(TIM4,ZERO_SEC);
			TIM_SetCompare2(TIM4,ZERO_SEC);
			TIM_SetCompare4(TIM4,ZERO_SEC);
			break;
		case 1:
			TIM_SetCompare1(TIM4,ONE_SEC);
			TIM_SetCompare2(TIM4,ONE_SEC);
			TIM_SetCompare4(TIM4,ONE_SEC);
			break;
		case 2:
			TIM_SetCompare1(TIM4,TWO_SEC);
			TIM_SetCompare2(TIM4,TWO_SEC);
			TIM_SetCompare4(TIM4,TWO_SEC);
			break;
		case 3:
			TIM_SetCompare1(TIM4,THREE_SEC);
			TIM_SetCompare2(TIM4,THREE_SEC);
			TIM_SetCompare4(TIM4,THREE_SEC);
			break;
		case 4:
			TIM_SetCompare1(TIM4,FOUR_SEC);
			TIM_SetCompare2(TIM4,FOUR_SEC);
			TIM_SetCompare4(TIM4,FOUR_SEC);
			break;
		case 5:
			TIM_SetCompare1(TIM4,FIVE_SEC);
			TIM_SetCompare2(TIM4,FIVE_SEC);
			TIM_SetCompare4(TIM4,FIVE_SEC);
			break;
		case 6:
			TIM_SetCompare1(TIM4,SIX_SEC);
			TIM_SetCompare2(TIM4,SIX_SEC);
			TIM_SetCompare4(TIM4,SIX_SEC);
			break;
		case 7:
			TIM_SetCompare1(TIM4,SEVEN_SEC);
			TIM_SetCompare2(TIM4,SEVEN_SEC);
			TIM_SetCompare4(TIM4,SEVEN_SEC);
			break;
		case 8:
			TIM_SetCompare1(TIM4,EIGHT_SEC);
			TIM_SetCompare2(TIM4,EIGHT_SEC);
			TIM_SetCompare4(TIM4,EIGHT_SEC);
			break;
		case 9:
			TIM_SetCompare1(TIM4,NINE_SEC);
			TIM_SetCompare2(TIM4,NINE_SEC);
			TIM_SetCompare4(TIM4,NINE_SEC);
			aer_phase=0;
			break;
		default:
			aer_phase=0;
			break;
		}
	}
}
