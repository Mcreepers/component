#include "com_sr04.h"
#include "tim.h"

/**
 * @brief Construct a new com_sr04::com_sr04 object
 * @note Must use SR04TimOverflowIsr in HAL_TIM_PeriodElapsedCallback,use SR04_Tim_IC_Isr in TIMx_IC_IRQHandler.Otherwise, it will not work.
*/
com_sr04::com_sr04(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, TIM_HandleTypeDef *htim, uint8_t Tim_Channel, uint32_t *Tim_overflow_Cnt)
	:GPIOx(GPIOx), GPIO_Pin(GPIO_Pin), htim(htim), Tim_Channel(Tim_Channel), Tim_overflow_Cnt(Tim_overflow_Cnt)
{}

void com_sr04::SR04_Start()
{
	static uint8_t SR04_Start_Flag = 0;
	if(SR04_Start_Flag == 0)
	{
		HAL_TIM_Base_Start_IT(htim);
		HAL_TIM_IC_Start_IT(htim, Tim_Channel);
		SR04_Start_Flag = 1;
	}

	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);
	Delay_us(12);//over 10us
	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET);
}

void com_sr04::SR04TimOverflowIsr()
{
	(*Tim_overflow_Cnt)++;
}

void com_sr04::SR04_Tim_IC_Isr()
{

	if(edge_state == 0)
	{
		Start_Time = HAL_TIM_ReadCapturedValue(htim, Tim_Channel);
		__HAL_TIM_SET_CAPTUREPOLARITY(htim, Tim_Channel, TIM_INPUTCHANNELPOLARITY_FALLING);
		*Tim_overflow_Cnt = 0;
		edge_state = 1;
	}
	else if(edge_state == 1)
	{
		static float Distance_temp = 0;
		End_Time = HAL_TIM_ReadCapturedValue(htim, Tim_Channel);
		End_Time += *Tim_overflow_Cnt * htim->Init.Period;
		Duration = End_Time - Start_Time;
		Distance = (Duration * 0.034 / 2) * Iterative + Distance_temp * (1 - Iterative);
		Distance_temp = Distance;
		__HAL_TIM_SET_CAPTUREPOLARITY(htim, Tim_Channel, TIM_INPUTCHANNELPOLARITY_RISING);
		edge_state = 0;
		SR04_Run_Flag = 1;
	}
}

float com_sr04::Get_SR04_Distance()
{
	if(Distance >= 450)
	{
		return 450;
	}
	else if(Distance <= 0)
	{
		return 0;
	}
	return Distance;
}