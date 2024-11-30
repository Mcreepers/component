/*
 * @Author: Mcreeper 1507907006@qq.com
 * @Date: 2024-04-09 17:13:37
 * @LastEditors: Mcreeper 1507907006@qq.com
 * @LastEditTime: 2024-04-17 17:41:28
 * @FilePath: \SmartHomee:\Component\Src\com_mq2.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "com_mq2.h"

com_mq2::com_mq2(ADC_HandleTypeDef *hadc, GPIO_TypeDef *port, uint16_t pin)
{
	this->hadc = hadc;
	this->port = port;
	this->pin = pin;
}

/**
 * @description: Get the status of the MQ2 sensor
 * @param {*}
 * @return 0: no gas detected, 1: gas detected
*/
uint8_t com_mq2::Get_MQ2_Status()
{
	MQ2_Status = HAL_GPIO_ReadPin(port, pin);
	return MQ2_Status;
}

/**
 * @description: Get the ADC value of the MQ2 sensor
 * @param {*}
 * @return {uint16_t} The ADC value of the MQ2 sensor
*/
uint16_t com_mq2::Get_MQ2_ADC()
{
	HAL_ADC_Start(hadc);
	HAL_ADC_PollForConversion(hadc, 100);
	if(HAL_IS_BIT_SET(HAL_ADC_GetState(hadc), HAL_ADC_STATE_REG_EOC))
	{
		MQ2_ADC = HAL_ADC_GetValue(hadc);
	}
	return MQ2_ADC;
}