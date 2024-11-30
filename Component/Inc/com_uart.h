#ifndef __COM_UART_H
#define __COM_UART_H

#include "component.h"

enum Uart_StatusTypeDef
{
	RX_RXNE = 0,
	RX_IDLE,
	RX_RXNE_IDLE,
	RX_ERROR
};

class com_uart
{
private:
	uint16_t Uart_Len;
	Uart_StatusTypeDef Uart_Set_State;
	Uart_StatusTypeDef Uart_State;
	uint8_t Uart_Data[128];

	UART_HandleTypeDef *huart;
public:
	com_uart(UART_HandleTypeDef *huart_, Uart_StatusTypeDef Uart_State_) :huart(huart_), Uart_Set_State(Uart_State_)
	{
		Uart_Len = 0;
	};

	uint8_t Uart_Tx_Fun(uint8_t *buf, uint16_t len);
	uint8_t Uart_RxRXNE_Fun(uint16_t len);
	uint8_t Uart_RxIDLE_Fun(uint16_t len);

	uint16_t Get_Uart_Len() { return Uart_Len; };
	uint16_t Get_Uart_Len_Clear() { uint16_t len = Uart_Len; Uart_Len = 0; return len; };
	uint8_t *Get_Uart_Data() { return Uart_Data; };
};



#endif // __COM_UART_H