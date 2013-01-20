/* Copyright (c) 2009-2010, Code Aurora Forum. All rights reserved.
 *  Copyright (c) 2011, Pantech Co., Ltd.  All rights reserved.
 *  Jung-Sik Jeong <chero@pantech.com>
 *
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
#include "msm_fb.h"
#include <linux/clk.h>
#include <asm/system.h> 
#include <mach/vreg.h>
#include "../../../arch/arm/mach-msm/proc_comm.h"

#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/switch.h>
#define GPIO_HIGH_VALUE 1
#define GPIO_LOW_VALUE  0

#ifdef CONFIG_MACH_MSM8X55_J_MASAI_E
#define LCD_ROTATE_180		1
#endif

#define NOP()     do {asm volatile ("NOP");} while(0);

static int spi_cs;
static int spi_sclk;
static int spi_mosi;
typedef enum{
	SMEM_PROC_COMM_CUSTOMER_CMD1_SET_SW_RESET,
	SMEM_PROC_COMM_CUSTOMER_CMD1_SET_USER_RESET,
	SMEM_PROC_COMM_CUSTOMER_CMD1_SW_RESET,
	SMEM_PROC_COMM_CUSTOMER_CMD1_USER_RESET,
	SMEM_PROC_COMM_CUSTOMER_CMD1_SW_RESET_IMM,
	SMEM_PROC_COMM_CUSTOMER_CMD1_USER_RESET_IMM,
	SMEM_PROC_COMM_CUSTOMER_CMD1_SET_BL_STATUS,
	SMEM_PROC_COMM_CUSTOMER_CMD1_POWER_DOWN,
	SMEM_PROC_COMM_CUSTOMER_CMD1_NOTI_TO_MARM,
	SMEM_PROC_COMM_CUSTOMER_CMD1_PMIC_VIB_MOT_CONTROL,
	SMEM_PROC_COMM_CUSTOMER_CMD1_GET_CHARGING_STATE,
	SMEM_PROC_COMM_CUSTOMER_CMD1_SET_LCD_STATE,

	SMEM_PROC_COMM_CUSTOMER_CMD1_MAX,
	SMEM_PROC_COMM_CUSTOMER_CMD1_FB_OFFSET
}smem_proc_comm_customer_cmd1_type;


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
static struct panel_state_type lcdc_state = { 0,0,0 };
static struct msm_panel_common_pdata *lcdc_common_pt_pdata;

static struct switch_dev sky_lcd_dev;
struct class *sky_lcd_class;
static atomic_t device_count;




#define LCD_DIM_CON 130
#ifndef LCD_RESET
#define LCD_RESET	122
#endif

#define T_EN 	10
#define T_ON	10
#define T_OFF	5	
#define T_DIS	500	

#ifdef F_SKYDISP_MDP_VG_CLEAR_HOLD_CHANGE
extern void force_mdp4_overlay_unset(void);
extern void force_mdp4_overlay_control(bool onoff);

#endif
#define MAX_CMD  127
u8 parameter_list[MAX_CMD];
unsigned char command_list[3] = {0x20,0x00,0x40}; 


static void common_spi_init(void);


extern void sky_sys_rst_set_smem_info(uint32_t *reason, uint32_t *reset, uint32_t *backlight, uint32_t* silent_boot);
extern boolean sky_sys_rst_is_silent_boot_mode(void);
extern bool sky_sys_rst_GetSilentBoot_bl_off(void);
static boolean silent_mode_1st_skip = FALSE;
static boolean silent_mode_2nd_skip = FALSE;
static boolean silent_mode_skip_complete = FALSE;
static boolean silent_mode_bl_set_exit = TRUE;



#if(BOARD_VER == PT10)
static void send_spi_command(u8 command, u8 count, u8 *parameter_list)
{
   u8 i,j = 0;

  
   gpio_set_value(spi_cs, GPIO_LOW_VALUE);
   NOP();
   NOP();

   gpio_set_value(spi_sclk, GPIO_LOW_VALUE);
   NOP();

   gpio_set_value(spi_mosi, GPIO_LOW_VALUE);

   NOP(); 
   gpio_set_value(spi_sclk, GPIO_HIGH_VALUE);   
   NOP();
   NOP();
   
   for(i=0;i<8;i++)
   {
       gpio_set_value(spi_sclk, GPIO_LOW_VALUE);
       NOP();
      
      if(command & bit_shift[i])
        gpio_set_value(spi_mosi, GPIO_HIGH_VALUE);        
      else
        gpio_set_value(spi_mosi, GPIO_LOW_VALUE);        
      
      NOP(); 
      gpio_set_value(spi_sclk, GPIO_HIGH_VALUE); 
      NOP();
      NOP();
   }


   for(j=0; j<count; j++)
   {
      gpio_set_value(spi_sclk, GPIO_LOW_VALUE);
      NOP();
    
      gpio_set_value(spi_mosi, GPIO_HIGH_VALUE); 
      
      NOP(); 
      gpio_set_value(spi_sclk, GPIO_HIGH_VALUE);    
      NOP();
      NOP();
      
      for(i=0;i<8;i++)
      {
         gpio_set_value(spi_sclk, GPIO_LOW_VALUE);
         NOP();
       
         if(parameter_list[j] & bit_shift[i])
            gpio_set_value(spi_mosi, GPIO_HIGH_VALUE); 
         else
            gpio_set_value(spi_mosi, GPIO_LOW_VALUE); 
   
         
         NOP(); 
         gpio_set_value(spi_sclk, GPIO_HIGH_VALUE);    
         NOP(); 
         NOP();     
      }  
   }
      
      gpio_set_value(spi_mosi, GPIO_LOW_VALUE); 
      gpio_set_value(spi_sclk, GPIO_LOW_VALUE);

   
   NOP();
   NOP();
   gpio_set_value(spi_cs, GPIO_HIGH_VALUE);

   NOP();
}
#else
static void send_spi_command(u8 command, u8 count, u8 *parameter_list)
{
   u8 i,j = 0;

 
   gpio_set_value(spi_cs, GPIO_LOW_VALUE);
   NOP();
   NOP();

   for(i=0;i<8;i++)
   {
       gpio_set_value(spi_sclk, GPIO_LOW_VALUE);
       NOP();
     
      if(command & bit_shift[i]){
        gpio_set_value(spi_mosi, GPIO_HIGH_VALUE);        
      }else{
        gpio_set_value(spi_mosi, GPIO_LOW_VALUE);        
      }
      
      NOP(); 
      gpio_set_value(spi_sclk, GPIO_HIGH_VALUE); 
      NOP();
      NOP();
   }

   for(j=0; j<count; j++)
   {
      for(i=0;i<8;i++)
      {
         gpio_set_value(spi_sclk, GPIO_LOW_VALUE);
         NOP();
        
         if(parameter_list[j] & bit_shift[i])
            gpio_set_value(spi_mosi, GPIO_HIGH_VALUE); 
         else
            gpio_set_value(spi_mosi, GPIO_LOW_VALUE); 
		 
         
         NOP(); 
         gpio_set_value(spi_sclk, GPIO_HIGH_VALUE);    
         NOP(); 
         NOP();     
      }  
   }
    
      gpio_set_value(spi_mosi, GPIO_LOW_VALUE); 
      gpio_set_value(spi_sclk, GPIO_LOW_VALUE);

   NOP();
   NOP();
   gpio_set_value(spi_cs, GPIO_HIGH_VALUE);

   NOP();
}
#endif
static void lcdc_reset(void)
{
	gpio_set_value(LCD_RESET, GPIO_HIGH_VALUE);
	mdelay(10);
	gpio_set_value(LCD_RESET, GPIO_LOW_VALUE);
	mdelay(10);
	gpio_set_value(LCD_RESET, GPIO_HIGH_VALUE);
	mdelay(10);
	printk(KERN_WARNING "[SKY_LCD] %s \n",__func__);
	
}
static void lcdc_common_pt_disp_powerup(void)
{
      if (!lcdc_state.disp_powered_up && !lcdc_state.display_on) {
       
            lcdc_state.disp_powered_up = TRUE;
      }
}

#if(BOARD_VER >= WS10)
void boot_send_spi_command_ready(uint16 addr, uint16 spi_data )
{
    uint8 i;
	
	parameter_list[0] = (uint8)((addr & 0xff00) >> 8);
	parameter_list[1] = (uint8)(addr & 0x00ff);
	parameter_list[2] = (uint8)(spi_data & 0x00ff) ;

	if(((addr == 0x1100) || (addr == 0x2900)) && spi_data == 0x00ff )
	{
		send_spi_command(command_list[0] , 1, &parameter_list[0]);
		send_spi_command(command_list[1] , 1, &parameter_list[1]);
		return;
	}
	for(i = 0; i < 3; i++)
	{
		send_spi_command(command_list[i] , 1, &parameter_list[i]);
	}

}
#endif
static void lcdc_common_pt_panel_setup(void)
{

     gpio_set_value(spi_sclk, GPIO_LOW_VALUE);
     gpio_set_value(spi_mosi, GPIO_LOW_VALUE);
     gpio_set_value(spi_cs, GPIO_HIGH_VALUE);
	 
#if(BOARD_VER == PT10) // #if(BOARD_VER == PT10)

    parameter_list[0] = 0xFF;
    parameter_list[1] = 0x83;
    parameter_list[2] = 0x63;
    send_spi_command(0xB9, 3, parameter_list);

    parameter_list[0] = 0x81;
    parameter_list[1] = 0x24;
    parameter_list[2] = 0x04;
    parameter_list[3] = 0x02;
    parameter_list[4] = 0x02;
    parameter_list[5] = 0x03;
    parameter_list[6] = 0x10;
    parameter_list[7] = 0x10;
    parameter_list[8] = 0x34;
    parameter_list[9] = 0x3C;
    parameter_list[10] = 0x3F;
    parameter_list[11] = 0x3F;
    send_spi_command(0xB1, 12, parameter_list);

    send_spi_command(0x11, 0, parameter_list);
    mdelay(10);

    send_spi_command(0x20, 0, parameter_list);

#ifdef LCD_ROTATE_180
    parameter_list[0] = 0x03;
    send_spi_command(0x36, 1, parameter_list);
#endif
#ifdef CONFIG_FB_MSM_DEFAULT_DEPTH_RGB565
    parameter_list[0] = 0x50;
#else
    parameter_list[0] = 0x70;
#endif
    send_spi_command(0x3A, 1, parameter_list);
    mdelay(150);

    parameter_list[0] = 0x78;
    parameter_list[1] = 0x24;
    parameter_list[2] = 0x04;
    parameter_list[3] = 0x02;
    parameter_list[4] = 0x02;
    parameter_list[5] = 0x03;
    parameter_list[6] = 0x10;
    parameter_list[7] = 0x10;
    parameter_list[8] = 0x34;
    parameter_list[9] = 0x3C;
    parameter_list[10] = 0x3F;
    parameter_list[11] = 0x3F;
    send_spi_command(0xB1, 12, parameter_list);

    parameter_list[0] = 0x01;
    send_spi_command(0xB3, 1, parameter_list);

    parameter_list[0] = 0x00;
    parameter_list[1] = 0x08;
    parameter_list[2] = 0x6E;
    parameter_list[3] = 0x07;
    parameter_list[4] = 0x01;
    parameter_list[5] = 0x01;
    parameter_list[6] = 0x62;
    parameter_list[7] = 0x01;
    parameter_list[8] = 0x57;
    send_spi_command(0xB4, 9, parameter_list);

    parameter_list[0] = 0x0B;
    send_spi_command(0xCC, 1, parameter_list);

    parameter_list[0] = 0x03;
    parameter_list[1] = 0x49;
    parameter_list[2] = 0x4E;
    parameter_list[3] = 0x4C;
    parameter_list[4] = 0x57;
    parameter_list[5] = 0xF4;
    parameter_list[6] = 0x0B;
    parameter_list[7] = 0x4E;
    parameter_list[8] = 0x92;
    parameter_list[9] = 0x57;
    parameter_list[10] = 0x1A;
    parameter_list[11] = 0x99;
    parameter_list[12] = 0x96;
    parameter_list[13] = 0x0C;
    parameter_list[14] = 0x10;
    parameter_list[15] = 0x01;
    parameter_list[16] = 0x47;
    parameter_list[17] = 0x4D;
    parameter_list[18] = 0x57;
    parameter_list[19] = 0x62;
    parameter_list[20] = 0xFF;
    parameter_list[21] = 0x0A;
    parameter_list[22] = 0x4E;
    parameter_list[23] = 0xD1;
    parameter_list[24] = 0x16;
    parameter_list[25] = 0x19;
    parameter_list[26] = 0x98;
    parameter_list[27] = 0xD6;
    parameter_list[28] = 0x0E;
    parameter_list[29] = 0x11;
    send_spi_command(0xE0, 30, parameter_list);
    
    mdelay(10);
    send_spi_command(0x29, 0, parameter_list);
	#else
	
    boot_send_spi_command_ready(0xf000 ,0x0055);
	boot_send_spi_command_ready(0xf001 ,0x00AA);
	boot_send_spi_command_ready(0xf002 ,0x0052);
	boot_send_spi_command_ready(0xf003 ,0x0008);
	boot_send_spi_command_ready(0xf004 ,0x0000); //MAUCCTR
	
	boot_send_spi_command_ready(0xB000 ,0x0000);
	boot_send_spi_command_ready(0xB001 ,0x0012);
	boot_send_spi_command_ready(0xB002 ,0x0009);
	boot_send_spi_command_ready(0xB003 ,0x0014);
	boot_send_spi_command_ready(0xB004 ,0x000A);//RGBCTR
	
	boot_send_spi_command_ready(0xB100 ,0x00EC);
	boot_send_spi_command_ready(0xB101 ,0x000A);//DOPCTR
	
	boot_send_spi_command_ready(0xB500 ,0x0050);//DPRSLCTR
	
	boot_send_spi_command_ready(0xB600 ,0x0007);//SDHDTCTR
	
	boot_send_spi_command_ready(0xB700 ,0x0000);
	boot_send_spi_command_ready(0xB701 ,0x0073);//GSEQCTR
	
	boot_send_spi_command_ready(0xB800 ,0x0001);
	boot_send_spi_command_ready(0xB801 ,0x0003);
	boot_send_spi_command_ready(0xB802 ,0x0005);
	boot_send_spi_command_ready(0xB803 ,0x0002);//SDEQCTR
	
	boot_send_spi_command_ready(0xB900 ,0x0000);	
	boot_send_spi_command_ready(0xB901 ,0x0000); //SDNDACTR
	
	boot_send_spi_command_ready(0xBA00 ,0x0002); //SDVPCTR
	
	boot_send_spi_command_ready(0xBB00 ,0x0088);
	boot_send_spi_command_ready(0xBB01 ,0x0008);
	boot_send_spi_command_ready(0xBB02 ,0x0088);//SGOPCTR
	
	boot_send_spi_command_ready(0xBC00 ,0x0001);
	boot_send_spi_command_ready(0xBC01 ,0x0001);
	boot_send_spi_command_ready(0xBC02 ,0x0001);//INVCTR
	
	boot_send_spi_command_ready(0xBD00 ,0x0001);
	boot_send_spi_command_ready(0xBD01 ,0x0071);
	boot_send_spi_command_ready(0xBD02 ,0x000E);
	boot_send_spi_command_ready(0xBD03 ,0x0056);
	boot_send_spi_command_ready(0xBD04 ,0x0000);//DPFRCTR1

	boot_send_spi_command_ready(0xBE00 ,0x0001);
	boot_send_spi_command_ready(0xBE01 ,0x0071);
	boot_send_spi_command_ready(0xBE02 ,0x000E);
	boot_send_spi_command_ready(0xBE03 ,0x0056);
	boot_send_spi_command_ready(0xBE04 ,0x0000);//DPFRCTR2
	
    boot_send_spi_command_ready(0xBF00 ,0x0001);
	boot_send_spi_command_ready(0xBF01 ,0x0071);
	boot_send_spi_command_ready(0xBF02 ,0x000E);
	boot_send_spi_command_ready(0xBF03 ,0x0056);
	boot_send_spi_command_ready(0xBF04 ,0x0000);//DPFRCTR3
	
	boot_send_spi_command_ready(0xCA00 ,0x0001);
	boot_send_spi_command_ready(0xCA01 ,0x00E4);
	boot_send_spi_command_ready(0xCA02 ,0x00E4);
	boot_send_spi_command_ready(0xCA03 ,0x00E4);
	boot_send_spi_command_ready(0xCA04 ,0x00E4);
	boot_send_spi_command_ready(0xCA05 ,0x00E4);
	boot_send_spi_command_ready(0xCA06 ,0x00E4);
	boot_send_spi_command_ready(0xCA07 ,0x00E4);
	boot_send_spi_command_ready(0xCA08 ,0x0008);
	boot_send_spi_command_ready(0xCA09 ,0x0008);
	boot_send_spi_command_ready(0xCA0A ,0x0000);
	boot_send_spi_command_ready(0xCA0B ,0x0001);//DPTMCTR10

	
	boot_send_spi_command_ready(0xF000 ,0x0055);
	boot_send_spi_command_ready(0xF001 ,0x00AA);
	boot_send_spi_command_ready(0xF002 ,0x0052);
	boot_send_spi_command_ready(0xF003 ,0x0008);
	boot_send_spi_command_ready(0xF004 ,0x0001);//MAUCCTR
	
	boot_send_spi_command_ready(0xB000 ,0x000A);
	boot_send_spi_command_ready(0xB001 ,0x000A);
	boot_send_spi_command_ready(0xB002 ,0x000A);//SETAvDD
	
	boot_send_spi_command_ready(0xB100 ,0x000A);
	boot_send_spi_command_ready(0xB101 ,0x000A);
	boot_send_spi_command_ready(0xB102 ,0x000A);//SETAvEE

	boot_send_spi_command_ready(0xB200 ,0x0002);
	boot_send_spi_command_ready(0xB201 ,0x0002);
	boot_send_spi_command_ready(0xB202 ,0x0002);//SETVCL

	
	boot_send_spi_command_ready(0xB300 ,0x0008);
	boot_send_spi_command_ready(0xB301 ,0x0008);
	boot_send_spi_command_ready(0xB302 ,0x0008);//SETVGH

	boot_send_spi_command_ready(0xB500 ,0x0005);
	boot_send_spi_command_ready(0xB501 ,0x0005);
	boot_send_spi_command_ready(0xB502 ,0x0005);//SETVGL_REG
	
	boot_send_spi_command_ready(0xB600 ,0x0045);
	boot_send_spi_command_ready(0xB601 ,0x0045);
	boot_send_spi_command_ready(0xB602 ,0x0045);//BT1CTR

	
	boot_send_spi_command_ready(0xB700 ,0x0025);
	boot_send_spi_command_ready(0xB701 ,0x0025);
	boot_send_spi_command_ready(0xB702 ,0x0025);//BT2CTR

	
	boot_send_spi_command_ready(0xB800 ,0x0036);
	boot_send_spi_command_ready(0xB801 ,0x0036);
	boot_send_spi_command_ready(0xB802 ,0x0036);//BT3CTR
	
	boot_send_spi_command_ready(0xB900 ,0x0036);
	boot_send_spi_command_ready(0xB901 ,0x0036);
	boot_send_spi_command_ready(0xB902 ,0x0036);//BT4CTR
	
	boot_send_spi_command_ready(0xBA00 ,0x0015);
	boot_send_spi_command_ready(0xBA01 ,0x0015);
	boot_send_spi_command_ready(0xBA02 ,0x0015);//BT5CTR

	boot_send_spi_command_ready(0xBF00 ,0x0001);//VGHCTR
	boot_send_spi_command_ready(0xC200 ,0x0001);//BTENCTR
	boot_send_spi_command_ready(0x1100 ,0x00ff);//SLPOUT
	mdelay(120);
	
	boot_send_spi_command_ready(0x3500 ,0x0000);//TEON
	
	boot_send_spi_command_ready(0x2900 ,0x00ff);//DISON

	mdelay(40);
#endif
}
static void sky_smem_set_lcd_state( uint32_t state)
{
	uint32_t type; 

	type = SMEM_PROC_COMM_CUSTOMER_CMD1_SET_LCD_STATE;
	msm_proc_comm(PCOM_CUSTOMER_CMD1, &type, &state);
}

static int lcdc_common_panel_on(struct platform_device *pdev)
{

	if(!lcdc_state.disp_initialized){
#if(BOARD_VER == PT10)
        send_spi_command(0x11, 0, parameter_list);
#else
		lcdc_common_pt_disp_powerup();
		//lcdc_reset();
		//lcdc_common_pt_panel_setup(); 
		
		boot_send_spi_command_ready(0x1100 ,0x00ff);//SLPOUT
		mdelay(120);
		boot_send_spi_command_ready(0x2900 ,0x00ff);//DISON
		mdelay(40);
#endif

        lcdc_state.disp_initialized = TRUE;
    } 
	else{
		lcdc_reset();
		lcdc_common_pt_panel_setup();
 	}
    lcdc_state.display_on = TRUE;
	sky_smem_set_lcd_state(lcdc_state.display_on);
    return 0;
}

static int lcdc_common_panel_off(struct platform_device *pdev)
{


	if(lcdc_state.disp_powered_up && lcdc_state.display_on) {

#if(BOARD_VER == PT10)
    	send_spi_command(0x10, 0, parameter_list);
#else
	
		//boot_send_spi_command_ready(0x2800 ,0x00ff);
		//mdelay(10);
		//boot_send_spi_command_ready(0x1000 ,0x00ff);
		//mdelay(90);
		//gpio_set_value(LCD_RESET, GPIO_LOW_VALUE);
#endif	
    	lcdc_state.display_on = FALSE;
        lcdc_state.disp_initialized = FALSE;
		sky_smem_set_lcd_state(lcdc_state.display_on);
	}
    return 0;
}
void lcdc_masai_panel_off(int val)
{
	if(!val){
    	boot_send_spi_command_ready(0x2800 ,0x00ff);
        boot_send_spi_command_ready(0x1000 ,0x00ff);
        mdelay(90); 
	}else
		boot_send_spi_command_ready(0x2800 ,0x00ff);
		
}

EXPORT_SYMBOL(lcdc_masai_panel_off);

static void common_spi_init(void)
{
	spi_sclk    = *(lcdc_common_pt_pdata->gpio_num);
    spi_cs      = *(lcdc_common_pt_pdata->gpio_num + 1);
    spi_mosi  	= *(lcdc_common_pt_pdata->gpio_num + 2);
}
static void control_backlight(int level)
{

	int bl_level;
	int i;
	unsigned long flags;
	
	bl_level = 30 - (level * 3);

    if(!level) {
        gpio_set_value(LCD_DIM_CON,GPIO_LOW_VALUE);
        udelay(T_DIS);	
#ifdef F_SKYDISP_MDP_VG_CLEAR_HOLD_CHANGE
		 force_mdp4_overlay_control(1); 
#endif

		return;
    }
#ifdef F_SKYDISP_MDP_VG_CLEAR_HOLD_CHANGE
		force_mdp4_overlay_control(0); 
#endif


	if(level == 1)
		bl_level += 1;
	
	if(level == 11)
		bl_level = 30;
	
	local_save_flags(flags);
	local_irq_disable();
	
    gpio_set_value(LCD_DIM_CON,GPIO_LOW_VALUE);
    udelay(T_DIS);
    gpio_set_value(LCD_DIM_CON,GPIO_HIGH_VALUE);
    udelay(T_ON);

    for(i = 0; i <  bl_level; i++) {
        gpio_set_value(LCD_DIM_CON,GPIO_LOW_VALUE);
        udelay(T_OFF);
        gpio_set_value(LCD_DIM_CON,GPIO_HIGH_VALUE);
     	udelay(T_ON);
     }
	local_irq_restore(flags);

    printk(KERN_WARNING "[SKY_LCD] %s : bl_level = %d \n",__func__,bl_level);


}

static void lcdc_common_pt_set_backlight(struct msm_fb_data_type *mfd)
{

	int bl_level;
	

	int silent_boot = 0;
	int backlight =0;


	bl_level = mfd->bl_level;


	if(sky_sys_rst_is_silent_boot_mode() &&(sky_sys_rst_GetSilentBoot_bl_off() == 0))// 1: LCD-OFF, 0:LCD-ON
	{
		sky_sys_rst_set_smem_info(NULL, NULL, &backlight, &silent_boot);
		silent_mode_bl_set_exit = TRUE;
		printk(KERN_WARNING"#######################\n");
	}
	
	if(sky_sys_rst_is_silent_boot_mode() && sky_sys_rst_GetSilentBoot_bl_off())  // 1이면 LCD_OFF 상태
	{
	    if(silent_mode_skip_complete)
       	{
       		silent_mode_skip_complete = FALSE;
		   	silent_mode_bl_set_exit = TRUE;	
			sky_sys_rst_set_smem_info(NULL, NULL, &backlight, &silent_boot);
       	}		
	    if(silent_mode_2nd_skip)
       	{
       		silent_mode_2nd_skip = FALSE;
			silent_mode_skip_complete = TRUE;
			silent_mode_bl_set_exit = FALSE;
       	}
		if(!silent_mode_1st_skip)	
		{
			silent_mode_1st_skip = TRUE;
			silent_mode_2nd_skip = TRUE;
			silent_mode_bl_set_exit = FALSE;
		}	
		printk(KERN_WARNING"!!!!!!!!!!!!!!!!!!!!!!\n");
		printk("%s :: silent_mode_1st_skip: %d, silent_mode_2nd_skip: %d\n", __func__, silent_mode_1st_skip, silent_mode_2nd_skip);
		printk("%s :: silent_mode_skip_complete: %d, silent_mode_bl_set_exit: %d\n", __func__, silent_mode_skip_complete, silent_mode_bl_set_exit);
	}
	if (silent_mode_bl_set_exit){		

		if(lcdc_state.display_on == TRUE){
			control_backlight(bl_level);
			printk(KERN_WARNING "[SKY_LCD_BACKLIGHT] %s = ON\n",__func__);
		}
		else{
			control_backlight(0);
			printk(KERN_WARNING "[SKY_LCD_BACKLIGHT] %s = OFF\n",__func__);
		}
	
	}


}

#ifdef F_SKYDISP_REMOVE_NOISE_MDP_INIT
int sky_lcd_screen_off(void)
{

//    ENTER_FUNC2();
//    send_spi_command(0x10, 0, parameter_list);
//	 boot_send_spi_command_ready(0x2800 ,0x00ff);
//     boot_send_spi_command_ready(0x1000 ,0x00ff);
//	 lcdc_state.display_on = FALSE;
//     lcdc_state.disp_initialized = FALSE;
  	
//    mdelay(20); //wait for several frame :: guide
   // flag_sky_panel_off = TRUE; 
  
//#ifdef F_SKYDISP_REMOVE_LIGHTNING	
  //  flag_sky_panel_off_blctl = TRUE;
//#endif
//    EXIT_FUNC2();
    return 0;
}
#endif


static int __devinit common_pt_probe(struct platform_device *pdev)
{
    if (pdev->id == 0) {
    	lcdc_common_pt_pdata = pdev->dev.platform_data;
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

static int __init lcdc_common_panel_init(void)
{
    int ret;
    struct msm_panel_info *pinfo;

    ret = platform_driver_register(&this_driver);
    if (ret)
          return ret;

    pinfo = &common_pt_panel_data.panel_info;

	common_spi_init();
	


#ifdef CONFIG_FB_MSM_LCDC_COMMON_WVGA_PANEL
    pinfo->xres = 480;
    pinfo->yres = 800;
    MSM_FB_SINGLE_MODE_PANEL(pinfo);
    pinfo->type = LCDC_PANEL;
    pinfo->pdest = DISPLAY_1;
    pinfo->bl_max = 10;
    pinfo->wait_cycle = 0;
#ifdef CONFIG_FB_MSM_DEFAULT_DEPTH_RGB565
    pinfo->bpp = 16;
#else
    pinfo->bpp = 24;
#endif

#if(BOARD_VER == PT10) //#if(BOARD_VER == PT10) ----------> WS10
	pinfo->fb_num = 2;
	pinfo->clk_rate = 24576 * 1000;
	pinfo->lcdc.h_back_porch =10;
	pinfo->lcdc.h_front_porch = 8;
	pinfo->lcdc.h_pulse_width = 10;
	pinfo->lcdc.v_back_porch = 2;
	pinfo->lcdc.v_front_porch = 2;
	pinfo->lcdc.v_pulse_width = 2;
	pinfo->lcdc.border_clr = 0;	 
	pinfo->lcdc.underflow_clr = 0x00; 	 
	pinfo->lcdc.hsync_skew = 0;
    
#else

	pinfo->fb_num = 2;
	pinfo->clk_rate = 24576 * 1000;
	pinfo->lcdc.h_back_porch =8;
	pinfo->lcdc.h_front_porch = 8;
	pinfo->lcdc.h_pulse_width = 8;
	pinfo->lcdc.v_back_porch = 9;
	pinfo->lcdc.v_front_porch = 9;
	pinfo->lcdc.v_pulse_width = 9;
	pinfo->lcdc.border_clr = 0;	  
	pinfo->lcdc.underflow_clr = 0x00;	
	pinfo->lcdc.hsync_skew = 0;

	  
#endif 
#endif
    ret = platform_device_register(&this_device);
    if (ret) {
          printk(KERN_ERR "%s not able to register the device\n",
                 __func__);
          goto fail_driver;
    }

	 //init_common_backlight(24); //  shinbrad_20110208
    return ret;

fail_driver:
    platform_driver_unregister(&this_driver);

    return ret;
}

static ssize_t event_show(struct device *dev, struct device_attribute *attr,
        char *buf)
{
    //struct switch_dev *sdev = (struct switch_dev *)
        //dev_get_drvdata(dev);

	lcdc_reset();
	lcdc_common_pt_panel_setup();
	*buf = 0;
    return 0;
}


static DEVICE_ATTR(event, S_IRUGO | S_IWUSR, event_show, NULL);

static int create_sky_class(void)
{
    if (!sky_lcd_class) {
        sky_lcd_class = class_create(THIS_MODULE, "sky_ctr");
        if (IS_ERR(sky_lcd_class))
            return PTR_ERR(sky_lcd_class);
        atomic_set(&device_count, 0);
    }

    return 0;
}

static int sky_lcd_dev_register(struct switch_dev *sdev)
{
    int ret;

    if (!sky_lcd_class) {
        ret = create_sky_class();
        if (ret < 0)
            return ret;
    }

    sdev->index = atomic_inc_return(&device_count);
    sdev->dev = device_create(sky_lcd_class, NULL,
        MKDEV(0, sdev->index), NULL, sdev->name);
    if (IS_ERR(sdev->dev))
        return PTR_ERR(sdev->dev);

    ret = device_create_file(sdev->dev, &dev_attr_event);
    if (ret < 0)    
        goto err_create_file_3; 
    dev_set_drvdata(sdev->dev, sdev);
    sdev->state = 0;
    return 0;

err_create_file_3:
    device_destroy(sky_lcd_class, MKDEV(0, sdev->index));
    printk(KERN_ERR "switch: Failed to register driver %s\n", sdev->name);

    return ret;
}

void sky_lcd_dev_unregister(struct switch_dev *sdev)
{

    device_remove_file(sdev->dev, &dev_attr_event); 
    atomic_dec(&device_count);
    dev_set_drvdata(sdev->dev, NULL);
    device_destroy(sky_lcd_class, MKDEV(0, sdev->index));
}


static int __init sky_lcd_class_init(void)
{
    int ret;
    sky_lcd_dev.name="sky_lcd";
    ret=sky_lcd_dev_register(&sky_lcd_dev);  
    return ret;
}

static void __exit sky_lcd_class_exit(void)
{
    sky_lcd_dev_unregister(&sky_lcd_dev); 
    class_destroy(sky_lcd_class);
    
}

module_init(sky_lcd_class_init);
module_exit(sky_lcd_class_exit);

device_initcall(lcdc_common_panel_init);

