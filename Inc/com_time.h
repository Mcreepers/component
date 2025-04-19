#ifndef __COM_TIME_H__
#define __COM_TIME_H__

#ifdef __cplusplus
extern "C"
{
#endif
    unsigned char com_delay_ms(unsigned int ms);
    unsigned int com_get_tick(void);
    unsigned int com_get_tick_os(void);
    unsigned char com_delay_ms_os(unsigned int ms);
#ifdef __cplusplus
}
#endif

// #include "component.h"
#include "com_types.h"
#include <cstdint>

class com_time
{
  public:
    com_time();
    // com_time(volatile uint32_t *us_tick_ptr, uint16_t period);//unit is us
    // com_time(TIMHandlerTypeDef *htim);
    void Init(volatile uint32_t *tick_ptr, uint16_t period);
    void Init(volatile uint32_t *tick_ptr, uint16_t period, uint16_t unit);

    uint8_t delay_us(uint32_t us);
    uint8_t delay_ms(uint32_t ms);
#ifdef COM_CMSIS_OS
    uint8_t delay_us_os(uint32_t us);
    uint8_t delay_ms_os(uint32_t ms);
#endif
    uint8_t add_time_us_count(uint32_t us);

    uint64_t get_time_us_count(void);
    uint64_t get_time_ms_count(void);

    void time_update(void);

  private:
    uint32_t us_tick;
    uint32_t us_tick_last;

    uint64_t tick_count;

    uint16_t period;
    uint16_t unit;
    volatile uint32_t *tick_ptr; // = &htim->Instance->CNT;unit is us
    bool us_ptr_flag;            // check if tick_ptr is used
};

#endif // __COM_TIME_H__
