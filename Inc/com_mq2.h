/*
 * @Author: Mcreeper 1507907006@qq.com
 * @Date: 2024-04-09 17:13:51
 * @LastEditors: Mcreeper 1507907006@qq.com
 * @LastEditTime: 2024-04-17 15:51:54
 * @FilePath: \SmartHomee:\Component\Inc\com_mq2.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __COM_MQ2_H
#define __COM_MQ2_H

#include "component.h"

class com_mq2
{
public:
	com_mq2(ADC_HandleTypeDef *hadc, GPIO_TypeDef *port, uint16_t pin);
	uint8_t Get_MQ2_Status();
	uint16_t Get_MQ2_ADC();
private:
	ADC_HandleTypeDef *hadc;
	GPIO_TypeDef *port;
	uint16_t pin;

	uint8_t MQ2_Status;
	uint16_t MQ2_ADC;
};

#endif // __COM_MQ2_H