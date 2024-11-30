#include "com_buzzer.h"

using namespace music;

com_buzzer::com_buzzer(TIM_HandleTypeDef *htim, uint8_t channel)
	: htim(htim), channel(channel)
{
	music_note_ptr = (uint16_t *)music_note;
	music_freq_ptr = (uint16_t *)music_freq;
	Passive_or_Active = 0;
}

com_buzzer::com_buzzer(GPIO_TypeDef *GPIOx, uint16_t GPIO_PinState, uint16_t aTime)
	: GPIOx(GPIOx), GPIO_Pin(GPIO_PinState), aTime(aTime)
{
	music_note_ptr = (uint16_t *)music_note;
	music_freq_ptr = (uint16_t *)music_freq;
	Passive_or_Active = 1;
}

void com_buzzer::Delay(uint32_t ms)
{
	com_delay_ms_os(ms);
}

uint8_t com_buzzer::Set_PWM(uint16_t pwm)
{
	uint8_t ret = COM_OK;
	if(Passive_or_Active == 0)
	{
		if(PWM_Opened == false)
		{
			ret = HAL_TIM_PWM_Start(htim, channel);
			PWM_Opened = true;
		}
		__HAL_TIM_SET_AUTORELOAD(htim, (Clock / pwm - 1));
		__HAL_TIM_SET_PRESCALER(htim, 8 - 1);
		__HAL_TIM_SET_COMPARE(htim, channel, ((Clock / pwm - 1) / Sound));
	}
	else if(Passive_or_Active == 1)
	{
		HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);
	}
	return ret;
}

uint8_t com_buzzer::Open_PWM(void)
{
	uint8_t ret = COM_OK;
	if(Passive_or_Active == 0)
	{
		ret = HAL_TIM_PWM_Start(htim, channel);
	}
	else if(Passive_or_Active == 1)
	{
		HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);
	}
	return ret;

}

uint8_t com_buzzer::Close_PWM(void)
{
	uint8_t ret = COM_OK;
	if(Passive_or_Active == 0)
	{
		ret = HAL_TIM_PWM_Stop(htim, channel);
		PWM_Opened = false;
	}
	else if(Passive_or_Active == 1)
	{
		HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);
	}
	return ret;
}

uint8_t com_buzzer::Active_Set(uint16_t pwm, uint16_t Time)
{
	if(pwm > 2000 || Time < aTime)
	{
		return COM_ERROR;
	}
	for(int j = 0;j < Time / aTime;j++)
	{
		HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET);
		Delay((pwm / (2000 / aTime)));
		HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);
		Delay((aTime - pwm / (2000 / aTime)));
	}
	return COM_OK;
}

uint8_t com_buzzer::Play_Music(song_type music)
{
	uint8_t ret = COM_OK;
	switch(music)
	{
	case Always_with_me:
	for(int i = 0; i < sizeof(Always_with_me_note) / sizeof(Always_with_me_note[0]); i++)
	{
		Set_Music_PWM(trans_note_to_freq(Always_with_me_note[i].mName), Always_with_me_note[i].mTime);
	}
	break;

	default:
	ret = COM_ERROR;
	break;
	}
	return ret;
}

uint8_t com_buzzer::Play_Music_Once(song_type music)
{
	uint8_t ret = COM_OK;
	static uint32_t count = 0;
	static song_type music_temp = music;
	uint32_t Len = 0;
	if(music_temp != music)
	{
		count = 0;
		music_temp = music;
	}
	switch(music)
	{
	case Always_with_me:
	Len = sizeof(Always_with_me_note) / sizeof(Always_with_me_note[0]);
	Set_Music_PWM(trans_note_to_freq(Always_with_me_note[count].mName), Always_with_me_note[count].mTime);
	break;

	default:
	ret = COM_ERROR;
	break;
	}

	if(count > Len)
	{
		count = 0;
		ret = COM_BUSY;
	}
	count++;
	return ret;
}

uint8_t com_buzzer::Set_Music_PWM(uint16_t pwm, uint16_t Time)
{
	uint8_t ret = COM_OK;
	if(Passive_or_Active == 0)
	{
		Set_PWM(pwm);
		Delay(Time);
	}
	else if(Passive_or_Active == 1)
	{
		ret = Active_Set(pwm, Time);
	}
	return ret;
}

uint16_t com_buzzer::trans_note_to_freq(uint8_t note)
{
	if(note > 17)
	{
		return 0;
	}
	return music_freq_ptr[note];
}

////定义低音音名C
//#define L1 262
//#define L2 294
//#define L3 330
//#define L4 349
//#define L5 392
//#define L6 440
//#define L7 494
////定义中音音名C
//#define M1 523
//#define M2 587
//#define M3 659
//#define M4 698
//#define M5 784
//#define M6 880
//#define M7 988
////定义高音音名C
//#define H1 1047
//#define H2 1175
//#define H3 1319
//#define H4 1397
//#define H5 1568
//#define H6 1760
//#define H7 1976
// //定义低音音名F (单位是Hz)
// #define L1 349
// #define L2 392
// #define L3 440
// #define L4 466
// #define L5 523
// #define L6 587
// #define L7 659
// //定义中音音名F
// #define M1 698
// #define M2 784
// #define M3 880
// #define M4 932
// #define M5 1047
// #define M6 1175
// #define M7 1319
// //定义高音音名F
// #define H1 1397
// #define H2 1568
// #define H3 1760
// #define H4 1865