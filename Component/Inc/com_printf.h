#ifndef __COM_PRINTF_H
#define __COM_PRINTF_H

#include "component.h"

class com_printf
{
public:
	com_printf();
	uint8_t printf(const char *format, ...);
	uint8_t(*Output_ptr)(uint8_t *, uint16_t);

	uint8_t printf_add(const char *format, ...);
	uint8_t printf_add(void *buf, uint16_t len);
	uint8_t printf_out();
private:
	void *ptr;
	uint16_t Lenth;
	uint8_t Buf[128];
};


#endif