#ifndef __COM_KEY_H
#define __COM_KEY_H

#include "component.h"

class com_key {
public:
  com_key() = default;
  com_key(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, bool mode = 0);
#if defined(HAL_TIM_MODULE_ENABLED)
  com_key(TIM_HandleTypeDef *htim, uint32_t Channel, bool mode = 0);
#endif

  void Set_Key_Press(bool mode);
  bool Get_Key_Press();
  uint8_t Get_Key_Count(bool mode);

  void Key_IC_ISR();
  void Key_GPIO_ISR();
  void Key_Stroke();

private:
  GPIO_TypeDef *GPIOx;
  uint16_t GPIO_Pin;
#if defined(HAL_TIM_MODULE_ENABLED)
  TIM_HandleTypeDef *htim;
#endif
  uint32_t Channel;
  uint8_t Start_State;

  uint8_t HardWare = 0; // 0:NO HardWare 1:GPIO 2:TIM_IC
  uint8_t Mode;         // 0:High 1:Low 2:Rising 3:Falling
  bool Key_Press;
  uint8_t Key_Count;
};

#endif