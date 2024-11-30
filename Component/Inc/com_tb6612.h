#ifndef __COM_TB6612_H
#define __COM_TB6612_H

#include "component.h"

class com_tb6612
{
public:
	com_tb6612(TIM_HandleTypeDef *htim, uint16_t Tim_Channel, GPIO_TypeDef *port, uint16_t pin1, uint16_t pin2);
	//com_tb6612(GPIO_TypeDef *port, uint16_t pin1, uint16_t pin2, uint16_t pin3, uint16_t pin4);
	void Set_Motor_Speed(int16_t speed);
	void Stop_Motor();
private:
	TIM_HandleTypeDef *htim;
	uint16_t Tim_Channel;
	GPIO_TypeDef *port;
	uint16_t pin1;
	uint16_t pin2;
	//uint16_t pin3;
	//uint16_t pin4;
};

#endif // __COM_TB6612_H