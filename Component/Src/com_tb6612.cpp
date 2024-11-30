/*
 * @Author: Mcreeper 1507907006@qq.com
 * @Date: 2024-04-09 17:13:07
 * @LastEditors: Mcreeper 1507907006@qq.com
 * @LastEditTime: 2024-04-17 16:18:18
 * @FilePath: \SmartHomee:\Component\Src\com_tb6612.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "com_tb6612.h"

#define abs(x) ((x)>0?(x):-(x))

com_tb6612::com_tb6612(TIM_HandleTypeDef *htim, uint16_t Tim_Channel, GPIO_TypeDef *port, uint16_t pin1, uint16_t pin2)
{
	this->htim = htim;
	this->Tim_Channel = Tim_Channel;
	this->port = port;
	this->pin1 = pin1;
	this->pin2 = pin2;
}

/**
 * @description: Set the speed of the motor
 * @param {int16_t} speed: The speed of the motor, range from 0 to 1000
 * @return {*}
 * @note: The motor will start when the speed is not 0
*/
void com_tb6612::Set_Motor_Speed(int16_t speed)
{
	static uint8_t Start_Status = 0;
	if(Start_Status == 0)
	{
		HAL_TIM_PWM_Start(htim, Tim_Channel);
		Start_Status = 1;
	}

	if(speed > 1000)
	{
		speed = 1000;
	}
	else if(speed < -1000)
	{
		speed = -1000;
	}


	if(speed > 0)
	{
		HAL_GPIO_WritePin(port, pin1, GPIO_PIN_SET);
		HAL_GPIO_WritePin(port, pin2, GPIO_PIN_RESET);
	}
	else if(speed < 0)
	{
		HAL_GPIO_WritePin(port, pin1, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(port, pin2, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(port, pin1, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(port, pin2, GPIO_PIN_RESET);
	}
	speed = abs(speed);
	__HAL_TIM_SET_COMPARE(htim, Tim_Channel, speed);
}

void com_tb6612::Stop_Motor()
{
	HAL_GPIO_WritePin(port, pin1, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(port, pin2, GPIO_PIN_RESET);
	__HAL_TIM_SET_COMPARE(htim, Tim_Channel, 0);
}

