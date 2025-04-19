#ifndef __COM_STEER_H
#define __COM_STEER_H

#include "component.h"

//__weak class com_steer {};

class com_steer
{
public:
	com_steer(TIM_HandleTypeDef *htim, uint32_t channel);
	uint16_t Trans_Steer_Angle(float angle);
	float Get_Steer_Angle();
	uint16_t Get_Steer_Duty();
	void Set_Steer_Angle(float angle);
	void Set_Steer_Duty(uint16_t duty);
	void Set_Steer(float angle);
	void Set_Steer(uint16_t duty);
	void Set_Steer();

	void Set_Steer_Offset(uint16_t offset_duty) { this->offset_duty = (offset_duty - 15 * duration / 200); };
	void Set_Steer_Direction(bool direction) { this->direction = direction; };
private:
	TIM_HandleTypeDef *htim;
	uint32_t channel;

	uint16_t duration = 2000;

	float angle;
	uint16_t duty;
	int16_t offset_duty;
	bool direction = 0;

	uint8_t Strat_Status;
	bool High_Level_or_Low = true;//true is high level 15,false is low level 185
};



#endif