#include "component.h"
#include "string.h"

com_jdy_at::com_jdy_at()
{
	memset(SPP_ADDR, 0, 18);
}

uint8_t com_jdy_at::AT_Read_Cmd(uint8_t *cmd, uint16_t len)
{
	if(len > 0)
	{
		if(memcmp((char *)cmd, "+OK", 3) == 0)
		{
			Status = COM_JDY_AT_READY;
		}
		else if(memcmp((char *)cmd, "+DEV:1=", 7) == 0)
		{
			memcpy(SPP_ADDR, cmd + 7, 12);
		}
		else if(memcmp((char *)cmd, "+CONNECTED>>", 12) == 0)
		{
			Status = COM_JDY_AT_SPP;
		}
		else if(memcmp((char *)cmd, "AT\r\n", 4) == 0)
		{
			Status = COM_JDY_AT_SPP;
		}
		else if(memcmp((char *)cmd, "SPP_CONNECTED", 2) == 0)
		{
			Status = COM_JDY_AT_SPP_CONNECTED;
		}
	}
	else
	{
		return COM_ERROR;
	}
	return Status;
}

uint8_t com_jdy_at::AT_Get_Cmd(COM_JDY_AT_StatusTypeDef status, uint8_t *cmd, uint16_t &len)
{
	uint8_t ret = COM_OK;
	if(Status == COM_JDY_AT_NONE && status != COM_JDY_AT_READY)
	{
		return COM_ERROR;
	}
	else
	{
		switch(status)
		{
		case COM_JDY_AT_READY:
		len = 4;
		memcpy(cmd, "AT\r\n", 4);
		break;
		case COM_JDY_AT_SPP:
		len = SPP_ADDR_Len + 7 + 2;
		memcpy(cmd, "AT+CONA", 7);
		memcpy(cmd + 7, SPP_ADDR, SPP_ADDR_Len);
		memcpy(cmd + 7 + SPP_ADDR_Len, "\r\n", 2);
		break;
		case COM_JDY_AT_BLE:
		break;
		case COM_JDY_AT_SPP_CONNECTED:
		len = 13;
		memcpy(cmd, "SPP_CONNECTED", 13);
		break;
		default:
		ret = COM_ERROR;
		break;
		}
	}
	return ret;
}

uint8_t com_jdy_at::AT_Set_SPP(const char *addr, uint16_t len)
{
	uint8_t ret = COM_OK;
	if(addr != NULL)
	{
		SPP_ADDR_Len = len;
		memcpy(SPP_ADDR, addr, len);
	}
	else
	{
		ret = COM_ERROR;
	}
	return ret;
}