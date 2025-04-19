#ifndef __COM_JDY_AT_H
#define __COM_JDY_AT_H

#include "component.h"

typedef enum
{
    COM_JDY_AT_NONE = 0,
    COM_JDY_AT_READY,
    COM_JDY_AT_ERROR,
    COM_JDY_AT_BUSY,
    COM_JDY_AT_TIMEOUT,
    COM_JDY_AT_SPP,
    COM_JDY_AT_SPP_CONNECTED,
    COM_JDY_AT_BLE,
}COM_JDY_AT_StatusTypeDef;

class com_jdy_at
{
public:
    com_jdy_at();
    uint8_t AT_Read_Cmd(uint8_t *cmd, uint16_t len);
    uint8_t AT_Get_Cmd(COM_JDY_AT_StatusTypeDef status, uint8_t *cmd, uint16_t &len);

    uint8_t AT_Set_SPP(const char *addr, uint16_t len);
    //uint8_t AT_Set_BLE(const char *addr,uint16_t len);

    uint8_t AT_Get_Status() { return Status; }
    uint8_t AT_SPP_CONNECTED() { return Status = COM_JDY_AT_SPP_CONNECTED; }
private:
    COM_JDY_AT_StatusTypeDef Status;

    uint8_t SPP_ADDR[18];
    uint16_t SPP_ADDR_Len;
    uint8_t CMD[64];
};

#endif