#ifndef __COM_SR04_H
#define __COM_SR04_H

#include "component.h"
#if defined(HAL_TIM_MODULE_ENABLED)

class com_sr04 {
   public:
    com_sr04(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, TIM_HandleTypeDef *htim,
             uint8_t Tim_Channel, uint32_t *Tim_overflow_Cnt);
    void SR04_Start();
    void SR04TimOverflowIsr();
    void SR04_Tim_IC_Isr();
    float Get_SR04_Distance();
    bool Get_SR04_Run_Flag() { return SR04_Run_Flag; };
    TIM_HandleTypeDef *GET_SR04_Tim() { return htim; };
    uint8_t GET_SR04_Tim_Channel() { return Tim_Channel; };

   private:
    GPIO_TypeDef *GPIOx;      // Trig
    uint16_t GPIO_Pin;        // Trig
    TIM_HandleTypeDef *htim;  // Echo
    uint8_t Tim_Channel;      // Echo
    // bool Mode;//0:GPIO 1:PWM

    uint32_t *Tim_overflow_Cnt;
    uint8_t edge_state;
    uint32_t Start_Time;
    uint32_t End_Time;
    uint32_t Duration;

    float Distance = 0;
    bool SR04_Run_Flag = 0;
    float Iterative = 0.9f;
};
#endif
#endif  // __COM_SR04_H