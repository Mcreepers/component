#include "com_time.h"

com_time::com_time()
{
	this->us_ptr_flag = false;
	this->tick_count = 0;
}

uint8_t com_time::delay_us(uint32_t us)
{
	uint64_t start = tick_count;
	while(tick_count - start < us)
	{
		time_update();
	}
	return COM_OK;
}

void com_time::Init(volatile uint32_t *tick_ptr, uint16_t period)
{
	this->tick_ptr = tick_ptr;
	this->period = period + 1;
	this->unit = 1;
	this->us_ptr_flag = true;
	this->tick_count = 0;
}

/**
 * @brief: Init the time
 * @param: tick_ptr: the pointer to the tick
 * @param: period: the period of the tick
 * @param: unit: 1 us = unit tick
*/
void com_time::Init(volatile uint32_t *tick_ptr, uint16_t period, uint16_t unit = 1)
{
	this->tick_ptr = tick_ptr;
	this->period = period + 1;
	this->unit = unit;
	this->us_ptr_flag = true;
	this->tick_count = 0;
}

uint8_t com_time::delay_ms(uint32_t ms)
{
	uint64_t start = tick_count;
	while(tick_count - start < ms * 1000 * unit)
	{
		time_update();
	}
	return COM_OK;
}

/**
 * @brief: update the time
 * @todo improve
 */
void com_time::time_update(void)
{
	if(us_ptr_flag)
	{
		us_tick = *tick_ptr;
		int32_t error = us_tick - us_tick_last;
		if(error <= 0)
		{
			tick_count += period;
		}
		tick_count += error;
	}
	us_tick_last = us_tick;
}

uint8_t com_time::add_time_us_count(uint32_t us)
{
	if(us_ptr_flag)
	{
		return COM_ERROR;
	}
	us_tick_last = us_tick;
	us_tick = us * unit;
	tick_count += us;
	return COM_OK;
}

uint64_t com_time::get_time_us_count(void)
{
	return tick_count;
}

uint64_t com_time::get_time_ms_count(void)
{
//	return tick_count / 1000;
	return (tick_count * 0x4189374BC6A7EF9DULL) >> 40;
}

unsigned char com_delay_ms(unsigned int ms)
{
	HAL_Delay(ms);
	return 0;
}

unsigned char com_delay_ms_os(unsigned int ms)
{
#ifdef COM_CMSIS_OS
	return osDelay(ms);
#endif
	return com_delay_ms(ms);
}

unsigned int com_get_tick(void)
{
	return HAL_GetTick();

}
unsigned int com_get_tick_os(void)
{
#ifdef COM_CMSIS_OS
	return osKernelGetTickCount();
#endif
	return com_get_tick();
}

#ifdef COM_CMSIS_OS

#include "cmsis_os2.h"

uint8_t com_time::delay_us_os(uint32_t us)
{
	uint32_t ms = us / 1000;
	osDelay(ms);
	while(tick_count < us * unit)
	{
		time_update();
	}
	return COM_OK;
}

uint8_t com_time::delay_ms_os(uint32_t ms)
{
	return osDelay(ms);
}
#endif

// uint8_t com_time::time_update(uint32_t us)
// {
// 	tick_count += us;
// 	if(tick_count >= 1000)
// 	{
// 		ms_count += tick_count / 1000;
// 		tick_count = tick_count % 1000;
// 	}
// 	return COM_OK;
// }

