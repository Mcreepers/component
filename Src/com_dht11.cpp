#include "com_dht11.h"
#include "tim.h"
#include "math.h"

com_dht11::com_dht11(GPIO_TypeDef *DHT11_GPIO, uint16_t DHT11_PIN)
{
	this->DHT11_GPIO = DHT11_GPIO;
	this->DHT11_PIN = DHT11_PIN;
}

void com_dht11::DHT11_Start()
{
	DHT_GPIO_OUTPUT();
	HAL_GPIO_WritePin(DHT11_GPIO, DHT11_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(DHT11_GPIO, DHT11_PIN, GPIO_PIN_RESET);
	osDelay(20);
	HAL_GPIO_WritePin(DHT11_GPIO, DHT11_PIN, GPIO_PIN_SET);

	DHT_GPIO_INPUT();
	while(HAL_GPIO_ReadPin(DHT11_GPIO, DHT11_PIN) == GPIO_PIN_SET);
	while(HAL_GPIO_ReadPin(DHT11_GPIO, DHT11_PIN) == GPIO_PIN_RESET);
	while(HAL_GPIO_ReadPin(DHT11_GPIO, DHT11_PIN) == GPIO_PIN_SET);
}

uint8_t com_dht11::DHT11_Read_Byte()
{
	uint8_t temp;
	for(uint8_t i = 0; i < 8; i++)
	{
		while(HAL_GPIO_ReadPin(DHT11_GPIO, DHT11_PIN) == GPIO_PIN_RESET);
		Delay_us(50);
		temp = 0;
		if(HAL_GPIO_ReadPin(DHT11_GPIO, DHT11_PIN) == GPIO_PIN_SET)
		{
			temp = 1;
			while(HAL_GPIO_ReadPin(DHT11_GPIO, DHT11_PIN) == GPIO_PIN_SET);
		}
		Buf <<= 1;
		Buf |= temp;
	}
	return Buf;
}

uint8_t com_dht11::DHT11_Read()
{
	DHT11_Start();
	DHT_GPIO_INPUT();

	for(uint8_t i = 0; i < 5; i++)
	{
		Data[i] = DHT11_Read_Byte();
	}
	if(Data[0] + Data[1] + Data[2] + Data[3] == Data[4])
	{
		static float temperature_temp = 0, humidity_temp = 0;
		float decimal = 0;
		for(int i = 0; i < 8; i++)
		{
			decimal += ((Data[1] >> i) & 1) * powf(2, -i - 1);
		}
		humidity = (Data[0] + decimal) * Iterative + humidity_temp * (1 - Iterative);
		humidity_temp = humidity;
		decimal = 0;
		for(int i = 0; i < 8; i++)
		{
			decimal += ((Data[3] >> i) & 1) * powf(2, -i - 1);
		}
		temperature = (Data[2] + decimal) * Iterative + temperature_temp * (1 - Iterative);
		temperature_temp = temperature;
		return COM_OK;
	}
	else
	{
		return COM_ERROR;
	}
}

void com_dht11::DHT_GPIO_INPUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Pin = DHT11_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(DHT11_GPIO, &GPIO_InitStructure);
}

void com_dht11::DHT_GPIO_OUTPUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Pin = DHT11_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(DHT11_GPIO, &GPIO_InitStructure);
}