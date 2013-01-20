/* Copyright (c) 2009-2010, Code Aurora Forum. All rights reserved.
 *  Copyright (c) 2011, Pantech Co., Ltd.  All rights reserved.
 * support SVGA LCD 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 */

#include <linux/kernel.h>
#include <linux/gpio.h>
#ifdef F_SKYDISP_PROC_PANEL_ONOFF
#include<linux/fs.h>
#include<linux/errno.h>
#include<linux/types.h>
#include<linux/fcntl.h>

#include<asm/uaccess.h>
#include<asm/io.h>

#include<linux/stat.h>
#include<linux/proc_fs.h>
#endif

#include "msm_fb.h"
#include <linux/clk.h>

#define GPIO_HIGH_VALUE 1
#define GPIO_LOW_VALUE  0

#define NOP()     do {asm volatile ("NOP");} while(0);

static int lcdc_spi_cs;
static int lcdc_spi_sclk;
static int lcdc_spi_mosi;

#ifdef F_SKYDISP_PROC_PANEL_ONOFF
struct proc_dir_entry *pantech_panel_onoff_fp	= NULL;
char data_str[PAGE_SIZE-80] = { 0, };
#endif

static unsigned char bit_shift[8] = { (1 << 7), /* MSB */ 
  (1 << 6),
  (1 << 5),
  (1 << 4),
  (1 << 3),
  (1 << 2),
  (1 << 1),
  (1 << 0)                   /* LSB */
};

struct panel_state_type{
      boolean disp_initialized;
      boolean display_on;
      boolean disp_powered_up;
};

//boolean first_pass = FALSE;
//boolean end_level = TRUE;
//static u8 old_bl_level=0;

static struct panel_state_type lcdc_state = { 0 };

static struct msm_panel_common_pdata *lcdc_common_pt_pdata;

#define LCD_DIM_CON 130

#if 0
#define T_EN 	12
#define T_ON	12
#define T_OFF	10
#define T_DIS	600	
#else
#define T_EN    10	
#define T_ON    10	
#define T_OFF   5 	
#define T_DIS	500	
#endif

#define MAX_CMD  127

/* shinbrad 20110610 fixed code for overlay using dma */
#ifdef F_SKYDISP_MDP_VG_CLEAR_HOLD_CHANGE
extern void force_mdp4_overlay_unset(void);
extern void force_mdp4_overlay_control(bool onoff);
#endif
/* end */

u8 parameter_list[MAX_CMD];

static void common_spi_init(void);
static void common_backlight(int level);

static void send_spi_command(u8 command, u8 count, u8 *parameter_list)
{
   u8 i,j = 0;

   /* Enable the Chip Select */
   gpio_set_value(lcdc_spi_cs, GPIO_LOW_VALUE);
   NOP();
   NOP();

   /* Before we enter here the Clock should be Low ! */
   for(i=0;i<8;i++)
   {
       gpio_set_value(lcdc_spi_sclk, GPIO_LOW_VALUE);
       NOP();
      /* #1: Drive the Data (High or Low) */
      if(command & bit_shift[i]){
        gpio_set_value(lcdc_spi_mosi, GPIO_HIGH_VALUE);        
      }else{
        gpio_set_value(lcdc_spi_mosi, GPIO_LOW_VALUE);        
      }
      /* #2: Drive the Clk High*/
      NOP(); 
      gpio_set_value(lcdc_spi_sclk, GPIO_HIGH_VALUE); 
      NOP();
      NOP();
   }

   for(j=0; j<count; j++)
   {
      for(i=0;i<8;i++)
      {
         gpio_set_value(lcdc_spi_sclk, GPIO_LOW_VALUE);
         NOP();
         /* #1: Drive the Data (High or Low) */
         if(parameter_list[j] & bit_shift[i])
            gpio_set_value(lcdc_spi_mosi, GPIO_HIGH_VALUE); 
         else
            gpio_set_value(lcdc_spi_mosi, GPIO_LOW_VALUE); 
		 
         /* #2: Drive the Clk High*/
         NOP(); 
         gpio_set_value(lcdc_spi_sclk, GPIO_HIGH_VALUE);    
         NOP(); 
         NOP();     
      }  
   }
      /* Idle state of SDO (MOSI) is Low */
      gpio_set_value(lcdc_spi_mosi, GPIO_LOW_VALUE); 
      gpio_set_value(lcdc_spi_sclk, GPIO_LOW_VALUE);

   /* Now Disable the Chip Select */
   NOP();
   NOP();
   gpio_set_value(lcdc_spi_cs, GPIO_HIGH_VALUE);

   NOP();
}


static void lcdc_common_pt_disp_powerup(void)
{
      if (!lcdc_state.disp_powered_up && !lcdc_state.display_on) {
            /* Reset the hardware first */
            /* Include DAC power up implementation here */
      //      common_backlight(15);
            lcdc_state.disp_powered_up = TRUE;
      }
}

static void lcdc_common_pt_panel_setup(void)
{
     gpio_set_value(lcdc_spi_sclk, GPIO_LOW_VALUE);
     gpio_set_value(lcdc_spi_mosi, GPIO_LOW_VALUE);
     gpio_set_value(lcdc_spi_cs, GPIO_HIGH_VALUE);

#if (BOARD_VER < WS10)
    parameter_list[0] = 0x01;
    parameter_list[1] = 0x05;
    parameter_list[2] = 0x06;
    parameter_list[3] = 0x0A;
    parameter_list[4] = 0x18;
    parameter_list[5] = 0x0E;
    parameter_list[6] = 0x22;
    parameter_list[7] = 0x23;
    parameter_list[8] = 0x24;
    send_spi_command(0x98,9,parameter_list);
	
    parameter_list[0] = 0x2B;
    parameter_list[1] = 0x51;
    send_spi_command(0x99,2,parameter_list);

    parameter_list[0] = 0x02;
    parameter_list[1] = 0x06;
    parameter_list[2] = 0x08;
    parameter_list[3] = 0x0A;
    parameter_list[4] = 0x0C;
    parameter_list[5] = 0x01;
    send_spi_command(0x9B,6,parameter_list);

    parameter_list[0] = 0x00;
    parameter_list[1] = 0x1F;
    parameter_list[2] = 0x12;
    parameter_list[3] = 0x01;
    parameter_list[4] = 0xF8;
    parameter_list[5] = 0x88;
    parameter_list[6] = 0x87;
    parameter_list[7] = 0x00;
    send_spi_command(0xA2,8,parameter_list);

    parameter_list[0] = 0x00;
    parameter_list[1] = 0x1F;
    parameter_list[2] = 0x12;
    parameter_list[3] = 0x01;
    parameter_list[4] = 0xF8;
    parameter_list[5] = 0x88;
    parameter_list[6] = 0x87;
    parameter_list[7] = 0x01;
    send_spi_command(0xA3,8,parameter_list);

    parameter_list[0] = 0x00;
    parameter_list[1] = 0x1F;
    parameter_list[2] = 0x10;
    parameter_list[3] = 0x40;
    parameter_list[4] = 0x08;
    parameter_list[5] = 0x77;
    parameter_list[6] = 0x77;
    parameter_list[7] = 0x00;
    send_spi_command(0xA4,8,parameter_list);

    parameter_list[0] = 0x00;
    parameter_list[1] = 0x1F;
    parameter_list[2] = 0x10;
    parameter_list[3] = 0x42;
    parameter_list[4] = 0x08;
    parameter_list[5] = 0x77;
    parameter_list[6] = 0x77;
    parameter_list[7] = 0x01;
    send_spi_command(0xA5,8,parameter_list);

    parameter_list[0] = 0x04;
    parameter_list[1] = 0x20;
    parameter_list[2] = 0x13;
    parameter_list[3] = 0x43;
    parameter_list[4] = 0x2B;
    parameter_list[5] = 0x88;
    parameter_list[6] = 0x77;
    parameter_list[7] = 0x02;
    send_spi_command(0xA6,8,parameter_list);

    parameter_list[0] = 0x04;
    parameter_list[1] = 0x20;
    parameter_list[2] = 0x13;
    parameter_list[3] = 0x43;
    parameter_list[4] = 0x2B;
    parameter_list[5] = 0x88;
    parameter_list[6] = 0x77;
    parameter_list[7] = 0x02;
    send_spi_command(0xA7,8,parameter_list);

    parameter_list[0] = 0x59;
    send_spi_command(0xB4,1,parameter_list);

    parameter_list[0] = 0x35;
    parameter_list[1] = 0x03;
    send_spi_command(0xB5,2,parameter_list);

    parameter_list[0] = 0x15;
    parameter_list[1] = 0x44;
    parameter_list[2] = 0x10;
    parameter_list[3] = 0x2E;
    parameter_list[4] = 0x18;
    parameter_list[5] = 0x00;
    parameter_list[6] = 0x30;
    parameter_list[7] = 0x33;
    send_spi_command(0xB7,8,parameter_list);

    parameter_list[0] = 0x1E;
    parameter_list[1] = 0x44;
    parameter_list[2] = 0x10;
    parameter_list[3] = 0x2E;
    parameter_list[4] = 0x18;
    parameter_list[5] = 0x00;
    parameter_list[6] = 0x30;
    parameter_list[7] = 0x33;
    send_spi_command(0xB8,8,parameter_list);

    parameter_list[0] = 0x49;
    parameter_list[1] = 0x11;
    parameter_list[2] = 0x01;
    parameter_list[3] = 0x00;
    parameter_list[4] = 0x30;
    send_spi_command(0xB9,5,parameter_list);

    parameter_list[0] = 0x4E;
    parameter_list[1] = 0x11;
    parameter_list[2] = 0x01;
    parameter_list[3] = 0x00;
    parameter_list[4] = 0x30;
    send_spi_command(0xBA,5,parameter_list);

    parameter_list[0] = 0x11;
    parameter_list[1] = 0x01;
    parameter_list[2] = 0x00;
    parameter_list[3] = 0x30;
    send_spi_command(0xBB,4,parameter_list);

    parameter_list[0] = 0x06;
    send_spi_command(0xBC,1,parameter_list);

    parameter_list[0] = 0x01;
    send_spi_command(0xB0,1,parameter_list);

    parameter_list[0] = 0xC8;
    send_spi_command(0xC0,1,parameter_list);

    parameter_list[0] = 0x00;
    send_spi_command(0xC2,1,parameter_list);

    parameter_list[0] = 0x00;
    send_spi_command(0xC3,1,parameter_list);

    parameter_list[0] = 0x0E;
    send_spi_command(0xC4,1,parameter_list);

    parameter_list[0] = 0x21;
    send_spi_command(0xC5,1,parameter_list);

    parameter_list[0] = 0x00;
    send_spi_command(0xC8,1,parameter_list);

    parameter_list[0] = 0x0F;
    send_spi_command(0xCA,1,parameter_list);

    parameter_list[0] = 0x44;
    send_spi_command(0xCB,1,parameter_list);

    parameter_list[0] = 0x0E;
    send_spi_command(0xCC,1,parameter_list);

    parameter_list[0] = 0x00;
    send_spi_command(0xD4,1,parameter_list);

    parameter_list[0] = 0x27;
    send_spi_command(0xDB,1,parameter_list);

    parameter_list[0] = 0x01;
    send_spi_command(0x96,1,parameter_list);

    parameter_list[0] = 0x02;
    send_spi_command(0xB0,1,parameter_list);

    parameter_list[0] = 0xA9;
    send_spi_command(0xFE,1,parameter_list);

    parameter_list[0] = 0x55;
    send_spi_command(0xFF,1,parameter_list);

    parameter_list[0] = 0x01;
    send_spi_command(0x96,1,parameter_list);

#endif
    send_spi_command(0x11,0,parameter_list);

    mdelay(130);
    send_spi_command(0x29,0,parameter_list);
  //  mdelay(10);

}

static void lcdc_common_pt_panel_sleep(void)
{
	printk("[%s] Enter...\n",__FUNCTION__);
#if 0
    send_spi_command(0x28,0,parameter_list);
    send_spi_command(0x10,0,parameter_list);
    mdelay(130);
#else  /* power off sequence */
    send_spi_command(0x28,0,parameter_list);
    send_spi_command(0x10,0,parameter_list);
    mdelay(120);
	printk("[%s] End...\n",__FUNCTION__);
	
#endif
}

static void lcdc_common_pt_panel_wakeup(void)
{
	printk("[%s] Enter...\n",__FUNCTION__);
    mdelay(50);
#if (BOARD_VER < WS10)
    parameter_list[0] = 0x01;
    parameter_list[1] = 0x05;
    parameter_list[2] = 0x06;
    parameter_list[3] = 0x0A;
    parameter_list[4] = 0x18;
    parameter_list[5] = 0x0E;
    parameter_list[6] = 0x22;
    parameter_list[7] = 0x23;
    parameter_list[8] = 0x24;
    send_spi_command(0x98,9,parameter_list);

    parameter_list[0] = 0x2B;
    parameter_list[1] = 0x51;
    send_spi_command(0x99,2,parameter_list);

    parameter_list[0] = 0x02;
    parameter_list[1] = 0x06;
    parameter_list[2] = 0x08;
    parameter_list[3] = 0x0A;
    parameter_list[4] = 0x0C;
    parameter_list[5] = 0x01;
    send_spi_command(0x9B,6,parameter_list);

    parameter_list[0] = 0x00;
    parameter_list[1] = 0x1F;
    parameter_list[2] = 0x12;
    parameter_list[3] = 0x01;
    parameter_list[4] = 0xF8;
    parameter_list[5] = 0x88;
    parameter_list[6] = 0x87;
    parameter_list[7] = 0x00;
    send_spi_command(0xA2,8,parameter_list);

    parameter_list[0] = 0x00;
    parameter_list[1] = 0x1F;
    parameter_list[2] = 0x12;
    parameter_list[3] = 0x01;
    parameter_list[4] = 0xF8;
    parameter_list[5] = 0x88;
    parameter_list[6] = 0x87;
    parameter_list[7] = 0x01;
    send_spi_command(0xA3,8,parameter_list);

    parameter_list[0] = 0x00;
    parameter_list[1] = 0x1F;
    parameter_list[2] = 0x10;
    parameter_list[3] = 0x40;
    parameter_list[4] = 0x08;
    parameter_list[5] = 0x77;
    parameter_list[6] = 0x77;
    parameter_list[7] = 0x00;
    send_spi_command(0xA4,8,parameter_list);

    parameter_list[0] = 0x00;
    parameter_list[1] = 0x1F;
    parameter_list[2] = 0x10;
    parameter_list[3] = 0x42;
    parameter_list[4] = 0x08;
    parameter_list[5] = 0x77;
    parameter_list[6] = 0x77;
    parameter_list[7] = 0x01;
    send_spi_command(0xA5,8,parameter_list);

    parameter_list[0] = 0x04;
    parameter_list[1] = 0x20;
    parameter_list[2] = 0x13;
    parameter_list[3] = 0x43;
    parameter_list[4] = 0x2B;
    parameter_list[5] = 0x88;
    parameter_list[6] = 0x77;
    parameter_list[7] = 0x02;
    send_spi_command(0xA6,8,parameter_list);

    parameter_list[0] = 0x04;
    parameter_list[1] = 0x20;
    parameter_list[2] = 0x13;
    parameter_list[3] = 0x43;
    parameter_list[4] = 0x2B;
    parameter_list[5] = 0x88;
    parameter_list[6] = 0x77;
    parameter_list[7] = 0x02;
    send_spi_command(0xA7,8,parameter_list);

    parameter_list[0] = 0x59;
    send_spi_command(0xB4,1,parameter_list);

    parameter_list[0] = 0x35;
    parameter_list[1] = 0x03;
    send_spi_command(0xB5,2,parameter_list);

    parameter_list[0] = 0x15;
    parameter_list[1] = 0x44;
    parameter_list[2] = 0x10;
    parameter_list[3] = 0x2E;
    parameter_list[4] = 0x18;
    parameter_list[5] = 0x00;
    parameter_list[6] = 0x30;
    parameter_list[7] = 0x33;
    send_spi_command(0xB7,8,parameter_list);

    parameter_list[0] = 0x1E;
    parameter_list[1] = 0x44;
    parameter_list[2] = 0x10;
    parameter_list[3] = 0x2E;
    parameter_list[4] = 0x18;
    parameter_list[5] = 0x00;
    parameter_list[6] = 0x30;
    parameter_list[7] = 0x33;
    send_spi_command(0xB8,8,parameter_list);

    parameter_list[0] = 0x49;
    parameter_list[1] = 0x11;
    parameter_list[2] = 0x01;
    parameter_list[3] = 0x00;
    parameter_list[4] = 0x30;
    send_spi_command(0xB9,5,parameter_list);

    parameter_list[0] = 0x4E;
    parameter_list[1] = 0x11;
    parameter_list[2] = 0x01;
    parameter_list[3] = 0x00;
    parameter_list[4] = 0x30;
    send_spi_command(0xBA,5,parameter_list);

    parameter_list[0] = 0x06;
    send_spi_command(0xBC,1,parameter_list);

    parameter_list[0] = 0x01;
    send_spi_command(0xB0,1,parameter_list);

    parameter_list[0] = 0xC8;
    send_spi_command(0xC0,1,parameter_list);

    parameter_list[0] = 0x00;
    send_spi_command(0xC2,1,parameter_list);

    parameter_list[0] = 0x00;
    send_spi_command(0xC3,1,parameter_list);

    parameter_list[0] = 0x0E;
    send_spi_command(0xC4,1,parameter_list);

    parameter_list[0] = 0x21;
    send_spi_command(0xC5,1,parameter_list);

    parameter_list[0] = 0x00;
    send_spi_command(0xC8,1,parameter_list);

    parameter_list[0] = 0x0F;
    send_spi_command(0xCA,1,parameter_list);

    parameter_list[0] = 0x44;
    send_spi_command(0xCB,1,parameter_list);

    parameter_list[0] = 0x0E;
    send_spi_command(0xCC,1,parameter_list);

    parameter_list[0] = 0x00;
    send_spi_command(0xD4,1,parameter_list);

    parameter_list[0] = 0x27;
    send_spi_command(0xDB,1,parameter_list);

    parameter_list[0] = 0x01;
    send_spi_command(0x96,1,parameter_list);

    parameter_list[0] = 0x02;
    send_spi_command(0xB0,1,parameter_list);

    parameter_list[0] = 0xA9;
    send_spi_command(0xFE,1,parameter_list);

    parameter_list[0] = 0x55;
    send_spi_command(0xFF,1,parameter_list);

    parameter_list[0] = 0x01;
    send_spi_command(0x96,1,parameter_list);

#endif
    send_spi_command(0x11,0,parameter_list);

    mdelay(130);

    send_spi_command(0x29,0,parameter_list);
	printk("[%s] End...\n",__FUNCTION__);
}

static int lcdc_common_panel_on(struct platform_device *pdev)
{
//#if 0
#if defined(CONFIG_MACH_MSM8X55_MINI)
	struct msm_fb_data_type *mfd;

	mfd = platform_get_drvdata(pdev);

	if(!mfd)
	{
		printk("[%s]device structure is null (platform_get_drv_data).....\n",__FUNCTION__);
		return -ENODEV;
	}
	if(mfd->key != MFD_KEY)
	{
		printk("[%s]device key is not match.. (MFD_KEY)....\n",__FUNCTION__);
		return -EINVAL;
	}
#endif
      if (!lcdc_state.disp_initialized) {
            common_spi_init();
            lcdc_common_pt_disp_powerup();
            lcdc_common_pt_panel_setup();
            lcdc_state.disp_initialized = TRUE;
      } else { 
	  		lcdc_common_pt_panel_wakeup();
	 		 printk("[%s] Display panel wake up  .................\n", __FUNCTION__);
		}

      lcdc_state.display_on = TRUE;
	  printk("[%s] Display panel on .................\n", __FUNCTION__);

      return 0;
}


#ifdef F_SKYDISP_REMOVE_NOISE_MDP_INIT
int sky_lcd_screen_off(void)
{
//    ENTER_FUNC2();
//    send_spi_command(0x10, 0, parameter_list);
	
  	lcdc_common_pt_panel_sleep();
    mdelay(20); //wait for several frame :: guide
   // flag_sky_panel_off = TRUE; 
   lcdc_state.display_on = FALSE;
//#ifdef F_SKYDISP_REMOVE_LIGHTNING	
  //  flag_sky_panel_off_blctl = TRUE;
//#endif
//    EXIT_FUNC2();
    return 0;
}
#endif

static int lcdc_common_panel_off(struct platform_device *pdev)
{
      if (lcdc_state.disp_powered_up && lcdc_state.display_on) {

		
	    	lcdc_common_pt_panel_sleep();

          lcdc_state.display_on = FALSE;
          lcdc_state.disp_initialized = FALSE;
		  printk("[%s] Display panel off .................\n", __FUNCTION__);
      }
      return 0;
}

static void common_spi_init(void)
{
#if (BOARD_VER >= WS10)
      lcdc_spi_cs    = 172;
      lcdc_spi_mosi  = 173;
      lcdc_spi_sclk  = 174;
#elif (BOARD_VER < WS10)
      spi_sclk  = 45;
      spi_cs    = 46;
      spi_mosi  = 47;
#endif /* BOARD VER */
}

static void common_backlight(int level)
{
     int bl_value,i;
	 unsigned long flags;
     // printk("[%s] level = %d \n",__FUNCTION__,level); 
#if 0  // for 30Mhz  Because of brightness, H/W required it at 30.7Mhz shinbrad
     if(level == 6)  // android default 
       level += 1;
#endif
     bl_value = 32-(level*2);
     

     printk("[%s] Current Backlight value : %d, Dimming loop value : %d \n",__FUNCTION__, level , bl_value);

     if(!level) {
        gpio_set_value(LCD_DIM_CON,GPIO_LOW_VALUE);
        udelay(T_DIS);

		  printk("[%s] Backlight OFF!....\n",__FUNCTION__);
#ifdef F_SKYDISP_MDP_VG_CLEAR_HOLD_CHANGE
      force_mdp4_overlay_control(1); //screen_hold
#endif
		return;
     }

#ifdef F_SKYDISP_MDP_VG_CLEAR_HOLD_CHANGE
    force_mdp4_overlay_control(0); //release screen_hold
#endif

/* shinbrad shinjg */
	  local_save_flags(flags);
	  local_irq_disable();

     gpio_set_value(LCD_DIM_CON,GPIO_LOW_VALUE);
     udelay(T_DIS);
     gpio_set_value(LCD_DIM_CON,GPIO_HIGH_VALUE);
     
     udelay(T_ON);


     for(i=0;i<(bl_value)-1;i++) {
        gpio_set_value(LCD_DIM_CON,GPIO_LOW_VALUE);
        udelay(T_OFF);
        gpio_set_value(LCD_DIM_CON,GPIO_HIGH_VALUE);
     	udelay(T_ON);
      }
/* shinbrad shinjg */
	  local_irq_restore(flags);	
}

static void lcdc_common_pt_set_backlight(struct msm_fb_data_type *mfd)
{
      int bl_level; //, pre_level = 0;

      bl_level = mfd->bl_level;



      if(lcdc_state.display_on == TRUE)
	  {
    	  common_backlight(bl_level);

     	   printk(KERN_WARNING "%s: set lcd backlight! %d\n",
          			              __func__,bl_level);
	  }else{
    	   common_backlight(0);
     	   printk(KERN_WARNING "%s: panel is not set !...backlight off(0)\n", __func__);
	  }
			
}


static int __devinit common_pt_probe(struct platform_device *pdev)
{
      if (pdev->id == 0) {
            lcdc_common_pt_pdata = pdev->dev.platform_data;
            common_spi_init();
            return 0;
      }
      msm_fb_add_device(pdev);
      return 0;
}

static struct msm_fb_panel_data common_pt_panel_data = {
      .on = lcdc_common_panel_on,
      .off = lcdc_common_panel_off,
      .set_backlight = lcdc_common_pt_set_backlight,
};

static struct platform_device this_device = {
      .name   = "lcdc_common_pt",
      .id   = 1,
      .dev  = {
            .platform_data = &common_pt_panel_data,
      }
};

static struct platform_driver this_driver = {
      .probe  = common_pt_probe,
      .driver = {
            .name   = "lcdc_common_pt",
      },
};


#ifdef F_SKYDISP_PROC_PANEL_ONOFF
int read_pantech_panel_onoff(char *page, char **start, off_t off, int count, int *eof, void *data_unused)
{
	char *data;
	char *buf;
	printk("[%s] enter..........\n",__FUNCTION__);
	
	data = (char*)data_unused;

	buf = page;
	buf += sprintf(buf,"Pantech Panel status = [%s]\n", data);

	*eof = 1;

	printk("[%s] end..........\n",__FUNCTION__);
	
	return buf-page;
}
int write_pantech_panel_onoff(struct file *file, const char __user *buffer, unsigned long count, void *data)
{
	int len;
	char *rev_data;

	printk("[%s] enter..........\n",__FUNCTION__);

	rev_data = (char*)data;
	
	if (copy_from_user(rev_data, buffer, count)) 
		return -EFAULT;
	
	len = strlen(data);
	
	if (rev_data[len-1] == '\n')
	  	rev_data[--len] = 0;

	printk("data = [%s], rev_data = [%s]\n", (char*)data,rev_data);
	printk("[%s] end..........\n",__FUNCTION__);

	if(!strncmp(rev_data,"off",3))
	{
		printk("[%s] Jump Panel OFF sequence......\n",__FUNCTION__);
		lcdc_common_pt_panel_sleep();
	}
	else if(!strncmp(rev_data,"on",2))	
	{
		printk("[%s] Jump Panel ON sequence......\n",__FUNCTION__);
		lcdc_common_pt_panel_wakeup();
	}
	return count;	
}

int pantech_panel_onoff_init(void)
{
	pantech_panel_onoff_fp = create_proc_entry("pantech_panel_onoff", S_IFREG | S_IRWXU ,NULL);
	
	if(pantech_panel_onoff_fp)
	{
		pantech_panel_onoff_fp->data 			= data_str;
		pantech_panel_onoff_fp->read_proc 		= read_pantech_panel_onoff;
		pantech_panel_onoff_fp->write_proc 		= write_pantech_panel_onoff;
	}
	
	return 0;
}

void pantech_panel_onoff_exit(void)
{
		remove_proc_entry("pantech_panel_onoff",pantech_panel_onoff_fp);
}
		
module_init(pantech_panel_onoff_init);
module_exit(pantech_panel_onoff_exit);

MODULE_LICENSE("Dual BSD/GPL");
#endif

static int __init lcdc_common_panel_init(void)
{
      int ret;
      struct msm_panel_info *pinfo;

      ret = platform_driver_register(&this_driver);
      if (ret)
            return ret;

      pinfo = &common_pt_panel_data.panel_info;

#ifdef CONFIG_FB_MSM_LCDC_COMMON_SVGA_PANEL
      pinfo->xres = 600;
      pinfo->yres = 800;
      MSM_FB_SINGLE_MODE_PANEL(pinfo);
      pinfo->type = LCDC_PANEL;
      pinfo->pdest = DISPLAY_1;
      pinfo->bl_max = 16;
      pinfo->wait_cycle = 0;
#ifdef CONFIG_FB_MSM_DEFAULT_DEPTH_RGB565
      pinfo->bpp = 16;
#else
      pinfo->bpp = 24;
#endif
      pinfo->fb_num = 2;
#if 0
      pinfo->clk_rate = 24576 * 1000;// - 500;
#else
      pinfo->clk_rate = 30720 * 1000;// - 500;
#endif

      pinfo->lcdc.h_back_porch =12;
      pinfo->lcdc.h_front_porch = 18;
      pinfo->lcdc.h_pulse_width = 6;
      pinfo->lcdc.v_back_porch = 4;
      pinfo->lcdc.v_front_porch = 6;
      pinfo->lcdc.v_pulse_width = 2;
      pinfo->lcdc.border_clr = 0;      /* blk */
      pinfo->lcdc.underflow_clr = 0x00;      /* blue -> black*/
      pinfo->lcdc.hsync_skew = 0;
	
#endif 
      ret = platform_device_register(&this_device);
      if (ret) {
            printk(KERN_ERR "%s not able to register the device\n",
                   __func__);
            goto fail_driver;
      }

      return ret;

fail_driver:
      platform_driver_unregister(&this_driver);

      return ret;
}

device_initcall(lcdc_common_panel_init);
