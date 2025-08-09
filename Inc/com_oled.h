#ifndef __COM_OLED_H
#define __COM_OLED_H
/*!
 * @file          com_oled.h
 * @author        Mcreeper
 * @date          2024-04-10
 * @brief         oled driver
 * @note		  use OLED_Begin() to initialize the OLED,than
 * OLED_Update() to show
 * @note		  use OLED_Begin() to initialize the OLED,than
 * OLED_Update() to show
 */
#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#ifdef __cplusplus
}
#endif

#include "com_screen_std.h"
#include "component.h"

class com_oled {
   public:
    com_oled();
#if defined(HAL_I2C_MODULE_ENABLED)
    com_oled(I2C_HandleTypeDef *hi2c);
#endif
#if defined(HAL_SPI_MODULE_ENABLED)
    com_oled(SPI_HandleTypeDef *hspi, GPIO_TypeDef *oled_res_port,
             uint16_t oled_res_pin, GPIO_TypeDef *oled_dc_port,
             uint16_t oled_dc_pin, GPIO_TypeDef *oled_cs_port,
             uint16_t oled_cs_pin);
#endif
    com_oled(GPIO_TypeDef *oled_scl_port, uint16_t oled_scl_pin,
             GPIO_TypeDef *oled_sda_port, uint16_t oled_sda_pin);
    uint8_t Set_Screen_Type(Screen_type type);
    uint8_t Begin();
    uint8_t Begin_Fast();
    void Clear();
    void Clear(uint8_t x, uint8_t y, uint8_t Width, uint8_t Height);
    uint8_t Update();
    uint8_t Update(uint8_t x, uint8_t y, uint8_t Width, uint8_t Height);

    uint8_t OLED_SetPos(uint8_t x, uint8_t y);
    uint8_t OLED_Fill(uint8_t bmp_data);
    uint8_t OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t Width, uint8_t Height,
                         uint8_t BMP[]);
    uint8_t SetPos(uint8_t x, uint8_t y);
    uint8_t Fill(uint8_t bmp_data);
    uint8_t DrawBMP(uint8_t x0, uint8_t y0, uint8_t Width, uint8_t Height,
                    uint8_t BMP[]);

    class show {
       public:
        show(com_oled &oled) : oled(oled) {}
        uint8_t Char(uint8_t x, uint8_t y, uint8_t chr, uint8_t Size);
        uint8_t String(uint8_t x, uint8_t y, const char *String, uint8_t Size);
        uint8_t Num(uint8_t x, uint8_t y, int num, uint8_t len, uint8_t size);
        uint8_t Hex(uint8_t x, uint8_t y, uint32_t num, uint8_t len,
                    uint8_t size);
        uint8_t Bin(uint8_t x, uint8_t y, uint32_t num, uint8_t len,
                    uint8_t size);
        uint8_t Float(uint8_t x, uint8_t y, double num, uint8_t size);
        uint8_t Chinese(uint8_t x, uint8_t y, char *Chinese);
        uint8_t Printf(uint8_t x, uint8_t y, uint8_t size, const char *fmt,
                       ...);
        uint8_t Printf(uint8_t x, uint8_t y, uint8_t size, uint8_t *buf,
                       uint16_t len);
    class show {
       public:
        show(com_oled &oled) : oled(oled) {}
        uint8_t Char(uint8_t x, uint8_t y, uint8_t chr, uint8_t Size);
        uint8_t String(uint8_t x, uint8_t y, const char *String, uint8_t Size);
        uint8_t Num(uint8_t x, uint8_t y, int num, uint8_t len, uint8_t size);
        uint8_t Hex(uint8_t x, uint8_t y, uint32_t num, uint8_t len,
                    uint8_t size);
        uint8_t Bin(uint8_t x, uint8_t y, uint32_t num, uint8_t len,
                    uint8_t size);
        uint8_t Float(uint8_t x, uint8_t y, double num, uint8_t size);
        uint8_t Chinese(uint8_t x, uint8_t y, char *Chinese);
        uint8_t Printf(uint8_t x, uint8_t y, uint8_t size, const char *fmt,
                       ...);
        uint8_t Printf(uint8_t x, uint8_t y, uint8_t size, uint8_t *buf,
                       uint16_t len);

        uint8_t Point(uint8_t x, uint8_t y, uint8_t cmd);
        uint8_t GetPoint(uint8_t x, uint8_t y);
        uint8_t Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2,
                     uint8_t cmd);
        uint8_t Rectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2,
                          uint8_t IsFilled, uint8_t cmd);
        uint8_t Triangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2,
                         uint8_t x3, uint8_t y3, uint8_t IsFilled, uint8_t cmd);
        uint8_t Circle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t cmd,
                       uint8_t IsFilled);
        uint8_t Ellipse(uint8_t x0, uint8_t y0, uint8_t a, uint8_t b,
                        uint8_t cmd, uint8_t IsFilled);
        uint8_t Arc(uint8_t x0, uint8_t y0, uint8_t r, int16_t StartAngle,
                    int16_t EndAngle, uint8_t IsFilled, uint8_t cmd);

       private:
        com_oled &oled;
    } Show;

   private:
    uint8_t OLED_Set_Cursor(uint8_t page, uint8_t x);
    void OLED_Reverse();
    void OLED_Reverse(uint8_t x, uint8_t y, uint8_t Width, uint8_t Height);
    uint8_t OLED_ShowImage(uint8_t x, uint8_t y, uint8_t Width, uint8_t Height,
                           const uint8_t *Image);
        uint8_t Point(uint8_t x, uint8_t y, uint8_t cmd);
        uint8_t GetPoint(uint8_t x, uint8_t y);
        uint8_t Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2,
                     uint8_t cmd);
        uint8_t Rectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2,
                          uint8_t IsFilled, uint8_t cmd);
        uint8_t Triangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2,
                         uint8_t x3, uint8_t y3, uint8_t IsFilled, uint8_t cmd);
        uint8_t Circle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t cmd,
                       uint8_t IsFilled);
        uint8_t Ellipse(uint8_t x0, uint8_t y0, uint8_t a, uint8_t b,
                        uint8_t cmd, uint8_t IsFilled);
        uint8_t Arc(uint8_t x0, uint8_t y0, uint8_t r, int16_t StartAngle,
                    int16_t EndAngle, uint8_t IsFilled, uint8_t cmd);

       private:
        com_oled &oled;
    } Show;

   private:
    uint8_t Set_Cursor(uint8_t page, uint8_t x);
    void Reverse();
    void Reverse(uint8_t x, uint8_t y, uint8_t Width, uint8_t Height);
    uint8_t ShowImage(uint8_t x, uint8_t y, uint8_t Width, uint8_t Height,
                      const uint8_t *Image);

    uint8_t OLED_WR_Byte(uint8_t data, uint8_t cmd);
    uint8_t OLED_WR_Data(uint8_t *data, uint8_t len);
    uint8_t WR_Byte(uint8_t data, uint8_t cmd);
    uint8_t WR_Data(uint8_t *data, uint8_t len);

    void OLED_I2C_Start();
    void OLED_I2C_Stop();
    void OLED_I2C_WaitAck();
    uint8_t OLED_I2C_Transmit(uint8_t *data, uint16_t len);
    void I2C_Start();
    void I2C_Stop();
    void I2C_WaitAck();
    uint8_t I2C_Transmit(uint8_t *data, uint16_t len);

    void OLED_WR_SDA(uint8_t data);
    void OLED_WR_SCL(uint8_t data);
    void WR_SDA(uint8_t data);
    void WR_SCL(uint8_t data);

    uint8_t Addres;            // OLED I2C address
    uint8_t OLED_Buf[8][128];  // Dispay buffer.only suit less than SAA1306
                               // 128*64/8=1024 bytes
    uint8_t Delay_Time = 3;    // Delay time.if the display is not normal,
                               // increase the delay time or pin speed
    uint8_t Timeout = 100;     // Timeout time.if the display is not normal,
                               // increase the timeout time
#if defined(HAL_I2C_MODULE_ENABLED)
    I2C_HandleTypeDef *hi2c;  // I2C handle
#endif
#if defined(HAL_SPI_MODULE_ENABLED)
    SPI_HandleTypeDef *hspi;  // SPI handle
#endif
    GPIO_TypeDef *OLED_RES_Port;  // RES port
    uint16_t OLED_RES_Pin;        // RES pin
    GPIO_TypeDef *OLED_DC_Port;   // DC port
    uint16_t OLED_DC_Pin;         // DC pin
    GPIO_TypeDef *OLED_CS_Port;   // CS port
    uint16_t OLED_CS_Pin;         // CS pin
    GPIO_TypeDef *OLED_SCL_Port;  // SCL port
    uint16_t OLED_SCL_Pin;        // SCL pin
    GPIO_TypeDef *OLED_SDA_Port;  // SDA port
    uint16_t OLED_SDA_Pin;        // SDA pin
    uint8_t Addres;            // OLED I2C address
    uint32_t OLED_Buf[8][36];  // Static buffer for OLED display
    uint8_t **OLED_Buf_Ptr;    // Dispay buffer.only suit less than SAA1306
                               // 128*64/8=1024 bytes
    uint8_t Delay_Time = 3;    // Delay time.if the display is not normal,
                               // increase the delay time or pin speed
    uint8_t Timeout = 100;     // Timeout time.if the display is not normal,
                               // increase the timeout time
    uint16_t start_x = 0;
    uint16_t start_y = 0;
    uint16_t end_x = 0;
    uint16_t end_y = 0;

#if defined(HAL_I2C_MODULE_ENABLED)
    I2C_HandleTypeDef *hi2c;  // I2C handle
#endif
#if defined(HAL_SPI_MODULE_ENABLED)
    SPI_HandleTypeDef *hspi;  // SPI handle
#endif
    GPIO_TypeDef *oled_res_port;  // RES port
    uint16_t oled_res_pin;        // RES pin
    GPIO_TypeDef *oled_dc_port;   // DC port
    uint16_t oled_dc_pin;         // DC pin
    GPIO_TypeDef *oled_cs_port;   // CS port
    uint16_t oled_cs_pin;         // CS pin
    GPIO_TypeDef *oled_scl_port;  // SCL port
    uint16_t oled_scl_pin;        // SCL pin
    GPIO_TypeDef *oled_sda_port;  // SDA port
    uint16_t oled_sda_pin;        // SDA pin

    oled_hardware hardware;
    oled_type type;
    uint8_t DMA = 0;  // DMA.1:use DMA;0:do not use DMA
    uint8_t OLED_Fresh = 0;
    Screen_hardware hardware;
    Screen_type type;
    uint8_t DMA = 0;  // DMA.1:use DMA;0:do not use DMA
    uint8_t OLED_Fresh = 0;

    uint32_t Pow(uint32_t x, uint32_t y);
    uint8_t Pnpoly(uint8_t nvert, int16_t *vertx, int16_t *verty, int16_t testx,
                   int testy);
    uint8_t IsInAngle(int16_t x, int16_t y, int16_t StartAngle,
                      int16_t EndAngle);
};

#endif  // __COM_OLED_H
#endif  // __COM_OLED_H