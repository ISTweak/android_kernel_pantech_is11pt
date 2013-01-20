/* drivers/video/msm/logo.c
 *
 * Show Logo in RLE 565 format
 *
 * Copyright (C) 2008 Google Incorporated
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fb.h>
#include <linux/vt_kern.h>
#include <linux/unistd.h>
#include <linux/syscalls.h>

#include <linux/irq.h>
#include <asm/system.h>

#if defined(CONFIG_MACH_MSM8X55_MINI)
#define fb_width(fb)	((fb)->var.xres + 8) // 600 + 8  shinbrad ( shinjg ) 20110322
#else
#define fb_width(fb)	((fb)->var.xres)
#endif

#define fb_height(fb)	((fb)->var.yres)
#define fb_size(fb)	((fb)->var.xres * (fb)->var.yres * 2)

typedef unsigned int IBUF_TYPE;
#if defined(CONFIG_PANTECH_FB_24BPP_RGB888)
static void memset32(void *_ptr, unsigned int val, unsigned count)
{
        unsigned int *ptr = _ptr;
        count >>= 2;
        while (count--)
                *ptr++ = val;
}
#else
static void memset16(void *_ptr, unsigned short val, unsigned count)
{
	unsigned short *ptr = _ptr;
	count >>= 1;
	while (count--)
		*ptr++ = val;
}
#endif

/* 565RLE image format: [count(2 bytes), rle(2 bytes)] */
int load_565rle_image(char *filename)
{
	struct fb_info *info;
	int fd, count, err = 0;
	unsigned max;
#ifdef CONFIG_PANTECH_FB_24BPP_RGB888
	IBUF_TYPE *data, *bits, *ptr;
#else
	unsigned short *data, *bits, *ptr;
#endif

	info = registered_fb[0];
	if (!info) {
		printk(KERN_WARNING "%s: Can not access framebuffer\n",
			__func__);
		return -ENODEV;
	}

	fd = sys_open(filename, O_RDONLY, 0);
	if (fd < 0) {
		printk(KERN_WARNING "%s: Can not open %s\n",
			__func__, filename);
		return -ENOENT;
	}
	count = sys_lseek(fd, (off_t)0, 2);
	if (count <= 0) {
		err = -EIO;
		goto err_logo_close_file;
	}
	sys_lseek(fd, (off_t)0, 0);
	data = kmalloc(count, GFP_KERNEL);
	if (!data) {
		printk(KERN_WARNING "%s: Can not alloc data\n", __func__);
		err = -ENOMEM;
		goto err_logo_close_file;
	}
	if (sys_read(fd, (char *)data, count) != count) {
		err = -EIO;
		goto err_logo_free_data;
	}

	max = fb_width(info) * fb_height(info);
	ptr = data;
#ifdef CONFIG_PANTECH_FB_24BPP_RGB888
	bits = (IBUF_TYPE *)(info->screen_base);
#else
	bits = (unsigned short *)(info->screen_base);
#endif

	while (count > 3) {
		unsigned n = ptr[0];
		if (n > max)
			break;
#ifdef CONFIG_PANTECH_FB_24BPP_RGB888
		memset32((unsigned int *)bits, ptr[1], n << 2);
#else
		memset16(bits, ptr[1], n << 1);
#endif
		bits += n;
		max -= n;
		ptr += 2;
		count -= 4;
	}

err_logo_free_data:
	kfree(data);
err_logo_close_file:
	sys_close(fd);
	return err;
}
EXPORT_SYMBOL(load_565rle_image);

#ifdef SKY_LCD_LOADBAR_ANDROID
#ifdef CONFIG_PANTECH_FB_24BPP_RGB888
#define PIXEL_TO_32(RGB)	((((RGB)&0xF800)<<8) | (((RGB)&0x07E0)<<5) | (((RGB)&0x001F)<<3))
/*static IBUF_TYPE loading_progress_t[8] =
{
	PIXEL_TO_32(0x9c13), PIXEL_TO_32(0xd61a), PIXEL_TO_32(0xc598), PIXEL_TO_32(0xa454),
	PIXEL_TO_32(0x8310), PIXEL_TO_32(0x9c13), PIXEL_TO_32(0xc598), PIXEL_TO_32(0x9c13)
};*/
static IBUF_TYPE loading_progress_t[17] =
{
	PIXEL_TO_32(0xFFFF), PIXEL_TO_32(0xFFDF), PIXEL_TO_32(0xF79E), PIXEL_TO_32(0xEF5D), 
	PIXEL_TO_32(0xE71C), PIXEL_TO_32(0xDEDB), PIXEL_TO_32(0xD6BA), PIXEL_TO_32(0xD69A), 
	PIXEL_TO_32(0xD69A), PIXEL_TO_32(0xCE59), PIXEL_TO_32(0xCE59), PIXEL_TO_32(0xD69A), 
	PIXEL_TO_32(0xDEDB), PIXEL_TO_32(0xE71C), PIXEL_TO_32(0xEF7D), PIXEL_TO_32(0xF7BE), 
	PIXEL_TO_32(0xFFDF)
};
		
#else

static IBUF_TYPE loading_progress_t[8] =
{
	0x9c13, 0xd61a, 0xc598, 0xa454, 0x8310, 0x9c13, 0xc598, 0x9c13
};
#endif

int sky_lcdc_display_loadingbar(int ratio)
{
	struct fb_info *info;
	int	err = 0;
#ifdef CONFIG_PANTECH_FB_24BPP_RGB888
	IBUF_TYPE *load_bar;
	unsigned int i;
	IBUF_TYPE BAR_COLOR = 0xFFFF;
#else
	unsigned short *load_bar;
	unsigned short i;
	unsigned short BAR_COLOR = 0xFFFF;
#endif
	
 
#if defined(CONFIG_MACH_MSM8X55_MINI)
	const unsigned short ST_X=100, ST_Y=738;
	const unsigned short SCREEN_WIDTH = 608;
	const unsigned short BAR_WIDTH =418;
	
#elif defined(CONFIG_MACH_MSM8X55_LASER2)
	const unsigned short ST_X=40, ST_Y=738;
	const unsigned short SCREEN_WIDTH = 480;
	const unsigned short BAR_WIDTH = 400;
#else
	const unsigned short ST_X=100, ST_Y=738;
	const unsigned short SCREEN_WIDTH = 480;
	const unsigned short BAR_WIDTH = 441;
#endif
	const unsigned short BAR_HEIGHT = 17;

	int cr = 0;
	
	if (ratio > 100) ratio = 100;
	cr = (int)(ratio * BAR_WIDTH/100 );
	
	info = registered_fb[0];
	
	if (!info) {
		printk(KERN_WARNING "%s: Can not access framebuffer\n",__FUNCTION__);
		return -ENODEV;
	}

#ifdef CONFIG_PANTECH_FB_24BPP_RGB888
//#ifdef CONFIG_MACH_MSM8X55_MINI
//	load_bar = (IBUF_TYPE *)(info->screen_base) + (SCREEN_WIDTH * ST_Y + ST_X);
//#endif
	load_bar = (IBUF_TYPE *)(info->screen_base) + SCREEN_WIDTH * ST_Y + ST_X;
#else
	load_bar = (unsigned short *)(info->screen_base) + SCREEN_WIDTH * ST_Y + ST_X;
#endif
	for (i=0 ; i<BAR_HEIGHT; i++)
	{
		BAR_COLOR = loading_progress_t[i];
#ifdef CONFIG_PANTECH_FB_24BPP_RGB888
		memset32((unsigned int *)load_bar, BAR_COLOR, cr<<2);
#else
		memset16(load_bar, BAR_COLOR, cr<<1);
#endif
		load_bar = load_bar + SCREEN_WIDTH;
	}

	return err;
}

EXPORT_SYMBOL(sky_lcdc_display_loadingbar);

#define SKY_LOADBAR_KERNEL_MODULE_LOAD_MAX_RATIO				33//33	// see the "INIT_LOADBAR_START_RATIO"
//=================================================//

int __init sky_display_loadingbar_increase(void)
{
	static int		g_nRatio = 0;
	int				err = 0;
	struct fb_info	*info = registered_fb[0];

	if(0 == g_nRatio)	// avoid first call...
	{
		g_nRatio = 1;
		return 0;
	}



	if(SKY_LOADBAR_KERNEL_MODULE_LOAD_MAX_RATIO < g_nRatio)	// 1 ~ 33%
	{
		printk(KERN_DEBUG "sky_display_loadingbar_increase() DROP COUNT: %d\n", g_nRatio - SKY_LOADBAR_KERNEL_MODULE_LOAD_MAX_RATIO);
		g_nRatio++;
		return 0;
	}

	if(0 > (err = sky_lcdc_display_loadingbar(g_nRatio)))
		return err;
	if(0 > (err = fb_pan_display(info, &info->var)))
		return err;

	printk(KERN_DEBUG "########## sky_display_loadingbar_increase(%d)\n", g_nRatio);

	++g_nRatio;

	if(SKY_LOADBAR_KERNEL_MODULE_LOAD_MAX_RATIO < g_nRatio)	// 1 ~ 33%
	{
		printk(KERN_DEBUG "sky_display_loadingbar_increase() Droped: %d\n", g_nRatio);
		g_nRatio = 0;
		return 99;
	}
	
	return err;
}

module_init(sky_display_loadingbar_increase);


#endif
