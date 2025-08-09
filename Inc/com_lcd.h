#ifndef __COM_LCD_H
#define __COM_LCD_H
/*!
 * @file          com_lcd.h
 * @author        Mcreeper
 * @date          2024-04-10
 * @brief         LCD driver
 * @note		  use LCD_Begin() to initialize the LCD,than
 * LCD_Update() to show
 */
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#ifdef __cplusplus
}
#endif

#include "com_screen_std.h"
#include "component.h"

enum lcd_color {
    TFT_COLOR_RED = 0xF800,
    TFT_COLOR_GREEN = 0x07E0,
    TFT_COLOR_BLUE = 0x001F,
    TFT_COLOR_BLACK = 0x0000,
    TFT_COLOR_WHITE = 0xFFFF,
    TFT_COLOR_YELLOW = 0xFFE0
};

class com_lcd {
   public:
    com_lcd();
#if defined(HAL_I2C_MODULE_ENABLED)
    com_lcd(I2C_HandleTypeDef *hi2c);
#endif
#if defined(HAL_SPI_MODULE_ENABLED)
    com_lcd(SPI_HandleTypeDef *hspi, GPIO_TypeDef *lcd_res_port,
            uint16_t lcd_res_pin, GPIO_TypeDef *lcd_dc_port,
            uint16_t lcd_dc_pin, GPIO_TypeDef *lcd_cs_port,
            uint16_t lcd_cs_pin);
#endif
    com_lcd(GPIO_TypeDef *lcd_scl_port, uint16_t lcd_scl_pin,
            GPIO_TypeDef *lcd_sda_port, uint16_t lcd_sda_pin);
    uint8_t Set_Screen_Type(Screen_type type);
    uint8_t Begin();
    uint8_t Begin_Fast();

    uint8_t SetPos(uint8_t x, uint8_t y);
    uint8_t Fill(uint16_t color);
    uint8_t DrawBMP(uint8_t x0, uint8_t y0, uint8_t Width, uint8_t Height,
                    uint8_t BMP[]);

    class show {
       public:
        show(com_lcd &LCD) : LCD(LCD) {}
        uint8_t Char(uint16_t x, uint16_t y, uint8_t chr, uint8_t Size,
                     uint16_t color);
        uint8_t String(uint16_t x, uint16_t y, const char *String, uint8_t Size,
                       uint16_t color);
        uint8_t Num(uint16_t x, uint16_t y, int num, uint8_t len, uint8_t size,
                    uint16_t color);
        uint8_t Hex(uint16_t x, uint16_t y, uint32_t num, uint8_t len,
                    uint8_t size, uint16_t color);
        uint8_t Bin(uint16_t x, uint16_t y, uint32_t num, uint8_t len,
                    uint8_t size, uint16_t color);
        uint8_t Float(uint16_t x, uint16_t y, double num, uint8_t size,
                      uint16_t color);
        uint8_t Chinese(uint16_t x, uint16_t y, char *Chinese, uint16_t color);
        uint8_t Printf(uint16_t x, uint16_t y, uint8_t size, uint16_t color,
                       const char *fmt, ...);
        uint8_t Printf(uint16_t x, uint16_t y, uint8_t size, uint16_t color,
                       uint8_t *buf, uint16_t len);

        uint8_t Point(uint16_t x, uint16_t y, uint16_t color);
        uint8_t Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
                     uint16_t color);
        uint8_t Rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
                          uint8_t IsFilled, uint16_t color);
        uint8_t Triangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
                         uint16_t x3, uint16_t y3, uint8_t IsFilled,
                         uint16_t color);
        uint8_t Circle(uint16_t x0, uint16_t y0, uint16_t r, uint8_t IsFilled,
                       uint16_t color);
        uint8_t Ellipse(uint16_t x0, uint16_t y0, uint16_t a, uint16_t b,
                        uint8_t IsFilled, uint16_t color);
        uint8_t Arc(uint16_t x0, uint16_t y0, uint16_t r, int16_t StartAngle,
                    int16_t EndAngle, uint8_t IsFilled, uint16_t color);

       private:
        com_lcd &LCD;
    } Show;

   private:
    uint8_t Set_Address(uint16_t _startx, uint16_t _starty, uint16_t _endx,
                        uint16_t _endy);
    uint8_t ShowImage(uint16_t x, uint16_t y, uint16_t Width, uint16_t Height,
                      uint16_t color, const uint8_t *Image);

    uint8_t WR_Byte(uint8_t data, uint8_t cmd);
    uint8_t WR_Data(uint8_t *data, uint8_t len);

    void I2C_Start();
    void I2C_Stop();
    void I2C_WaitAck();
    uint8_t I2C_Transmit(uint8_t *data, uint16_t len);

    void WR_SDA(uint8_t data);
    void WR_SCL(uint8_t data);

    uint8_t Addres;  // LCD I2C address
    // uint16_t LCD_Buf[320][320];  // Static buffer for LCD display
    uint8_t Delay_Time = 3;  // Delay time.if the display is not normal,
                             // increase the delay time or pin speed
    uint8_t Timeout = 100;   // Timeout time.if the display is not normal,
                             // increase the timeout time
    uint16_t size_x = 128;
    uint16_t size_y = 64;
    uint8_t horizontal = 0;
    uint16_t background = 0;

#if defined(HAL_I2C_MODULE_ENABLED)
    I2C_HandleTypeDef *hi2c;  // I2C handle
#endif
#if defined(HAL_SPI_MODULE_ENABLED)
    SPI_HandleTypeDef *hspi;  // SPI handle
#endif
    GPIO_TypeDef *lcd_res_port;  // RES port
    uint16_t lcd_res_pin;        // RES pin
    GPIO_TypeDef *lcd_dc_port;   // DC port
    uint16_t lcd_dc_pin;         // DC pin
    GPIO_TypeDef *lcd_cs_port;   // CS port
    uint16_t lcd_cs_pin;         // CS pin
    GPIO_TypeDef *lcd_scl_port;  // SCL port
    uint16_t lcd_scl_pin;        // SCL pin
    GPIO_TypeDef *lcd_sda_port;  // SDA port
    uint16_t lcd_sda_pin;        // SDA pin

    Screen_hardware hardware;
    Screen_type type;
    uint8_t DMA = 0;  // DMA.1:use DMA;0:do not use DMA
    uint8_t LCD_Fresh = 0;

    uint32_t Pow(uint32_t x, uint32_t y);
    uint8_t Pnpoly(uint8_t nvert, int32_t *vertx, int32_t *verty, int32_t testx,
                   int testy);
    uint8_t IsInAngle(int16_t x, int16_t y, int16_t StartAngle,
                      int16_t EndAngle);
};

#endif  // __COM_LCD_H