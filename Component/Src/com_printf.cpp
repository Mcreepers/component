#include "com_printf.h"

#include <stdarg.h>
#include "stdio.h"
#include "string.h"

com_printf::com_printf()
{
	Output_ptr = NULL;
}

uint8_t com_printf::printf(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	Lenth = vsnprintf((char *)Buf, 1024, (char *)format, args);
	va_end(args);
	if(Output_ptr == NULL)
	{
		return COM_ERROR;
	}
	else
	{
		Output_ptr(Buf, Lenth);
	}
	return COM_OK;
}

uint8_t com_printf::printf_add(const char *format, ...)
{
	if(Lenth > 100)
	{
		printf_out();
		return COM_BUSY;
	}
	va_list args;
	va_start(args, format);
	Lenth += vsnprintf((char *)Buf + Lenth, 1024 - Lenth, (char *)format, args);
	va_end(args);

	return COM_OK;
}

uint8_t com_printf::printf_add(void *buf, uint16_t len)
{
	if(Lenth > 100)
	{
		printf_out();
		return COM_BUSY;
	}
	memcpy(Buf + Lenth, buf, len);
	Lenth += len;
	return COM_OK;
}

uint8_t com_printf::printf_out()
{
	uint8_t ret = COM_OK;
	if(Output_ptr == NULL || Lenth == 0)
	{
		return COM_ERROR;
	}
	else
	{
		ret = Output_ptr(Buf, Lenth);
		Lenth = 0;
	}
	return ret;
}
