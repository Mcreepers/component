#include "component.h"

/**
 * @brief Construct a new com_key::com_key object
 * @param GPIOx GPIO_TypeDef
 * @param GPIO_Pin GPIO PIN
 * @param mode Release level 0:High 1:Low
*/
com_key::com_key(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, bool mode)
{
	this->GPIOx = GPIOx;
	this->GPIO_Pin = GPIO_Pin;
	this->HardWare = 1;
	this->Key_Press = false;
	this->Key_Count = 0;
	this->Mode = mode;
}

/**
 * @brief Construct a new com_key::com_key object
 * @param htim TIM_HandleTypeDef
 * @param Channel Tim Channel
 * @param mode 0:Rising 1:Falling
*/
com_key::com_key(TIM_HandleTypeDef *htim, uint32_t Channel, bool mode)
{
	this->htim = htim;
	this->Channel = Channel;
	this->HardWare = 2;
	this->Key_Press = false;
	this->Key_Count = 0;
	this->Mode = mode + 2;
}

/**
 * @brief Set the Key Press object
 * @param mode true:Press false:Release
*/
void com_key::Set_Key_Press(bool mode)
{
	static bool Key_Press = false;
	this->Key_Press = mode;

	if(Key_Press != mode)
	{
		if(mode == false)
		{
			this->Key_Count++;
		}
		Key_Press = mode;
	}
}

/**
 * @brief Get the Key Press object
 * @return true:Press false:Release
 * @note If use EXIT mode,whenever possible use GET_KEY_COUNT(1) instead
*/
bool com_key::Get_Key_Press()
{
	return this->Key_Press;
}

/**
 * @brief Get the Key Count object
 * @param mode 0:No Clear 1:Clear
*/
uint8_t com_key::Get_Key_Count(bool mode)
{
	uint8_t Count = this->Key_Count;
	if(mode == 1)
	{
		this->Key_Count = 0;
	}
	return Count;
}

void com_key::Key_IC_ISR()
{
	if(HardWare != 2) { return; }
	static bool edge_state = false;
	if(this->Mode == 2)
	{
		if(edge_state == false)
		{
			edge_state = true;
			Set_Key_Press(true);
			__HAL_TIM_SET_CAPTUREPOLARITY(this->htim, this->Channel, TIM_INPUTCHANNELPOLARITY_FALLING);
		}
		else
		{
			edge_state = false;
			Set_Key_Press(false);
			__HAL_TIM_SET_CAPTUREPOLARITY(this->htim, this->Channel, TIM_INPUTCHANNELPOLARITY_RISING);
		}
	}
	else if(this->Mode == 3)
	{
		if(edge_state == false)
		{
			edge_state = true;
			Set_Key_Press(true);
			__HAL_TIM_SET_CAPTUREPOLARITY(this->htim, this->Channel, TIM_INPUTCHANNELPOLARITY_RISING);
		}
		else
		{
			edge_state = false;
			Set_Key_Press(false);
			__HAL_TIM_SET_CAPTUREPOLARITY(this->htim, this->Channel, TIM_INPUTCHANNELPOLARITY_FALLING);
		}
	}
}

void com_key::Key_GPIO_ISR()
{
	if(HardWare != 1) { return; }
	if(this->Mode == 0)
	{
		Set_Key_Press(!HAL_GPIO_ReadPin(this->GPIOx, this->GPIO_Pin));
	}
	else if(this->Mode == 1)
	{
		Set_Key_Press(HAL_GPIO_ReadPin(this->GPIOx, this->GPIO_Pin));
	}
}

/**
 * @brief This function is used to reduce key stroke
 * @note This function can use in ALL mode
 * @note Please call this function in a 20ms loop.
 * @todo Add a function to set the time interval
 * @todo test in all mode
*/
void com_key::Key_Stroke()
{
	static uint8_t count = 0;
	if(HardWare == 0)
	{
		if(this->Key_Press == true)
		{
			count++;
		}
		else
		{
			count = 0;
		}
	}
	else if(HardWare == 1)
	{
		if(this->Key_Press == true && this->Mode == 0 && HAL_GPIO_ReadPin(this->GPIOx, this->GPIO_Pin) == 0)
		{
			count++;
		}
		else if(this->Key_Press == true && this->Mode == 1 && HAL_GPIO_ReadPin(this->GPIOx, this->GPIO_Pin) == 1)
		{
			count++;
		}
		else
		{
			Set_Key_Press(false);
			count = 0;
		}
	}
	if(count > 1)
	{
		this->Key_Count++;
	}
}
