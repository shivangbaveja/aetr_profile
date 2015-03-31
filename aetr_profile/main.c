#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "duty_define.h"

void TIM4_Configuration(void);
void PWM_Output_Configuration();
void aer_update();
void throttle_update();

uint32_t time_now=0,old_time=0,time_1min=0,throttle_phase=0,aer_phase=0;

int main(void)
{
	TIM4_Configuration();
	PWM_Output_Configuration();

	//setting PWM pulses two neutral position
	TIM_SetCompare1(TIM3,1500);
	TIM_SetCompare2(TIM3,1500);
	TIM_SetCompare3(TIM3,1000);
	TIM_SetCompare4(TIM3,1500);

	//delay for 15 seconds
	while(1)
	{
		time_now = TIM_GetCounter(TIM4);
		if(time_now < old_time)
		{
			if((time_now + 50000 - old_time) > 30000)		// 2000 counter corresponds to 1 second
			{
				old_time = time_now;
				break;
			}
		}
	}

    while(1)
    {
    	time_now = TIM_GetCounter(TIM4);
    	if(time_now < old_time)
		{
			if((time_now + 50000 - old_time) > 2000)		// 2000 counter corresponds to 1 second
			{
				old_time = time_now;
				aer_update();		//change the aileron, elevator, rudder pulse and set aer_pase to zero after the flight profile is over
				aer_phase++;
				time_1min++;
				if(time_1min>=60)
				{
					throttle_update();			// this function will update the throttle pulse value every 60 second and when the whole flight profile is over it will make the throttle_phase counter zero
					time_1min=0;
					throttle_phase++;
				}
			}
		}
    }
}
void TIM4_Configuration(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4 , ENABLE);

	TIM_TimeBaseStructure.TIM_Period=50000;
	TIM_TimeBaseStructure.TIM_Prescaler= (36000-1);
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);
	TIM_ARRPreloadConfig(TIM4, ENABLE);
	TIM_Cmd(TIM4, ENABLE);
}

void PWM_Output_Configuration()
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	TIM_TimeBaseStructure.TIM_Period = 20000;
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1;			//1us accuracy
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 1500;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
}

void throttle_update()
{
	switch(throttle_phase)
	{
	case 0:
		TIM_SetCompare3(TIM3,TAKE_OFF_POWER_UP);
		break;
	case 1:
		TIM_SetCompare3(TIM3,MAINTAIN_ALT);
		break;
	case 2:
		TIM_SetCompare3(TIM3,CLIMB);
		break;
	case 3:
		TIM_SetCompare3(TIM3,MAINTAIN_ALT);
		break;
	case 4:
		TIM_SetCompare3(TIM3,DESCENT);
		break;
	case 5:
		TIM_SetCompare3(TIM3,MAINTAIN_ALT);
		break;
	case 6:
		TIM_SetCompare3(TIM3,CLIMB);
		break;
	case 7:
		TIM_SetCompare3(TIM3,MAINTAIN_ALT);
		break;
	case 8:
		TIM_SetCompare3(TIM3,DESCENT);
		break;
	case 9:
		TIM_SetCompare3(TIM3,MAINTAIN_ALT);
		break;
	case 10:
		TIM_SetCompare3(TIM3,DESCENT);
		throttle_phase=0;
		break;
	default:
		throttle_phase=0;
		break;
	}
}

void aer_update()
{
	switch(aer_phase)
	{
	case 0:
		TIM_SetCompare1(TIM3,ZERO_SEC);
		TIM_SetCompare2(TIM3,ZERO_SEC);
		TIM_SetCompare4(TIM3,ZERO_SEC);
		break;
	case 1:
		TIM_SetCompare1(TIM3,ONE_SEC);
		TIM_SetCompare2(TIM3,ONE_SEC);
		TIM_SetCompare4(TIM3,ONE_SEC);
		break;
	case 2:
		TIM_SetCompare1(TIM3,TWO_SEC);
		TIM_SetCompare2(TIM3,TWO_SEC);
		TIM_SetCompare4(TIM3,TWO_SEC);
		break;
	case 3:
		TIM_SetCompare1(TIM3,THREE_SEC);
		TIM_SetCompare2(TIM3,THREE_SEC);
		TIM_SetCompare4(TIM3,THREE_SEC);
		break;
	case 4:
		TIM_SetCompare1(TIM3,FOUR_SEC);
		TIM_SetCompare2(TIM3,FOUR_SEC);
		TIM_SetCompare4(TIM3,FOUR_SEC);
		break;
	case 5:
		TIM_SetCompare1(TIM3,FIVE_SEC);
		TIM_SetCompare2(TIM3,FIVE_SEC);
		TIM_SetCompare4(TIM3,FIVE_SEC);
		break;
	case 6:
		TIM_SetCompare1(TIM3,SIX_SEC);
		TIM_SetCompare2(TIM3,SIX_SEC);
		TIM_SetCompare4(TIM3,SIX_SEC);
		break;
	case 7:
		TIM_SetCompare1(TIM3,SEVEN_SEC);
		TIM_SetCompare2(TIM3,SEVEN_SEC);
		TIM_SetCompare4(TIM3,SEVEN_SEC);
		break;
	case 8:
		TIM_SetCompare1(TIM3,EIGHT_SEC);
		TIM_SetCompare2(TIM3,EIGHT_SEC);
		TIM_SetCompare4(TIM3,EIGHT_SEC);
		break;
	case 9:
		TIM_SetCompare1(TIM3,NINE_SEC);
		TIM_SetCompare2(TIM3,NINE_SEC);
		TIM_SetCompare4(TIM3,NINE_SEC);
		aer_phase=0;
		break;
	default:
		aer_phase=0;
		break;
	}
}
