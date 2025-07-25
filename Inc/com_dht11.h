#ifndef __COM_DHT11_H
#define __COM_DHT11_H

#include "component.h"

class com_dht11 {
public:
  com_dht11(GPIO_TypeDef *DHT11_GPIO, uint16_t DHT11_PIN);
  void DHT11_Start();
  uint8_t DHT11_Read_Byte();
  uint8_t DHT11_Read();

  float Get_Humidity() { return humidity; }
  float Get_Temperature() { return temperature; }

private:
  void DHT_GPIO_INPUT(void);
  void DHT_GPIO_OUTPUT(void);

  GPIO_TypeDef *DHT11_GPIO;
  uint16_t DHT11_PIN;

  uint8_t Buf;
  uint8_t Data[5];
  float Iterative = 0.5f;

  float humidity;
  float temperature;
};

#endif // __COM_DHT11_H