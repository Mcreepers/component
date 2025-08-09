#include "com_lcd.h"

#include <stdarg.h>
#include <stdio.h>

#include <cmath>
#include <cstdint>
#include <cstring>

#include "com_screen_std.h"
#include "math.h"
#include "string.h"

using namespace font_library;

#define DISPLAY_OVERTURN 0
#define DISPLAY_BOTTOM_TO_TOP 0
#define DISPLAY_RIGHT_TO_LEFT 0

com_lcd::com_lcd() : Show(*this) {
    // hardware = Hardware_IIC;
    Addres = 0x78;  // 0x78 0x7A
}
#if defined(HAL_I2C_MODULE_ENABLED)
com_lcd::com_lcd(I2C_HandleTypeDef *hi2c) : hi2c(hi2c), Show(*this) {
    hardware = Hardware_IIC;
    Addres = 0x78;  // 0x78 0x7A
}
#endif

com_lcd::com_lcd(SPI_HandleTypeDef *hspi, GPIO_TypeDef *lcd_res_port,
                 uint16_t lcd_res_pin, GPIO_TypeDef *lcd_dc_port,
                 uint16_t lcd_dc_pin, GPIO_TypeDef *lcd_cs_port,
                 uint16_t lcd_cs_pin)
    : hspi(hspi),
      lcd_res_port(lcd_res_port),
      lcd_res_pin(lcd_res_pin),
      lcd_dc_port(lcd_dc_port),
      lcd_dc_pin(lcd_dc_pin),
      lcd_cs_port(lcd_cs_port),
      lcd_cs_pin(lcd_cs_pin),
      Show(*this) {
    hardware = Hardware_SPI;
    Addres = 0x78;  // 0x78 0x7A
}

com_lcd::com_lcd(GPIO_TypeDef *lcd_scl_port, uint16_t lcd_scl_pin,
                 GPIO_TypeDef *lcd_sda_port, uint16_t lcd_sda_pin)
    : lcd_scl_port(lcd_scl_port),
      lcd_scl_pin(lcd_scl_pin),
      lcd_sda_port(lcd_sda_port),
      lcd_sda_pin(lcd_sda_pin),
      Show(*this) {
    hardware = Software_IIC;
    Addres = 0x78;  // 0x78 0x7A
}
uint8_t com_lcd::Set_Screen_Type(Screen_type type) {
    this->type = type;
    switch (type) {
        case LCD_128x64:
            this->size_x = 128;
            this->size_y = 64;
            break;
        case LCD_128x32:
            this->size_x = 128;
            this->size_y = 32;
            break;
        case LCD_96x16:
            this->size_x = 96;
            this->size_y = 16;
            break;
        case LCD_320X172:
            this->size_x = 320;
            this->size_y = 172;
            break;
    }
    return COM_OK;
}
/// @brief LCD initialization
/// @return COM_StatusTypeDef COM_OK: success, COM_ERROR: fail
uint8_t com_lcd::Begin() {
    uint8_t ret = COM_OK;
    uint8_t lcd_data = 0;

    if (hardware == Hardware_SPI) {
        HAL_GPIO_WritePin(lcd_res_port, lcd_res_pin, GPIO_PIN_RESET);
        com_delay_ms_os(100);
        HAL_GPIO_WritePin(lcd_res_port, lcd_res_pin, GPIO_PIN_SET);
    }
    // // st7789驱动
    // TFT_SCLK_Set();  // 特别注意！！
    // TFT_RST_Clr();
    // com_delay_ms_os(1000);
    // TFT_RST_Set();
    ret += WR_Byte(0x11, 1);  // Sleep Out
    com_delay_ms_os(120);
    /*
        (0,0)*********240***********->
        *
        *
        *
        280           240x280
        *
        *
        *
        ↓
    **/
    ret += WR_Byte(0x36, 1);  // 屏幕显示方向设置
    if (horizontal == 0) {
        ret += WR_Byte(0x00, 0);  // 竖屏
    } else if (horizontal == 1) {
        ret += WR_Byte(0xC0, 0);  // 竖屏
    } else if (horizontal == 2) {
        ret += WR_Byte(0x70, 0);  // 横屏
    } else {
        ret += WR_Byte(0xA0, 0);  // 横屏
    }
    ret += WR_Byte(0x3A, 1);  // 颜色数据格式RGB565 65k真彩色
    ret += WR_Byte(0x05, 0);

    //-------------ST7789V Frame rate setting-----------//
    ret += WR_Byte(0xb2, 1);  // Porch Setting
    ret += WR_Byte(0x0C, 0);
    ret += WR_Byte(0x0C, 0);
    ret += WR_Byte(0x00, 0);
    ret += WR_Byte(0x33, 0);
    ret += WR_Byte(0x33, 0);

    ret += WR_Byte(0xb7, 1);  // Gate Control
    ret += WR_Byte(0x72, 0);  // 12.2v   -10.43v
    //--------------ST7789V Power setting---------------//
    ret += WR_Byte(0xBB, 1);  // VCOM
    ret += WR_Byte(0x3D, 0);

    ret += WR_Byte(0xC0, 1);  // Power control
    ret += WR_Byte(0x2c, 0);

    ret += WR_Byte(0xC2, 1);  // VDV and VRH Command Enable
    ret += WR_Byte(0x01, 0);

    ret += WR_Byte(0xC3, 1);  // VRH Set
    ret += WR_Byte(0x19, 0);  // 4.3+( vcom+vcom offset+vdv)

    ret += WR_Byte(0xC4, 1);  // VDV Set
    ret += WR_Byte(0x20, 0);  // 0v

    ret += WR_Byte(0xC6, 1);  // Frame Rate Control in Normal Mode
    ret += WR_Byte(0X0F, 0);  // 111Hz

    ret += WR_Byte(0xd0, 1);  // Power Control 1
    ret += WR_Byte(0xa4, 0);
    ret += WR_Byte(0xa1, 0);

    // ret += WR_Byte(0xE8, 1);  // Power Control 1
    // ret += WR_Byte(0x03, 0);

    // ret += WR_Byte(0xE9, 1);  // Equalize time control
    // ret += WR_Byte(0x09, 0);
    // ret += WR_Byte(0x09, 0);
    // ret += WR_Byte(0x08, 0);
    //---------------ST7789V gamma setting-------------//
    ret += WR_Byte(0xE0, 1);  // Set Gamma
    ret += WR_Byte(0xD0, 0);
    ret += WR_Byte(0x04, 0);
    ret += WR_Byte(0x0D, 0);
    ret += WR_Byte(0x11, 0);
    ret += WR_Byte(0x13, 0);
    ret += WR_Byte(0x2B, 0);
    ret += WR_Byte(0x3F, 0);
    ret += WR_Byte(0x54, 0);
    ret += WR_Byte(0x4C, 0);
    ret += WR_Byte(0x18, 0);
    ret += WR_Byte(0x0D, 0);
    ret += WR_Byte(0x0B, 0);
    ret += WR_Byte(0x1F, 0);
    ret += WR_Byte(0x23, 0);

    ret += WR_Byte(0xE1, 1);  // Set Gamma
    ret += WR_Byte(0xD0, 0);
    ret += WR_Byte(0x04, 0);
    ret += WR_Byte(0x0C, 0);
    ret += WR_Byte(0x11, 0);
    ret += WR_Byte(0x13, 0);
    ret += WR_Byte(0x2C, 0);
    ret += WR_Byte(0x3F, 0);
    ret += WR_Byte(0x44, 0);
    ret += WR_Byte(0x51, 0);
    ret += WR_Byte(0x2F, 0);
    ret += WR_Byte(0x1F, 0);
    ret += WR_Byte(0x1F, 0);
    ret += WR_Byte(0x20, 0);
    ret += WR_Byte(0x23, 0);

    ret += WR_Byte(0x21, 1);  // 反显
    ret += WR_Byte(0x29, 1);  // 开启显示

    if (ret != COM_OK) {
        return COM_ERROR;
    }
    return ret;
}

uint8_t com_lcd::Begin_Fast() {
    uint8_t ret = COM_OK;

    if (hardware == Hardware_SPI) {
        HAL_GPIO_WritePin(lcd_res_port, lcd_res_pin, GPIO_PIN_RESET);
        com_delay_ms_os(100);
        HAL_GPIO_WritePin(lcd_res_port, lcd_res_pin, GPIO_PIN_SET);
    }

    ret += WR_Byte(0x01, 1);  // Software Reset
    com_delay_ms_os(150);     // DELAY150ms
    ret += WR_Byte(0x11, 1);  // Sleep Out
    com_delay_ms_os(120);     // DELAY120ms
    ret += WR_Byte(0x3A, 1);  // 颜色数据格式RGB565 65k真彩色
    ret += WR_Byte(0x05, 0);  // ✅ 0x05 = 16位/像素 (RGB565)
    ret += WR_Byte(0x36, 1);  // 屏幕显示方向设置
    ret += WR_Byte(0x00, 0);  // 竖屏
    ret += WR_Byte(0x21, 1);  // 反显
    ret += WR_Byte(0x13, 1);  // Normal Display Mode On
    ret += WR_Byte(0x29, 1);  // 开启显示

    if (ret != COM_OK) {
        return COM_ERROR;
    }
    return ret;
}

/// @brief
/// @param x
/// @param y
/// @return
uint8_t com_lcd::SetPos(uint8_t x, uint8_t y) {
    uint8_t ret = COM_OK;
    return ret;
}

/**
 * @brief
 * @param
 * @param
 */
uint8_t com_lcd::Fill(uint16_t color) {
    uint8_t ret = COM_OK;
    ret += Set_Address(0, 0, size_x - 1, size_y - 1);

    uint8_t color_data[2] = {(uint8_t)(color >> 8), (uint8_t)(color & 0xFF)};

    for (uint32_t i = 0; i < (uint32_t)size_x * size_y; i++) {
        ret += WR_Data(color_data, 2);
    }
    if (ret != COM_OK) {
        return COM_ERROR;
    }
    return ret;
}

/**
 * @brief
 * @param
 * @param
 */
// uint8_t com_lcd::show::BMP(uint8_t x0, uint8_t y0, uint8_t Width, uint8_t
// Height, uint8_t BMP[])
//{
//
// }

/**
 * @brief display a point
 * @param x  x coordinate
 * @param y  y coordinate
 * @param color  color of the point
 * @return COM_StatusTypeDef COM_OK: success, COM_ERROR: fail
 */
uint8_t com_lcd::show::Point(uint16_t x, uint16_t y, uint16_t color) {
    uint8_t ret = COM_OK;
    if (x > this->LCD.size_x || y > this->LCD.size_y) {
        return COM_ERROR;
    }
    ret += this->LCD.Set_Address(x, y, x, y);

    uint8_t color_data[2] = {(uint8_t)(color >> 8), (uint8_t)(color & 0xFF)};
    ret += this->LCD.WR_Data(color_data, 2);

    if (ret != COM_OK) {
        return COM_ERROR;
    }
    return ret;
}

/**
 * @brief display a line
 * @param x1  start x coordinate
 * @param y1  start y coordinate
 * @param x2  end x coordinate
 * @param y2  end y coordinate
 * @param color  color of the line
 * @return COM_StatusTypeDef COM_OK: success, COM_ERROR: fail
 * @todo change not tested
 */
uint8_t com_lcd::show::Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
                            uint16_t color) {
    uint8_t ret = COM_OK;
    int16_t x, y, dx, dy, d, incrE, incrNE, temp;
    int16_t x0 = x1, y0 = y1, X = x2, Y = y2;
    uint8_t yflag = 0, xyflag = 0;

    if (y0 == Y) {
        for (x = x0; x <= X; x++) {
            ret += Point(x, y0, color);
        }
    } else if (x0 == X) {
        for (y = y0; y <= Y; y++) {
            ret += Point(x0, y, color);
        }
    } else {
        if (x0 > X) {
            temp = x0;
            x0 = X;
            X = temp;
            temp = y0;
            y0 = Y;
            Y = temp;
        }

        if (y0 > Y) {
            y0 = -y0;
            Y = -Y;

            yflag = 1;
        }

        if (Y - y0 > X - x0) {
            xyflag = 1;
            temp = x0;
            x0 = y0;
            y0 = temp;
            temp = X;
            X = Y;
            Y = temp;
        }

        dx = X - x0;
        dy = Y - y0;
        incrE = 2 * dy;
        incrNE = 2 * (dy - dx);
        d = 2 * dy - dx;
        x = x0;
        y = y0;

        if (yflag && xyflag) {
            ret += Point(y, -x, color);
        } else if (yflag) {
            ret += Point(x, -y, color);
        } else if (xyflag) {
            ret += Point(y, x, color);
        } else {
            ret += Point(x, y, color);
        }

        while (x < X) {
            x++;
            if (d < 0) {
                d += incrE;
            } else {
                y++;
                d += incrNE;
            }

            if (yflag && xyflag) {
                ret += Point(y, -x, color);
            } else if (yflag) {
                ret += Point(x, -y, color);
            } else if (xyflag) {
                ret += Point(y, x, color);
            } else {
                ret += Point(x, y, color);
            }
        }
    }
    if (ret != COM_OK) {
        return COM_ERROR;
    }
    return ret;
}

/**
 * @brief display a rectangle
 * @param x1  start x coordinate
 * @param y1  start y coordinate
 * @param x2  end x coordinate
 * @param y2  end y coordinate
 * @param IsFilled  0: no fill, 1: fill
 * @param color  color of the rectangle
 * @return COM_StatusTypeDef COM_OK: success, COM_ERROR: fail
 */
uint8_t com_lcd::show::Rectangle(uint16_t x1, uint16_t y1, uint16_t x2,
                                 uint16_t y2, uint8_t IsFilled,
                                 uint16_t color) {
    uint8_t ret = COM_OK;
    if (IsFilled) {
        for (uint16_t i = y1; i <= y2; i++) {
            for (uint16_t j = x1; j <= x2; j++) {
                ret += Point(j, i, color);
            }
        }
    } else {
        ret += Line(x1, y1, x2, y1, color);
        ret += Line(x1, y1, x1, y2, color);
        ret += Line(x1, y2, x2, y2, color);
        ret += Line(x2, y1, x2, y2, color);
    }
    if (ret != COM_OK) {
        return COM_ERROR;
    }
    return ret;
}

/**
 * @brief display a triangle
 * @param x1  x1 coordinate
 * @param y1  y1 coordinate
 * @param x2  x2 coordinate
 * @param y2  y2 coordinate
 * @param x3  x3 coordinate
 * @param y3  y3 coordinate
 * @param IsFilled  0: no fill, 1: fill
 * @param color  color of the triangle
 * @return COM_StatusTypeDef COM_OK: success, COM_ERROR: fail
 */
uint8_t com_lcd::show::Triangle(uint16_t x1, uint16_t y1, uint16_t x2,
                                uint16_t y2, uint16_t x3, uint16_t y3,
                                uint8_t IsFilled, uint16_t color) {
    uint8_t ret = COM_OK;
    int32_t vx[] = {x1, x2, x3};
    int32_t vy[] = {y1, y2, y3};

    if (IsFilled) {
        uint16_t minx = x1, miny = y1, maxx = x1, maxy = y1;
        if (x2 < minx) {
            minx = x2;
        }
        if (x3 < minx) {
            minx = x3;
        }
        if (y2 < miny) {
            miny = y2;
        }
        if (y3 < miny) {
            miny = y3;
        }
        if (x2 > maxx) {
            maxx = x2;
        }
        if (x3 > maxx) {
            maxx = x3;
        }
        if (y2 > maxy) {
            maxy = y2;
        }
        if (y3 > maxy) {
            maxy = y3;
        }

        for (uint16_t i = miny; i <= maxy; i++) {
            for (uint16_t j = minx; j <= maxx; j++) {
                if (this->LCD.Pnpoly(3, vx, vy, j, i)) {
                    ret += Point(j, i, color);
                }
            }
        }
    } else {
        ret += Line(x1, y1, x2, y2, color);
        ret += Line(x1, y1, x3, y3, color);
        ret += Line(x2, y2, x3, y3, color);
    }

    if (ret != COM_OK) {
        return COM_ERROR;
    }
    return ret;
}

/**
 * @brief display a circle
 * @param x0  x coordinate
 * @param y0  y coordinate
 * @param r  radius
 * @param IsFilled  0: no fill, 1: fill
 * @param color  color of the circle
 * @return COM_StatusTypeDef COM_OK: success, COM_ERROR: fail
 */
uint8_t com_lcd::show::Circle(uint16_t x0, uint16_t y0, uint16_t r,
                              uint8_t IsFilled, uint16_t color) {
    uint8_t ret = COM_OK;
    int16_t x = 0, y = r, d = 1 - r;

    ret += Point(x0 + x, y0 + y, color);
    ret += Point(x0 + x, y0 - y, color);
    ret += Point(x0 - x, y0 + y, color);
    ret += Point(x0 - x, y0 - y, color);

    if (IsFilled) {
        for (uint16_t i = -y; i <= y; i++) {
            ret += Point(x0, y0 + i, color);
        }
    }
    while (x < y) {
        x++;
        if (d < 0) {
            d += 2 * x + 3;
        } else {
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
        ret += Point(x0 + x, y0 + y, color);
        ret += Point(x0 - x, y0 + y, color);
        ret += Point(x0 + x, y0 - y, color);
        ret += Point(x0 - x, y0 - y, color);
        ret += Point(x0 + y, y0 + x, color);
        ret += Point(x0 - y, y0 + x, color);
        ret += Point(x0 + y, y0 - x, color);
        ret += Point(x0 - y, y0 - x, color);
        if (IsFilled) {
            for (uint16_t i = -y; i <= y; i++) {
                ret += Point(x0 + x, y0 + i, color);
                ret += Point(x0 - x, y0 + i, color);
            }
            for (uint16_t i = -x; i <= x; i++) {
                ret += Point(x0 + y, y0 + i, color);
                ret += Point(x0 - y, y0 + i, color);
            }
        }
    }
    if (ret != COM_OK) {
        return COM_ERROR;
    }
    return ret;
}

/**
 * @brief display a ellipse
 * @param x0  x coordinate
 * @param y0  y coordinate
 * @param a  long axis
 * @param b  short axis
 * @param IsFilled  0: no fill, 1: fill
 * @param color  color of the ellipse
 * @return COM_StatusTypeDef COM_OK: success, COM_ERROR: fail
 * @todo change not tested
 */
uint8_t com_lcd::show::Ellipse(uint16_t x0, uint16_t y0, uint16_t a, uint16_t b,
                               uint8_t IsFilled, uint16_t color) {
    uint8_t ret = COM_OK;
    int16_t x = 0, y = b, d = 2 * b * b + a * a * (1 - 2 * b);
    float d1, d2;

    ret += Point(x0 + x, y0 + y, color);
    ret += Point(x0 + x, y0 - y, color);
    ret += Point(x0 - x, y0 + y, color);
    ret += Point(x0 - x, y0 - y, color);

    if (IsFilled) {
        for (uint16_t i = -y; i <= y; i++) {
            ret += Point(x0, y0 + i, color);
        }
    }

    Point(x0 + x, y0 + y, color);
    Point(x0 - x, y0 + y, color);
    Point(x0 + x, y0 - y, color);
    Point(x0 - x, y0 - y, color);

    while (b * b * (x + 1) < a * a * (y - 0.5)) {
        if (d1 <= 0) {
            d1 += b * b * (2 * x + 3);
        } else {
            d1 += b * b * (2 * x + 3) + a * a * (-2 * y + 2);
            y--;
        }
        x++;

        if (IsFilled) {
            for (uint16_t i = -y; i < y; i++) {
                ret += Point(x0 + x, y0 + i, color);
                ret += Point(x0 - x, y0 + i, color);
            }
        }

        ret += Point(x0 + x, y0 + y, color);
        ret += Point(x0 - x, y0 - y, color);
        ret += Point(x0 - x, y0 + y, color);
        ret += Point(x0 + x, y0 - y, color);
    }

    /*画椭圆两侧部分*/
    d2 = b * b * (x + 0.5) * (x + 0.5) + a * a * (y - 1) * (y - 1) -
         a * a * b * b;

    while (y >= 0) {
        y--;
        if (d2 < 0) {
            d2 += 2 * b * b * (2 * x + 3);
            x++;
        } else {
            d2 += 2 * b * b * (2 * x + 3) + a * a * (2 - 2 * y);
        }
        ret += Point(x0 + x, y0 + y, color);
        ret += Point(x0 - x, y0 + y, color);
        ret += Point(x0 + x, y0 - y, color);
        ret += Point(x0 - x, y0 - y, color);
        if (IsFilled) {
            for (uint16_t i = -y; i <= y; i++) {
                ret += Point(x0 + x, y0 + i, color);
                ret += Point(x0 - x, y0 + i, color);
            }
        }
    }
    if (ret != COM_OK) {
        return COM_ERROR;
    }
    return ret;
}

/**
 * @brief display a arc
 * @param x0  x coordinate
 * @param y0  y coordinate
 * @param r  radius
 * @param StartAngle  start angle
 * @param EndAngle  end angle
 * @param color  color of the arc
 * @return COM_StatusTypeDef COM_OK: success, COM_ERROR: fail
 */
uint8_t com_lcd::show::Arc(uint16_t x0, uint16_t y0, uint16_t r,
                           int16_t StartAngle, int16_t EndAngle,
                           uint8_t IsFilled, uint16_t color) {
    uint8_t ret = COM_OK, Status = 0;
    int16_t x = 0, y = r, d = 1 - r;
    if (this->LCD.IsInAngle(x, y, StartAngle, EndAngle)) {
        ret += Point(x0 + x, y0 + y, color);
    }
    if (this->LCD.IsInAngle(-x, -y, StartAngle, EndAngle)) {
        ret += Point(x0 - x, y0 - y, color);
    }
    if (this->LCD.IsInAngle(y, x, StartAngle, EndAngle)) {
        ret += Point(x0 + y, y0 + x, color);
    }
    if (this->LCD.IsInAngle(-y, -x, StartAngle, EndAngle)) {
        ret += Point(x0 - y, y0 - x, color);
    }

    if (IsFilled) {
        for (int16_t i = -y; i < y; i++) {
            if (this->LCD.IsInAngle(0, i, StartAngle, EndAngle)) {
                ret += Point(x0, y0 + i, color);
            }
        }
    }

    while (x < y) {
        x++;
        if (d < 0) {
            d += 2 * x + 1;
        } else {
            y--;
            d += 2 * (x - y) + 1;
        }

        Point(x0 + x, y0 + y, this->LCD.IsInAngle(x, y, StartAngle, EndAngle));
        Point(x0 + y, y0 + x, this->LCD.IsInAngle(y, x, StartAngle, EndAngle));
        Point(x0 - x, y0 - y,
              this->LCD.IsInAngle(-x, -y, StartAngle, EndAngle));
        Point(x0 - y, y0 - x,
              this->LCD.IsInAngle(-y, -x, StartAngle, EndAngle));
        Point(x0 + x, y0 - y, this->LCD.IsInAngle(x, -y, StartAngle, EndAngle));
        Point(x0 + y, y0 - x, this->LCD.IsInAngle(y, -x, StartAngle, EndAngle));
        Point(x0 - x, y0 + y, this->LCD.IsInAngle(-x, y, StartAngle, EndAngle));
        Point(x0 - y, y0 + x, this->LCD.IsInAngle(-y, x, StartAngle, EndAngle));

        if (IsFilled) {
            for (int16_t i = -y; i < y; i++) {
                if (this->LCD.IsInAngle(x, i, StartAngle, EndAngle)) {
                    ret += Point(x0 + x, y0 + i, color);
                }
                if (this->LCD.IsInAngle(-x, i, StartAngle, EndAngle)) {
                    ret += Point(x0 - x, y0 + i, color);
                }
            }
            for (int16_t i = -x; i < x; i++) {
                if (this->LCD.IsInAngle(-y, i, StartAngle, EndAngle)) {
                    ret += Point(x0 - y, y0 + i, color);
                }
                if (this->LCD.IsInAngle(y, i, StartAngle, EndAngle)) {
                    ret += Point(x0 + y, y0 - y, color);
                }
            }
        }
    }

    if (ret != COM_OK) {
        return COM_ERROR;
    }
    return ret;
}

/**
 * @brief display a char
 * @param x: x coordinate
 * @param y: y coordinate
 * @param chr: char
 * @param Size: font size.use Font_Size
 * @return COM_StatusTypeDef COM_OK: success, COM_ERROR: fail
 */
uint8_t com_lcd::show::Char(uint16_t x, uint16_t y, uint8_t chr, uint8_t Size,
                            uint16_t color) {
    switch (Size) {
        case Font_6X8:
            this->LCD.ShowImage(x, y, 6, 8, color,
                                (uint8_t *)&asc2_0608[chr - ' ']);
            break;
        case Font_8X16:
            this->LCD.ShowImage(x, y, 8, 16, color,
                                (uint8_t *)&asc2_0816[chr - ' ']);
            break;
        case Font_12X12:
            this->LCD.ShowImage(x, y, 12, 12, color,
                                (uint8_t *)&asc2_1212[chr - ' ']);
            break;
        case Font_16X16:
            this->LCD.ShowImage(x, y, 16, 16, color,
                                (uint8_t *)&asc2_1616[chr - ' ']);
            break;
        case Font_24X24:
            this->LCD.ShowImage(x, y, 24, 24, color,
                                (uint8_t *)&asc2_2424[chr - ' ']);
            break;
        case Font_CHINESE:
            this->LCD.ShowImage(x, y, 16, 16, color,
                                (uint8_t *)&Chinese1616[chr]);
        default:
            return COM_ERROR;
            break;
    }
    return COM_OK;
}

/**
 * @brief display a string
 * @param x: x coordinate
 * @param y: y coordinate
 * @param String: string
 * @param Size: font size
 */
uint8_t com_lcd::show::String(uint16_t x, uint16_t y, const char *String,
                              uint8_t Size, uint16_t color) {
    uint8_t ret = COM_OK;
    for (uint16_t i = 0; String[i] != '\0'; i++)  // traverse
    {
        ret += Char(x + i * Size, y, String[i], Size, color);
    }
    if (ret != COM_OK) {
        return COM_ERROR;
    }
    return ret;
}

/**
 * @brief display a number
 * @param x: x coordinate
 * @param y: y coordinate
 * @param num: number.num range is -2147483648-2147483647
 * @param len: number length
 * @param size: font size.use Font_Size
 */
uint8_t com_lcd::show::Num(uint16_t x, uint16_t y, int num, uint8_t len,
                           uint8_t size, uint16_t color) {
    uint8_t status = 0, ret = COM_OK;
    if (num < 0) {
        ret += Char(x, y, '-', size, color);
        num = -num;
        status = 1;
    }
    for (uint16_t i = status; i < len + status; i++) {
        ret +=
            Char(x + size * i, y,
                 num / this->LCD.Pow(10, len - i - 1) % 10 + '0', size, color);
    }
    if (ret != COM_OK) {
        return COM_ERROR;
    }
    return ret;
}

/**
 * @brief display a hex number
 * @param x  x coordinate
 * @param y  y coordinate
 * @param num  Hex number
 * @param len  number length
 * @param size  font size.use Font_Size
 * @return COM_StatusTypeDef COM_OK: success, COM_ERROR: fail
 */
uint8_t com_lcd::show::Hex(uint16_t x, uint16_t y, uint32_t num, uint8_t len,
                           uint8_t size, uint16_t color) {
    uint8_t ret = COM_OK, tempnum;
    for (uint16_t i = 0; i < len; i++) {
        tempnum = num / this->LCD.Pow(16, len - i - 1) % 16;
        if (tempnum < 10) {
            ret += Char(x + size * i, y, tempnum + '0', size, color);
        } else {
            ret += Char(x + size * i, y, tempnum - 10 + 'A', size, color);
        }
    }
    if (ret != COM_OK) {
        return COM_ERROR;
    }
    return ret;
}

/**
 * @brief display a binary number
 * @param x  x coordinate
 * @param y  y coordinate
 * @param num  Binary number
 * @param len  number length
 * @param size  font size.use Font_Size
 * @return COM_StatusTypeDef COM_OK: success, COM_ERROR: fail
 */
uint8_t com_lcd::show::Bin(uint16_t x, uint16_t y, uint32_t num, uint8_t len,
                           uint8_t size, uint16_t color) {
    uint8_t ret = COM_OK;
    for (uint16_t i = 0; i < len; i++) {
        ret += Char(x + size * i, y,
                    num / this->LCD.Pow(2, len - i - 1) % 2 + '0', size, color);
    }
    return ret;
}

/**
 * @brief display a float character
 * @param x  x coordinate
 * @param y  y coordinate
 * @param num  float number
 * @param size  font size.use Font_Size
 * @return COM_StatusTypeDef COM_OK: success, COM_ERROR: fail
 */
uint8_t com_lcd::show::Float(uint16_t x, uint16_t y, double num, uint8_t size,
                             uint16_t color) {
    uint8_t ret = COM_OK;
    uint8_t status = 0;
    if (num < 0) {
        ret += Char(x, y, '-', size, color);
        num = -num;
        status = 1;
    }
    ret += Num(x + size * status, y, (int)num, 1, size, color);
    ret += Char(x + size * (status + 1), y, '.', size, color);
    ret += Num(x + size * (status + 2), y, (int)((num - (int)num) * 1000), 3,
               size, color);
    if (ret != COM_OK) {
        return COM_ERROR;
    }
    return ret;
}

/**
 * @brief display a Chinese character
 * @param x  x coordinate
 * @param y  y coordinate
 * @param Chinese  Chinese string
 * @return COM_StatusTypeDef COM_OK: success, COM_ERROR: fail
 * @todo search the Chinese character in the Chinese1616 array,from
 * https://gitee.com/zeruns/STM32-HAL-LCD-I2C
 */
uint8_t com_lcd::show::Chinese(uint16_t x, uint16_t y, char *Chinese,
                               uint16_t color) {
    uint8_t ret = COM_OK, pChinese = 0, pIndex;
    char ChineseIndex[CHN_CHAR_WIDTH + 1] = {0};
    for (uint16_t i = 0; i < CHN_CHAR_WIDTH; i++) {
        ChineseIndex[pChinese] = Chinese[i];
        pChinese++;
        if (pChinese >= CHN_CHAR_WIDTH) {
            ChineseIndex[pChinese + 1] = '\0';
            pIndex = 0;
            // for(pIndex = 0;strcmp(Chinese1616[pIndex], ChineseIndex) != 0 &&
            // pIndex < 1300;pIndex++) {}
            this->LCD.ShowImage(x + ((i + 1) / CHN_CHAR_WIDTH - 1) * 16, y, 16,
                                16, color, (uint8_t *)&Chinese1616[pIndex]);
        }
    }

    return ret;
}

/**
 * @brief display a string
 * @param x  x coordinate
 * @param y  y coordinate
 * @param size  font size.use Font_Size
 * @param fmt  format
 * @return COM_StatusTypeDef COM_OK: success, COM_ERROR: fail
 */
uint8_t com_lcd::show::Printf(uint16_t x, uint16_t y, uint8_t size,
                              uint16_t color, const char *fmt, ...) {
    uint8_t ret = COM_OK;
    va_list ap;
    va_start(ap, fmt);
    char string[64] = {0};
    vsprintf(string, fmt, ap);
    va_end(ap);
    ret += String(x, y, string, size, color);
    if (ret != COM_OK) {
        return COM_ERROR;
    }
    return ret;
}

/**
 * @brief display a string
 * @param x  x coordinate
 * @param y  y coordinate
 * @param size  font size.use Font_Size
 * @param buf  string
 * @param len  string length
 * @return COM_StatusTypeDef COM_OK: success, COM_ERROR: fail
 */
uint8_t com_lcd::show::Printf(uint16_t x, uint16_t y, uint8_t size,
                              uint16_t color, uint8_t *buf, uint16_t len) {
    uint8_t ret = COM_OK;
    char string[64] = {0};
    memcpy(string, buf, len);
    ret += String(x, y, string, size, color);
    if (ret != COM_OK) {
        return COM_ERROR;
    }
    return ret;
}

/**
 * @brief LCD set address
 * @param _startx: start x coordinate
 * @param _starty: start y coordinate
 * @param _endx: end x coordinate
 * @param _endy: end y coordinate
 * @return COM_StatusTypeDef COM_OK: success, COM_ERROR: fail
 * @note This function sets the address of the LCD to display data.
 */
uint8_t com_lcd::Set_Address(uint16_t _startx, uint16_t _starty, uint16_t _endx,
                             uint16_t _endy) {
    uint8_t ret = COM_OK;
    _startx = _startx > size_x ? size_x : _startx;
    _starty = _starty > size_y ? size_y : _starty;
    _endx = _endx > size_x ? size_x : _endx;
    _endy = _endy > size_y ? size_y : _endy;

    ret += WR_Byte(0x2a, 1);  // set extended instruction
    if (horizontal == 0) {
        ret += WR_Byte(_startx >> 8, 0);
        ret += WR_Byte(_startx, 0);
        ret += WR_Byte(_endx >> 8, 0);
        ret += WR_Byte(_endx, 0);
        ret += WR_Byte(0x2b, 1);  // 行地址设置
        ret += WR_Byte(_starty >> 8, 0);
        ret += WR_Byte(_starty, 0);
        ret += WR_Byte(_endy >> 8, 0);
        ret += WR_Byte(_endy, 0);
    } else if (horizontal == 1) {
        ret += WR_Byte(_startx >> 8, 0);
        ret += WR_Byte(_startx, 0);
        ret += WR_Byte(_endx >> 8, 0);
        ret += WR_Byte(_endx, 0);
        ret += WR_Byte(0x2b, 1);  // 行地址设置
        ret += WR_Byte((_starty + 80) >> 8, 0);
        ret += WR_Byte((_starty + 80), 0);
        ret += WR_Byte((_endy + 80) >> 8, 0);
        ret += WR_Byte((_endy + 80), 0);
    } else if (horizontal == 2) {
        ret += WR_Byte(_startx >> 8, 0);
        ret += WR_Byte(_startx, 0);
        ret += WR_Byte(_endx >> 8, 0);
        ret += WR_Byte(_endx, 0);
        ret += WR_Byte(0x2b, 1);  // 行地址设置
        ret += WR_Byte(_starty >> 8, 0);
        ret += WR_Byte(_starty, 0);
        ret += WR_Byte(_endy >> 8, 0);
        ret += WR_Byte(_endy, 0);
    } else {
        ret += WR_Byte((_startx + 80) >> 8, 0);
        ret += WR_Byte((_startx + 80), 0);
        ret += WR_Byte((_endx + 80) >> 8, 0);
        ret += WR_Byte((_endx + 80), 0);
        ret += WR_Byte(0x2b, 1);  // 行地址设置
        ret += WR_Byte(_starty >> 8, 0);
        ret += WR_Byte(_starty, 0);
        ret += WR_Byte(_endy >> 8, 0);
        ret += WR_Byte(_endy, 0);
    }
    ret += WR_Byte(0x2c, 1);  // set extended instruction

    if (ret != COM_OK) {
        return COM_ERROR;
    }
    return ret;
}

uint8_t com_lcd::ShowImage(uint16_t x, uint16_t y, uint16_t Width,
                           uint16_t Height, uint16_t color,
                           const uint8_t *Image) {
    if (x + Width > size_x || y + Height > size_y) {
        return COM_ERROR;
    }
    uint8_t ret = COM_OK;
    ret += Set_Address(x, y, x + Width - 1, y + Height - 1);

    uint8_t byte_width = (Width + 7) / 8;
    uint8_t color_data[2] = {(uint8_t)(color >> 8), (uint8_t)(color & 0xFF)};
    uint8_t bg_data[2] = {(uint8_t)(background >> 8),
                          (uint8_t)(background & 0xFF)};

    for (uint16_t row = 0; row < Height; row++) {
        for (uint16_t col = 0; col < Width; col++) {
            uint16_t byte_index = row * byte_width + col / 8;
            uint8_t bit_index = 7 - (col % 8);

            if (Image[byte_index] & (1 << bit_index)) {
                ret += WR_Data(color_data, 2);
            } else {
                ret += WR_Data(bg_data, 2);
            }
        }
    }
    LCD_Fresh = 1;
    return COM_OK;
}

/**
 * @brief write data or command
 * @param data: data or command
 * @param cmd: 0 write data, 1 write command
 */
uint8_t com_lcd::WR_Byte(uint8_t data, uint8_t cmd) {
    uint8_t ret = COM_OK;
    if (hardware == Hardware_IIC) {
#if defined(HAL_I2C_MODULE_ENABLED)
        if (cmd == 0) {
            uint8_t TxData[2] = {0x40, data};
            ret = HAL_I2C_Master_Transmit(hi2c, Addres, TxData, 2, Timeout);
        } else {
            uint8_t TxData[2] = {0x00, data};
            ret = HAL_I2C_Master_Transmit(hi2c, Addres, TxData, 2, Timeout);
        }
#endif
    } else if (hardware == Software_IIC) {
        if (cmd == 0) {
            for (uint16_t i = 0; i < 8; i++) {
                WR_SDA((data & 0x80) >> i);
                I2C_WaitAck();
            }
            I2C_WaitAck();
        } else {
            I2C_Start();
            WR_SDA(Addres);
            WR_SDA(0x00);
            WR_SDA(data);
            I2C_Stop();
        }
    } else if (hardware == Hardware_SPI) {
#if defined(HAL_SPI_MODULE_ENABLED)
        HAL_GPIO_WritePin(lcd_cs_port, lcd_cs_pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(lcd_dc_port, lcd_dc_pin,
                          cmd ? GPIO_PIN_RESET : GPIO_PIN_SET);
        ret = HAL_SPI_Transmit(hspi, &data, 1, Timeout);
        HAL_GPIO_WritePin(lcd_cs_port, lcd_cs_pin, GPIO_PIN_SET);
#endif
    }
    return ret;
}

/**
 * @brief  write datas
 * @param  data: data start address
 * @param  len: data length
 */
uint8_t com_lcd::WR_Data(uint8_t *data, uint8_t len) {
    uint8_t ret = COM_OK;
    if (hardware == Hardware_IIC) {
#if defined(HAL_I2C_MODULE_ENABLED)
        uint8_t TxData[len + 1];
        TxData[0] = 0x40;
        memcpy(TxData + 1, data, len);
        if (DMA == 0) {
            ret =
                HAL_I2C_Master_Transmit(hi2c, Addres, TxData, len + 1, Timeout);
        } else {
            ret = I2C_Transmit(TxData, len + 1);
            // HAL_I2C_Master_Transmit_DMA(hi2c, Addres, TxData, len + 1);
        }
#endif
    } else if (hardware == Software_IIC) {
        I2C_Start();
        WR_SDA(Addres);
        WR_SDA(0x40);
        for (uint16_t i = 0; i < len; i++) {
            WR_SDA(data[i]);
        }
        I2C_Stop();
    } else if (hardware == Hardware_SPI) {
#if defined(HAL_SPI_MODULE_ENABLED)
        HAL_GPIO_WritePin(lcd_cs_port, lcd_cs_pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(lcd_dc_port, lcd_dc_pin, GPIO_PIN_SET);
        if (DMA == 0) {
            HAL_SPI_Transmit(hspi, data, len, Timeout);
        } else {
            HAL_SPI_Transmit_DMA(hspi, data, len);
        }
        HAL_GPIO_WritePin(lcd_cs_port, lcd_cs_pin, GPIO_PIN_SET);
#endif
    }
    return ret;
}

// start a IIC communication
void com_lcd::I2C_Start() {
    WR_SDA(1);
    WR_SCL(1);
    WR_SDA(0);
    WR_SCL(0);
}

// stop a IIC communication
void com_lcd::I2C_Stop() {
    WR_SDA(0);
    WR_SCL(1);
    WR_SDA(1);
}

// wait for a ack
void com_lcd::I2C_WaitAck() {
    WR_SCL(1);
    WR_SCL(0);
}

#if defined(HAL_I2C_MODULE_ENABLED)
uint8_t com_lcd::I2C_Transmit(uint8_t *data, uint16_t len) {
    while (HAL_I2C_GetState(hi2c) != HAL_I2C_STATE_READY) {
    }
    while (HAL_I2C_Master_Transmit_DMA(hi2c, Addres, data, len) != HAL_OK) {
        if (HAL_I2C_GetError(hi2c) != HAL_I2C_ERROR_AF) {
            return COM_ERROR;
        }
    }
    return COM_OK;
}
#endif
// set SDA
void com_lcd::WR_SDA(uint8_t data) {
    HAL_GPIO_WritePin(lcd_sda_port, lcd_sda_pin, (GPIO_PinState)data);
    for (volatile uint16_t i = 0; i < Delay_Time; i++) {
    }
}

// set SCL
void com_lcd::WR_SCL(uint8_t data) {
    HAL_GPIO_WritePin(lcd_scl_port, lcd_scl_pin, (GPIO_PinState)data);
    for (volatile uint16_t i = 0; i < Delay_Time; i++) {
    }
}

/**
 * @brief pow function
 * @param x: base
 * @param y: exponent
 * @return result
 */
uint32_t com_lcd::Pow(uint32_t x, uint32_t y) {
    uint32_t result = 1;
    while (y--) {
        result *= x;
    }
    return result;
}

/**
 * @brief judge if a point is in designated polygon
 * @param nvert: vertex number
 * @param vertx: x coordinate of vertex
 * @param verty: y coordinate of vertex
 * @param testx: x coordinate of test point
 * @param testy: y coordinate of test point
 * @return 1: in polygon, 0: not in polygon
 */
uint8_t com_lcd::Pnpoly(uint8_t nvert, int32_t *vertx, int32_t *verty,
                        int32_t testx, int testy) {
    uint32_t i, j, c = 0;
    for (i = 0, j = nvert - 1; i < nvert; j = i++) {
        if (((verty[i] > testy) != (verty[j] > testy)) &&
            (testx < (vertx[j] - vertx[i]) * (testy - verty[i]) /
                             (verty[j] - verty[i]) +
                         vertx[i])) {
            c = !c;
        }
    }
    return c;
}

/**
 * @brief judge if a point is in designated angle
 * @param x: x coordinate
 * @param y: y coordinate
 * @param StartAngle: start angle
 * @param EndAngle: end angle
 * @return 1: in angle, 0: not in angle
 */
uint8_t com_lcd::IsInAngle(int16_t x, int16_t y, int16_t StartAngle,
                           int16_t EndAngle) {
    int16_t Angle = atan2(y, x) / 3.1415926 * 180;

    if (StartAngle < EndAngle) {
        if (Angle >= StartAngle && Angle <= EndAngle) {
            return 1;
        }
    } else {
        if (Angle >= StartAngle || Angle <= EndAngle) {
            return 1;
        }
    }
    return 0;
}
