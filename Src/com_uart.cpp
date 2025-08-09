#include "com_uart.h"

#if defined(HAL_UART_MODULE_ENABLED)
uint8_t com_uart::Uart_Tx_Fun(uint8_t *buf, uint16_t len) {
    uint8_t ret = HAL_UART_Transmit(huart, buf, len, 0xffff);
    return ret;
}

uint8_t com_uart::Uart_RxRXNE_Fun(uint16_t len) {
    uint8_t ret = 0;
    if (Uart_Set_State == RX_RXNE) {
        ret = HAL_UART_Receive_IT(huart, Uart_Data, len);
    } else if (Uart_Set_State == RX_RXNE_IDLE) {
        if (HAL_UART_Receive_IT(huart, Uart_Data + Uart_Len, 1) == HAL_OK) {
            Uart_State = RX_RXNE;
            Uart_Len++;
            ret = COM_OK;
        } else {
            Uart_State = RX_ERROR;
            ret = COM_ERROR;
        };
    }
    return ret;
}

uint8_t com_uart::Uart_RxIDLE_Fun(uint16_t len) {
    uint8_t ret = 0;
    if (Uart_Set_State == RX_IDLE) {
        Uart_Len = len;
        ret = HAL_UARTEx_ReceiveToIdle_IT(huart, Uart_Data, 128);
    } else if (Uart_Set_State == RX_RXNE_IDLE) {
        Uart_Len = 0;
        ret = HAL_UARTEx_ReceiveToIdle_IT(huart, Uart_Data, 1);
    }
    return ret;
}
// Path: Component/Src/com_uart.cpp
#endif  // HAL_UART_MODULE_ENABLED