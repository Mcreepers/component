#include "MessageTask.h"
#include "com_uart.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_uart.h"
#include "stm32g4xx_hal_uart_ex.h"
#include "usart.h"
#include "usbd_cdc_if.h"

cMessage Message;
extern com_os OS;

uint8_t uart_tx_fun(uint8_t *buf, uint16_t len)
{
    return Message.Uart_Tx_Fun(buf, len);
}

void messagetask(void *argument) { Message.Task(); }

cMessage::cMessage() : com_uart(&huart1, RX_IDLE) {}

void cMessage::Task()
{
    if (!Init_Flag)
    {
        Output_ptr = &uart_tx_fun;
        HAL_UARTEx_ReceiveToIdle_IT(&huart1, Message.Get_Uart_Data(), 128);
    }

    printf_add("Heart: %d\n", Heart);
    printf_out();
    Heart++;
    OS.Delay(200);
}
