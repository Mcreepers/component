#include "component.h"
#include "math.h"

#if defined(HAL_TIM_MODULE_ENABLED)

com_steer::com_steer(TIM_HandleTypeDef *htim, uint32_t channel)
    : htim(htim), channel(channel) {}

uint16_t com_steer::Trans_Steer_Angle(float angle) {
    this->angle = angle;
    this->duty =
        roundf((angle * 10 + 500) / 1800 * duration + 500) + offset_duty;
    return this->duty;
}

float com_steer::Get_Steer_Angle() { return this->angle; }

uint16_t com_steer::Get_Steer_Duty() { return this->duty; }

void com_steer::Set_Steer_Angle(float angle) {
    this->angle = angle;
    if (High_Level_or_Low == false) {
        this->duty = roundf(angle / 1800 * duration + 185 * duration / 200) +
                     offset_duty;
    } else {
        this->duty =
            roundf(angle / 1800 * duration + 15 * duration / 200) + offset_duty;
    }
}

void com_steer::Set_Steer_Duty(uint16_t duty) {
    this->duty = duty;
    if (High_Level_or_Low == false) {
        this->angle = (duty - 185 * duration / 200) * 180 / duration;
    } else {
        this->angle = (duty - 15 * duration / 200) * 180 / duration;
    }
}

void com_steer::Set_Steer(float angle) {
    Set_Steer_Angle(angle);
    Set_Steer();
}

void com_steer::Set_Steer(uint16_t duty) {
    Set_Steer_Duty(duty);
    Set_Steer();
}

void com_steer::Set_Steer() {
    if (Strat_Status == 0) {
        HAL_TIM_PWM_Start(htim, channel);
        Strat_Status = 1;
    }
    __HAL_TIM_SET_COMPARE(htim, channel, duty);
}
#endif  // HAL_TIM_MODULE_ENABLED