#ifndef __COM_BUZZER_H
#define __COM_BUZZER_H

#include "component.h"

//Note duration
#define TT 2000

typedef enum song_type
{
	Always_with_me = 0,
	song_type_count
}song;

class com_buzzer
{
public:
	com_buzzer(TIM_HandleTypeDef *htim, uint8_t channel);//passive buzzer
	com_buzzer(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint16_t aTime);//active buzzer

	uint8_t Open_PWM(void);
	uint8_t Set_PWM(uint16_t pwm);
	uint8_t Play_Music(song_type music);
	uint8_t Play_Music_Once(song_type music);
	uint8_t Close_PWM(void);
private:
	TIM_HandleTypeDef *htim;
	uint8_t channel;
	GPIO_TypeDef *GPIOx;
	uint8_t GPIO_Pin;
	uint16_t aTime;//周期ms
	uint32_t Clock = 168000000;//时钟频率
	uint16_t Sound = 100;//
	bool PWM_Opened = false;

	uint8_t Passive_or_Active;//0:passive 1:active

	uint16_t *music_note_ptr;
	uint16_t *music_freq_ptr;

	void Delay(uint32_t ms);
	uint8_t Active_Set(uint16_t pwm, uint16_t Time);
	uint8_t Set_Music_PWM(uint16_t pwm, uint16_t Time);

	uint16_t trans_note_to_freq(uint8_t note);
//    uint16_t trans_freq_to_note(uint16_t freq);
};

namespace music
{
	const uint16_t music_note[3][7] =
	{
		{262, 294, 330, 349, 392, 440, 494},
		{523, 587, 659, 698, 784, 880, 988},
		{1047, 1175, 1319, 1397, 1568, 1760, 1976}
	};

	const uint16_t music_freq[3][7] =
	{
		{349, 392, 440, 466, 523, 587, 659},
		{698, 784, 880, 932, 1047, 1175, 1319},
		{1397, 1568, 1760, 1865}
	};

	typedef enum note_type
	{
		L1 = 0, L2, L3, L4, L5, L6, L7,
		M1, M2, M3, M4, M5, M6, M7,
		H1, H2, H3, H4, H5, H6, H7
	}note;

	typedef struct
	{
		short mName; //音名
		short mTime; //时值，全音符，二分音符，四分音符
	}tNote;

	const tNote Always_with_me_note[] =
	{
	{0,TT / 2},{M1,TT / 8},{M2,TT / 8},{M3,TT / 8},{M1,TT / 8},{M5,TT / 4 + TT / 8},{M3,TT / 8},
	{M2,TT / 4},{M5,TT / 4},{M2,TT / 4},{M1,TT / 8},{L6,TT / 8},{M3,TT / 4 + TT / 8},{M1,TT / 8},
	{L7,TT / 2},{M1,TT / 8},{L7,TT / 8},{L6,TT / 4},{L7,TT / 4},{M1,TT / 8},{M2,TT / 8},
	{L5,TT / 4},{M1,TT / 4},{M2,TT / 8},{M3,TT / 8},{M4,TT / 4},{M4,TT / 8},{M3,TT / 8},{M2,TT / 8},{M1,TT / 8},
	{M2,TT / 2},{M1,TT / 8},{M2,TT / 8},{M3,TT / 8},{M1,TT / 8},{M5,TT / 4 + TT / 8},{M3,TT / 8},
	{M2,TT / 4},{M5,TT / 4},{M2,TT / 4},{M1,TT / 8},{L6,TT / 8},{L6,TT / 4},{L7,TT / 8},{M1,TT / 8},
	{L5,TT / 2},{0,TT / 8},{L5,TT / 8},{L6,TT / 4},{L7,TT / 4},{M1,TT / 8},{M2,TT / 8},//question
	{L5,TT / 4},{M1,TT / 4},{M2,TT / 8},{M3,TT / 8},{M4,TT / 4},{M4,TT / 8},{M3,TT / 8},{M2,TT / 8},{M1,TT / 8},{M1,TT / 2 + TT / 4},

	{0,TT / 2},{M3,TT / 8},{M4,TT / 8},{M5,TT / 4},{M5,TT / 4},{M5,TT / 4},{M5,TT / 4},{M5,TT / 8},{M6,TT / 8},{M5,TT / 8},{M4,TT / 8},
	{M3,TT / 4},{M3,TT / 4},{M3,TT / 4},{M3,TT / 4},{M3,TT / 8},{M4,TT / 8},{M3,TT / 8},{M2,TT / 8},
	{M1,TT / 4},{M1,TT / 4},{M1,TT / 8},{L7,TT / 8},{L6,TT / 4},{L7,TT / 4},{L7,TT / 8},{M1,TT / 8},
	{M2,TT / 4},{M2,TT / 8},{M3,TT / 8},{M2,TT / 8},{M3,TT / 8},{M2,TT / 2},{M3,TT / 8},{M4,TT / 8},
	{M5,TT / 4},{M5,TT / 4},{M5,TT / 4},{M5,TT / 4},{M5,TT / 8},{M6,TT / 8},{M5,TT / 8},{M4,TT / 8},
	{M3,TT / 4},{M3,TT / 4},{M3,TT / 4},{M3,TT / 8},{M4,TT / 8},{M3,TT / 8},{M2,TT / 8},{M1,TT / 8},{L7,TT / 8},
	{L6,TT / 4},{L6,TT / 8},{L7,TT / 8},{M1,TT / 8},{M2,TT / 8},{L5,TT / 4},{M1,TT / 4},{M2,TT / 8},{M3,TT / 8},
	{M2,TT / 4 + TT / 8},{M2,TT / 8},{M2,TT / 8},{M1,TT / 8},{M1,TT / 2 + TT / 4},

	{0,TT / 2},{M1,TT / 8},{M2,TT / 8},{M3,TT / 8},{M1,TT / 8},{M5,TT / 4 + TT / 8},{M3,TT / 8},
	{M2,TT / 4},{M5,TT / 4},{M2,TT / 4},{M1,TT / 8},{L6,TT / 8},{M3,TT / 4 + TT / 8},{M1,TT / 8},
	{L7,TT / 2},{M1,TT / 8},{L7,TT / 8},{L6,TT / 4},{L7,TT / 4},{M1,TT / 8},{M2,TT / 8},
	{L5,TT / 4},{M1,TT / 4},{M2,TT / 8},{M3,TT / 8},{M4,TT / 4},{M4,TT / 8},{M3,TT / 8},{M2,TT / 8},{M1,TT / 8},
	{M2,TT / 2},{M1,TT / 8},{M2,TT / 8},{M3,TT / 8},{M1,TT / 8},{M5,TT / 4 + TT / 8},{M3,TT / 8},
	{M2,TT / 4},{M5,TT / 4},{M2,TT / 4},{M1,TT / 8},{L6,TT / 8},{L6,TT / 4},{L7,TT / 8},{M1,TT / 8},
	{L5,TT / 2},{0,TT / 8},{L5,TT / 8},{L6,TT / 4},{L7,TT / 4},{M1,TT / 8},{M2,TT / 8},//question
	{L5,TT / 4},{M1,TT / 4},{M2,TT / 8},{M3,TT / 8},{M4,TT / 4},{M4,TT / 8},{M3,TT / 8},{M2,TT / 8},{M1,TT / 8},{M1,TT / 2 + TT / 4},

	{0,TT / 2},{M3,TT / 8},{M4,TT / 8},{M5,TT / 4},{M5,TT / 4},{M5,TT / 4},{M5,TT / 4},{M5,TT / 8},{M6,TT / 8},{M5,TT / 8},{M4,TT / 8},
	{M3,TT / 4},{M3,TT / 4},{M3,TT / 4},{M3,TT / 4},{M3,TT / 8},{M4,TT / 8},{M3,TT / 8},{M2,TT / 8},
	{M1,TT / 4},{M1,TT / 4},{M1,TT / 8},{L7,TT / 8},{L6,TT / 4},{L7,TT / 4},{L7,TT / 8},{M1,TT / 8},
	{M2,TT / 4},{M2,TT / 8},{M3,TT / 8},{M2,TT / 8},{M3,TT / 8},{M2,TT / 2},{M3,TT / 8},{M4,TT / 8},
	{M5,TT / 4},{M5,TT / 4},{M5,TT / 4},{M5,TT / 4},{M5,TT / 8},{M6,TT / 8},{M5,TT / 8},{M4,TT / 8},
	{M3,TT / 4},{M3,TT / 4},{M3,TT / 4},{M3,TT / 8},{M4,TT / 8},{M3,TT / 8},{M2,TT / 8},{M1,TT / 8},{L7,TT / 8},
	{L6,TT / 4},{L6,TT / 8},{L7,TT / 8},{M1,TT / 8},{M2,TT / 8},{L5,TT / 4},{M1,TT / 4},{M2,TT / 8},{M3,TT / 8},
	{M2,TT / 4 + TT / 8},{M2,TT / 8},{M2,TT / 8},{M1,TT / 8},{M1,TT / 2 + TT / 4},
	};
};

#endif // __COM_BUZZER_H