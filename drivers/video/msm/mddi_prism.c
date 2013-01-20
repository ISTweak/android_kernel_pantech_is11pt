/* Copyright (c) 2008-2009, Code Aurora Forum. All rights reserved.
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
 

#include "msm_fb.h"
#include "mddihost.h"
#include "mddihosti.h"
#include <mach/vreg.h>
#include <asm/irq.h>
#include <asm/system.h>
#include <mach/gpio.h>


#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/switch.h>


/*LCD HW Feature*/
#define MDDI_RESET_PIN			122			//Low Active
#define LCD_DIM_CON_PIN			130			
#define LCD_VSync_PIN			30

#if(BOARD_VER > WS10)
#define MDDI_PMIC_GPIO	43
#endif


#define GPIO_HIGH_VALUE			1
#define GPIO_LOW_VALUE			0

#define SKY_LCD_WIDTH       	480
#define SKY_LCD_HEIGHT      	800

#define T_EN 	10
#define T_ON	10
#define T_OFF	5	
#define T_DIS	500


#define SKY_LCD_HW_RESET(){ \
	gpio_set_value(MDDI_RESET_PIN, GPIO_HIGH_VALUE); \
	mddi_wait(5); \
    gpio_set_value(MDDI_RESET_PIN ,GPIO_LOW_VALUE); \
    mddi_wait(20); \
    gpio_set_value(MDDI_RESET_PIN, GPIO_HIGH_VALUE); \
    mddi_wait(10); \
}
#ifdef F_SKYDISP_MDP_VG_CLEAR_HOLD_CHANGE
extern void force_mdp4_overlay_unset(void);
extern void force_mdp4_overlay_control(bool onoff);

#endif


static int __devinit mddi_sky_probe(struct platform_device *pdev);	
static int mddi_sky_lcd_on(struct platform_device *pdev);
static int mddi_sky_lcd_off(struct platform_device *pdev);
static void mddi_sky_set_backlight(struct msm_fb_data_type *mfd);


static struct switch_dev sky_lcd_dev;
struct class *sky_lcd_class;
static atomic_t device_count;

static int backlight_count = 0;
typedef struct Lcd_state
{
	int LCD_init;
	int LCD_power;
	int LCD_on;
}Lcd_state,* pLcd_state;
 
static Lcd_state mddi_lcd_state = {0,};

static struct platform_driver this_driver = {
	.probe  = mddi_sky_probe,
	.driver = {
		.name   = "mddi_sky_lcd",
	},
};

static struct msm_fb_panel_data mddi_sky_panel_data = {
	.on = mddi_sky_lcd_on,
	.off = mddi_sky_lcd_off,
	.set_backlight = mddi_sky_set_backlight,

	//.set_vsync_notifier = mddi_sky_vsync_set_handler,   
};

static struct platform_device this_device = {
	.name   = "mddi_sky_lcd",
	.id		= 1,
	.dev	= {
		.platform_data = &mddi_sky_panel_data,
	}
};


static struct msm_panel_common_pdata *mddi_sky_pdata;

static uint32 sky_lcd_gpio_init_table[] = {
	GPIO_CFG(LCD_DIM_CON_PIN,  0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA
),
	GPIO_CFG(MDDI_RESET_PIN,   0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA
),
#if(BOARD_VER > WS10)
	GPIO_CFG(MDDI_PMIC_GPIO,   0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA
),
#endif
};

__attribute__((__aligned__(32)))
static uint8 sky_mddi_lcd_reg_val[][36] = {
	{0x20, 0x00, 0x00, 0x00, 0xC8,},
	{0x05, 0x00, 0x00, 0x00, 0x48, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x0F,},
	{0x46, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,},
	{0x30, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,},
	{0x01, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00,},
	{0x07, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,},
	{0x12, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x76},
	{0x41, 0x00, 0x00, 0x00, 0x63,},
	{0x01, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x67, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00,0x10,0x00, 0x00, 0x00, 0x03,},
	{0x01, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00,0x67, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00,0x01, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00,0x30, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x03,},
	{0x01, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x67, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00,0x10,0x00, 0x00, 0x00, 0x03,},
	{0x01, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00,0x67, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00,0x01, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00,0x30, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x03,},
	{0x01, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x67, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00,0x10,0x00, 0x00, 0x00, 0x03,},
	{0x01, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00,0x67, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00,0x01, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00,0x30, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x03,},
};
	




//static uint16 mddi_sky_vsync_attempts;
static uint32 sky_mddi_rows_per_second		  = 0;	
static uint32 sky_mddi_rows_per_refresh		  = 0;	
static uint32 sky_mddi_usecs_per_refresh 	  = 0;	
//static boolean mddi_sky_monitor_refresh_value = TRUE;
//static boolean mddi_sky_report_refresh_measurements = FALSsE;

//static msm_fb_vsync_handler_type mddi_sky_vsync_handler;
//static void *mddi_sky_vsync_handler_arg;
//static void mddi_sky_vsync_set_handler(msm_fb_vsync_handler_type handler, void *arg);

//static mddi_client_capability_type  MddiCap;

extern boolean mddi_vsync_detect_enabled;

extern bool bWakeupEnabled;
extern int iWakeupCount;
extern int iSleepCount;


static void sky_lcd_gpio_init(uint32 *table, int len, unsigned enable)
{
  int n, rc;

  for (n = 0; n < len; n++) {
  	printk("Cnt:%d", n);
    rc = gpio_tlmm_config(table[n],
      enable ? GPIO_CFG_ENABLE : GPIO_CFG_DISABLE);
    if (rc) 
      printk(KERN_ERR "%s: gpio_tlmm_config(%#x)=%d\n",
        __func__, table[n], rc);

  }
 
}



static int mddi_sky_prim_lcd_initial(void)
{
	int ret = 0;

	mddi_wait(100);
	ret = mddi_queue_register_write(0x20, 0x00, FALSE, 0);
	if(ret){
			printk(KERN_ERR "[MDDI_SKY_LCD] %s: mddi_queue_register_write failed(%d) : %d\n", __func__, 0x20, ret);
	}
	
	ret = mddi_queue_register_write(0x29, 0x00, FALSE, 0);
	if(ret){
			printk(KERN_ERR "[MDDI_SKY_LCD] %s: mddi_queue_register_write failed(%d) : %d\n", __func__, 0x29, ret);
	}
	
	ret = mddi_queue_register_write(0x3A, 0x07, FALSE, 0);
	if(ret){
			printk(KERN_ERR "[MDDI_SKY_LCD] %s: mddi_queue_register_write failed(%d) : %d\n", __func__, 0x3A, ret);
	}
	
	ret = mddi_queue_register_write_extn(0xB2, (uint32 *)sky_mddi_lcd_reg_val[0], MDDI_DATA_PACKET_8_BYTES / 4, FALSE, 0);
	if(ret){
			printk(KERN_ERR "[MDDI_SKY_LCD] %s: mddi_queue_register_write failed(%d) : %d\n", __func__, 0xB2, ret);
	}
	
	ret = mddi_queue_register_write(0xB3, 0x02, FALSE, 0);
	if(ret){
			printk(KERN_ERR "[MDDI_SKY_LCD] %s: mddi_queue_register_write failed(%d) : %d\n", __func__, 0xB3, ret);
	}
	
	ret = mddi_queue_register_write(0xB4, 0x04, FALSE, 0);
	if(ret){
			printk(KERN_ERR "[MDDI_SKY_LCD] %s: mddi_queue_register_write failed(%d) : %d\n", __func__, 0xB4, ret);
	}
	ret = mddi_queue_register_write(0xB5, 0x42, FALSE, 0);
	if(ret){
			printk(KERN_ERR "[MDDI_SKY_LCD] %s: mddi_queue_register_write failed(%d) : %d\n", __func__, 0xB5, ret);
	}


	ret = mddi_queue_register_write_extn(0xB6, (uint32 *)sky_mddi_lcd_reg_val[1], MDDI_DATA_PACKET_24_BYTES / 4, FALSE, 0);
	if(ret){
			printk(KERN_ERR "[MDDI_SKY_LCD] %s: mddi_queue_register_write failed(%d) : %d\n", __func__, 0xB6, ret);
	}
	ret = mddi_queue_register_write_extn(0xB7, (uint32 *)sky_mddi_lcd_reg_val[2], MDDI_DATA_PACKET_20_BYTES / 4, FALSE, 0);
	if(ret){
			printk(KERN_ERR "[MDDI_SKY_LCD] %s: mddi_queue_register_write failed(%d) : %d\n", __func__, 0xB7, ret);
	}
	ret = mddi_queue_register_write_extn(0xE0, (uint32 *)sky_mddi_lcd_reg_val[3], MDDI_DATA_PACKET_24_BYTES / 4, FALSE, 0);
	if(ret){
			printk(KERN_ERR "[MDDI_SKY_LCD] %s: mddi_queue_register_write failed(%d) : %d\n", __func__, 0xE0, ret);
	}
	ret = mddi_queue_register_write_extn(0xC0, (uint32 *)sky_mddi_lcd_reg_val[4], MDDI_DATA_PACKET_8_BYTES / 4, FALSE, 0);
	if(ret){
			printk(KERN_ERR "[MDDI_SKY_LCD] %s: mddi_queue_register_write failed(%d) : %d\n", __func__, 0xC0, ret);
	}
	ret = mddi_queue_register_write_extn(0xC3, (uint32 *)sky_mddi_lcd_reg_val[5], MDDI_DATA_PACKET_20_BYTES / 4, FALSE, 0);
	if(ret){
			printk(KERN_ERR "[MDDI_SKY_LCD] %s: mddi_queue_register_write failed(%d) : %d\n", __func__, 0xC3, ret);
	}
	ret = mddi_queue_register_write_extn(0xC4, (uint32 *)sky_mddi_lcd_reg_val[6], MDDI_DATA_PACKET_24_BYTES / 4, FALSE, 0);
	if(ret){
			printk(KERN_ERR "[MDDI_SKY_LCD] %s: mddi_queue_register_write failed(%d) : %d\n", __func__, 0xC4, ret);
	}

	ret = mddi_queue_register_write(0xC5, 0x69, FALSE, 0);
	if(ret){
			printk(KERN_ERR "[MDDI_SKY_LCD] %s: mddi_queue_register_write failed(%d) : %d\n", __func__, 0xC5, ret);
	}

	
	ret = mddi_queue_register_write_extn(0xC6, (uint32 *)sky_mddi_lcd_reg_val[7], MDDI_DATA_PACKET_8_BYTES / 4, FALSE, 0);
	if(ret){
			printk(KERN_ERR "[MDDI_SKY_LCD] %s: mddi_queue_register_write failed(%d) : %d\n", __func__, 0xC6, ret);
	}
	ret = mddi_queue_register_write_extn(0xD0, (uint32 *)sky_mddi_lcd_reg_val[8], MDDI_DATA_PACKET_36_BYTES / 4, FALSE, 0);
	if(ret){
			printk(KERN_ERR "[MDDI_SKY_LCD] %s: mddi_queue_register_write failed(%d) : %d\n", __func__, 0xD0, ret);
	}
	ret = mddi_queue_register_write_extn(0xD1, (uint32 *)sky_mddi_lcd_reg_val[9], MDDI_DATA_PACKET_36_BYTES / 4, FALSE, 0);
	if(ret){
			printk(KERN_ERR "[MDDI_SKY_LCD] %s: mddi_queue_register_write failed(%d) : %d\n", __func__, 0xD1, ret);
	}
	ret = mddi_queue_register_write_extn(0xD2, (uint32 *)sky_mddi_lcd_reg_val[10], MDDI_DATA_PACKET_36_BYTES / 4, FALSE, 0);
	if(ret){
			printk(KERN_ERR "[MDDI_SKY_LCD] %s: mddi_queue_register_write failed(%d) : %d\n", __func__, 0xD2, ret);
	}
	ret = mddi_queue_register_write_extn(0xD3, (uint32 *)sky_mddi_lcd_reg_val[11], MDDI_DATA_PACKET_36_BYTES / 4, FALSE, 0);
	if(ret){
			printk(KERN_ERR "[MDDI_SKY_LCD] %s: mddi_queue_register_write failed(%d) : %d\n", __func__, 0xD3, ret);
	}
	ret = mddi_queue_register_write_extn(0xD4, (uint32 *)sky_mddi_lcd_reg_val[12], MDDI_DATA_PACKET_36_BYTES / 4, FALSE, 0);
	if(ret){
			printk(KERN_ERR "[MDDI_SKY_LCD] %s: mddi_queue_register_write failed(%d) : %d\n", __func__, 0xD4, ret);
	}
	ret = mddi_queue_register_write_extn(0xD5, (uint32 *)sky_mddi_lcd_reg_val[13], MDDI_DATA_PACKET_36_BYTES / 4, FALSE, 0);
	if(ret){
			printk(KERN_ERR "[MDDI_SKY_LCD] %s: mddi_queue_register_write failed(%d) : %d\n", __func__, 0xD5, ret);
	}

	
	ret = mddi_queue_register_write(0x11, 0x00, FALSE, 0);
	if(ret){
			printk(KERN_ERR "[MDDI_SKY_LCD] %s: mddi_queue_register_write failed(%d) : %d\n", __func__, 0x11, ret);
	}


	mddi_wait(100);
	
	
	if (ret) {
		return FALSE;
	}
	else {
		return TRUE;
	}
}



void common_backlight(int level)
{
	int bl_value,i;
    unsigned long frags; 
    bl_value = 32-(level*2);
    if(!level) {
        gpio_set_value(LCD_DIM_CON_PIN,GPIO_LOW_VALUE);
        udelay(T_DIS);
#ifdef F_SKYDISP_MDP_VG_CLEAR_HOLD_CHANGE
		force_mdp4_overlay_control(1); 
#endif

		return;
    }
#ifdef F_SKYDISP_MDP_VG_CLEAR_HOLD_CHANGE
	force_mdp4_overlay_control(0); 
#endif

	
	local_save_flags(frags);
	local_irq_disable();
    gpio_set_value(LCD_DIM_CON_PIN,GPIO_LOW_VALUE);
    udelay(T_DIS);
    gpio_set_value(LCD_DIM_CON_PIN,GPIO_HIGH_VALUE);
    udelay(T_ON);

    for(i = 0; i < bl_value; i++) {
        gpio_set_value(LCD_DIM_CON_PIN,GPIO_LOW_VALUE);
        udelay(T_OFF);
        gpio_set_value(LCD_DIM_CON_PIN,GPIO_HIGH_VALUE);
     	udelay(T_ON);
    	}
	local_irq_restore(frags);
	printk(KERN_WARNING "%s: set lcd backlight! %d\n",
				  __func__,bl_value);

}

static void mddi_sky_set_backlight(struct msm_fb_data_type *mfd)
{

	int bl_level;
	bl_level = mfd->bl_level;
	if(mddi_lcd_state.LCD_on == FALSE && (backlight_count)){
		common_backlight(bl_level);
		
	}
	else{
		common_backlight(0);
	}
	backlight_count = 1;
}

static int mddi_sky_lcd_on(struct platform_device *pdev)
{
	if(mddi_lcd_state.LCD_on == TRUE && mddi_lcd_state.LCD_power == TRUE)
	{
		mddi_queue_register_write(0x11, 0x00, FALSE, 0);
		mdelay(150);	
		mddi_queue_register_write(0x29, 0x00, FALSE, 0);

	}
	else
	{
		SKY_LCD_HW_RESET();
		mddi_sky_prim_lcd_initial();
	}
	mddi_lcd_state.LCD_on	 = FALSE; 
	mddi_lcd_state.LCD_power = FALSE;
	printk(KERN_WARNING"[SKY_LCD] %s \n",__func__);

	return 0;
}


static int mddi_sky_lcd_off(struct platform_device *pdev)
{
	if(mddi_lcd_state.LCD_on == FALSE && mddi_lcd_state.LCD_power == FALSE)
	{

		mddi_lcd_state.LCD_on	 = TRUE; 
		mddi_lcd_state.LCD_power = TRUE;
	}
	printk(KERN_WARNING"[SKY_LCD] %s \n",__func__);
	return 0;
}
void mddi_laser_panel_off(void)
{	
	mddi_queue_register_write(0x28, 0x00, FALSE, 0);	
	mddi_queue_register_write(0x10, 0x00, FALSE, 0);   
	mdelay(150); 
	printk(KERN_WARNING"[SKY_LCD] %s \n",__func__);
}
EXPORT_SYMBOL(mddi_laser_panel_off);

#if 0
void mddi_sky_lcd_vsync_detected(boolean detected)
{
	/* static timetick_type start_time = 0; */
	static struct timeval start_time;
	static boolean first_time = TRUE;
	/* uint32 mdp_cnt_val = 0; */
	/* timetick_type elapsed_us; */
	struct timeval now;
	uint32 elapsed_us;
	uint32 num_vsyncs;

	if ((detected) || (mddi_sky_vsync_attempts > 5)) {
		if ((detected) && (mddi_sky_monitor_refresh_value)) {
			/* if (start_time != 0) */
			if (!first_time) {
				jiffies_to_timeval(jiffies, &now);
				elapsed_us =
				    (now.tv_sec - start_time.tv_sec) * 1000000 +
				    now.tv_usec - start_time.tv_usec;
				/*
				* LCD is configured for a refresh every usecs,
				* so to determine the number of vsyncs that
				* have occurred since the last measurement add
				* half that to the time difference and divide
				* by the refresh rate.
				*/
				num_vsyncs = (elapsed_us +
					      (sky_mddi_usecs_per_refresh >>
					       1)) /
				    sky_mddi_usecs_per_refresh;
				/*
				 * LCD is configured for * hsyncs (rows) per
				 * refresh cycle. Calculate new rows_per_second
				 * value based upon these new measurements.
				 * MDP can update with this new value.
				 */
				sky_mddi_rows_per_second =
				    (sky_mddi_rows_per_refresh * 1000 *
				     num_vsyncs) / (elapsed_us / 1000);
			}
			/* start_time = timetick_get(); */
			first_time = FALSE;
			jiffies_to_timeval(jiffies, &start_time);
			if (mddi_sky_report_refresh_measurements) {
				/* mdp_cnt_val = MDP_LINE_COUNT; */
			}
		}
		/* if detected = TRUE, client initiated wakeup was detected */
		if (mddi_sky_vsync_handler != NULL) {
			(*mddi_sky_vsync_handler)
			    (mddi_sky_vsync_handler_arg);
			mddi_sky_vsync_handler_arg = NULL;
		}
		mddi_vsync_detect_enabled = FALSE;
		mddi_sky_vsync_attempts = 0;
		
#if 0 //- TODO		
		/* need to clear this vsync wakeup */ 
		if (!mddi_queue_register_write_int(REG_INTR, 0x0000)) {
			MDDI_MSG_ERR("Vsync interrupt clear failed!\n");
		}
#endif		

		if (!detected) {
			/* give up after 5 failed attempts but show error */
			MDDI_MSG_NOTICE("Vsync detection failed!\n");
		} else if ((mddi_sky_monitor_refresh_value) &&
			(mddi_sky_report_refresh_measurements)) {
			MDDI_MSG_NOTICE("  Lines Per Second=%d!\n",
				sky_mddi_rows_per_second);
		}
	} else
		/* if detected = FALSE, we woke up from hibernation, but did not
		 * detect client initiated wakeup.
		 */
		mddi_sky_vsync_attempts++;
}



void mddi_sky_vsync_set_handler(msm_fb_vsync_handler_type handler, void *arg)
{
	boolean error = FALSE;
	unsigned long flags;

	/* Disable interrupts */
	spin_lock_irqsave(&mddi_host_spin_lock, flags);
	/* INTLOCK(); */

	if (mddi_sky_vsync_handler != NULL)
		error = TRUE;

	/* Register the handler for this particular GROUP interrupt source */
	mddi_sky_vsync_handler = handler;
	mddi_sky_vsync_handler_arg = arg;

	/* Restore interrupts */
	spin_unlock_irqrestore(&mddi_host_spin_lock, flags);
	/* INTFREE(); */

	if (error)
		MDDI_MSG_ERR("MDDI: Previous Vsync handler never called\n");
#if 0	//- TODO
	/* Enable the vsync wakeup */
	mddi_queue_register_write(REG_INTR, 0x8100, FALSE, 0);
#endif

	mddi_sky_vsync_attempts = 1;
	mddi_vsync_detect_enabled = TRUE;
}				
#endif
static void lcd_state_init(void)
{

	mddi_lcd_state.LCD_init 	= TRUE;
	mddi_lcd_state.LCD_on   	= TRUE;
	mddi_lcd_state.LCD_power	= TRUE;
}
static int __devinit mddi_sky_probe(struct platform_device *pdev)
{
	if (pdev->id == 0) {
		mddi_sky_pdata = pdev->dev.platform_data;
		return 0;
	}
	lcd_state_init();
	#if(BOARD_VER == WS10)	
	SKY_LCD_HW_RESET();
	mddi_sky_prim_lcd_initial();
	#endif
	msm_fb_add_device(pdev);
	return 0;
}

static int __init mddi_sky_init(void)
{
	int ret;
	struct msm_panel_info *pinfo;

	/* Timing variables for tracking vsync*/
	/* Frame Frequency = Fosc/(8*NHW|PIHW*(Line+B) [Hz] = 60Hz
 	 * Fosc : 20Mhz, Line = 480, B(B+F proch) = 8+8 , NHW = 83
	 * horizontal count = 320+2+2
	 * vertical count   = 480+16 - Vertical B/F porch =8/8
	 * dot_clock >= 35Mhz
	*/

	
	
	sky_mddi_rows_per_second      = 25000000 / 480;//29760;	// 60Hz * 496
	sky_mddi_rows_per_refresh     = 818; 	// vertical count
	sky_mddi_usecs_per_refresh    = (818 + 480) / 25000000;//16666;	// 1/60Hz

	sky_lcd_gpio_init(sky_lcd_gpio_init_table, 
						ARRAY_SIZE(sky_lcd_gpio_init_table), 1);
	
	gpio_set_value(MDDI_RESET_PIN, 1);
#if(BOARD_VER > WS10)
	gpio_set_value(MDDI_PMIC_GPIO, 1);
#endif	
	ret = platform_driver_register(&this_driver);
	if (!ret) {
		pinfo = &mddi_sky_panel_data.panel_info;
		pinfo->xres = SKY_LCD_WIDTH;
		pinfo->yres = SKY_LCD_HEIGHT;
		pinfo->type = MDDI_PANEL;
		pinfo->pdest = DISPLAY_1;
		pinfo->mddi.vdopkt = MDDI_DEFAULT_PRIM_PIX_ATTR;
		pinfo->wait_cycle = 0;
		pinfo->bpp = 24;
		pinfo->fb_num = 2;
		pinfo->clk_rate = 368000 * 1000;
		pinfo->clk_min = 368000 * 1000;
		pinfo->clk_max = 368000 * 1000;
		pinfo->lcd.vsync_enable = TRUE;
		pinfo->lcd.refx100 =
			(sky_mddi_rows_per_second * 100) /
			sky_mddi_rows_per_refresh;
		pinfo->lcd.v_back_porch = 8;
		pinfo->lcd.v_front_porch = 10;
		pinfo->lcd.v_pulse_width = 0;
		pinfo->lcd.hw_vsync_mode = FALSE;
		pinfo->lcd.vsync_notifier_period = (1 * HZ);
		pinfo->bl_max = 16;
		//pinfo->lcd.rev = 2;
		

		ret = platform_device_register(&this_device);
		if (ret)
			platform_driver_unregister(&this_driver);
	
	}
/* - TODO
	if (!ret)
		mddi_lcd.vsync_detected = mddi_sky_lcd_vsync_detected;
*/


	return ret;
}
static ssize_t event_show(struct device *dev, struct device_attribute *attr,
        char *buf)
{
    //struct switch_dev *sdev = (struct switch_dev *)
        //dev_get_drvdata(dev);

	SKY_LCD_HW_RESET();
	mddi_sky_prim_lcd_initial();

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

module_init(mddi_sky_init);

#if 0
/* Copyright (c) 2008-2010, Code Aurora Forum. All rights reserved.
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

#include "msm_fb.h"
#include "mddihost.h"
#include "mddihosti.h"

static int prism_lcd_on(struct platform_device *pdev);
static int prism_lcd_off(struct platform_device *pdev);

static int prism_lcd_on(struct platform_device *pdev)
{
	/* Set the MDP pixel data attributes for Primary Display */
	mddi_host_write_pix_attr_reg(0x00C3);

	return 0;
}

static int prism_lcd_off(struct platform_device *pdev)
{
	return 0;
}

static int __devinit prism_probe(struct platform_device *pdev)
{
	msm_fb_add_device(pdev);

	return 0;
}

static struct platform_driver this_driver = {
	.probe  = prism_probe,
	.driver = {
		.name   = "mddi_prism_wvga",
	},
};

static struct msm_fb_panel_data prism_panel_data = {
	.on = prism_lcd_on,
	.off = prism_lcd_off,
};

static struct platform_device this_device = {
	.name   = "mddi_prism_wvga",
	.id	= 0,
	.dev	= {
		.platform_data = &prism_panel_data,
	}
};

static int __init prism_init(void)
{
	int ret;
	struct msm_panel_info *pinfo;

#ifdef CONFIG_FB_MSM_MDDI_AUTO_DETECT
	u32 id;

	ret = msm_fb_detect_client("mddi_prism_wvga");
	if (ret == -ENODEV)
		return 0;

	if (ret) {
		id = mddi_get_client_id();

		if (((id >> 16) != 0x4474) || ((id & 0xffff) == 0x8960))
			return 0;
	}
#endif
	ret = platform_driver_register(&this_driver);
	if (!ret) {
		pinfo = &prism_panel_data.panel_info;
		pinfo->xres = 800;
		pinfo->yres = 480;
		MSM_FB_SINGLE_MODE_PANEL(pinfo);
		pinfo->type = MDDI_PANEL;
		pinfo->pdest = DISPLAY_1;
		pinfo->mddi.vdopkt = MDDI_DEFAULT_PRIM_PIX_ATTR;
		pinfo->wait_cycle = 0;
		pinfo->bpp = 18;
		pinfo->fb_num = 2;
		pinfo->clk_rate = 153600000;
		pinfo->clk_min = 140000000;
		pinfo->clk_max = 160000000;
		pinfo->lcd.vsync_enable = TRUE;
		pinfo->lcd.refx100 = 6050;
		pinfo->lcd.v_back_porch = 23;
		pinfo->lcd.v_front_porch = 20;
		pinfo->lcd.v_pulse_width = 105;
		pinfo->lcd.hw_vsync_mode = TRUE;
		pinfo->lcd.vsync_notifier_period = 0;

		ret = platform_device_register(&this_device);
		if (ret)
			platform_driver_unregister(&this_driver);
	}

	return ret;
}

module_init(prism_init);
#endif

