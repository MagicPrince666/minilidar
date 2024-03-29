/*
 * common.h
 *
 * Author: Tomi Valkeinen <tomi.valkeinen@nokia.com>
 * Copyright (C) 2009-2012 Tomi Valkeinen

 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __COMMON_H__
#define __COMMON_H__

#include <linux/fb.h>
#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <memory>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define ASSERT(x)                                                \
    if (!(x)) {                                                  \
        perror("assert(" __FILE__ ":" TOSTRING(__LINE__) "): "); \
        exit(1);                                                 \
    }
#define FBCTL0(ctl)                                              \
    if (ioctl(fd, ctl)) {                                        \
        perror("fbctl0(" __FILE__ ":" TOSTRING(__LINE__) "): "); \
        exit(1);                                                 \
    }
#define FBCTL1(ctl, arg1)                                        \
    if (ioctl(fd, ctl, arg1)) {                                  \
        perror("fbctl1(" __FILE__ ":" TOSTRING(__LINE__) "): "); \
        exit(1);                                                 \
    }

#define IOCTL0(fd, ctl)                                          \
    if (ioctl(fd, ctl)) {                                        \
        perror("ioctl0(" __FILE__ ":" TOSTRING(__LINE__) "): "); \
        exit(1);                                                 \
    }
#define IOCTL1(fd, ctl, arg1)                                    \
    if (ioctl(fd, ctl, arg1)) {                                  \
        perror("ioctl1(" __FILE__ ":" TOSTRING(__LINE__) "): "); \
        exit(1);                                                 \
    }

class LcdRgb
{
public:
    LcdRgb(std::string dev = "/dev/fb0");
    ~LcdRgb();

    bool Init();

    /**
     * @brief 清除屏幕
     * @param color 填充颜色
     */
    void FillScreenSolid(uint32_t color);

    /**
     * @brief 输出字符串
     * @param x
     * @param y
     * @param s
     * @param maxlen
     * @param color
     * @param clear
     * @param clearlen
     * @return int
     */
    int32_t FbPutString(int32_t x, int32_t y, const char *s, uint32_t maxlen,
                    uint32_t color, bool clear, int32_t clearlen);
    /**
     * @brief 将值打印到屏幕
     * @param x
     * @param y
     * @param value
     * @param maxlen
     * @param color
     * @param clear
     * @param clearlen
     * @return int
     */
    int32_t FbPutValue(int32_t x, int32_t y, int32_t value, uint32_t maxlen,
                   uint32_t color, bool clear, int32_t clearlen);

    /**
     * @brief 画点
     * @param x
     * @param y
     * @param color
     */
    void FbDrawPixel(int32_t x, int32_t y, uint32_t color);

    /**
     * @brief 画线
     * @param x1
     * @param y1
     * @param x2
     * @param y2
     */
    void FbDrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color);

    /**
     * @brief 画圆
     * @param x
     * @param y
     * @param r
     * @param color
     */
    void FbDrawCircle(int32_t x, int32_t y, int32_t r, uint32_t color);

    /**
     * @brief 清空区域
     * @param x
     * @param y
     * @param w
     * @param h
     */
    void FbClearArea(int32_t x, int32_t y, int32_t w, int32_t h);

    /**
     * @brief 画矩形
     * (x1,y1),(x2,y2):矩形的对角坐标
     */
    void FbDrawRectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color);

private:
    struct fb_info {
        int fd;

        uint8_t *ptr;

        struct fb_var_screeninfo var;
        struct fb_fix_screeninfo fix;
        uint32_t bytespp;
        int32_t i_line_width;
        int32_t i_pixel_width;
    };
    void FbPutChar(int32_t x, int32_t y, char c,
                   uint32_t color);

    std::shared_ptr<struct fb_info> fb_info_;
    std::string fb_name_;
    int bl_fd_{-1};
};

#define RGB_BLACK 0x000000
#define RGB_WHITE 0xFFFFFF
#define RGB_GRAY 0x808080
#define RGB_RED 0xFF0000
#define RGB_ORANGE 0xFFA500
#define RGB_GREEN 0x008000
#define RGB_BLUE 0x0000FF
#define RGB_SILVER 0xC0C0C0
#define RGB_BROWN 0xA52A2A
#define RGB_CYAN 0x00FFFF
#define RGB_CERISE 0xDE3163
#define RGB_YELLOW 0xFFFF00
#define RGB_GOLDEN 0xFFD700
#define RGB_VERMILION 0xFF4D00

#endif
