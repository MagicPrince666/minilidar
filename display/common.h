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

#include <stdio.h>
#include <stdint.h>
#include <linux/fb.h>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define ASSERT(x) if (!(x)) \
	{ perror("assert(" __FILE__ ":" TOSTRING(__LINE__) "): "); exit(1); }
#define FBCTL0(ctl) if (ioctl(fd, ctl))\
	{ perror("fbctl0(" __FILE__ ":" TOSTRING(__LINE__) "): "); exit(1); }
#define FBCTL1(ctl, arg1) if (ioctl(fd, ctl, arg1))\
	{ perror("fbctl1(" __FILE__ ":" TOSTRING(__LINE__) "): "); exit(1); }

#define IOCTL0(fd, ctl) if (ioctl(fd, ctl))\
	{ perror("ioctl0(" __FILE__ ":" TOSTRING(__LINE__) "): "); exit(1); }
#define IOCTL1(fd, ctl, arg1) if (ioctl(fd, ctl, arg1))\
	{ perror("ioctl1(" __FILE__ ":" TOSTRING(__LINE__) "): "); exit(1); }

struct fb_info
{
	int fd;

	void *ptr;

	struct fb_var_screeninfo var;
	struct fb_fix_screeninfo fix;
	uint32_t bytespp;
};

class LcdRgb {
public:
	LcdRgb(int fb_num);
	~LcdRgb();
	void fb_update_window(int fd, short x, short y, short w, short h);
	void fb_sync_gfx(int fd);
	void fill_screen_solid(uint32_t color);
	int fb_put_string(int x, int y, const char *s, uint32_t maxlen,
			int color, bool clear, int clearlen);
	int fb_put_string(int x, int y, int value, uint32_t maxlen,
			int color, bool clear, int clearlen);

private:
	void draw_pixel(int x, int y, uint32_t color);
	void fb_clear_area(int x, int y, int w, int h);
	void fb_put_char(int x, int y, char c,
		uint32_t color);

	struct fb_info* fb_info_;
	int bl_fd_{-1};
};


#define RGB_BLACK	0x000000
#define	RGB_WHITE	0xFFFFFF
#define	RGB_GRAY	0x808080
#define	RGB_RED		0xFF0000
#define	RGB_ORANGE	0xFFA500
#define	RGB_GREEN	0x008000
#define	RGB_BLUE	0x0000FF
#define	RGB_SILVER	0xC0C0C0
#define	RGB_BROWN	0xA52A2A
#define	RGB_CYAN	0x00FFFF
#define	RGB_CERISE	0xDE3163
#define RGB_YELLOW	0xFFFF00
#define	RGB_GOLDEN	0xFFD700
#define	RGB_VERMILION	0xFF4D00

#endif
