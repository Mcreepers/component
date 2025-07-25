#include "com_oled.h"

#include <stdarg.h>
#include <stdio.h>

#include "com_oled_std.h"
#include "math.h"
#include "string.h"

com_oled::com_oled() : Show(*this) {
    // hardware = Hardware_IIC;
    Addres = 0x78;  // 0x78 0x7A
}
#if defined(HAL_I2C_MODULE_ENABLED)
com_oled::com_oled(I2C_HandleTypeDef *hi2c) : hi2c(hi2c), Show(*this) {
    hardware = Hardware_IIC;
    Addres = 0x78;  // 0x78 0x7A
}
#endif

com_oled::com_oled(SPI_HandleTypeDef *hspi, GPIO_TypeDef *OLED_RES_Port,
                   uint16_t OLED_RES_Pin, GPIO_TypeDef *OLED_DC_Port,
                   uint16_t OLED_DC_Pin, GPIO_TypeDef *OLED_CS_Port,
                   uint16_t OLED_CS_Pin)
    : hspi(hspi),
      OLED_RES_Port(OLED_RES_Port),
      OLED_RES_Pin(OLED_RES_Pin),
      OLED_DC_Port(OLED_DC_Port),
      OLED_DC_Pin(OLED_DC_Pin),
      OLED_CS_Port(OLED_CS_Port),
      OLED_CS_Pin(OLED_CS_Pin),
      Show(*this) {
    hardware = Hardware_SPI;
    Addres = 0x78;  // 0x78 0x7A
}

com_oled::com_oled(GPIO_TypeDef *OLED_SCL_Port, uint16_t OLED_SCL_Pin,
                   GPIO_TypeDef *OLED_SDA_Port, uint16_t OLED_SDA_Pin)
    : OLED_SCL_Port(OLED_SCL_Port),
      OLED_SCL_Pin(OLED_SCL_Pin),
      OLED_SDA_Port(OLED_SDA_Port),
      OLED_SDA_Pin(OLED_SDA_Pin),
      Show(*this) {
    hardware = Software_IIC;
    Addres = 0x78;  // 0x78 0x7A
}

/// @brief OLED initialization
/// @return COM_StatusTypeDef COM_OK: success, COM_ERROR: fail
uint8_t com_oled::OLED_Begin() {
    uint8_t ret = COM_OK;

    if (hardware == Hardware_SPI) {
        HAL_GPIO_WritePin(OLED_RES_Port, OLED_RES_Pin, GPIO_PIN_RESET);
        com_delay_ms_os(100);
        HAL_GPIO_WritePin(OLED_RES_Port, OLED_RES_Pin, GPIO_PIN_SET);
    }

    ret += OLED_WR_Byte(0xAE, 1);  // 设置显示开启/关闭，0xAE关闭，0xAF开启

    ret += OLED_WR_Byte(0xD5, 1);  // 设置显示时钟分频比/振荡器频率
    ret += OLED_WR_Byte(0x80, 1);  // 0x00~0xFF

    ret += OLED_WR_Byte(0xA8, 1);  // 设置多路复用率
    ret += OLED_WR_Byte(0x3F, 1);  // 0x0E~0x3F

    ret += OLED_WR_Byte(0xD3, 1);  // 设置显示偏移
    ret += OLED_WR_Byte(0x00, 1);  // 0x00~0x7F

    ret += OLED_WR_Byte(0x40, 1);  // 设置显示开始行，0x40~0x7F

    ret += OLED_WR_Byte(0xA1, 1);  // 设置左右方向，0xA1正常，0xA0左右反置

    ret += OLED_WR_Byte(0xC8, 1);  // 设置上下方向，0xC8正常，0xC0上下反置

    ret += OLED_WR_Byte(0xDA, 1);  // 设置COM引脚硬件配置
    ret += OLED_WR_Byte(0x12, 1);

    ret += OLED_WR_Byte(0x81, 1);  // 设置对比度
    ret += OLED_WR_Byte(0xCF, 1);  // 0x00~0xFF

    ret += OLED_WR_Byte(0xD9, 1);  // 设置预充电周期
    ret += OLED_WR_Byte(0xF1, 1);

    ret += OLED_WR_Byte(0xDB, 1);  // 设置VCOMH取消选择级别
    ret += OLED_WR_Byte(0x30, 1);

    ret += OLED_WR_Byte(0xA4, 1);  // 设置整个显示打开/关闭

    ret += OLED_WR_Byte(0xA6, 1);  // 设置正常/反色显示，0xA6正常，0xA7反色

    ret += OLED_WR_Byte(0x8D, 1);  // 设置充电泵
    ret += OLED_WR_Byte(0x14, 1);

    ret += OLED_WR_Byte(0xAF, 1);  // 开启显示

    OLED_Clear();   // 清空显存数组
    OLED_Update();  // 更新显示，清屏，防止初始化后未显示内容时花屏

    if (ret != COM_OK) {
        return COM_ERROR;
    }
    return ret;
}

uint8_t com_oled::OLED_Begin_Fast() {
    uint8_t ret = COM_OK;

    if (hardware == Hardware_SPI) {
        HAL_GPIO_WritePin(OLED_RES_Port, OLED_RES_Pin, GPIO_PIN_RESET);
        com_delay_ms_os(100);
        HAL_GPIO_WritePin(OLED_RES_Port, OLED_RES_Pin, GPIO_PIN_SET);
    }

    ret += OLED_WR_Byte(0xAE, 1);  // Set display on/off
    ret += OLED_WR_Byte(0xA6, 1);  // Set normal/reverse display

    OLED_Clear();   // Clear OLED_Buf
    OLED_Update();  // Update display, clear screen to prevent screen flicker
                    // when no content is displayed after initialization

    if (ret != COM_OK) {
        return COM_ERROR;
    }
    return ret;
}

// clear OLED_Buf
void com_oled::OLED_Clear() {
    memset(OLED_Buf, 0, sizeof(OLED_Buf));
    OLED_Fresh = 1;
}

/**
 * @brief clear a area of OLED_Buf
 * @param x: x start
 * @param y: y start
 * @param Width: x end
 * @param Height: y end
 * @note To display, use OLED_Update() after this function
 */
void com_oled::OLED_Clear(uint8_t x, uint8_t y, uint8_t Width, uint8_t Height) {
    if (x > 127 || y > 63) {
        return;
    }
    if (x + Width > 128) {
        Width = 128 - x;
    }
    if (y + Height > 64) {
        Height = 64 - y;
    }

    for (uint8_t i = y; i < (y + Height); i++) {
        for (uint8_t j = x; j < x + Width; j++) {
            OLED_Buf[i / 8][j] &= ~(0x01 << (i % 8));
        }
    }
    OLED_Fresh = 1;
}

// display all OLED_Buf
uint8_t com_oled::OLED_Update() {
    if (OLED_Fresh == 0) {
        return COM_OK;
    }
    OLED_Fresh = 0;
    uint8_t ret = COM_OK;
    for (uint8_t i = 0; i < 8; i++) {
        ret += OLED_Set_Cursor(i, 0);
        ret += OLED_WR_Data(OLED_Buf[i], 128);
    }
    if (ret != COM_OK) {
        return COM_ERROR;
    }
    return ret;
}

/**
 * @brief display a area of OLED_Buf
 * @param x: x start
 * @param y: y start
 * @param Width: x end
 * @param Height: y end
 * @return COM_StatusTypeDef
 */
uint8_t com_oled::OLED_Update(uint8_t x, uint8_t y, uint8_t Width,
                              uint8_t Height) {
    if (OLED_Fresh == 0) {
        return COM_OK;
    }
    OLED_Fresh = 0;
    uint8_t ret = COM_OK;
    if (x > 127 || y > 63) {
        return COM_ERROR;
    }
    if (x + Width > 128) {
        Width = 128 - x;
    }
    if (y + Height > 64) {
        Height = 64 - y;
    }
    for (uint8_t i = y / 8; i < (y + Height) / 8 + 1; i++) {
        ret += OLED_Set_Cursor(i, x);
        ret += OLED_WR_Data(&OLED_Buf[i][x], Width);
    }
    if (ret != COM_OK) {
        return COM_ERROR;
    }
    return ret;
}

/// @brief
/// @param x
/// @param y
/// @return
uint8_t com_oled::OLED_SetPos(uint8_t x, uint8_t y) {
    uint8_t ret = COM_OK;
    return ret;
}

/**
 * @brief
 * @param
 * @param
 */
uint8_t com_oled::OLED_Fill(uint8_t bmp_data) {
    uint8_t ret = COM_OK;
    return ret;
}

/**
 * @brief
 * @param
 * @param
 */
// uint8_t com_oled::show::BMP(uint8_t x0, uint8_t y0, uint8_t Width, uint8_t
// Height, uint8_t BMP[])
//{
//
// }

/**
 * @brief display a point
 * @param x  x coordinate
 * @param y  y coordinate
 * @param cmd  0: clear, 1: display
 * @return COM_StatusTypeDef COM_OK: success, COM_ERROR: fail
 */
uint8_t com_oled::show::Point(uint8_t x, uint8_t y, uint8_t cmd) {
    uint8_t ret = COM_OK;
    if (x > 127 || y > 63) {
        return COM_ERROR;
    }
    if (cmd == 0) {
        oled.OLED_Buf[y / 8][x] &= ~(1 << (y % 8));
    } else {
        oled.OLED_Buf[y / 8][x] |= 1 << (y % 8);
    }
    oled.OLED_Fresh = 1;
    return ret;
}

/**
 * @brief get a point status
 * @param x  x coordinate
 * @param y  y coordinate
 * @return 1: light, 0: no light
 */
uint8_t com_oled::show::GetPoint(uint8_t x, uint8_t y) {
    if (x > 127 || y > 63) {
        return 0;
    }
    return (oled.OLED_Buf[y / 8][x] & 0x01 << (y % 8));
}

/**
 * @brief display a line
 * @param x1  start x coordinate
 * @param y1  start y coordinate
 * @param x2  end x coordinate
 * @param y2  end y coordinate
 * @param cmd  0: clear, 1: display
 * @return COM_StatusTypeDef COM_OK: success, COM_ERROR: fail
 * @todo change not tested
 */
uint8_t com_oled::show::Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2,
                             uint8_t cmd) {
    uint8_t ret = COM_OK;
    int16_t x, y, dx, dy, d, incrE, incrNE, temp;
    int16_t x0 = x1, y0 = y1, X = x2, Y = y2;
    uint8_t yflag = 0, xyflag = 0;

    if (y0 == Y) {
        for (x = x0; x <= X; x++) {
            ret += Point(x, y0, cmd);
        }
    } else if (x0 == X) {
        for (y = y0; y <= Y; y++) {
            ret += Point(x0, y, cmd);
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
            ret += Point(y, -x, cmd);
        } else if (yflag) {
            ret += Point(x, -y, cmd);
        } else if (xyflag) {
            ret += Point(y, x, cmd);
        } else {
            ret += Point(x, y, cmd);
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
                ret += Point(y, -x, cmd);
            } else if (yflag) {
                ret += Point(x, -y, cmd);
            } else if (xyflag) {
                ret += Point(y, x, cmd);
            } else {
                ret += Point(x, y, cmd);
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
 * @param cmd  0: clear, 1: display
 * @return COM_StatusTypeDef COM_OK: success, COM_ERROR: fail
 */
uint8_t com_oled::show::Rectangle(uint8_t x1, uint8_t y1, uint8_t x2,
                                  uint8_t y2, uint8_t IsFilled, uint8_t cmd) {
    uint8_t ret = COM_OK;
    if (IsFilled) {
        for (uint8_t i = y1; i <= y2; i++) {
            for (uint8_t j = x1; j <= x2; j++) {
                ret += Point(j, i, cmd);
            }
        }
    } else {
        ret += Line(x1, y1, x2, y1, cmd);
        ret += Line(x1, y1, x1, y2, cmd);
        ret += Line(x1, y2, x2, y2, cmd);
        ret += Line(x2, y1, x2, y2, cmd);
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
 * @param cmd  0: clear, 1: display
 * @return COM_StatusTypeDef COM_OK: success, COM_ERROR: fail
 */
uint8_t com_oled::show::Triangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2,
                                 uint8_t x3, uint8_t y3, uint8_t IsFilled,
                                 uint8_t cmd) {
    uint8_t ret = COM_OK;
    int16_t vx[] = {x1, x2, x3};
    int16_t vy[] = {y1, y2, y3};

    if (IsFilled) {
        uint8_t minx = x1, miny = y1, maxx = x1, maxy = y1;
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

        for (uint8_t i = miny; i <= maxy; i++) {
            for (uint8_t j = minx; j <= maxx; j++) {
                if (oled.Pnpoly(3, vx, vy, j, i)) {
                    ret += Point(j, i, cmd);
                }
            }
        }
    } else {
        ret += Line(x1, y1, x2, y2, cmd);
        ret += Line(x1, y1, x3, y3, cmd);
        ret += Line(x2, y2, x3, y3, cmd);
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
 * @param cmd  0: clear, 1: display
 * @return COM_StatusTypeDef COM_OK: success, COM_ERROR: fail
 */
uint8_t com_oled::show::Circle(uint8_t x0, uint8_t y0, uint8_t r,
                               uint8_t IsFilled, uint8_t cmd) {
    uint8_t ret = COM_OK;
    int16_t x = 0, y = r, d = 1 - r;

    ret += Point(x0 + x, y0 + y, cmd);
    ret += Point(x0 + x, y0 - y, cmd);
    ret += Point(x0 - x, y0 + y, cmd);
    ret += Point(x0 - x, y0 - y, cmd);

    if (IsFilled) {
        for (uint8_t i = -y; i <= y; i++) {
            ret += Point(x0, y0 + i, cmd);
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
        ret += Point(x0 + x, y0 + y, cmd);
        ret += Point(x0 - x, y0 + y, cmd);
        ret += Point(x0 + x, y0 - y, cmd);
        ret += Point(x0 - x, y0 - y, cmd);
        ret += Point(x0 + y, y0 + x, cmd);
        ret += Point(x0 - y, y0 + x, cmd);
        ret += Point(x0 + y, y0 - x, cmd);
        ret += Point(x0 - y, y0 - x, cmd);
        if (IsFilled) {
            for (uint8_t i = -y; i <= y; i++) {
                ret += Point(x0 + x, y0 + i, cmd);
                ret += Point(x0 - x, y0 + i, cmd);
            }
            for (uint8_t i = -x; i <= x; i++) {
                ret += Point(x0 + y, y0 + i, cmd);
                ret += Point(x0 - y, y0 + i, cmd);
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
 * @param cmd  0: clear, 1: display
 * @return COM_StatusTypeDef COM_OK: success, COM_ERROR: fail
 * @todo change not tested
 */
uint8_t com_oled::show::Ellipse(uint8_t x0, uint8_t y0, uint8_t a, uint8_t b,
                                uint8_t IsFilled, uint8_t cmd) {
    uint8_t ret = COM_OK;
    int16_t x = 0, y = b, d = 2 * b * b + a * a * (1 - 2 * b);
    float d1, d2;

    ret += Point(x0 + x, y0 + y, cmd);
    ret += Point(x0 + x, y0 - y, cmd);
    ret += Point(x0 - x, y0 + y, cmd);
    ret += Point(x0 - x, y0 - y, cmd);

    if (IsFilled) {
        for (uint8_t i = -y; i <= y; i++) {
            ret += Point(x0, y0 + i, cmd);
        }
    }

    Point(x0 + x, y0 + y, cmd);
    Point(x0 - x, y0 + y, cmd);
    Point(x0 + x, y0 - y, cmd);
    Point(x0 - x, y0 - y, cmd);

    while (b * b * (x + 1) < a * a * (y - 0.5)) {
        if (d1 <= 0) {
            d1 += b * b * (2 * x + 3);
        } else {
            d1 += b * b * (2 * x + 3) + a * a * (-2 * y + 2);
            y--;
        }
        x++;

        if (IsFilled) {
            for (uint8_t i = -y; i < y; i++) {
                ret += Point(x0 + x, y0 + i, cmd);
                ret += Point(x0 - x, y0 + i, cmd);
            }
        }

        ret += Point(x0 + x, y0 + y, cmd);
        ret += Point(x0 - x, y0 - y, cmd);
        ret += Point(x0 - x, y0 + y, cmd);
        ret += Point(x0 + x, y0 - y, cmd);
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
        ret += Point(x0 + x, y0 + y, cmd);
        ret += Point(x0 - x, y0 + y, cmd);
        ret += Point(x0 + x, y0 - y, cmd);
        ret += Point(x0 - x, y0 - y, cmd);
        if (IsFilled) {
            for (uint8_t i = -y; i <= y; i++) {
                ret += Point(x0 + x, y0 + i, cmd);
                ret += Point(x0 - x, y0 + i, cmd);
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
 * @param cmd  0: clear, 1: display
 * @return COM_StatusTypeDef COM_OK: success, COM_ERROR: fail
 */
uint8_t com_oled::show::Arc(uint8_t x0, uint8_t y0, uint8_t r,
                            int16_t StartAngle, int16_t EndAngle,
                            uint8_t IsFilled, uint8_t cmd) {
    uint8_t ret = COM_OK, Status = 0;
    int16_t x = 0, y = r, d = 1 - r;
    if (oled.IsInAngle(x, y, StartAngle, EndAngle)) {
        ret += Point(x0 + x, y0 + y, cmd);
    }
    if (oled.IsInAngle(-x, -y, StartAngle, EndAngle)) {
        ret += Point(x0 - x, y0 - y, cmd);
    }
    if (oled.IsInAngle(y, x, StartAngle, EndAngle)) {
        ret += Point(x0 + y, y0 + x, cmd);
    }
    if (oled.IsInAngle(-y, -x, StartAngle, EndAngle)) {
        ret += Point(x0 - y, y0 - x, cmd);
    }

    if (IsFilled) {
        for (int16_t i = -y; i < y; i++) {
            if (oled.IsInAngle(0, i, StartAngle, EndAngle)) {
                ret += Point(x0, y0 + i, cmd);
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

        Point(x0 + x, y0 + y, oled.IsInAngle(x, y, StartAngle, EndAngle));
        Point(x0 + y, y0 + x, oled.IsInAngle(y, x, StartAngle, EndAngle));
        Point(x0 - x, y0 - y, oled.IsInAngle(-x, -y, StartAngle, EndAngle));
        Point(x0 - y, y0 - x, oled.IsInAngle(-y, -x, StartAngle, EndAngle));
        Point(x0 + x, y0 - y, oled.IsInAngle(x, -y, StartAngle, EndAngle));
        Point(x0 + y, y0 - x, oled.IsInAngle(y, -x, StartAngle, EndAngle));
        Point(x0 - x, y0 + y, oled.IsInAngle(-x, y, StartAngle, EndAngle));
        Point(x0 - y, y0 + x, oled.IsInAngle(-y, x, StartAngle, EndAngle));

        if (IsFilled) {
            for (int16_t i = -y; i < y; i++) {
                if (oled.IsInAngle(x, i, StartAngle, EndAngle)) {
                    ret += Point(x0 + x, y0 + i, cmd);
                }
                if (oled.IsInAngle(-x, i, StartAngle, EndAngle)) {
                    ret += Point(x0 - x, y0 + i, cmd);
                }
            }
            for (int16_t i = -x; i < x; i++) {
                if (oled.IsInAngle(-y, i, StartAngle, EndAngle)) {
                    ret += Point(x0 - y, y0 + i, cmd);
                }
                if (oled.IsInAngle(y, i, StartAngle, EndAngle)) {
                    ret += Point(x0 + y, y0 - y, cmd);
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
uint8_t com_oled::show::Char(uint8_t x, uint8_t y, uint8_t chr, uint8_t Size) {
    switch (Size) {
        case OLED_6X8:
            oled.OLED_ShowImage(x, y, 6, 8, (uint8_t *)&asc2_0608[chr - ' ']);
            break;
        case OLED_8X16:
            oled.OLED_ShowImage(x, y, 8, 16, (uint8_t *)&asc2_0816[chr - ' ']);
            break;
        case OLED_12X12:
            oled.OLED_ShowImage(x, y, 12, 12, (uint8_t *)&asc2_1212[chr - ' ']);
            break;
        case OLED_16X16:
            oled.OLED_ShowImage(x, y, 16, 16, (uint8_t *)&asc2_1616[chr - ' ']);
            break;
        case OLED_24X24:
            oled.OLED_ShowImage(x, y, 24, 24, (uint8_t *)&asc2_2424[chr - ' ']);
            break;
        case OLED_CHINESE:
            oled.OLED_ShowImage(x, y, 16, 16, (uint8_t *)&Chinese1616[chr]);
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
uint8_t com_oled::show::String(uint8_t x, uint8_t y, const char *String,
                               uint8_t Size) {
    uint8_t ret = COM_OK;
    for (uint8_t i = 0; String[i] != '\0'; i++)  // traverse
    {
        ret += Char(x + i * Size, y, String[i], Size);
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
uint8_t com_oled::show::Num(uint8_t x, uint8_t y, int num, uint8_t len,
                            uint8_t size) {
    uint8_t status = 0, ret = COM_OK;
    if (num < 0) {
        ret += Char(x, y, '-', size);
        num = -num;
        status = 1;
    }
    for (uint8_t i = status; i < len + status; i++) {
        ret += Char(x + size * i, y, num / oled.Pow(10, len - i - 1) % 10 + '0',
                    size);
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
uint8_t com_oled::show::Hex(uint8_t x, uint8_t y, uint32_t num, uint8_t len,
                            uint8_t size) {
    uint8_t ret = COM_OK, tempnum;
    for (uint8_t i = 0; i < len; i++) {
        tempnum = num / oled.Pow(16, len - i - 1) % 16;
        if (tempnum < 10) {
            ret += Char(x + size * i, y, tempnum + '0', size);
        } else {
            ret += Char(x + size * i, y, tempnum - 10 + 'A', size);
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
uint8_t com_oled::show::Bin(uint8_t x, uint8_t y, uint32_t num, uint8_t len,
                            uint8_t size) {
    uint8_t ret = COM_OK;
    for (uint8_t i = 0; i < len; i++) {
        ret += Char(x + size * i, y, num / oled.Pow(2, len - i - 1) % 2 + '0',
                    size);
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
uint8_t com_oled::show::Float(uint8_t x, uint8_t y, double num, uint8_t size) {
    uint8_t ret = COM_OK;
    uint8_t status = 0;
    if (num < 0) {
        ret += Char(x, y, '-', size);
        num = -num;
        status = 1;
    }
    ret += Num(x + size * status, y, (int)num, 1, size);
    ret += Char(x + size * (status + 1), y, '.', size);
    ret += Num(x + size * (status + 2), y, (int)((num - (int)num) * 1000), 3,
               size);
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
 * https://gitee.com/zeruns/STM32-HAL-OLED-I2C
 */
uint8_t com_oled::show::Chinese(uint8_t x, uint8_t y, char *Chinese) {
    uint8_t ret = COM_OK, pChinese = 0, pIndex;
    char ChineseIndex[OLED_CHN_CHAR_WIDTH + 1] = {0};
    for (uint8_t i = 0; i < OLED_CHN_CHAR_WIDTH; i++) {
        ChineseIndex[pChinese] = Chinese[i];
        pChinese++;
        if (pChinese >= OLED_CHN_CHAR_WIDTH) {
            ChineseIndex[pChinese + 1] = '\0';
            pIndex = 0;
            // for(pIndex = 0;strcmp(Chinese1616[pIndex], ChineseIndex) != 0 &&
            // pIndex < 1300;pIndex++) {}
            oled.OLED_ShowImage(x + ((i + 1) / OLED_CHN_CHAR_WIDTH - 1) * 16, y,
                                16, 16, (uint8_t *)&Chinese1616[pIndex]);
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
uint8_t com_oled::show::Printf(uint8_t x, uint8_t y, uint8_t size,
                               const char *fmt, ...) {
    uint8_t ret = COM_OK;
    va_list ap;
    va_start(ap, fmt);
    char string[64] = {0};
    vsprintf(string, fmt, ap);
    va_end(ap);
    ret += String(x, y, string, size);
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
uint8_t com_oled::show::Printf(uint8_t x, uint8_t y, uint8_t size, uint8_t *buf,
                               uint16_t len) {
    uint8_t ret = COM_OK;
    char string[64] = {0};
    memcpy(string, buf, len);
    ret += String(x, y, string, size);
    if (ret != COM_OK) {
        return COM_ERROR;
    }
    return ret;
}

/**
 * @brief OLED set cursor
 * @param page: page number
 * @param x: x number
 * @return COM_StatusTypeDef
 * @note 1 page = 8 rows
 * @note if using 1.3 inch OLED(SH1106), please add 0x02 to x
 */
uint8_t com_oled::OLED_Set_Cursor(uint8_t page, uint8_t x) {
    uint8_t ret = COM_OK;
    x += 0x02;
    ret += OLED_WR_Byte(0xb0 | page, 1);
    ret += OLED_WR_Byte(0x10 | (x & 0xf0) >> 4, 1);
    ret += OLED_WR_Byte(0x00 | (x & 0x0f), 1);
    if (ret != COM_OK) {
        return COM_ERROR;
    }
    return ret;
}

// reverse OLED_Buf.To display, use OLED_Update() after this function
void com_oled::OLED_Reverse() {
    for (uint8_t i = 0; i < 8; i++) {
        for (uint8_t j = 0; j < 128; j++) {
            OLED_Buf[i][j] ^= 0xff;
        }
    }
    OLED_Fresh = 1;
}

/**
 * @brief display a area of OLED_Buf
 * @param x: x start
 * @param y: y start
 * @param Width: x end
 * @param Height: y end
 * @note To display, use OLED_Update() after this function
 */
void com_oled::OLED_Reverse(uint8_t x, uint8_t y, uint8_t Width,
                            uint8_t Height) {
    if (x > 127 || y > 63) {
        return;
    }
    if (x + Width > 128) {
        Width = 128 - x;
    }
    if (y + Height > 64) {
        Height = 64 - y;
    }
    for (uint8_t i = y; i < y + Height + 1; i++) {
        for (uint8_t j = x; j < x + Width; j++) {
            OLED_Buf[i][j] ^= 0xff;
        }
    }
    OLED_Fresh = 1;
}

uint8_t com_oled::OLED_ShowImage(uint8_t x, uint8_t y, uint8_t Width,
                                 uint8_t Height, const uint8_t *Image) {
    if (x > 127 || y > 63) {
        return COM_ERROR;
    }
    OLED_Clear(x, y, Width, Height);
    for (uint8_t i = 0; i < (Height - 1) / 8 + 1; i++) {
        for (uint8_t j = 0; j < Width; j++) {
            if (x + j > 127 || y / 8 + i > 7) {
                return COM_ERROR;
            }
            OLED_Buf[y / 8 + i][x + j] = Image[i * Width + j] << (y % 8);
            if (y / 8 + i + 1 > 7) {
                continue;
            }
            OLED_Buf[y / 8 + i + 1][x + j] |=
                Image[i * Width + j] >> (8 - y % 8);
        }
    }
    OLED_Fresh = 1;
    return COM_OK;
}

/**
 * @brief write data or command
 * @param data: data or command
 * @param cmd: 0 write data, 1 write command
 */
uint8_t com_oled::OLED_WR_Byte(uint8_t data, uint8_t cmd) {
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
            for (uint8_t i = 0; i < 8; i++) {
                OLED_WR_SDA((data & 0x80) >> i);
                OLED_I2C_WaitAck();
            }
            OLED_I2C_WaitAck();
        } else {
            OLED_I2C_Start();
            OLED_WR_SDA(Addres);
            OLED_WR_SDA(0x00);
            OLED_WR_SDA(data);
            OLED_I2C_Stop();
        }
    } else if (hardware == Hardware_SPI) {
#if defined(HAL_SPI_MODULE_ENABLED)
        HAL_GPIO_WritePin(OLED_CS_Port, OLED_CS_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(OLED_DC_Port, OLED_DC_Pin,
                          cmd ? GPIO_PIN_RESET : GPIO_PIN_SET);
        ret = HAL_SPI_Transmit(hspi, &data, 1, Timeout);
        HAL_GPIO_WritePin(OLED_CS_Port, OLED_CS_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(OLED_DC_Port, OLED_DC_Pin, GPIO_PIN_SET);
#endif
    }
    return ret;
}

/**
 * @brief  write datas
 * @param  data: data start address
 * @param  len: data length
 */
uint8_t com_oled::OLED_WR_Data(uint8_t *data, uint8_t len) {
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
            ret = OLED_I2C_Transmit(TxData, len + 1);
            // HAL_I2C_Master_Transmit_DMA(hi2c, Addres, TxData, len + 1);
        }
#endif
    } else if (hardware == Software_IIC) {
        OLED_I2C_Start();
        OLED_WR_SDA(Addres);
        OLED_WR_SDA(0x40);
        for (uint8_t i = 0; i < len; i++) {
            OLED_WR_SDA(data[i]);
        }
        OLED_I2C_Stop();
    } else if (hardware == Hardware_SPI) {
#if defined(HAL_SPI_MODULE_ENABLED)
        HAL_GPIO_WritePin(OLED_CS_Port, OLED_CS_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(OLED_DC_Port, OLED_DC_Pin, GPIO_PIN_SET);
        if (DMA == 0) {
            HAL_SPI_Transmit(hspi, data, len, Timeout);
        } else {
            HAL_SPI_Transmit_DMA(hspi, data, len);
        }
        HAL_GPIO_WritePin(OLED_CS_Port, OLED_CS_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(OLED_DC_Port, OLED_DC_Pin, GPIO_PIN_SET);
#endif
    }
    return ret;
}

// start a IIC communication
void com_oled::OLED_I2C_Start() {
    OLED_WR_SDA(1);
    OLED_WR_SCL(1);
    OLED_WR_SDA(0);
    OLED_WR_SCL(0);
}

// stop a IIC communication
void com_oled::OLED_I2C_Stop() {
    OLED_WR_SDA(0);
    OLED_WR_SCL(1);
    OLED_WR_SDA(1);
}

// wait for a ack
void com_oled::OLED_I2C_WaitAck() {
    OLED_WR_SCL(1);
    OLED_WR_SCL(0);
}

#if defined(HAL_I2C_MODULE_ENABLED)
uint8_t com_oled::OLED_I2C_Transmit(uint8_t *data, uint16_t len) {
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
void com_oled::OLED_WR_SDA(uint8_t data) {
    HAL_GPIO_WritePin(OLED_SDA_Port, OLED_SDA_Pin, (GPIO_PinState)data);
    for (volatile uint16_t i = 0; i < Delay_Time; i++) {
    }
}

// set SCL
void com_oled::OLED_WR_SCL(uint8_t data) {
    HAL_GPIO_WritePin(OLED_SCL_Port, OLED_SCL_Pin, (GPIO_PinState)data);
    for (volatile uint16_t i = 0; i < Delay_Time; i++) {
    }
}

/**
 * @brief pow function
 * @param x: base
 * @param y: exponent
 * @return result
 */
uint32_t com_oled::Pow(uint32_t x, uint32_t y) {
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
uint8_t com_oled::Pnpoly(uint8_t nvert, int16_t *vertx, int16_t *verty,
                         int16_t testx, int testy) {
    uint8_t i, j, c = 0;
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
uint8_t com_oled::IsInAngle(int16_t x, int16_t y, int16_t StartAngle,
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
