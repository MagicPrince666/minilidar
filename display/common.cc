/*
 * common.c
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

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <new>

#include "common.h"
#include "font_8x8.h"

#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"
#include "spdlog/fmt/ostr.h"

#define MIN(x, y) ((x) > (y) ? (y) : (x))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

#define BL_POWER "/sys/class/backlight/backlight/bl_power"

LcdRgb::LcdRgb(std::string dev) :
fb_name_(dev)
{
    fb_info_ = new (std::nothrow) fb_info;
}

LcdRgb::~LcdRgb()
{
    FillScreenSolid(0x0);

    if (fb_info_->ptr) {
        int32_t stat = munmap(fb_info_->ptr, fb_info_->fix.smem_len);
        if (stat < 0) {
            perror("Error munmap'ing framebuffer device");
        }
    }
    if (fb_info_->fd > 0) {
        close(fb_info_->fd);
    }

    if (bl_fd_) {
        write(bl_fd_, "1", 1);
        close(bl_fd_);
    }

    delete fb_info_;
}

bool LcdRgb::Init()
{
	if (access(BL_POWER, F_OK) == 0) {
        bl_fd_ = open(BL_POWER, O_RDWR);
        ASSERT(bl_fd_ >= 0);
        write(bl_fd_, "0", 1);
    }

    fb_info_->fd = open(fb_name_.c_str(), O_RDWR);

    ASSERT(fb_info_->fd >= 0);

    IOCTL1(fb_info_->fd, FBIOGET_VSCREENINFO, &fb_info_->var);
    IOCTL1(fb_info_->fd, FBIOGET_FSCREENINFO, &fb_info_->fix);

    spdlog::info("fb res {}x{} virtual {}x{}, line_len {}, bpp {}",
           fb_info_->var.xres, fb_info_->var.yres,
           fb_info_->var.xres_virtual, fb_info_->var.yres_virtual,
           fb_info_->fix.line_length, fb_info_->var.bits_per_pixel);

    /*计算屏幕缓冲区大小*/
    int32_t screensize = fb_info_->var.xres * fb_info_->var.yres * fb_info_->var.bits_per_pixel / 8;

    fb_info_->i_line_width   = fb_info_->var.xres * fb_info_->var.bits_per_pixel / 8;
    fb_info_->i_pixel_width  = fb_info_->var.bits_per_pixel / 8;

    fb_info_->ptr = (uint8_t *)mmap(nullptr, screensize, PROT_WRITE | PROT_READ,
                                     MAP_SHARED, fb_info_->fd, 0);
    // spdlog::info("ptr {} size {}", fb_info_->ptr, screensize);

    ASSERT(fb_info_->ptr != MAP_FAILED);
    return true;
}

void LcdRgb::FbClearArea(int32_t x, int32_t y, int32_t w, int32_t h)
{
    int32_t i = 0;
    int32_t loc;
    char *fbuffer                 = (char *)fb_info_->ptr;
    struct fb_var_screeninfo *var = &fb_info_->var;
    struct fb_fix_screeninfo *fix = &fb_info_->fix;

    for (i = 0; i < h; i++) {
        loc = (x + var->xoffset) * (var->bits_per_pixel / 8) + (y + i + var->yoffset) * fix->line_length;
        memset(fbuffer + loc, 0, w * var->bits_per_pixel / 8);
    }
}

void LcdRgb::FbPutChar(int32_t x, int32_t y, char c,
                       uint32_t color)
{
    int32_t j, bits;
    uint32_t loc;
    uint8_t *p8;
    uint16_t *p16;
    uint32_t *p32;
    struct fb_var_screeninfo *var = &fb_info_->var;
    struct fb_fix_screeninfo *fix = &fb_info_->fix;

    for (uint32_t i = 0; i < 8; i++) {
        bits = fontdata_8x8[8 * c + i];
        for (j = 0; j < 8; j++) {
            loc = (x + j + var->xoffset) * (var->bits_per_pixel / 8) + (y + i + var->yoffset) * fix->line_length;
            if (loc >= 0 && loc < fix->smem_len &&
                ((bits >> (7 - j)) & 1)) {
                switch (var->bits_per_pixel) {
                case 8:
                    p8  = (uint8_t *)(fb_info_->ptr) + loc;
                    *p8 = color;
                case 16:
                    p16  = (uint16_t *)(fb_info_->ptr) + loc;
                    *p16 = color;
                    break;
                case 24:
                case 32:
                    p32  = (uint32_t *)(fb_info_->ptr) + loc;
                    *p32 = color;
                    break;
                }
            }
        }
    }
}

int32_t LcdRgb::FbPutString(int32_t x, int32_t y, const char *s, uint32_t maxlen,
                        uint32_t color, bool clear, int32_t clearlen)
{
    int32_t w = 0;

    if (clear) {
        FbClearArea(x, y, clearlen * 8, 8);
    }

    for (uint32_t i = 0; i < strlen(s) && i < maxlen; i++) {
        FbPutChar((x + 8 * i), y, s[i], color);
        w += 8;
    }

    return w;
}

int32_t LcdRgb::FbPutValue(int32_t x, int32_t y, int32_t value, uint32_t maxlen,
                       uint32_t color, bool clear, int32_t clearlen)
{
    int32_t w        = 0;
    char str[40] = {0};

    int32_t len  = sprintf(str, "%d", value);
    str[len] = 0;

    if (clear) {
        FbClearArea(x, y, clearlen * 8, 8);
    }

    for (uint32_t i = 0; i < strlen(str) && i < maxlen; i++) {
        FbPutChar((x + 8 * i), y, str[i], color);
        w += 8;
    }

    return w;
}

void LcdRgb::FbDrawPixel(int32_t x, int32_t y, uint32_t color)
{
    uint8_t *pucPen8 = fb_info_->ptr + y * fb_info_->i_line_width + x * fb_info_->i_pixel_width;
    uint16_t *pwPen16;
    uint32_t *pdwPen32;
    int32_t iRed, iGreen, iBlue;

    pwPen16  = (uint16_t *)pucPen8;
    pdwPen32 = (uint32_t *)pucPen8;

    switch (fb_info_->var.bits_per_pixel) {
    case 8: {
        *pucPen8 = color; /*对于8BPP：color 为调色板的索引值，其颜色取决于调色板的数值*/
        break;
    }
    case 16: {
        iRed     = (color >> 16) & 0xff;
        iGreen   = (color >> 8) & 0xff;
        iBlue    = (color >> 0) & 0xff;
        color  = ((iRed >> 3) << 11) | ((iGreen >> 2) << 5) | (iBlue >> 3); /*格式：RGB565*/
        *pwPen16 = color;
        break;
    }
    case 32: {
        *pdwPen32 = color;
        break;
    }
    default: {
        // DBG_PRINTF("can't surport %dbpp", t_fb_var_.bits_per_pixel);
        break;
    }
    }
}

void LcdRgb::FbDrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color)
{
    int32_t t;
    int32_t xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int32_t incx, incy, uRow, uCol;
    delta_x = x2 - x1; //计算坐标增量
    delta_y = y2 - y1;
    uRow    = x1;
    uCol    = y1;
    if (delta_x > 0) {
        incx = 1; //设置单步方向
    } else if (delta_x == 0) {
        incx = 0; //垂直线
    } else {
        incx    = -1;
        delta_x = -delta_x;
    }
    if (delta_y > 0) {
        incy = 1;
    } else if (delta_y == 0) {
        incy = 0; //水平线
    } else {
        incy    = -1;
        delta_y = -delta_y;
    }
    if (delta_x > delta_y) {
        distance = delta_x; //选取基本增量坐标轴
    } else {
        distance = delta_y;
    }

    for (t = 0; t <= distance + 1; t++) { //画线输出
        FbDrawPixel(uRow, uCol, color);
        xerr += delta_x;
        yerr += delta_y;
        if (xerr > distance) {
            xerr -= distance;
            uRow += incx;
        }
        if (yerr > distance) {
            yerr -= distance;
            uCol += incy;
        }
    }
}

void LcdRgb::FbDrawCircle(int32_t x, int32_t y, int32_t r, uint32_t color)
{
    int32_t a, b, num;
    a = 0;
    b = r;
    while (2 * b * b >= r * r) { // 1/8圆即可
        FbDrawPixel(x + a, y - b, color); // 0~1
        FbDrawPixel(x - a, y - b, color); // 0~7
        FbDrawPixel(x - a, y + b, color); // 4~5
        FbDrawPixel(x + a, y + b, color); // 4~3

        FbDrawPixel(x + b, y + a, color); // 2~3
        FbDrawPixel(x + b, y - a, color); // 2~1
        FbDrawPixel(x - b, y - a, color); // 6~7
        FbDrawPixel(x - b, y + a, color); // 6~5

        a++;
        num = (a * a + b * b) - r * r;
        if (num > 0) {
            b--;
            a--;
        }
    }
}

void LcdRgb::FillScreenSolid(uint32_t color)
{
    uint32_t h = fb_info_->var.yres;
    uint32_t w = fb_info_->var.xres;

    for (uint32_t y = 0; y < h; y++) {
        for (uint32_t x = 0; x < w; x++) {
            FbDrawPixel(x, y, color);
        }
    }
}

void LcdRgb::FbDrawRectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color)
{
    FbDrawLine(x1, y1, x2, y1, color);
    FbDrawLine(x1, y1, x1, y2, color);
    FbDrawLine(x1, y2, x2, y2, color);
    FbDrawLine(x2, y1, x2, y2, color);
}