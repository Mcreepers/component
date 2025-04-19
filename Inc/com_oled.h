#ifndef __COM_OLED_H
#define __COM_OLED_H
/*!
 * @file          com_oled.h
 * @author        Mcreeper
 * @date          2024-04-10
 * @brief         oled driver
 * @note		  use OLED_Begin() to initialize the OLED,than OLED_Update() to show
 */
#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#ifdef __cplusplus
}
#endif

#include "component.h"

typedef enum oled_type
{
	OLED_128x64 = 0x01,
	OLED_128x32 = 0x02,
	OLED_96x16 = 0x03
}oled_Type;//OLED type

typedef enum
{
	Hardware_IIC = 0x01,
	Software_IIC = 0x02,
	Hardware_SPI = 0x03,
//	Software_SPI = 0x04
}oled_hardware;//Hardware or software IIC

typedef enum
{
	OLED_6X8 = 0x06,
	OLED_8X16 = 0x08,
	OLED_12X12 = 0x12,
	OLED_16X16 = 0x16,
	OLED_24X24 = 0x24,
	OLED_CHINESE = 0x00,
}Font_Size;//Font size

class com_oled
{
public:
	com_oled();
	com_oled(I2C_HandleTypeDef *hi2c);
	// com_oled(SPI_HandleTypeDef *hspi, GPIO_TypeDef *OLED_RES_Port, uint16_t OLED_RES_Pin,
	// 	GPIO_TypeDef *OLED_DC_Port, uint16_t OLED_DC_Pin, GPIO_TypeDef *OLED_CS_Port, uint16_t OLED_CS_Pin);
	com_oled(GPIO_TypeDef *OLED_SCL_Port, uint16_t OLED_SCL_Pin,
		GPIO_TypeDef *OLED_SDA_Port, uint16_t OLED_SDA_Pin);
	uint8_t OLED_Begin();
	uint8_t OLED_Begin_Fast();
	void OLED_Clear();
	void OLED_Clear(uint8_t x, uint8_t y, uint8_t Width, uint8_t Height);
	uint8_t OLED_Update();
	uint8_t OLED_Update(uint8_t x, uint8_t y, uint8_t Width, uint8_t Height);

	uint8_t OLED_SetPos(uint8_t x, uint8_t y);
	uint8_t OLED_Fill(uint8_t bmp_data);
	uint8_t OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t Width, uint8_t Height, uint8_t BMP[]);

	class show
	{
	public:
		show(com_oled &oled) :oled(oled) {}
		uint8_t Char(uint8_t x, uint8_t y, uint8_t chr, uint8_t Size);
		uint8_t String(uint8_t x, uint8_t y, const char *String, uint8_t Size);
		uint8_t Num(uint8_t x, uint8_t y, int num, uint8_t len, uint8_t size);
		uint8_t Hex(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size);
		uint8_t Bin(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size);
		uint8_t Float(uint8_t x, uint8_t y, double num, uint8_t size);
		uint8_t Chinese(uint8_t x, uint8_t y, char *Chinese);
		uint8_t Printf(uint8_t x, uint8_t y, uint8_t size, const char *fmt, ...);
		uint8_t Printf(uint8_t x, uint8_t y, uint8_t size, uint8_t *buf, uint16_t len);

		uint8_t Point(uint8_t x, uint8_t y, uint8_t cmd);
		uint8_t GetPoint(uint8_t x, uint8_t y);
		uint8_t Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t cmd);
		uint8_t Rectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t IsFilled, uint8_t cmd);
		uint8_t Triangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t x3, uint8_t y3, uint8_t IsFilled, uint8_t cmd);
		uint8_t Circle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t cmd, uint8_t IsFilled);
		uint8_t Ellipse(uint8_t x0, uint8_t y0, uint8_t a, uint8_t b, uint8_t cmd, uint8_t IsFilled);
		uint8_t Arc(uint8_t x0, uint8_t y0, uint8_t r, int16_t StartAngle, int16_t EndAngle, uint8_t IsFilled, uint8_t cmd);
	private:
		com_oled &oled;
	}Show;
private:
	uint8_t OLED_Set_Cursor(uint8_t page, uint8_t x);
	void OLED_Reverse();
	void OLED_Reverse(uint8_t x, uint8_t y, uint8_t Width, uint8_t Height);
	uint8_t OLED_ShowImage(uint8_t x, uint8_t y, uint8_t Width, uint8_t Height, const uint8_t *Image);

	uint8_t OLED_WR_Byte(uint8_t data, uint8_t cmd);
	uint8_t OLED_WR_Data(uint8_t *data, uint8_t len);

	void OLED_I2C_Start();
	void OLED_I2C_Stop();
	void OLED_I2C_WaitAck();
	uint8_t OLED_I2C_Transmit(uint8_t *data, uint16_t len);

	void OLED_WR_SDA(uint8_t data);
	void OLED_WR_SCL(uint8_t data);

	uint8_t Addres;//OLED I2C address
	uint8_t OLED_Buf[8][128];//Dispay buffer.only suit less than SAA1306 128*64/8=1024 bytes
	uint8_t Delay_Time = 3;//Delay time.if the display is not normal, increase the delay time or pin speed
	uint8_t Timeout = 100;//Timeout time.if the display is not normal, increase the timeout time

	I2C_HandleTypeDef *hi2c;//I2C handle
//	SPI_HandleTypeDef *hspi;//SPI handle
	GPIO_TypeDef *OLED_RES_Port;//RES port
	uint16_t OLED_RES_Pin;//RES pin
	GPIO_TypeDef *OLED_DC_Port;//DC port
	uint16_t OLED_DC_Pin;//DC pin
	GPIO_TypeDef *OLED_CS_Port;//CS port
	uint16_t OLED_CS_Pin;//CS pin
	GPIO_TypeDef *OLED_SCL_Port;//SCL port
	uint16_t OLED_SCL_Pin;//SCL pin
	GPIO_TypeDef *OLED_SDA_Port;//SDA port
	uint16_t OLED_SDA_Pin;//SDA pin

	oled_hardware hardware;
	oled_type type;
	uint8_t DMA = 0;//DMA.1:use DMA;0:do not use DMA
	uint8_t OLED_Fresh = 0;

	uint32_t Pow(uint32_t x, uint32_t y);
	uint8_t Pnpoly(uint8_t nvert, int16_t *vertx, int16_t *verty, int16_t testx, int testy);
	uint8_t IsInAngle(int16_t x, int16_t y, int16_t StartAngle, int16_t EndAngle);
};

#endif // __COM_OLED_H