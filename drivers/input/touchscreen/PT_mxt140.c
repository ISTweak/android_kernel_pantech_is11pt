/* file_name: PT_qt602240.c
 *
 * description: Quantum TSP driver.
 *
 * Copyright (C) 2008-2010 Atmel Co. Ltd.
 *
 */

#include <linux/module.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/earlysuspend.h>
#include <asm/io.h>
#include <linux/gpio.h>
#include <mach/vreg.h>
#include <mach/gpio.h>
#include <linux/miscdevice.h>
#include <linux/hrtimer.h>
#include <linux/mutex.h>
#include <linux/pm.h>
#include <linux/slab.h>
#include <asm/mach-types.h>
#include "PT_mxt140.h"


#define TOUCH_IO
#define SKY_PROCESS_CMD_KEY
//#define NOINITCHIP
#define RECHECK_AFTER_CALIBRATION
//#define USE_FACE_SUP

#ifdef TOUCH_IO
/* File IO */
static void apply_touch_config(void);
static uint8_t qt_charger_mode_config(unsigned long mode);
/*static*/ void set_charger_mode(unsigned long arg);
void  clear_event(void);
int  qt602240_front_test_init(void);

static int open(struct inode *inode, struct file *file);
static int release(struct inode *inode, struct file *file);
static int ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);

extern void gp2ap002s00f_restart(void);

//20100717 jjangu_device
void init_hw_chg_reset(void);

static struct file_operations fops = 
{
    .owner =    THIS_MODULE,
    .ioctl =    ioctl,
    .open =     open,
    .release =  release
};

static struct miscdevice touch_io = 
{
	.minor =    MISC_DYNAMIC_MINOR,
	.name =     "qt602240",
	.fops =     &fops
};

typedef enum 
{
    POWER_CONFIG_IDLEACQINT=1,
    POWER_CONFIG_ACTVACQINT,
    POWER_CONFIG_ACTV2IDLETO,        

    ACQUISITION_CONFIG_CHRGTIME=11,
    ACQUISITION_CONFIG_ATCHDRIFT,
    ACQUISITION_CONFIG_TCHDRIFT,
    ACQUISITION_CONFIG_DRIFTST,
    ACQUISITION_CONFIG_TCHAUTOCAL,
    ACQUISITION_CONFIG_SYNC,
    ACQUISITION_CONFIG_ATCHCALST,
    ACQUISITION_CONFIG_ATCHCALSTHR,   
#if defined(__VER_2_0__)
	ACQUISITION_CONFIG_ATCHFRCCALTHR,   
	ACQUISITION_CONFIG_ATCHFRCCALRATION,   
#endif
    TOUCHSCREEN_CONFIG_CTRL=31,
    TOUCHSCREEN_CONFIG_XORIGIN,
    TOUCHSCREEN_CONFIG_YORIGIN,
    TOUCHSCREEN_CONFIG_XSIZE,
    TOUCHSCREEN_CONFIG_YSIZE,
    TOUCHSCREEN_CONFIG_AKSCFG,
    TOUCHSCREEN_CONFIG_BLEN,
    TOUCHSCREEN_CONFIG_TCHTHR,
    TOUCHSCREEN_CONFIG_TCHDI,
    TOUCHSCREEN_CONFIG_ORIENT,
    TOUCHSCREEN_CONFIG_MRGTIMEOUT,
    TOUCHSCREEN_CONFIG_MOVHYSTI,
    TOUCHSCREEN_CONFIG_MOVHYSTN,
    TOUCHSCREEN_CONFIG_MOVFILTER,
    TOUCHSCREEN_CONFIG_NUMTOUCH,
    TOUCHSCREEN_CONFIG_MRGHYST,
    TOUCHSCREEN_CONFIG_MRGTHR,
    TOUCHSCREEN_CONFIG_AMPHYST,
    TOUCHSCREEN_CONFIG_XRANGE,
    TOUCHSCREEN_CONFIG_YRANGE,
    TOUCHSCREEN_CONFIG_XLOCLIP,
    TOUCHSCREEN_CONFIG_XHICLIP,
    TOUCHSCREEN_CONFIG_YLOCLIP,
    TOUCHSCREEN_CONFIG_YHICLIP,
    TOUCHSCREEN_CONFIG_XEDGECTRL,
    TOUCHSCREEN_CONFIG_XEDGEDIST,
    TOUCHSCREEN_CONFIG_YEDGECTRL,
    TOUCHSCREEN_CONFIG_YEDGEDIST,
    TOUCHSCREEN_CONFIG_JUMPLIMIT,
#if defined(__VER_2_0__)
       TOUCHSCREEN_CONFIG_TCHHYST,
#endif	
    KEYARRAY_CONFIG_CTRL=71,
    KEYARRAY_CONFIG_XORIGIN,
    KEYARRAY_CONFIG_YORIGIN,
    KEYARRAY_CONFIG_XSIZE,
    KEYARRAY_CONFIG_YSIZE,
    KEYARRAY_CONFIG_AKSCFG,
    KEYARRAY_CONFIG_BLEN,
    KEYARRAY_CONFIG_TCHTHR,
    KEYARRAY_CONFIG_TCHDI,

    COMC_CONFIG_CTRL=91,
    COMC_CONFIG_COMMAND,

    GPIOPWM_CONFIG_CTRL=101,
    GPIOPWM_CONFIG_REPORTMASK,
    GPIOPWM_CONFIG_DIR,
    GPIOPWM_CONFIG_INTPULLUP,
    GPIOPWM_CONFIG_OUT,
    GPIOPWM_CONFIG_WAKE,
    GPIOPWM_CONFIG_PWM,
    GPIOPWM_CONFIG_PERIOD,
    GPIOPWM_CONFIG_DUTY0,
    GPIOPWM_CONFIG_DUTY1,
    GPIOPWM_CONFIG_DUTY2,
    GPIOPWM_CONFIG_DUTY3,
    GPIOPWM_CONFIG_TRIGGER0,
    GPIOPWM_CONFIG_TRIGGER1,
    GPIOPWM_CONFIG_TRIGGER2,
    GPIOPWM_CONFIG_TRIGGER3,

    GRIPFACESUPPRESSION_CONFIG_CTRL=131,
    GRIPFACESUPPRESSION_CONFIG_XLOGRIP,
    GRIPFACESUPPRESSION_CONFIG_XHIGRIP,
    GRIPFACESUPPRESSION_CONFIG_YLOGRIP,
    GRIPFACESUPPRESSION_CONFIG_YHIGRIP,
    GRIPFACESUPPRESSION_CONFIG_MAXTCHS,
    GRIPFACESUPPRESSION_CONFIG_RESERVED,
    GRIPFACESUPPRESSION_CONFIG_SZTHR1,
    GRIPFACESUPPRESSION_CONFIG_SZTHR2,
    GRIPFACESUPPRESSION_CONFIG_SHPTHR1,
    GRIPFACESUPPRESSION_CONFIG_SHPTHR2,
    GRIPFACESUPPRESSION_CONFIG_SUPEXTTO,

    NOISE_SUPPRESSION_CONFIG_CTRL=161,
    NOISE_SUPPRESSION_CONFIG_RESERVED,
    NOISE_SUPPRESSION_CONFIG_RESERVED1,
    NOISE_SUPPRESSION_CONFIG_GCAFUL,
    NOISE_SUPPRESSION_CONFIG_GCAFLL,
    NOISE_SUPPRESSION_CONFIG_ACTVGCAFVALID,
    NOISE_SUPPRESSION_CONFIG_NOISETHR,
    NOISE_SUPPRESSION_CONFIG_FREQHOPSCALE,
    NOISE_SUPPRESSION_CONFIG_FREQ0,
    NOISE_SUPPRESSION_CONFIG_FREQ1,
    NOISE_SUPPRESSION_CONFIG_FREQ2,
    NOISE_SUPPRESSION_CONFIG_FREQ3,
    NOISE_SUPPRESSION_CONFIG_FREQ4,
    NOISE_SUPPRESSION_CONFIG_IDLEGCAFVALID,

    PROXIMITY_CONFIG_CTRL=201,
    PROXIMITY_CONFIG_XORIGIN,
    PROXIMITY_CONFIG_YORIGIN,
    PROXIMITY_CONFIG_XSIZE,
    PROXIMITY_CONFIG_YSIZE,
    PROXIMITY_CONFIG_BLEN,
    PROXIMITY_CONFIG_TCHTHR,
    PROXIMITY_CONFIG_TCHDI,
    PROXIMITY_CONFIG_AVERAGE,
    PROXIMITY_CONFIG_RATE,
    PROXIMITY_CONFIG_FXDDTHR,
    PROXIMITY_CONFIG_FXDDI,
    PROXIMITY_CONFIG_MVNULLRATE,
    PROXIMITY_CONFIG_MVDTHR,

    ONETOUCH_GESTURE_CONFIG_CTRL=221,
    ONETOUCH_GESTURE_CONFIG_NUMGEST,
    ONETOUCH_GESTURE_CONFIG_GESTEN,
    ONETOUCH_GESTURE_CONFIG_PRESSPROC,
    ONETOUCH_GESTURE_CONFIG_TAPTO,
    ONETOUCH_GESTURE_CONFIG_FLICKTO,
    ONETOUCH_GESTURE_CONFIG_DRAGTO,
    ONETOUCH_GESTURE_CONFIG_SPRESSTO,
    ONETOUCH_GESTURE_CONFIG_LPRESSTO,
    ONETOUCH_GESTURE_CONFIG_REPPRESSTO,
    ONETOUCH_GESTURE_CONFIG_FLICKTHR,
    ONETOUCH_GESTURE_CONFIG_DRAGTHR,
    ONETOUCH_GESTURE_CONFIG_TAPTHR,
    ONETOUCH_GESTURE_CONFIG_THROWTHR,
    ONETOUCH_GESTURE_CONFIG_PROCESS,

    SELFTEST_CONFIG_CTRL=241,
    SELFTEST_CONFIG_CMD,

    TWOTOUCH_GESTURE_CONFIG_CTRL=251,
    TWOTOUCH_GESTURE_CONFIG_NUMGEST,
    TWOTOUCH_GESTURE_CONFIG_GESTEN,
    TWOTOUCH_GESTURE_CONFIG_ROTATETHR,
    TWOTOUCH_GESTURE_CONFIG_ZOOMTHR,

    CTE_CONFIG_CTRL=271,
    CTE_CONFIG_CMD,
    CTE_CONFIG_MODE,
    CTE_CONFIG_IDLEGCAFDEPTH,
    CTE_CONFIG_ACTVGCAFDEPTH,
    CTE_CONFIG_VOLTAGE,


    APPLY_TOUCH_CONFIG=501,


    TOUCH_CHARGER_MODE = 701
}CONFIG_CMD;

typedef enum
{
    BATTERY_PLUGGED_NONE = 0,
    BATTERY_PLUGGED_AC = 1,
    BATTERY_PLUGGED_USB = 2,
    BATTERY_PLUGGED_SLEEP = 10
} CHARGER_MODE;

typedef enum
{
	CHARGER_UNPLUGGED_IDLEACQINT = 32,
    CHARGER_UNPLUGGED_ACTVACQINT = 16,
	CHARGER_UNPLUGGED_ACTV2IDLETO = 20,
	#if defined(__VER_2_0__)
		CHARGER_UNPLUGGED_TCHTHR =26,
		#if 0
		#if	(BOARD_VER ==MOON_ES10)
	    CHARGER_UNPLUGGED_TCHTHR = 34/*38*//*50*/, //terry test
	    #elif (BOARD_VER ==MOON_TP10)
        CHARGER_UNPLUGGED_TCHTHR = 40/*50*/,
	    #else
        CHARGER_UNPLUGGED_TCHTHR = 40/*50*//*50*//*30*//*50*/, //terry test
        #endif
		#endif //0
    #else
		CHARGER_UNPLUGGED_TCHTHR =26,
		#if 0
    	#if	(BOARD_VER ==MOON_ES10)
	    CHARGER_UNPLUGGED_TCHTHR = 30 //terry test
	    #else
        CHARGER_UNPLUGGED_TCHTHR = 43/*50*/, //terry test
        #endif
		#endif //0
    #endif
	CHARGER_UNPLUGGED_TCHDI = 2,
	CHARGER_UNPLUGGED_MOVFILTER = 15,	
	CHARGER_UNPLUGGED_CONFIG_NOISETHR = 30,
	CHARGER_UNPLUGGED_CONFIG_FREQ0 = 5,
	CHARGER_UNPLUGGED_CONFIG_FREQ1 = 10,
	CHARGER_UNPLUGGED_CONFIG_FREQ2 = 15,
	CHARGER_UNPLUGGED_CONFIG_FREQ3 = 25,
	CHARGER_UNPLUGGED_CONFIG_FREQ4 = 30,
	CHARGER_UNPLUGGED_IDLEGCAFDEPTH = 8,
	CHARGER_UNPLUGGED_ACTVGCAFDEPTH = 12,
	#if defined(__VER_2_0__)
    #if	1//(BOARD_VER ==MOON_ES10)
	CHARGER_UNPLUGGED_TCHHYSYT = 7,
	//#elif (BOARD_VER ==MOON_TP10)
    //CHARGER_UNPLUGGED_TCHHYSYT = 10/*12*/,
	#else
    CHARGER_UNPLUGGED_TCHHYSYT = 10/*12*//*7*//*12*/ /*13*/ /*8*/,
    #endif
    #endif

	CHARGER_PLUGGED_IDLEACQINT = 255,
	CHARGER_PLUGGED_ACTVACQINT = 255,
	CHARGER_PLUGGED_ACTV2IDLETO = 20,
	#if defined(__VER_2_0__)
        #if	1//(BOARD_VER ==MOON_ES10)
	    CHARGER_PLUGGED_TCHTHR = 45, //terry test
	    //#elif (BOARD_VER ==MOON_TP10)
        //CHARGER_PLUGGED_TCHTHR = 70,
	    #else
        CHARGER_PLUGGED_TCHTHR = 70/*36*//*50*/, //terry test
        #endif
    #else
	    #if	1//(BOARD_VER ==MOON_ES10)
	    CHARGER_PLUGGED_TCHTHR = 42/*50*/, //terry test
	    #else
        CHARGER_PLUGGED_TCHTHR = 48/*50*/, //terry test
        #endif
    #endif

   	CHARGER_PLUGGED_TCHDI = 3,
	CHARGER_PLUGGED_MOVFILTER = 15,	
	CHARGER_PLUGGED_CONFIG_NOISETHR = 30,
	CHARGER_PLUGGED_CONFIG_FREQ0 = 5,
	CHARGER_PLUGGED_CONFIG_FREQ1 = 10,
	CHARGER_PLUGGED_CONFIG_FREQ2 = 15,
	CHARGER_PLUGGED_CONFIG_FREQ3 = 25,
	CHARGER_PLUGGED_CONFIG_FREQ4 = 30,
	CHARGER_PLUGGED_IDLEGCAFDEPTH=16,
	CHARGER_PLUGGED_ACTVGCAFDEPTH = 32,
	#if defined(__VER_2_0__)
    #if	1//(BOARD_VER ==MOON_ES10)
	CHARGER_PLUGGED_TCHHYSYT = 11,
	//#elif (BOARD_VER ==MOON_TP10)
    //CHARGER_PLUGGED_TCHHYSYT = 17,
	#else
    CHARGER_PLUGGED_TCHHYSYT = 17/*13*//*9*/,
    #endif
    #endif

} CHARGER_THR;

#endif //TOUCH_IO

#define _SUPPORT_MULTITOUCH_
#define PRINTF_ENABLE
#define OPTION_WRITE_CONFIG
#define QT_FIRMUP_ENABLE
//#undef QT_FIRMUP_ENABLE

#ifdef QT_FIRMUP_ENABLE
#if defined(__VER_2_0__)
#if	1//(BOARD_VER ==MOON_ES10)
unsigned char QT602240_firmware[] = {
#include "ATMXT140__APPv20AA.h"
#else
unsigned char QT602240_firmware[] = {
#include "ATMXT140__APPv21AA.h"
#endif
};
#else
unsigned char QT602240_firmware[] = {
#include "ATMXT140__APPv10AC.h"
};
#endif

#define QT_WAITING_BOOTLOAD_COMMAND 0xC0
#define QT_WAITING_FRAME_DATA       0x80
#define QT_FRAME_CRC_CHECK          0x02
#define QT_FRAME_CRC_PASS           0x04
#define QT_FRAME_CRC_FAIL           0x03

#define I2C_APPL_ADDR_0         0x94
#define I2C_APPL_ADDR_1         0x96
#define I2C_BOOT_ADDR_0         0x48
#define I2C_BOOT_ADDR_1         0x4A

#define NUM_OF_I2C_ADDR    4

uint8_t i2c_addresses[] =
{
    I2C_APPL_ADDR_0,
    I2C_APPL_ADDR_1,
    I2C_BOOT_ADDR_0,
    I2C_BOOT_ADDR_1,
};

//static        void    __iomem         *gpio_pend_mask_mem;

//#define       INT_PEND_BASE   0xE0200A44

//20100819 TERRY ES6 =>
#if	0//(BOARD_VER == MOON_ES20)||(BOARD_VER == MOON_ES10)||(BOARD_VER == MOON_WS20)
extern int touch_id_type;
#endif

extern struct input_dev *global_input_dev;
//20100819 TERRY ES6 <=


uint8_t QT_i2c_address;

void QT_reprogram(void);
uint8_t QT_Boot_no_reset(void);
uint8_t QT_Boot(void);
#endif //QT_FIRMUP_ENABLE

#define  UPDATE_QT602240_FLASH          0

#define QT602240_I2C_ADDR       0x4A
#define QT602240_I2C_BOOT_ADDR  0x24

//#define IRQ_TOUCH_INT S3C_GPIOINT(J0,5)
#define GPIO_TOUCH_RST_N 166
#define GPIO_TOUCH_CHG  112
#define IRQ_TOUCH_INT   gpio_to_irq(GPIO_TOUCH_CHG)


/* -------------------------------------------------------------------- */
/* debug option */
/* -------------------------------------------------------------------- */
//#define TOUCH_DBG_ENABLE
#ifdef TOUCH_DBG_ENABLE
#define dbg(fmt, args...)   printk("[TOUCH]" fmt, ##args)
#else
#define dbg(fmt, args...)
#endif
#define dbg_func_in()       dbg("[FUNC_IN] %s\n", __func__)
#define dbg_func_out()      dbg("[FUNC_OUT] %s\n", __func__)
#define dbg_line()          dbg("[LINE] %d(%s)\n", __LINE__, __func__)
/* -------------------------------------------------------------------- */


#define SCREEN_RESOLUTION_X             480//320
#define SCREEN_RESOLUTION_Y             800/*420*///480

#define KEY_AREA_X		50/*40*/
#define KEY_AREA_Y_GAP	40//20

#define KEY_MENU_AREA_X_MIN	17
#define KEY_MENU_AREA_X_MAX	62

#define KEY_HOME_AREA_X_MIN	153
#define KEY_HOME_AREA_X_MAX	197

#define KEY_BACK_AREA_X_MIN	283
#define KEY_BACK_AREA_X_MAX	327

#define KEY_SEARCH_AREA_X_MIN	418
#define KEY_SEARCH_AREA_X_MAX	463




#define KEY_XMIN	0/*40*/
#define KEY_XMAX	SCREEN_RESOLUTION_X //- 40

#define USE_PERF_LEVEL_TS 1

#define USE_TS_EARLY_SUSPEND 0


/////////////////////////////////////////////////////////////////////////////////////////////////////////
static int __devinit qt602240_probe(struct i2c_client *client, const struct i2c_device_id *id);
static int __devexit qt602240_remove(struct i2c_client *client);
#ifdef CONFIG_PM
#ifdef CONFIG_HAS_EARLYSUSPEND
static int  qt602240_early_suspend(struct early_suspend *h);
static int  qt602240_late_resume(struct early_suspend *h);
#else
static int qt602240_resume(struct i2c_client *client);
static int qt602240_suspend(struct i2c_client *client, pm_message_t message);
#endif
#endif


//extern struct class *sec_class;
static struct class *touch_atmel_class;
struct device *ts_dev;

static ssize_t i2c_show(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t i2c_store(struct device *dev, struct device_attribute *attr,const char *buf, size_t size);
static ssize_t gpio_show(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t gpio_store(struct device *dev, struct device_attribute *attr,const char *buf, size_t size);
static ssize_t setup_show(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t setup_store(struct device *dev, struct device_attribute *attr,const char *buf, size_t size);


static DEVICE_ATTR(gpio, S_IRUGO | S_IWUSR, gpio_show, gpio_store);
static DEVICE_ATTR(i2c, S_IRUGO | S_IWUSR, i2c_show, i2c_store);
static DEVICE_ATTR(setup, S_IRUGO | S_IWUSR, setup_show, setup_store);


static const struct i2c_device_id qt602240_id[] = {
        { "qt602240-i2c", 0 },
        { }
};
MODULE_DEVICE_TABLE(i2c, qt602240_id);



static struct i2c_driver qt602240_driver = {
        .driver = {
                .name = "qt602240-i2c",
                .owner = THIS_MODULE,
        },
        .probe = qt602240_probe,
        .remove = __devexit_p(qt602240_remove),
#ifdef CONFIG_PM
#ifndef CONFIG_HAS_EARLYSUSPEND
        .suspend        = qt602240_suspend,
        .resume = qt602240_resume,
//#else  //!USE_TS_EARLY_SUSPEND
        //.suspend        = qt602240_suspend,
        //.resume = qt602240_resume,
#endif //CONFIG_HAS_EARLYSUSPEND
#endif //  CONFIG_PM
        .id_table = qt602240_id,
};

struct workqueue_struct *qt602240_wq;

struct qt602240 
{
        struct i2c_client *client;
        struct input_dev *input_dev;
        struct work_struct work;
        struct early_suspend    early_suspend;
        struct mutex    lock;
};
struct qt602240 *qt602240 = NULL;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


typedef struct
{
    info_id_t info_id;          /*! Info ID struct. */
    object_t *objects;          /*! Pointer to an array of objects. */
    uint16_t CRC;               /*! CRC field, low bytes. */
    uint8_t CRC_hi;             /*! CRC field, high byte. */
} info_block_t;


typedef struct
{
        uint8_t object_type;     /*!< Object type. */
        uint8_t instance;        /*!< Instance number. */
} report_id_map_t;

#ifdef _SUPPORT_MULTITOUCH_
typedef struct
{
        uint8_t id;                     /*!< id */
        int8_t status;          /*!< dn=1, up=0, none=-1 */
        int16_t x;                      /*!< X */
        int16_t y;                      /*!< Y */
    /*    
        int8_t mode;
        int16_t realx;                     
        int16_t realy;
        int area;
    */    
} report_finger_info_t;

#define MAX_NUM_FINGER  2/*5*/

static report_finger_info_t fingerInfo[MAX_NUM_FINGER];
static report_finger_info_t pre_fingerInfo[MAX_NUM_FINGER];
#endif //_SUPPORT_MULTITOUCH_

typedef enum
{
  TSC_KEY_LEFT,
  TSC_KEY_RIGHT,
  TSC_KEY_MAX
} tsc_key_code_type;

typedef enum
{
  TSC_KEY_RELEASED,
  TSC_KEY_PRESSED
} tsc_key_status_type;

typedef struct
{
    uint16_t code;                      /*!< key code */
    uint8_t status;                      /*!< key status (press/release) */
    bool update;     
} report_key_info_t;

typedef enum
{
  TSC_EVENT_NONE,
  TSC_EVENT_WINDOW,
  TSC_EVENT_KEY,
} tsc_key_mode_type;

//static report_key_info_t keyInfo[MAX_NUM_FINGER];

static info_block_t *info_block;

static report_id_map_t *report_id_map;

volatile uint8_t read_pending;

static int max_report_id = 0;

uint8_t max_message_length;

uint16_t message_processor_address;

/*! Command processor address. */
static uint16_t command_processor_address;

/*! Flag indicating if driver setup is OK. */
static enum driver_setup_t driver_setup = DRIVER_SETUP_INCOMPLETE;

/*! Pointer to message handler provided by main app. */
//static void (*application_message_handler)(void);

/*! Message buffer holding the latest message received. */
uint8_t *quantum_msg;

/*! \brief The current address pointer. */
static U16 address_pointer;

static uint8_t tsp_version;

/* flag to indicate if last calibration has been confirmed good */
static uint8_t cal_check_flag = 0u;

#ifdef NOINITCHIP
static bool NoInitChip = false;
#endif

/* Calibration checking routine calling code - in interrupt handler */
#ifdef RECHECK_AFTER_CALIBRATION
static uint8_t calibration_message = 0;
#endif
static uint8_t timer_tick = 0;
static struct hrtimer timer;
static bool timer_enabled = false;
static uint8_t facesup_flag = 0;
static ktime_t poll_time;

unsigned long charger_mode=0;
unsigned long pre_charger_mode=0;
static int check_call_sleep =0;

static int nCalCount=0;
static int nCheckCalInt =0;

static int CheckCalOk[MAX_NUM_FINGER];
static int CheckResumeCnt=0;

typedef enum
{
  TOUCH_CAL_START,
  TOUCH_CAL_END,
  TOUCH_INT_COMING,
  TOUCH_CAL_SUCCESS
} Cal_Int_mode;

/*------------------------------ functions prototype -----------------------------------*/
uint8_t init_touch_driver(uint8_t I2C_address);
void message_handler(U8 *msg, U8 length);
uint8_t reset_chip(void);
uint8_t calibrate_chip(void);
uint8_t diagnostic_chip(uint8_t mode);
uint8_t backup_config(void);
uint8_t get_variant_id(uint8_t *variant);
uint8_t get_family_id(uint8_t *family_id);
uint8_t get_build_number(uint8_t *build);
uint8_t get_version(uint8_t *version);
uint8_t write_power_config(gen_powerconfig_t7_config_t power_config);
uint8_t write_acquisition_config(gen_acquisitionconfig_t8_config_t acq_config);
uint8_t write_multitouchscreen_config(uint8_t screen_number, touch_multitouchscreen_t9_config_t cfg);
uint8_t write_keyarray_config(uint8_t key_array_number, touch_keyarray_t15_config_t cfg);
uint8_t write_linearization_config(uint8_t instance, proci_linearizationtable_t17_config_t cfg);
uint8_t write_comc_config(uint8_t instance, spt_comcconfig_t18_config_t cfg);
uint8_t write_gpio_config(uint8_t instance, spt_gpiopwm_t19_config_t cfg);
uint8_t write_gripsuppression_config(uint8_t instance, proci_gripfacesuppression_t20_config_t cfg);
uint8_t write_noisesuppression_config(uint8_t instance, procg_noisesuppression_t22_config_t cfg);
uint8_t write_proximity_config(uint8_t instance, touch_proximity_t23_config_t cfg);
uint8_t write_onetouchgesture_config(uint8_t instance, proci_onetouchgestureprocessor_t24_config_t cfg);
uint8_t write_selftest_config(uint8_t instance, spt_selftest_t25_config_t cfg);
uint8_t write_twotouchgesture_config(uint8_t instance, proci_twotouchgestureprocessor_t27_config_t cfg);
uint8_t write_CTE_config(spt_cteconfig_t28_config_t cfg);
uint8_t write_simple_config(uint8_t object_type, uint8_t instance, void *cfg);
uint8_t get_object_size(uint8_t object_type);
uint8_t type_to_report_id(uint8_t object_type, uint8_t instance);
uint8_t report_id_to_type(uint8_t report_id, uint8_t *instance);
uint8_t read_id_block(info_id_t *id);
uint8_t get_max_report_id(void);
uint16_t get_object_address(uint8_t object_type, uint8_t instance);
uint32_t get_stored_infoblock_crc(void);
uint8_t calculate_infoblock_crc(uint32_t *crc_pointer);
uint32_t CRC_24(uint32_t crc, uint8_t byte1, uint8_t byte2);
void write_message_to_usart(uint8_t msg[], uint8_t length);
void init_hw_setting(void);
void TSP_Restart(void);

U8 read_changeline(void);
U8 init_I2C(U8 I2C_address_arg);
U8 read_mem(U16 start, U8 size, U8 *mem);
U8 read_U16(U16 start, U16 *mem);
U8 write_mem(U16 start, U8 size, U8 *mem);

void read_all_register(void);

#ifdef SKY_PROCESS_CMD_KEY
static int ts_fops_ioctl(struct inode *inode, struct file *filp,unsigned int cmd, unsigned long arg);
static int ts_fops_open(struct inode *inode, struct file *filp);
#endif

/*------------------------------ for tunning ATmel - start ----------------------------*/
struct class *touch_class;
EXPORT_SYMBOL(touch_class);

struct device *switch_test;
EXPORT_SYMBOL(switch_test);


/******************************************************************************
* 
*
*       QT602240 Object table init
* 
*                                                             
* *****************************************************************************/
//General Object
gen_powerconfig_t7_config_t power_config = {0};                 //Power config settings.
gen_acquisitionconfig_t8_config_t acquisition_config = {0};     // Acquisition config. 


//Touch Object
touch_multitouchscreen_t9_config_t touchscreen_config = {0};    //Multitouch screen config.
touch_keyarray_t15_config_t keyarray_config = {0};              //Key array config.
touch_proximity_t23_config_t proximity_config = {0};        //Proximity config. 


//Signal Processing Objects
proci_gripfacesuppression_t20_config_t gripfacesuppression_config = {0};    //Grip / face suppression config.
procg_noisesuppression_t22_config_t noise_suppression_config = {0};         //Noise suppression config.
proci_onetouchgestureprocessor_t24_config_t onetouch_gesture_config = {0};  //One-touch gesture config. 
proci_twotouchgestureprocessor_t27_config_t twotouch_gesture_config = {0};  //Two-touch gesture config.


//Support Objects
spt_comcconfig_t18_config_t   comc_config = {0};            //Communication config settings.
spt_gpiopwm_t19_config_t  gpiopwm_config = {0};             //GPIO/PWM config
spt_selftest_t25_config_t selftest_config = {0};            //Selftest config.
spt_cteconfig_t28_config_t cte_config = {0};                //Capacitive touch engine config.



/*------------------------------ for tunning ATmel - end ----------------------------*/

#define __QT_CONFIG__

typedef enum 
{
	TOUCH_IOCTL_READ_LASTKEY=1001,
	TOUCH_IOCTL_DO_KEY,
	TOUCH_IOCTL_RELEASE_KEY, 
	TOUCH_IOCTL_CLEAN,
	TOUCH_IOCTL_DEBUG,
	TOUCH_IOCTL_RESTART,
  	TOUCH_IOCTL_PRESS_TOUCH,
	TOUCH_IOCTL_RELEASE_TOUCH,
	TOUCH_IOCTL_CHARGER_MODE,
	POWER_OFF,
	TOUCH_IOCTL_DELETE_ACTAREA = 2001,
	TOUCH_IOCTL_RECOVERY_ACTAREA,
	TOUCH_IOCTL_INIT = 3001,
	TOUCH_IOCTL_EXIT,
} TOUCH_IOCTL_CMD;


#ifdef SKY_PROCESS_CMD_KEY
static struct file_operations ts_fops = {
	.owner = THIS_MODULE,
	.open = ts_fops_open,
//	.release = ts_fops_close,
	.ioctl = ts_fops_ioctl,
};

static struct miscdevice touch_event = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "touch_fops",
	.fops = &ts_fops,
};

static int ts_fops_open(struct inode *inode, struct file *filp)
{
	filp->private_data = qt602240;
	return 0;
}

static int ts_fops_ioctl(struct inode *inode, struct file *filp,
	       unsigned int cmd, unsigned long arg)
{
	void __user *argp = (void __user *)arg;
	//struct ts* ts = qt602240;

	switch (cmd) 
	{
		case TOUCH_IOCTL_READ_LASTKEY:
			break;
		case TOUCH_IOCTL_DO_KEY:
			input_report_key(qt602240->input_dev, (int)argp, 1);
			break;
		case TOUCH_IOCTL_RELEASE_KEY:		
			input_report_key(qt602240->input_dev, (int)argp, 0);
			break;
        case TOUCH_IOCTL_DEBUG:
			queue_work(qt602240_wq, &qt602240->work);
			break;
		case TOUCH_IOCTL_CLEAN:
			dbg("Touch Screen Previous Data Clean ~!!\n");	
			clear_event();
			break;
		case TOUCH_IOCTL_RESTART:
			//dbg("Touch Screen HW Restart ~!!\n");
			//TSP_Restart();
			dbg("Touch Screen Calibration Restart ~!!\n");			
			calibrate_chip();
			break;
	 	case TOUCH_IOCTL_PRESS_TOUCH:
	 		input_report_abs(qt602240->input_dev, ABS_MT_POSITION_X, (int)(arg&0x0000FFFF));
	        input_report_abs(qt602240->input_dev, ABS_MT_POSITION_Y, (int)((arg >> 16) & 0x0000FFFF));
	        input_report_abs(qt602240->input_dev, ABS_MT_TOUCH_MAJOR, 255);
	        input_report_abs(qt602240->input_dev, ABS_MT_WIDTH_MAJOR, 1);
	        input_mt_sync(qt602240->input_dev);
	        input_sync(qt602240->input_dev);		  
			break;
 		case TOUCH_IOCTL_RELEASE_TOUCH:		
	 		input_report_abs(qt602240->input_dev, ABS_MT_POSITION_X, (int)(arg&0x0000FFFF));
	        input_report_abs(qt602240->input_dev, ABS_MT_POSITION_Y, (int)((arg >> 16) & 0x0000FFFF));
            input_report_abs(qt602240->input_dev, ABS_MT_TOUCH_MAJOR, 0);
            input_report_abs(qt602240->input_dev, ABS_MT_WIDTH_MAJOR, 1);
            input_mt_sync(qt602240->input_dev);
            input_sync(qt602240->input_dev);				
		 	break;		
		case TOUCH_IOCTL_CHARGER_MODE:
			qt_charger_mode_config(arg);
			break;
		case POWER_OFF: // ......zzz
			//pm_power_off();
			break;
		case TOUCH_IOCTL_DELETE_ACTAREA:
			touchscreen_config.yloclip = 0;		// Change Active area
			touchscreen_config.yhiclip = 0;
			if (write_multitouchscreen_config(0, touchscreen_config) != CFG_WRITE_OK)
			{
				dbg("qt_Multitouchscreen_config Error!!!\n");
			}
			break;
		case TOUCH_IOCTL_RECOVERY_ACTAREA:
			touchscreen_config.yloclip = 15;	// Change Active area
			touchscreen_config.yhiclip = 15;
			if (write_multitouchscreen_config(0, touchscreen_config) != CFG_WRITE_OK)
			{
				dbg("qt_Multitouchscreen_config Error!!!\n");
			}
			break;
		default:
	     	break;
	}

	return true;
}
#endif



/*****************************************************************************
*
*
*
*
*
*       QT602240  Configuration Block
*
*
*
*
* ***************************************************************************/


void qt_sleep_Init(void)
{

    if (driver_setup != DRIVER_SETUP_OK)
        return;

    /* Set Idle Acquisition Interval to 32 ms. */
    power_config.idleacqint = 0; 

    /* Set Active Acquisition Interval to 16 ms. */
    power_config.actvacqint = 0; 

    /* Set Active to Idle Timeout to 4 s (one unit = 200ms). */
    power_config.actv2idleto = 0; 


    /* Write power config to chip. */
    if (write_power_config(power_config) != CFG_WRITE_OK)
    {
        dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
    }
}



/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

void qt_Power_Config_Init(void)
{
    if (driver_setup != DRIVER_SETUP_OK)
        return;
	dbg("[QT] %s/%d\n",__func__,__LINE__);
    /* Set Idle Acquisition Interval to 32 ms. */
    power_config.idleacqint = 32;

    /* Set Active Acquisition Interval to 16 ms. */
    power_config.actvacqint = 16/*255*/;

    /* Set Active to Idle Timeout to 4 s (one unit = 200ms). */
    power_config.actv2idleto = 20/*5*/;


    /* Write power config to chip. */
    if (write_power_config(power_config) != CFG_WRITE_OK)
    {
        dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
    }
}


/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

void qt_Acquisition_Config_Init(void)
{
        if (driver_setup != DRIVER_SETUP_OK)
        return;
	dbg("[QT] %s/%d\n",__func__,__LINE__);
    acquisition_config.chrgtime = 10/*8*/; // 2us
    acquisition_config.reserved = 0;//5;
    acquisition_config.tchdrift = 5/*10*//*5*/;//20; // 4s
    acquisition_config.driftst =  5/*10*/;//20; // 4s
    acquisition_config.tchautocal = 0;
    acquisition_config.sync = 0; // disabled
    acquisition_config.atchcalst = 2;
    acquisition_config.atchcalsthr = CHARGER_UNPLUGGED_TCHTHR/*50*//*40*/;

#if defined(__VER_2_0__)
   acquisition_config.atchfrccalthr = 50;     /*!< Anti-touch force calibration threshold */
   acquisition_config.atchfrccalration = 25;  /*!< Anti-touch force calibration ratio */  
#endif

    if (write_acquisition_config(acquisition_config) != CFG_WRITE_OK)
    {
        dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
    }
}


/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

void qt_Multitouchscreen_Init(void)
{
        if (driver_setup != DRIVER_SETUP_OK)
        return;
	dbg("[QT] %s/%d\n",__func__,__LINE__);
    touchscreen_config.ctrl = 143; 
    touchscreen_config.xorigin = 0;
    touchscreen_config.yorigin = 2;
    touchscreen_config.xsize = 15;
    touchscreen_config.ysize = 9;

    touchscreen_config.akscfg = 0;

   #if 1//(BOARD_VER ==MOON_ES10)
    touchscreen_config.blen = 0x1;
   #else
    touchscreen_config.blen = 0x11;		// 0x11 -> 0x1,   1 -> 0 for gain 
   #endif 

#if defined(__VER_2_0__)
    #if	1//(BOARD_VER ==MOON_ES10)
        touchscreen_config.tchthr = CHARGER_UNPLUGGED_TCHTHR;	//terry test
    #else
        touchscreen_config.tchthr = CHARGER_UNPLUGGED_TCHTHR;
    #endif
#else
    #if	1//(BOARD_VER ==MOON_ES10)
        touchscreen_config.tchthr = CHARGER_UNPLUGGED_TCHTHR/*38*/;	//terry test
    #else
        touchscreen_config.tchthr = CHARGER_UNPLUGGED_TCHTHR;
    #endif
#endif
    touchscreen_config.tchdi = 2;
    touchscreen_config.orient = 1; //5;  //reverse

    touchscreen_config.mrgtimeout = 0;
    touchscreen_config.movhysti =  10/*8*/;
    touchscreen_config.movhystn = 2/*1*/;
    touchscreen_config.movfilter = 15;
#ifdef _SUPPORT_MULTITOUCH_     
    touchscreen_config.numtouch= MAX_NUM_FINGER;
#else
    touchscreen_config.numtouch= 1;
#endif
    touchscreen_config.mrghyst = 10;
    touchscreen_config.mrgthr = 20/*10*/;
    touchscreen_config.amphyst = 10;
    touchscreen_config.xrange = 886;	// 76.3:800=84.65=?
    touchscreen_config.yrange = 479;
    touchscreen_config.xloclip = 0/*40*/;
    touchscreen_config.xhiclip = 0/*20*//*60*/;
    touchscreen_config.yloclip = 20/*10*/;
    touchscreen_config.yhiclip = 30;
    touchscreen_config.xedgectrl = 0;
    touchscreen_config.xedgedist = 0;
    touchscreen_config.yedgectrl = 0;
    touchscreen_config.yedgedist = 0;
    touchscreen_config.jumplimit = 25;
#if defined(__VER_2_0__)
    #if	1//(BOARD_VER ==MOON_ES10)
    touchscreen_config.tchhyst      = CHARGER_UNPLUGGED_TCHHYSYT; //5 -> 20
    #else
    touchscreen_config.tchhyst      = CHARGER_UNPLUGGED_TCHHYSYT; 
    #endif
#endif    

    if (write_multitouchscreen_config(0, touchscreen_config) != CFG_WRITE_OK)
    {
        dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
    }
	dbg("[QT] %s/%d\n",__func__,__LINE__);
}


/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

void qt_KeyArray_Init(void)
{
    
    if (driver_setup != DRIVER_SETUP_OK)
        return;
	dbg("[QT] %s/%d\n",__func__,__LINE__);
    keyarray_config.ctrl = 0;
    keyarray_config.xorigin = 0;
    keyarray_config.yorigin = 0;
    keyarray_config.xsize = 0;
    keyarray_config.ysize = 0;
    keyarray_config.akscfg = 0;
    keyarray_config.blen = 0;
    keyarray_config.tchthr = 0;
    keyarray_config.tchdi = 0;
    keyarray_config.reserved[0] = 0;
    keyarray_config.reserved[1] = 0;

    if (write_keyarray_config(0, keyarray_config) != CFG_WRITE_OK)
    {
        dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
    }
}


/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

void qt_ComcConfig_Init(void)
{
    if (driver_setup != DRIVER_SETUP_OK)
        return;
	dbg("[QT] %s/%d\n",__func__,__LINE__);
    comc_config.ctrl = 0x00;
    comc_config.cmd = 0;//COMM_MODE1;

    if (get_object_address(SPT_COMCONFIG_T18, 0) != OBJECT_NOT_FOUND)
    {
        if (write_comc_config(0, comc_config) != CFG_WRITE_OK)
        {
            dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
        }
    }
}


/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

void qt_Gpio_Pwm_Init(void)
{
    if (driver_setup != DRIVER_SETUP_OK)
        return;
	dbg("[QT] %s/%d\n",__func__,__LINE__);
    gpiopwm_config.ctrl = 0;
    gpiopwm_config.reportmask = 0;
    gpiopwm_config.dir = 0;
    gpiopwm_config.intpullup = 0;
    gpiopwm_config.out = 0;
    gpiopwm_config.wake = 0;
    gpiopwm_config.pwm = 0;
    gpiopwm_config.period = 0;
    gpiopwm_config.duty[0] = 0;
    gpiopwm_config.duty[1] = 0;
    gpiopwm_config.duty[2] = 0;
    gpiopwm_config.duty[3] = 0;

    if (write_gpio_config(0, gpiopwm_config) != CFG_WRITE_OK)
    {
        dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
    }
}


/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

void qt_Grip_Face_Suppression_Config_Init(void)
{
    if (driver_setup != DRIVER_SETUP_OK)
        return;
	dbg("[QT] %s/%d\n",__func__,__LINE__);
    gripfacesuppression_config.ctrl = 0/*7*/;
    gripfacesuppression_config.xlogrip = 0;
    gripfacesuppression_config.xhigrip = 0;
    gripfacesuppression_config.ylogrip = 0;
    gripfacesuppression_config.yhigrip = 0;
    gripfacesuppression_config.maxtchs = 0;
    gripfacesuppression_config.reserved = 0;
    gripfacesuppression_config.szthr1 = 0/*30*/;
    gripfacesuppression_config.szthr2 = 0/*25*/;
    gripfacesuppression_config.shpthr1 = 0/*4*/;
    gripfacesuppression_config.shpthr2 = 0/*30*/;
    gripfacesuppression_config.supextto = 0/*5*/;

    /* Write grip suppression config to chip. */
    if (get_object_address(PROCI_GRIPFACESUPPRESSION_T20, 0) != OBJECT_NOT_FOUND)
    {
        if (write_gripsuppression_config(0, gripfacesuppression_config) !=
            CFG_WRITE_OK)
        {
            dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
        }
    }
}


/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

void qt_Noise_Suppression_Config_Init(void)
{
    if (driver_setup != DRIVER_SETUP_OK)
        return;
	dbg("[QT] %s/%d\n",__func__,__LINE__);
    #if	1//(BOARD_VER ==MOON_ES10)
    noise_suppression_config.ctrl = 5;
    #else
    noise_suppression_config.ctrl = 13/*5*/; //frequency hopping only  5 -> 13
    #endif
    noise_suppression_config.reserved = 0;
    noise_suppression_config.reserved1 = 0;
    noise_suppression_config.gcaful = 25/*0*/;
    noise_suppression_config.gcafll = -25/*-0*/;

    noise_suppression_config.actvgcafvalid = 4;
    noise_suppression_config.noisethr = 30/*35*/;
    noise_suppression_config.freqhopscale = 0/*1*/;

    #if 1//(BOARD_VER ==MOON_TP10)
    noise_suppression_config.freq[0] = 21/*5*/;
    noise_suppression_config.freq[1] = 31/*10*/;
    noise_suppression_config.freq[2] = 36/*15*/;
    noise_suppression_config.freq[3] = 42/*25*/;
    noise_suppression_config.freq[4] = 47/*30*/;
    #else
    noise_suppression_config.freq[0] = 21/*5*/;
    noise_suppression_config.freq[1] = 31/*10*/;
    noise_suppression_config.freq[2] = 36/*15*/;
    noise_suppression_config.freq[3] = 42/*25*/;
    noise_suppression_config.freq[4] = 47/*30*/;
    #endif
    
    noise_suppression_config.idlegcafvalid =  4;

    /* Write Noise suppression config to chip. */
    if (get_object_address(PROCG_NOISESUPPRESSION_T22, 0) != OBJECT_NOT_FOUND)
    {
        if (write_noisesuppression_config(0,noise_suppression_config) != CFG_WRITE_OK)
        {
            dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
        }
    }
}


/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

void qt_Proximity_Config_Init(void)
{
    if (driver_setup != DRIVER_SETUP_OK)
        return;
	dbg("[QT] %s/%d\n",__func__,__LINE__);
    proximity_config.ctrl = 0;
    proximity_config.xorigin = 0;
    proximity_config.xsize = 0;
    proximity_config.ysize = 0;
    proximity_config.reserved = 0;
    proximity_config.blen = 0;
    proximity_config.fxddthr = 0;
    proximity_config.fxddi = 0;
    proximity_config.average = 0;
    proximity_config.mvnullrate = 0;
    proximity_config.mvdthr = 0;

    if (get_object_address(TOUCH_PROXIMITY_T23, 0) != OBJECT_NOT_FOUND)
    {
        if (write_proximity_config(0, proximity_config) != CFG_WRITE_OK)
        {
            dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
        }
    }
}


/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

void qt_One_Touch_Gesture_Config_Init(void)
{
    if (driver_setup != DRIVER_SETUP_OK)
        return;
	dbg("[QT] %s/%d\n",__func__,__LINE__);
    /* Disable one touch gestures. */
    onetouch_gesture_config.ctrl = 0;
    onetouch_gesture_config.numgest = 0;

    onetouch_gesture_config.gesten = 0;
    onetouch_gesture_config.pressproc = 0;
    onetouch_gesture_config.tapto = 0;
    onetouch_gesture_config.flickto = 0;
    onetouch_gesture_config.dragto = 0;
    onetouch_gesture_config.spressto = 0;
    onetouch_gesture_config.lpressto = 0;
    onetouch_gesture_config.reppressto = 0;
    onetouch_gesture_config.flickthr = 0;
    onetouch_gesture_config.dragthr = 0;
    onetouch_gesture_config.tapthr = 0;
    onetouch_gesture_config.throwthr = 0;


    if (get_object_address(PROCI_ONETOUCHGESTUREPROCESSOR_T24, 0) !=
        OBJECT_NOT_FOUND)
    {
        if (write_onetouchgesture_config(0, onetouch_gesture_config) !=
            CFG_WRITE_OK)
        {
            dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
        }
    }
}


/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

void qt_Selftest_Init(void)
{
    if (driver_setup != DRIVER_SETUP_OK)
        return;
	dbg("[QT] %s/%d\n",__func__,__LINE__);
    selftest_config.ctrl = 0;
    selftest_config.cmd = 0;

    selftest_config.siglim[0].upsiglim = 0;
    selftest_config.siglim[0].losiglim = 0;

    selftest_config.siglim[1].upsiglim = 0;
    selftest_config.siglim[1].losiglim = 0;
    
    selftest_config.siglim[2].upsiglim = 0;
    selftest_config.siglim[2].losiglim = 0;
        
    if (get_object_address(SPT_SELFTEST_T25, 0) != OBJECT_NOT_FOUND)
    {
        if (write_selftest_config(0,selftest_config) != CFG_WRITE_OK)
        {
            dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
        }
    }
}


/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

void qt_Two_touch_Gesture_Config_Init(void)
{
    if (driver_setup != DRIVER_SETUP_OK)
        return;
	dbg("[QT] %s/%d\n",__func__,__LINE__);
    /* Disable two touch gestures. */
    twotouch_gesture_config.ctrl = 0;
    twotouch_gesture_config.numgest = 0;
    twotouch_gesture_config.reserved2 = 0;
    twotouch_gesture_config.gesten = 0;
    twotouch_gesture_config.rotatethr = 0;
    twotouch_gesture_config.zoomthr = 0;


    if (get_object_address(PROCI_TWOTOUCHGESTUREPROCESSOR_T27, 0) !=
        OBJECT_NOT_FOUND)
    {
        if (write_twotouchgesture_config(0, twotouch_gesture_config) !=
            CFG_WRITE_OK)
        {
            dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
        }
    }
}


/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

void qt_CTE_Config_Init(void)
{
    if (driver_setup != DRIVER_SETUP_OK)
        return;
	dbg("[QT] %s/%d\n",__func__,__LINE__);
    /* Set CTE config */
    cte_config.ctrl =  1/*0*/;
    cte_config.cmd = 0; 
    cte_config.mode =  2; //for (15X x 9Y mode)
    cte_config.idlegcafdepth = 8/*4*/;
    cte_config.actvgcafdepth = 12/*8*/;
    cte_config.voltage = 30; //(AVDD-2.7)/0.01 , 3V-2.7V /0.01 = 30;

    /* Write CTE config to chip. */
    if (get_object_address(SPT_CTECONFIG_T28, 0) != OBJECT_NOT_FOUND)
    {
        if (write_CTE_config(cte_config) != CFG_WRITE_OK)
        {
            dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
        }
    }
}


/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  - config normal touch scan interval
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

void qt_Power_Config_Resume(void)
{
	int ret=0;

	power_config.idleacqint = CHARGER_UNPLUGGED_IDLEACQINT;
	power_config.actvacqint = CHARGER_UNPLUGGED_ACTVACQINT;
	power_config.actv2idleto = CHARGER_UNPLUGGED_ACTV2IDLETO;

	if((ret = write_power_config(power_config)) != CFG_WRITE_OK)
	{
		int i=0;

		dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
		/* "Power config write failed!\n" */

		for(i=0; i<10; i++) 
		{
			dbg("\n[TSP][ERROR] config error %d,  line : %d i %d\n", ret, __LINE__, i);
			msleep(20);
			if ( (ret = write_power_config(power_config)) == CFG_WRITE_OK)
				break;
		}
	}
}


void qt_Grip_Face_Suppression_Config_On(void)
{
    if (driver_setup != DRIVER_SETUP_OK)
       return;

    gripfacesuppression_config.ctrl = 7; 

    /* Write grip suppression config to chip. */
    if (get_object_address(PROCI_GRIPFACESUPPRESSION_T20, 0) != OBJECT_NOT_FOUND)
    {
        if (write_gripsuppression_config(0, gripfacesuppression_config) !=
            CFG_WRITE_OK)
        {
            dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
        }
    }
}

void qt_Grip_Face_Suppression_Config_Off(void)
{
    if (driver_setup != DRIVER_SETUP_OK)
        return;

    gripfacesuppression_config.ctrl = 0; 

    /* Write grip suppression config to chip. */
    if (get_object_address(PROCI_GRIPFACESUPPRESSION_T20, 0) != OBJECT_NOT_FOUND)
    {
        if (write_gripsuppression_config(0, gripfacesuppression_config) !=
            CFG_WRITE_OK)
        {
            dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
        }
    }
}

/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

unsigned char Comm_Config_Process(unsigned char change_en)
{
        change_en = 0;

  return (change_en);
}


/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

uint8_t reset_chip(void)
{
    uint8_t data = 1u;

    if (driver_setup != DRIVER_SETUP_OK)
        return 0;
    
    return(write_mem(command_processor_address + RESET_OFFSET, 1, &data));
}


/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/
/*!
 * \brief Calibrates the chip.
 * 
 * This function will send a calibrate command to touch chip.
 * Whilst calibration has not been confirmed as good, this function will set
 * the ATCHCALST and ATCHCALSTHR to zero to allow a bad cal to always recover
 * 
 * @return WRITE_MEM_OK if writing the command to touch chip was successful.
 * 
 */
uint8_t calibrate_chip(void)
{
        uint8_t data = 1u;
        int ret = WRITE_MEM_OK;
        uint8_t atchcalst, atchcalsthr;

        if (driver_setup != DRIVER_SETUP_OK)
            return 0;

        /* change calibration suspend settings to zero until calibration confirmed good */
        /* store normal settings */
        atchcalst = acquisition_config.atchcalst;
        atchcalsthr = acquisition_config.atchcalsthr;

        /* resume calibration must be performed with zero settings */
        acquisition_config.atchcalst = 0;
        acquisition_config.atchcalsthr = 0; 

        dbg("[TSP] reset acq atchcalst=%d, atchcalsthr=%d\n", acquisition_config.atchcalst, acquisition_config.atchcalsthr );

        /* Write temporary acquisition config to chip. */
        if (write_acquisition_config(acquisition_config) != CFG_WRITE_OK)
        {
                /* "Acquisition config write failed!\n" */
                dbg("\n[TSP][ERROR] line : %d\n", __LINE__);
                ret = WRITE_MEM_FAILED; /* calling function should retry calibration call */
        }

        /* restore settings to the local structure so that when we confirm the 
         * cal is good we can correct them in the chip */
        /* this must be done before returning */
        acquisition_config.atchcalst = atchcalst;
        acquisition_config.atchcalsthr = atchcalsthr;


        /* send calibration command to the chip */
        if(ret == WRITE_MEM_OK)
        {
                /* change calibration suspend settings to zero until calibration confirmed good */
                ret = write_mem(command_processor_address + CALIBRATE_OFFSET, 1, &data);

                /* set flag for calibration lockup recovery if cal command was successful */
                if(ret == WRITE_MEM_OK)
                { 
                        /* set flag to show we must still confirm if calibration was good or bad */
                        cal_check_flag = 1u;
                }
        }

        msleep(120);

        return ret;
}

/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

uint8_t reportall_chip(uint8_t mode)
{
    uint8_t status;
    status = write_mem(command_processor_address + REPORTATLL_OFFSET, 1, &mode);
    return(status);
}

/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

uint8_t diagnostic_chip(uint8_t mode)
{
    uint8_t status;

    if (driver_setup != DRIVER_SETUP_OK)
        return 0;
    
    status = write_mem(command_processor_address + DIAGNOSTIC_OFFSET, 1, &mode);
    return(status);
}


/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

uint8_t backup_config(void)
{

    /* Write 0x55 to BACKUPNV register to initiate the backup. */
    uint8_t data = 0x55u;

    if (driver_setup != DRIVER_SETUP_OK)
        return 0;
    
    return(write_mem(command_processor_address + BACKUP_OFFSET, 1, &data));
}


/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

uint8_t get_version(uint8_t *version)
{

    if (info_block)
    {
        *version = info_block->info_id.version;
    }
    else
    {
        return(ID_DATA_NOT_AVAILABLE);
    }
    return (ID_DATA_OK);
}


/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

uint8_t get_family_id(uint8_t *family_id)
{

    if (info_block)
    {
        *family_id = info_block->info_id.family_id;
    }
    else
    {
        return(ID_DATA_NOT_AVAILABLE);
    }
    return (ID_DATA_OK);
}


/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

uint8_t get_build_number(uint8_t *build)
{

    if (info_block)
    {
        *build = info_block->info_id.build;
    }
    else
    {
        return(ID_DATA_NOT_AVAILABLE);
    }
    return (ID_DATA_OK);
}


/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

uint8_t get_variant_id(uint8_t *variant)
{

    if (info_block)
    {
        *variant = info_block->info_id.variant_id;
    }
    else
    {
        return(ID_DATA_NOT_AVAILABLE);
    }
    return (ID_DATA_OK);
}


/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

uint8_t write_power_config(gen_powerconfig_t7_config_t cfg)
{
    return(write_simple_config(GEN_POWERCONFIG_T7, 0, (void *) &cfg));
}


/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

uint8_t write_acquisition_config(gen_acquisitionconfig_t8_config_t cfg)
{
    return(write_simple_config(GEN_ACQUISITIONCONFIG_T8, 0, (void *) &cfg));
}


/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

uint8_t write_multitouchscreen_config(uint8_t instance, touch_multitouchscreen_t9_config_t cfg)
{
    uint16_t object_address;
    uint8_t *tmp;
    uint8_t status;
    uint8_t object_size;

    object_size = get_object_size(TOUCH_MULTITOUCHSCREEN_T9);
    if (object_size == 0)
    {
        return(CFG_WRITE_FAILED);
    }
    tmp = (uint8_t *) kmalloc(object_size, GFP_KERNEL | GFP_ATOMIC);
    if (tmp == NULL)
    {
        return(CFG_WRITE_FAILED);
    }

    memset(tmp,0,object_size);


    /* 18 elements at beginning are 1 byte. */
    memcpy(tmp, &cfg, 18);

    /* Next two are 2 bytes. */

    *(tmp + 18) = (uint8_t) (cfg.xrange &  0xFF);
    *(tmp + 19) = (uint8_t) (cfg.xrange >> 8);

    *(tmp + 20) = (uint8_t) (cfg.yrange &  0xFF);
    *(tmp + 21) = (uint8_t) (cfg.yrange >> 8);

    /* And the last 4(8) 1 bytes each again. */
    *(tmp + 22) = cfg.xloclip;
    *(tmp + 23) = cfg.xhiclip;
    *(tmp + 24) = cfg.yloclip;
    *(tmp + 25) = cfg.yhiclip;

    *(tmp + 26) = cfg.xedgectrl;
    *(tmp + 27) = cfg.xedgedist;
    *(tmp + 28) = cfg.yedgectrl;
    *(tmp + 29) = cfg.yedgedist;
    
#if defined(__VER_2_0__)
    if(info_block->info_id.version >= 0x16)
    {
        *(tmp + 30) = cfg.jumplimit;
    }
	
    *(tmp + 31) = cfg.tchhyst;
	
#else    
  //  if(info_block->info_id.version == 0x16)
    {
        *(tmp + 30) = cfg.jumplimit;
    }
#endif

    object_address = get_object_address(TOUCH_MULTITOUCHSCREEN_T9,
        instance);

    if (object_address == 0)
    {
        kfree(tmp);
        return(CFG_WRITE_FAILED);
    }

    status = write_mem(object_address, object_size, tmp);

    kfree(tmp);

    return(status);

}


/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

uint8_t write_keyarray_config(uint8_t instance, touch_keyarray_t15_config_t cfg)
{

    return(write_simple_config(TOUCH_KEYARRAY_T15, instance, (void *) &cfg));

}


/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

uint8_t write_comc_config(uint8_t instance, spt_comcconfig_t18_config_t cfg)
{

    return(write_simple_config(SPT_COMCONFIG_T18, instance, (void *) &cfg));

}



/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

uint8_t write_gpio_config(uint8_t instance, spt_gpiopwm_t19_config_t cfg)
{

    return(write_simple_config(SPT_GPIOPWM_T19, instance, (void *) &cfg));

}



/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

uint8_t write_gripsuppression_config(uint8_t instance, proci_gripfacesuppression_t20_config_t cfg)
{

    return(write_simple_config(PROCI_GRIPFACESUPPRESSION_T20, instance,
        (void *) &cfg));
}


/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

uint8_t write_noisesuppression_config(uint8_t instance, procg_noisesuppression_t22_config_t cfg)
{
    uint16_t object_address;
    uint8_t *tmp;
    uint8_t status;
    uint8_t object_size;

    object_size = get_object_size(PROCG_NOISESUPPRESSION_T22);
    if (object_size == 0)
    {
        return(CFG_WRITE_FAILED);
    }

    tmp = (uint8_t *) kmalloc(object_size, GFP_KERNEL | GFP_ATOMIC);
    
    
    if (tmp == NULL)
    {
        return(CFG_WRITE_FAILED);
    }

    memset(tmp,0,object_size);
    
    *(tmp + 0) = cfg.ctrl;
    *(tmp + 1) = cfg.reserved;
    *(tmp + 2) = cfg.reserved1;

    *(tmp + 3) = (uint8_t) (cfg.gcaful & 0x00FF);
    *(tmp + 4) = (uint8_t) (cfg.gcaful >> 8);
    
    *(tmp + 5) = (uint8_t) (cfg.gcafll & 0x00FF);
    *(tmp + 6) = (uint8_t) (cfg.gcafll >> 8);

    *(tmp + 7) = cfg.actvgcafvalid;
    *(tmp + 8) = cfg.noisethr;

    *(tmp + 9) = cfg.reserved2;
    *(tmp + 10) = cfg.freqhopscale;

    *(tmp + 11) = cfg.freq[0];
    *(tmp + 12) = cfg.freq[1];
    *(tmp + 13) = cfg.freq[2];
    *(tmp + 14) = cfg.freq[3];
    *(tmp + 15) = cfg.freq[4];
    
    *(tmp + 16) = cfg.idlegcafvalid;
    
    
    object_address = get_object_address(PROCG_NOISESUPPRESSION_T22,
        instance);

    if (object_address == 0)
    {
        return(CFG_WRITE_FAILED);
    }

    status = write_mem(object_address, object_size, tmp);

    kfree(tmp);

    return(status);
}



/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

uint8_t write_proximity_config(uint8_t instance, touch_proximity_t23_config_t cfg)
{
    uint16_t object_address;
    uint8_t *tmp;
    uint8_t status;
    uint8_t object_size;

    object_size = get_object_size(TOUCH_PROXIMITY_T23);
    if (object_size == 0)
    {
        return(CFG_WRITE_FAILED);
    }
    tmp = (uint8_t *) kmalloc(object_size, GFP_KERNEL | GFP_ATOMIC);
    if (tmp == NULL)
    {
        return(CFG_WRITE_FAILED);
    }

    memset(tmp,0,object_size);

    *(tmp + 0) = cfg.ctrl;
    *(tmp + 1) = cfg.xorigin;
    *(tmp + 2) = cfg.yorigin;
    *(tmp + 3) = cfg.xsize;
    *(tmp + 4) = cfg.ysize;
    *(tmp + 5) = cfg.reserved;
    *(tmp + 6) = cfg.blen;

    *(tmp + 7) = (uint8_t) (cfg.fxddthr & 0x00FF);
    *(tmp + 8) = (uint8_t) (cfg.fxddthr >> 8);

    *(tmp + 9) = cfg.fxddi;
    *(tmp + 10) = cfg.average;

    *(tmp + 11) = (uint8_t) (cfg.mvnullrate & 0x00FF);
    *(tmp + 12) = (uint8_t) (cfg.mvnullrate >> 8);

    object_address = get_object_address(TOUCH_PROXIMITY_T23,
        instance);

    if (object_address == 0)
    {
        kfree(tmp);
        return(CFG_WRITE_FAILED);
    }

    status = write_mem(object_address, object_size, tmp);

    kfree(tmp);

    return(status);
}


/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

uint8_t write_onetouchgesture_config(uint8_t instance, proci_onetouchgestureprocessor_t24_config_t cfg)
{

    uint16_t object_address;
    uint8_t *tmp;
    uint8_t status;
    uint8_t object_size;

    object_size = get_object_size(PROCI_ONETOUCHGESTUREPROCESSOR_T24);
    if (object_size == 0)
    {
        return(CFG_WRITE_FAILED);
    }
    tmp = (uint8_t *) kmalloc(object_size, GFP_KERNEL | GFP_ATOMIC);
    if (tmp == NULL)
    {
        return(CFG_WRITE_FAILED);
    }

    memset(tmp,0,object_size);

    *(tmp + 0) = cfg.ctrl;
    *(tmp + 1) = cfg.numgest;

    *(tmp + 2) = (uint8_t) (cfg.gesten & 0x00FF);
    *(tmp + 3) = (uint8_t) (cfg.gesten >> 8);

    memcpy((tmp+4), &cfg.pressproc, 7);

    *(tmp + 11) = (uint8_t) (cfg.flickthr & 0x00FF);
    *(tmp + 12) = (uint8_t) (cfg.flickthr >> 8);

    *(tmp + 13) = (uint8_t) (cfg.dragthr & 0x00FF);
    *(tmp + 14) = (uint8_t) (cfg.dragthr >> 8);

    *(tmp + 15) = (uint8_t) (cfg.tapthr & 0x00FF);
    *(tmp + 16) = (uint8_t) (cfg.tapthr >> 8);

    *(tmp + 17) = (uint8_t) (cfg.throwthr & 0x00FF);
    *(tmp + 18) = (uint8_t) (cfg.throwthr >> 8);

    object_address = get_object_address(PROCI_ONETOUCHGESTUREPROCESSOR_T24,
        instance);

    if (object_address == 0)
    {
        kfree(tmp);
        return(CFG_WRITE_FAILED);
    }

    status = write_mem(object_address, object_size, tmp);

    kfree(tmp);

    return(status);
}


/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

uint8_t write_selftest_config(uint8_t instance, spt_selftest_t25_config_t cfg)
{

    uint16_t object_address;
    uint8_t *tmp;
    uint8_t status;
    uint8_t object_size;

    object_size = get_object_size(SPT_SELFTEST_T25);
    if (object_size == 0)
    {
        return(CFG_WRITE_FAILED);
    }
    tmp = (uint8_t *) kmalloc(object_size, GFP_KERNEL | GFP_ATOMIC);


    if (tmp == NULL)
    {
        return(CFG_WRITE_FAILED);
    }

    memset(tmp,0,object_size);

    *(tmp + 0) = cfg.ctrl;
    *(tmp + 1) = cfg.cmd;


    *(tmp + 2) = (uint8_t) (cfg.siglim[0].upsiglim & 0x00FF);
    *(tmp + 3) = (uint8_t) (cfg.siglim[0].upsiglim >> 8);

    *(tmp + 4) = (uint8_t) (cfg.siglim[0].losiglim & 0x00FF);
    *(tmp + 5) = (uint8_t) (cfg.siglim[0].losiglim >> 8);

    *(tmp + 6) = (uint8_t) (cfg.siglim[1].upsiglim & 0x00FF);
    *(tmp + 7) = (uint8_t) (cfg.siglim[1].upsiglim >> 8);

    *(tmp + 8) = (uint8_t) (cfg.siglim[1].losiglim & 0x00FF);
    *(tmp + 9) = (uint8_t) (cfg.siglim[1].losiglim >> 8);

    *(tmp + 10) = (uint8_t) (cfg.siglim[2].upsiglim & 0x00FF);
    *(tmp + 11) = (uint8_t) (cfg.siglim[2].upsiglim >> 8);

    *(tmp + 12) = (uint8_t) (cfg.siglim[2].losiglim & 0x00FF);
    *(tmp + 13) = (uint8_t) (cfg.siglim[2].losiglim >> 8);
    
    object_address = get_object_address(SPT_SELFTEST_T25,
        instance);

    if (object_address == 0)
    {
        kfree(tmp);
        return(CFG_WRITE_FAILED);
    }

    status = write_mem(object_address, object_size, tmp);

    kfree(tmp);
    return(status);
}



/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

uint8_t write_twotouchgesture_config(uint8_t instance, proci_twotouchgestureprocessor_t27_config_t cfg)
{

    uint16_t object_address;
    uint8_t *tmp;
    uint8_t status;
    uint8_t object_size;

    object_size = get_object_size(PROCI_TWOTOUCHGESTUREPROCESSOR_T27);
    if (object_size == 0)
    {
        return(CFG_WRITE_FAILED);
    }
    tmp = (uint8_t *) kmalloc(object_size, GFP_KERNEL | GFP_ATOMIC);

    if (tmp == NULL)
    {
        return(CFG_WRITE_FAILED);
    }

    memset(tmp,0,object_size);

    *(tmp + 0) = cfg.ctrl;

    *(tmp + 1) = cfg.numgest;

    *(tmp + 2) = 0;

    *(tmp + 3) = cfg.gesten;

    *(tmp + 4) = cfg.rotatethr;

    *(tmp + 5) = (uint8_t) (cfg.zoomthr & 0x00FF);
    *(tmp + 6) = (uint8_t) (cfg.zoomthr >> 8);

    object_address = get_object_address(PROCI_TWOTOUCHGESTUREPROCESSOR_T27,
        instance);

    if (object_address == 0)
    {
        kfree(tmp);
        return(CFG_WRITE_FAILED);
    }

    status = write_mem(object_address, object_size, tmp);

    kfree(tmp);

    return(status);

}


/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

uint8_t write_CTE_config(spt_cteconfig_t28_config_t cfg)
{

    return(write_simple_config(SPT_CTECONFIG_T28, 0, (void *) &cfg));
}


/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

uint8_t write_simple_config(uint8_t object_type, uint8_t instance, void *cfg)
{
    uint16_t object_address;
    uint8_t object_size;

    object_address = get_object_address(object_type, instance);
    object_size = get_object_size(object_type);

    if ((object_size == 0) || (object_address == 0))
    {
        return(CFG_WRITE_FAILED);
    }

    return (write_mem(object_address, object_size, cfg));
}



/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

uint8_t get_object_size(uint8_t object_type)
{
    uint8_t object_table_index = 0;
    uint8_t object_found = 0;
    uint16_t size = OBJECT_NOT_FOUND;

    object_t *object_table;
    object_t obj;

    if(info_block == NULL)
	return 0;
	
    object_table = info_block->objects;
    while ((object_table_index < info_block->info_id.num_declared_objects) &&
        !object_found)
    {
        obj = object_table[object_table_index];
        /* Does object type match? */
        if (obj.object_type == object_type)
        {
            object_found = 1;
            size = obj.size + 1;
        }
        object_table_index++;
    }

    return(size);
}

#ifdef __NEW__
uint8_t get_object_size(uint8_t object_type)
{
    uint8_t object_table_index = 0;
    uint8_t object_found = 0;
    uint16_t size = OBJECT_NOT_FOUND;

    object_t * object_table;
    object_t obj;
    object_table = info_block.objects;
    while ((object_table_index < info_block.info_id.num_declared_objects) &&
        !object_found)
    {
        obj = object_table[object_table_index];
        /* Does object type match? */
        if (obj.object_type == object_type)
        {
            object_found = 1;
            size = obj.size + 1;
        }
        object_table_index++;
    }

    return(size);
}

#endif


/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

uint8_t type_to_report_id(uint8_t object_type, uint8_t instance)
{

    uint8_t report_id = 1;
    int8_t report_id_found = 0;

    while((report_id <= max_report_id) && (report_id_found == 0))
    {
        if((report_id_map[report_id].object_type == object_type) &&
            (report_id_map[report_id].instance == instance))
        {
            report_id_found = 1;
        }
        else
        {
            report_id++;
        }
    }
    if (report_id_found)
    {
        return(report_id);
    }
    else
    {
        return(ID_MAPPING_FAILED);
    }
}



/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

uint8_t report_id_to_type(uint8_t report_id, uint8_t *instance)
{

    if (report_id <= max_report_id)
    {
        *instance = report_id_map[report_id].instance;
        return(report_id_map[report_id].object_type);
    }
    else
    {
        return(ID_MAPPING_FAILED);
    }

}



/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

uint8_t read_id_block(info_id_t *id)
{
    uint8_t status;

    status = read_mem(0, 1, (void *) &id->family_id);
    if (status != READ_MEM_OK)
    {
		dbg(KERN_ERR "%s: read_mem family_id failed \n",__func__);
        return(status);
    }

    status = read_mem(1, 1, (void *) &id->variant_id);
    if (status != READ_MEM_OK)
    {
		dbg(KERN_ERR "%s: read_mem variant_id failed \n",__func__);
        return(status);
    }

    status = read_mem(2, 1, (void *) &id->version);
    if (status != READ_MEM_OK)
    {
		dbg(KERN_ERR "%s: read_mem version failed \n",__func__);
       
        return(status);
    }

    status = read_mem(3, 1, (void *) &id->build);
    if (status != READ_MEM_OK)
    {
        return(status);
    }

    status = read_mem(4, 1, (void *) &id->matrix_x_size);
    if (status != READ_MEM_OK)
    {
        return(status);
    }

    status = read_mem(5, 1, (void *) &id->matrix_y_size);
    if (status != READ_MEM_OK)
    {
        return(status);
    }

    status = read_mem(6, 1, (void *) &id->num_declared_objects);

    return(status);

}


/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

uint8_t get_max_report_id(void)
{
    return(max_report_id);
}


/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

uint16_t get_object_address(uint8_t object_type, uint8_t instance)
{
    uint8_t object_table_index = 0;
    uint8_t address_found = 0;
    uint16_t address = OBJECT_NOT_FOUND;

    object_t *object_table;
    object_t obj;

    if(info_block == NULL)
	return 0;

    object_table = info_block->objects;
    while ((object_table_index < info_block->info_id.num_declared_objects) &&
        !address_found)
    {
        obj = object_table[object_table_index];
        /* Does object type match? */
        if (obj.object_type == object_type)
        {

            address_found = 1;

            /* Are there enough instances defined in the FW? */
            if (obj.instances >= instance)
            {
                address = obj.i2c_address + (obj.size + 1) * instance;
            }
        }
        object_table_index++;
    }

    return(address);
}

#ifdef __NEW__
/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

uint16_t get_object_address(uint8_t object_type, uint8_t instance)
{
    uint8_t object_table_index = 0;
    uint8_t address_found = 0;
    uint16_t address = OBJECT_NOT_FOUND;

    object_t *object_table;
    object_t obj;
    object_table = info_block.objects;
    while ((object_table_index < info_block.info_id.num_declared_objects) &&
        !address_found)
    {
        obj = object_table[object_table_index];
        /* Does object type match? */
        if (obj.object_type == object_type)
        {

            address_found = 1;

            /* Are there enough instances defined in the FW? */
            if (obj.instances >= instance)
            {
                address = obj.i2c_address + (obj.size + 1) * instance;
            }
        }
        object_table_index++;
    }

    return(address);
}
#endif

/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

uint32_t get_stored_infoblock_crc()
{
    uint32_t crc;
    crc = (uint32_t) (((uint32_t) info_block->CRC_hi) << 16);
    crc = crc | info_block->CRC;
    return(crc);
}


/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

uint8_t calculate_infoblock_crc(uint32_t *crc_pointer)
{

    uint32_t crc = 0;
    uint16_t crc_area_size;
    uint8_t *mem;

    uint8_t i;

    uint8_t status;

    /* 7 bytes of version data, 6 * NUM_OF_OBJECTS bytes of object table. */
    crc_area_size = 7 + info_block->info_id.num_declared_objects * 6;

    mem = (uint8_t *) kmalloc(crc_area_size, GFP_KERNEL | GFP_ATOMIC);
    if (mem == NULL)
    {
        return(CRC_CALCULATION_FAILED);
    }

    status = read_mem(0, crc_area_size, mem);

    if (status != READ_MEM_OK)
    {
        return(CRC_CALCULATION_FAILED);
    }

    i = 0;
    while (i < (crc_area_size - 1))
    {
        crc = CRC_24(crc, *(mem + i), *(mem + i + 1));
        i += 2;
    }

    crc = CRC_24(crc, *(mem + i), 0);

    kfree(mem);

    /* Return only 24 bit CRC. */
    *crc_pointer = (crc & 0x00FFFFFF);
    return(CRC_CALCULATION_OK);

}


/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

uint32_t CRC_24(uint32_t crc, uint8_t byte1, uint8_t byte2)
{
    static const uint32_t crcpoly = 0x80001B;
    uint32_t result;
    uint16_t data_word;

    data_word = (uint16_t) ((uint16_t) (byte2 << 8u) | byte1);
    result = ((crc << 1u) ^ (uint32_t) data_word);

    if (result & 0x1000000)
    {
        result ^= crcpoly;
    }

    return(result);

}

/*****************************************************************************
*
*       QT602240  Bootloader Block
*                                                             
* ***************************************************************************/

void qt_mem_clear(void)
{


	gen_powerconfig_t7_config_t blnk_power_config = {0};                 //Power config settings.
	gen_acquisitionconfig_t8_config_t blnk_acquisition_config = {0};     // Acquisition config. 
	touch_multitouchscreen_t9_config_t blnk_touchscreen_config = {0};    //Multitouch screen config.
	touch_keyarray_t15_config_t blnk_keyarray_config = {0};              //Key array config.
	proci_gripfacesuppression_t20_config_t blnk_gripfacesuppression_config = {0};    //Grip / face suppression config.
    procg_noisesuppression_t22_config_t blnk_noise_suppression_config = {0};         //Noise suppression config.
    touch_proximity_t23_config_t blnk_proximity_t23_config = {0};         //Noise suppression config.
   	proci_onetouchgestureprocessor_t24_config_t blnk_onetouch_gesture_config = {0};  //One-touch gesture config. 
    proci_twotouchgestureprocessor_t27_config_t blnk_twotouch_gesture_config = {0};  //Two-touch gesture config.
    spt_gpiopwm_t19_config_t  blnk_gpiopwm_config = {0};             //GPIO/PWM config
    spt_selftest_t25_config_t blnk_selftest_config = {0};            //Selftest config.
    spt_cteconfig_t28_config_t blnk_cte_config = {0};                //Capacitive touch engine config.
    spt_comcconfig_t18_config_t   blnk_comc_config = {0};            //Communication config settings.
    						
	if (write_power_config(blnk_power_config) != CFG_WRITE_OK)
	{
        dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
	}

	if (write_acquisition_config(blnk_acquisition_config) != CFG_WRITE_OK)
	{
        dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
	}

	//Touch Object
	if (write_multitouchscreen_config(0, blnk_touchscreen_config) != CFG_WRITE_OK)
	{
        dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
	}

    if (write_keyarray_config(0, blnk_keyarray_config) != CFG_WRITE_OK)
	{
        dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
	}
   
    	//Signal Processing Objects
    
   	if (get_object_address(PROCI_GRIPFACESUPPRESSION_T20, 0) != OBJECT_NOT_FOUND)
   	{
        if (write_gripsuppression_config(0, blnk_gripfacesuppression_config) !=
            CFG_WRITE_OK)
        	{
            dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
        	}
    	}
  
    	if (get_object_address(PROCG_NOISESUPPRESSION_T22, 0) != OBJECT_NOT_FOUND)
    	{
        	if (write_noisesuppression_config(0,blnk_noise_suppression_config) != CFG_WRITE_OK)
        	{
            dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
        }
    }

    if (get_object_address(TOUCH_PROXIMITY_T23, 0) != OBJECT_NOT_FOUND)
    {
        if (write_proximity_config(0,blnk_proximity_t23_config) != CFG_WRITE_OK)
        {
            dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
        	}
    	}
  
    if (get_object_address(PROCI_ONETOUCHGESTUREPROCESSOR_T24, 0) !=
        OBJECT_NOT_FOUND)
    	{
        if (write_onetouchgesture_config(0, blnk_onetouch_gesture_config) !=
            CFG_WRITE_OK)
        	{
            dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
        	}
    	}
  
        if(info_block->info_id.version < 0x20)  //V2.0 삭제
        {
            if (get_object_address(PROCI_TWOTOUCHGESTUREPROCESSOR_T27, 0) !=
                 OBJECT_NOT_FOUND)
        	{
                if (write_twotouchgesture_config(0, blnk_twotouch_gesture_config) !=
                    CFG_WRITE_OK)
        	    {
                    dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
                }
        	}
    	}
    
    	//Support Objects
    
    	if (write_gpio_config(0, blnk_gpiopwm_config) != CFG_WRITE_OK)
    	{
        dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
    	}    
    
    
    	if (get_object_address(SPT_SELFTEST_T25, 0) != OBJECT_NOT_FOUND)
    	{
        	if (write_selftest_config(0,blnk_selftest_config) != CFG_WRITE_OK)
        	{
            dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
        	}
    	}
    
    	if (get_object_address(SPT_CTECONFIG_T28, 0) != OBJECT_NOT_FOUND)
    	{
        	if (write_CTE_config(blnk_cte_config) != CFG_WRITE_OK)
        	{
            dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
        	}
    	}
    
    
    	if (get_object_address(SPT_COMCONFIG_T18, 0) != OBJECT_NOT_FOUND)
    	{
        	if (write_comc_config(0, blnk_comc_config) != CFG_WRITE_OK)
        	{
            dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
        	}
    	}
}

void touch_data_init(void)
{
    int i=0;

    for ( i= 0; i<MAX_NUM_FINGER; i++ )
    {
         fingerInfo[i].status = -1;
    }

    calibration_message = 0; 
    timer_tick = 0;
    timer_enabled = false;
    facesup_flag = 0;
    //hrtimer_cancel(&timer);
    poll_time.tv.nsec = 100 * NSEC_PER_MSEC;
}

/*------------------------------ main block -----------------------------------*/
void quantum_touch_probe(void)
{
        U8 report_id;
        U8 object_type, instance;
        U32 crc, stored_crc;
		unsigned int crc_temp=0;
        U8 family_id=0, variant=0, build=0;
#ifdef NOINITCHIP
        uint16_t object_address;
        uint16_t xres, yres;
        U8 xysize[3],  status;
#endif

#ifdef QT_FIRMUP_ENABLE
       

        if (init_touch_driver( QT602240_I2C_ADDR ) == DRIVER_SETUP_OK)
        {
                dbg("\n[TSP] Touch device found\n");
        }
        else
        {
                dbg("\n[TSP][ERROR] Touch device NOT found\n");
                return ;
        }

#else
        if (init_touch_driver( QT602240_I2C_ADDR , (void *) &message_handler) == DRIVER_SETUP_OK)
        {
                dbg("\n[TSP] Touch device found\n");
        }
        else
        {
                dbg("\n[TSP][ERROR] Touch device NOT found\n");
                return ;
        }
#endif
				dbg("\n[TSP] start\n");
                /* Get and show the version information. */
                get_family_id(&family_id);
                get_variant_id(&variant);
                get_version(&tsp_version);
                get_build_number(&build);


                dbg("Version:        0x%x\n\r", tsp_version);
                dbg("Family:                 0x%x\n\r", family_id);
                dbg("Variant:        0x%x\n\r", variant);
                dbg("Build number:   0x%x\n\r", build);

                dbg("Matrix X size : %d\n\r", info_block->info_id.matrix_x_size);
                dbg("Matrix Y size : %d\n\r", info_block->info_id.matrix_y_size);

                if(calculate_infoblock_crc( &crc_temp) != CRC_CALCULATION_OK)
                {
                        dbg("Calculating CRC failed, skipping check!\n\r");
						crc=(U32)crc_temp;
                }
                else
                {
						crc=(U32)crc_temp;
                        dbg("Calculated CRC:\t");
                        write_message_to_usart((uint8_t *) &crc, 4);
                        dbg("\n\r");
                }


                stored_crc = get_stored_infoblock_crc();
                dbg("Stored CRC:\t");
                write_message_to_usart((uint8_t *) &stored_crc, 4);
                dbg("\n\r");


                if (stored_crc != crc)
                {
                        dbg("Warning: info block CRC value doesn't match the calculated!\n\r");
                }
                else
                {
                        dbg("Info block CRC value OK.\n\n\r");

                }

                /* Test the report id to object type / instance mapping: get the maximum
                * report id and print the report id map. */

                dbg("Report ID to Object type / instance mapping:\n\r");

                max_report_id = get_max_report_id();
                for (report_id = 0; report_id <= max_report_id; report_id++)
                {
                        object_type = report_id_to_type(report_id, &instance);
                        dbg("[TSP] Report ID : %d, Object Type : T%d, Instance : %d\n\r",report_id ,object_type,instance);
                }

#ifdef NOINITCHIP
                object_address = get_object_address(TOUCH_MULTITOUCHSCREEN_T9,
                    0);

                status = read_U16(object_address+18, &xres);
                status = read_U16(object_address+20, &yres);
                status = read_mem(object_address+3, 2, (U8 *) xysize);
                dbg("[TSP] Read Chip xyInfo : xres(%d), yres(%d), xsize(%d), ysize(%d)\n\r",xres , yres, xysize[0], xysize[1]);

                if(NoInitChip == false){
                    if(!((yres == 479 || yres == 480) && xysize[0] == 19 && xysize[1] == 11))
                        NoInitChip = true;
                }    
#endif

#ifdef OPTION_WRITE_CONFIG

                qt_Power_Config_Init();

                qt_Acquisition_Config_Init();

                qt_Multitouchscreen_Init();

                qt_KeyArray_Init();

                qt_ComcConfig_Init();

                qt_Gpio_Pwm_Init();

                qt_Grip_Face_Suppression_Config_Init();

                qt_Noise_Suppression_Config_Init();

                qt_Proximity_Config_Init();

                qt_One_Touch_Gesture_Config_Init();

                qt_Selftest_Init();

                qt_Two_touch_Gesture_Config_Init();

                qt_CTE_Config_Init();

                /* Backup settings to NVM. */
                if (backup_config() != WRITE_MEM_OK)
                {
                        dbg("Failed to backup, exiting...\n\r");
                        return;
                }
                else
                {
                        //dbg("Backed up the config to non-volatile memory!\n\r");
                }

#else
                dbg("Chip setup sequence was bypassed!\n\r");
#endif /* OPTION_WRITE_CONFIG */

                /* Calibrate the touch IC. */
                if (calibrate_chip() != WRITE_MEM_OK)
                {
                        dbg("Failed to calibrate, exiting...\n\r");
                        return;
                }
                else
                {
                        //dbg("Chip calibrated!\n\r");

                }
                dbg("\nWaiting for touch chip messages...\n\n\r");

                touch_data_init();
                  
}

/*------------------------------ Sub functions -----------------------------------*/
/*!
  \brief Initializes touch driver.

  This function initializes the touch driver: tries to connect to given 
  address, sets the message handler pointer, reads the info block and object
  table, sets the message processor address etc.

  @param I2C_address is the address where to connect.
  @param (*handler) is a pointer to message handler function.
  @return DRIVER_SETUP_OK if init successful, DRIVER_SETUP_FAILED 
  otherwise.
  */
uint8_t init_touch_driver(uint8_t I2C_address)
{
        int i;

        int current_report_id = 0;

        uint8_t tmp;
        uint16_t current_address;
        uint16_t crc_address;
        object_t *object_table;
        info_id_t *id;

        //uint32_t *CRC;
        uint8_t status;


        dbg("[QT] %s start\n",__func__);

#ifdef QT_FIRMUP_ENABLE
        QT_i2c_address = I2C_address;
#endif
        /* Try to connect. */
        if(init_I2C(I2C_address) != CONNECT_OK)
        {
                dbg("[TSP][ERROR] 1\n");
                return(DRIVER_SETUP_INCOMPLETE);
        }


        /* Read the info block data. */

        id = (info_id_t *) kmalloc(sizeof(info_id_t), GFP_KERNEL | GFP_ATOMIC);
        if (id == NULL)
        {
				dbg(KERN_ERR "%s: kmalloc failed \n",__func__);
                return(DRIVER_SETUP_INCOMPLETE);
        }
		dbg(KERN_ERR "%s: kmalloc success \n",__func__);

        if (read_id_block(id) != 1)
        {
                dbg("[TSP][ERROR] 2\n");
                kfree(id);
                return(DRIVER_SETUP_INCOMPLETE);
        }  
		dbg(KERN_ERR "%s: read_id_block success \n",__func__);

        /* Read object table. */

        object_table = (object_t *) kmalloc(id->num_declared_objects * sizeof(object_t), GFP_KERNEL | GFP_ATOMIC);
        if (object_table == NULL)
        {
                dbg("[TSP][ERROR] 3\n");
                 kfree(id);
                return(DRIVER_SETUP_INCOMPLETE);
        }
		dbg(KERN_ERR "%s: object_table success \n",__func__);
		


        /* Reading the whole object table block to memory directly doesn't work cause sizeof object_t
           isn't necessarily the same on every compiler/platform due to alignment issues. Endianness
           can also cause trouble. */

        current_address = OBJECT_TABLE_START_ADDRESS;


        for (i = 0; i < id->num_declared_objects; i++)
        {
                status = read_mem(current_address, 1, &(object_table[i]).object_type);
                if (status != READ_MEM_OK)
                {
                        dbg("[TSP][ERROR] 4\n");
                        kfree(id);
                        return(DRIVER_SETUP_INCOMPLETE);
                }
                current_address++;

                status = read_U16(current_address, &object_table[i].i2c_address);
                if (status != READ_MEM_OK)
                {
                        dbg("[TSP][ERROR] 5\n");
                        kfree(id);
                        return(DRIVER_SETUP_INCOMPLETE);
                }
                current_address += 2;

                status = read_mem(current_address, 1, (U8*)&object_table[i].size);
                if (status != READ_MEM_OK)
                {
                        dbg("[TSP][ERROR] 6\n");
                         kfree(id);
                        return(DRIVER_SETUP_INCOMPLETE);
                }
                current_address++;

                status = read_mem(current_address, 1, &object_table[i].instances);
                if (status != READ_MEM_OK)
                {
                        dbg("[TSP][ERROR] 7\n");
                         kfree(id);
                        return(DRIVER_SETUP_INCOMPLETE);
                }
                current_address++;

                status = read_mem(current_address, 1, &object_table[i].num_report_ids);
                if (status != READ_MEM_OK)
                {
                        dbg("[TSP][ERROR] 8\n");
                        kfree(id);
                        return(DRIVER_SETUP_INCOMPLETE);
                }
                current_address++;

                max_report_id += object_table[i].num_report_ids;

                /* Find out the maximum message length. */
                if (object_table[i].object_type == GEN_MESSAGEPROCESSOR_T5)
                {
                        max_message_length = object_table[i].size + 1;
                }
        }

        /* Check that message processor was found. */
        if (max_message_length == 0)
        {
                dbg("[TSP][ERROR] 9\n");
                kfree(id);
                return(DRIVER_SETUP_INCOMPLETE);
        }

        /* Read CRC. */
//20101220 TERRY GS1 : KlwCR OUT
#if 0
        CRC = (uint32_t *) kmalloc(sizeof(info_id_t), GFP_KERNEL | GFP_ATOMIC);
        if (CRC == NULL)
        {
                dbg("[TSP][ERROR] 10\n");
                return(DRIVER_SETUP_INCOMPLETE);
        }
#endif


        info_block = kmalloc(sizeof(info_block_t), GFP_KERNEL | GFP_ATOMIC);
        if (info_block == NULL)
        {
                dbg("err\n");
                kfree(id);
                return(DRIVER_SETUP_INCOMPLETE);
        }


        info_block->info_id = *id;

        info_block->objects = object_table;

        crc_address = OBJECT_TABLE_START_ADDRESS + 
                id->num_declared_objects * OBJECT_TABLE_ELEMENT_SIZE;

        status = read_mem(crc_address, 1u, &tmp);
        if (status != READ_MEM_OK)
        {
                dbg("[TSP][ERROR] 11\n");
                kfree(id);
                return(DRIVER_SETUP_INCOMPLETE);
        }
        info_block->CRC = tmp;

        status = read_mem(crc_address + 1u, 1u, &tmp);
        if (status != READ_MEM_OK)
        {
                dbg("[TSP][ERROR] 12\n");
                kfree(id);
                return(DRIVER_SETUP_INCOMPLETE);
        }
        info_block->CRC |= (tmp << 8u);

        status = read_mem(crc_address + 2, 1, &info_block->CRC_hi);
        if (status != READ_MEM_OK)
        {
                dbg("[TSP][ERROR] 13\n");
                kfree(id);
                return(DRIVER_SETUP_INCOMPLETE);
        }

        /* Store message processor address, it is needed often on message reads. */
        message_processor_address = get_object_address(GEN_MESSAGEPROCESSOR_T5, 0);

        if (message_processor_address == 0)
        {
                dbg("[TSP][ERROR] 14 !!\n");
                kfree(id);
                return(DRIVER_SETUP_INCOMPLETE);
        }

        /* Store command processor address. */
        command_processor_address = get_object_address(GEN_COMMANDPROCESSOR_T6, 0);
        if (command_processor_address == 0)
        {
                dbg("[TSP][ERROR] 15\n");
                kfree(id);
                return(DRIVER_SETUP_INCOMPLETE);
        }

        quantum_msg = kmalloc(max_message_length, GFP_KERNEL | GFP_ATOMIC);
        if (quantum_msg == NULL)
        {
                dbg("[TSP][ERROR] 16\n");
                kfree(id);
                return(DRIVER_SETUP_INCOMPLETE);
        }

        /* Allocate memory for report id map now that the number of report id's 
         * is known. */

        report_id_map = kmalloc(sizeof(report_id_map_t) * max_report_id, GFP_KERNEL | GFP_ATOMIC);

        if (report_id_map == NULL)
        {
                dbg("[TSP][ERROR] 17\n");
                kfree(id);
                return(DRIVER_SETUP_INCOMPLETE);
        }


        /* Report ID 0 is reserved, so start from 1. */

        report_id_map[0].instance = 0;
        report_id_map[0].object_type = 0;
        current_report_id = 1;

        for (i = 0; i < id->num_declared_objects; i++)
        {
                if (object_table[i].num_report_ids != 0)
                {
                        int instance;
                        for (instance = 0; 
                                        instance <= object_table[i].instances; 
                                        instance++)
                        {
                                int start_report_id = current_report_id;
                                for (; 
                                                current_report_id < 
                                                (start_report_id + object_table[i].num_report_ids);
                                                current_report_id++)
                                {
                                        report_id_map[current_report_id].instance = instance;
                                        report_id_map[current_report_id].object_type = 
                                                object_table[i].object_type;
                                }
                        }
                }
        }

        kfree(id);
        driver_setup = DRIVER_SETUP_OK;

        /* Initialize the pin connected to touch ic pin CHANGELINE to catch the
         * falling edge of signal on that line. */

        return(DRIVER_SETUP_OK);
}

void read_all_register(void)
{
        U16 addr = 0;
        U8 msg;
        U16 calc_addr = 0;

        dbg("0%2d : ", addr);

        for(addr = 0 ; addr < 1273 ; addr++)
        {
                calc_addr = addr;


                if(read_mem(addr, 1, &msg) == READ_MEM_OK)
                {
                        dbg("(0x%2x)", msg);
                        if( (addr+1) % 10 == 0)
                        {
                                dbg("\n");
                                dbg("%2d : ", addr+1);
                        }

                }else
                {
                        dbg("\n\n[TSP][ERROR] %s() read fail !! \n", __FUNCTION__);
                }
        }
}


void  clear_event(void)
{
    int i;   
    
    for ( i= 0; i<MAX_NUM_FINGER; i++ )
    {
        if(fingerInfo[i].status == TSC_EVENT_WINDOW)
        {            
           dbg("TSP_Fin=%d,Up_WIN_XY(%d, %d)-forced\n", i, fingerInfo[i].x, fingerInfo[i].y);
           input_report_abs(qt602240->input_dev, ABS_MT_POSITION_X, fingerInfo[i].x);
           input_report_abs(qt602240->input_dev, ABS_MT_POSITION_Y, fingerInfo[i].y);
           input_report_abs(qt602240->input_dev, ABS_MT_TOUCH_MAJOR, TSC_EVENT_NONE);//pressure[i]); // 0이면 Release, 아니면 Press 상태(Down or Move)
           input_report_abs(qt602240->input_dev, ABS_MT_WIDTH_MAJOR, fingerInfo[i].id);            // Size 대신 ID 전달
           input_mt_sync(qt602240->input_dev);
           input_sync(qt602240->input_dev);
         }
         else if(fingerInfo[i].status == TSC_EVENT_KEY && 
			(fingerInfo[i].x == KEY_SEARCH || fingerInfo[i].x == KEY_BACK || fingerInfo[i].x == KEY_HOME || fingerInfo[i].x == KEY_MENU))
		{	
			input_report_key(qt602240->input_dev, fingerInfo[i].x , TSC_EVENT_NONE);
	       	input_sync(qt602240->input_dev);
		}
        fingerInfo[i].status = -1;
    }
}

void cal_maybe_good(void)
{
   /* Check if the timer is enabled */
   if(timer_enabled)
   {
      if(timer_tick > 5) /* Check if the timer timedout of 0.5seconds */
      {
         /* cal was good - don't need to check any more */
         cal_check_flag = 0;
         /* Disable the timer */
         hrtimer_cancel(&timer);
         timer_enabled = false;      
         timer_tick = 0; 
#if 0//ndef USE_FACE_SUP         
        qt_Grip_Face_Suppression_Config_Off();
#endif
        if(facesup_flag){
            clear_event();
            facesup_flag = 0; 
        }    
         
         /* Write back the normal acquisition config to chip. */
         if (write_acquisition_config(acquisition_config) != CFG_WRITE_OK)
         {
            /* "Acquisition config write failed!\n" */
            dbg("[TSP] [ERROR] line : %d\n", __LINE__);
            //ret = WRITE_MEM_FAILED; /* calling function should retry calibration call */
         }
         dbg("[TSP] calibration really good\n");
      }
      else
      {
         cal_check_flag = 1u;
      }
   }
   else
   {
      /* Timer not enabled, so enable it */
      hrtimer_start(&timer, poll_time, HRTIMER_MODE_REL); 
      timer_enabled = true;
      timer_tick = 0u;
      cal_check_flag = 1u;
   }   
}


/* Calibration Checking routine - called from interrupt handler */

/*!
 * \brief Used to ensure that calibration was good
 *
 * This function will check if the last calibration was good.
 * 
 * It should be called on every touch message whilst 'cal_check_flag' is set,
 * it will recalibrate the chip if the calibration is bad. If the calibration
 * is good it will restore the ATCHCALST and ATCHCALSTHR settings in the chip 
 * so that we do not get water issues.
 *
 *
 */
int8_t check_chip_calibration(void)
{
        uint8_t data_buffer[100] = { 0, };
        uint8_t try_ctr = 0;
        uint8_t data_byte = 0xF3; /* dianostic command to get touch flags */
        uint16_t diag_address;
        uint8_t tch_ch = 0, atch_ch = 0;
        uint8_t check_mask;
        uint8_t i;
        uint8_t j;
        uint8_t x_line_limit;
        uint8_t CAL_THR = 10;


    if (driver_setup != DRIVER_SETUP_OK)
        return -1;
   
        /* we have had the first touchscreen or face suppression message 
         * after a calibration - check the sensor state and try to confirm if
         * cal was good or bad */

        /* get touch flags from the chip using the diagnostic object */
        /* write command to command processor to get touch flags - 0xF3 Command required to do this */
        write_mem(command_processor_address + DIAGNOSTIC_OFFSET, 1, &data_byte);
        /* get the address of the diagnostic object so we can get the data we need */
        diag_address = get_object_address(DEBUG_DIAGNOSTIC_T37,0);

        msleep(20); 

        /* read touch flags from the diagnostic object - clear buffer so the while loop can run first time */
        memset( data_buffer , 0xFF, sizeof( data_buffer ) );

        /* wait for diagnostic object to update */
        while(!((data_buffer[0] == 0xF3) && (data_buffer[1] == 0x00)))
        {
                /* wait for data to be valid  */
                if(try_ctr > 100)
                {
                        /* Failed! */
         //dbg("[TSP] Diagnostic Data did not update!!\n");
                        break;
                }
                msleep(5); 
                try_ctr++; /* timeout counter */
                read_mem(diag_address, 2,data_buffer);
      //dbg("[TSP] Waiting for diagnostic data to update, try %d\n", try_ctr);
        }

        
#if 0
        /* data is ready - read the detection flags */
        read_mem(diag_address, 82,data_buffer);
#else
      /* data is ready - read the detection flags */
      if((info_block->info_id.variant_id == 0x06)||(info_block->info_id.variant_id == 0x07))
      {
        read_mem(diag_address, (15*2*2 + 2),data_buffer);   /* For mxT140 */
      }
      else
      {
        read_mem(diag_address, (20*2*2 + 2),data_buffer);   /* For mxT224 */
      }
#endif
        /* data array is 20 x 16 bits for each set of flags, 2 byte header, 40 bytes for touch flags 40 bytes for antitouch flags*/

        /* count up the channels/bits if we recived the data properly */
        if((data_buffer[0] == 0xF3) && (data_buffer[1] == 0x00))
        {
#if 0
                /* mode 0 : 16 x line, mode 1 : 17 etc etc upto mode 4.*/
                x_line_limit = 19 + cte_config.mode;

                if(x_line_limit > 20)
			    {
			      /* hard limit at 20 so we don't over-index the array */
	              x_line_limit = 20;
	            }
			    
	    
                /* double the limit as the array is in bytes not words */
                x_line_limit = x_line_limit << 1;

                /* count the channels and print the flags to the log */
                for(i = 0; i < x_line_limit; i+=2) /* check X lines - data is in words so increment 2 at a time */
                {
                        /* print the flags to the log - only really needed for debugging */
                        //dbg("[TSP] Detect Flags X%d, %x%x, %x%x \n", i>>1,data_buffer[3+i],data_buffer[2+i],data_buffer[43+i],data_buffer[42+i]);

                        /* count how many bits set for this row */
                        for(j = 0; j < 8; j++)
                        {
                                /* create a bit mask to check against */
                                check_mask = 1 << j;

                                /* check detect flags */
                                if(data_buffer[2+i] & check_mask)
                                {
                                        tch_ch++;
                                }
                                if(data_buffer[3+i] & check_mask)
                                {
                                        tch_ch++;
                                }

                                /* check anti-detect flags */
								if(data_buffer[42+i] & check_mask)
								{
								  atch_ch++;
								}
								if(data_buffer[43+i] & check_mask)
								{
								  atch_ch++;
								}
						}
                }
#else

            /*  for mxT224
            Mode 0 :  16 x 14
            Mode 1 :  17 x 13
            Mode 2 :  18 x 12
            Mode 3 :  19 x 11
            Mode 3 :  20 x 10
            */
        
            /*  for mxT140
            Mode 0 :  13 x 11
            Mode 1 :  14 x 10
            Mode 2 :  15 x  9
            */
        
            /* mode 0 : 16 x line, mode 1 : 17 etc etc upto mode 4.*/
        
            if((info_block->info_id.variant_id == 0x06)||(info_block->info_id.variant_id == 0x07))
            {
                x_line_limit = 13 + cte_config.mode;
        
                if(x_line_limit > 15)
                {
                  /* hard limit at 20 so we don't over-index the array */
                  x_line_limit = 15;
                }
        
            }
            else
            {
                x_line_limit = 16 + cte_config.mode;
        
                if(x_line_limit > 20)
                {
                  /* hard limit at 20 so we don't over-index the array */
                  x_line_limit = 20;
                }
        
            }
            
            /* double the limit as the array is in bytes not words */
            x_line_limit = x_line_limit << 1;
        
            /* count the channels and print the flags to the log */
            for(i = 0; i < x_line_limit; i+=2) /* check X lines - data is in words so increment 2 at a time */
            {
              /* print the flags to the log - only really needed for debugging */
                if((info_block->info_id.variant_id == 0x06)||(info_block->info_id.variant_id == 0x07))
                {
                    dbg("[TSP] Detect Flags X%d, %x%x, %x%x \n\r", i>>1,data_buffer[3+i],data_buffer[2+i],data_buffer[33+i],data_buffer[32+i]);
                }
                else
                {
                    dbg("[TSP] Detect Flags X%d, %x%x, %x%x \n\r", i>>1,data_buffer[3+i],data_buffer[2+i],data_buffer[43+i],data_buffer[42+i]);
                }
        
              /* count how many bits set for this row */
              for(j = 0; j < 8; j++)
              {
                /* create a bit mask to check against */
                check_mask = 1 << j;
        
                /* check detect flags */
                if(data_buffer[2+i] & check_mask)
                {
                  tch_ch++;
                }
                if(data_buffer[3+i] & check_mask)
                {
                  tch_ch++;
                }
        
                /* check anti-detect flags */
                if((info_block->info_id.variant_id == 0x06)||(info_block->info_id.variant_id == 0x07))
                {
                    if(data_buffer[32+i] & check_mask)
                    {
                      atch_ch++;
                    }
                    if(data_buffer[33+i] & check_mask)
                    {
                      atch_ch++;
                    }
                }
                else
                {
                    if(data_buffer[42+i] & check_mask)
                    {
                      atch_ch++;
                    }
                    if(data_buffer[43+i] & check_mask)
                    {
                      atch_ch++;
                    }
                }
              }
            }
#endif


                /* print how many channels we counted */
                dbg("[TSP] Flags Counted channels: t:%d a:%d \n", tch_ch, atch_ch);

                /* send page up command so we can detect when data updates next time,
                 * page byte will sit at 1 until we next send F3 command */
                data_byte = 0x01;
                write_mem(command_processor_address + DIAGNOSTIC_OFFSET, 1, &data_byte);

                /* process counters and decide if we must re-calibrate or if cal was good */      
                if((tch_ch) && (atch_ch == 0))
                {
                    dbg("[TSP] cal good\n");
                    //dbg("[TSP] calibration maybe good\n");
                    cal_maybe_good();
                    nCalCount =0;
                    return 0;
                }
                else if((tch_ch + CAL_THR /*10*/ ) <= atch_ch ||  nCalCount > 2)  //20101105 TERRY ES6 : Check chip cal
                {
                    //dbg("[TSP] calibration was bad\n");
                    dbg("[TSP] cal bad\n");

                    clear_event();
                    touch_data_init();

                    /* cal was bad - must recalibrate and check afterwards */
                    calibrate_chip();

                    /* Disable the timer */
                    hrtimer_cancel(&timer);
                    timer_enabled = false;
                    timer_tick = 0;

                    nCalCount =0;
                    
                    return -1;
                }
                else
                {
                     nCalCount++;
                     //dbg("[TSP] calibration was not decided yet\n");
                     dbg("[TSP] cal not decided\n");
                     /* we cannot confirm if good or bad - we must wait for next touch 
                      * message to confirm */
                     cal_check_flag = 1u;
                     /* Reset the 100ms timer */
                     if(!timer_enabled)
                         hrtimer_start(&timer, poll_time, HRTIMER_MODE_REL);       
                     timer_enabled = true;
                     timer_tick = 0u;

                     return 0;
                }
        }
        return 0;
}

/* Just an example, __timer is used to check if timer is enabled or not
   Timer should run every 100ms */

/* Timer interrupt which overflows every 100ms */
//static enum hrtimer_restart check_chip_calibration_timer_func(struct hrtimer *timer)
static void check_chip_calibration_timer_func(struct hrtimer *timer)
{
   //dbg("[TSP] check_chip_calibration_timer_func\n");

   timer_tick++;
   /* Check if the chip sent a face suppression message when in calibration */

   /*
   if(facesup_flag)
   {
        facesup_flag = 0;
   
        check_chip_calibration();
        #ifdef RECHECK_AFTER_CALIBRATION
        calibration_message = 0;
        #endif
        }
   */
    hrtimer_start(timer, poll_time, HRTIMER_MODE_REL);   
}

void cfg_error( void )
{
	qt_mem_clear();
	reset_chip();
	msleep(100);

	qt_Power_Config_Init();
	qt_Acquisition_Config_Init();
	qt_Multitouchscreen_Init();
	qt_ComcConfig_Init();
	qt_Gpio_Pwm_Init();
	//qt_Grip_Face_Suppression_Config_Init();
	qt_Noise_Suppression_Config_Init();
	qt_Proximity_Config_Init();
	qt_One_Touch_Gesture_Config_Init();
	qt_Selftest_Init();
	//qt_Two_touch_Gesture_Config_Init();
	qt_CTE_Config_Init();

	/* Backup settings to NVM. */
	if (backup_config() != WRITE_MEM_OK)
	{
		dbg("Failed to backup, exiting...\n\r");
		return;
	}
	else
	{
		//dbg("Backed up the config to non-volatile memory!\n\r");
	}

	reset_chip();
	msleep(100);
}


void  get_message(struct work_struct * p)
{
        unsigned long x, y;
        unsigned int press = 3;
        uint8_t ret_val = MESSAGE_READ_FAILED;
        int size=0,i;   
   //     int btn_report=0;       
        uint8_t touch_message_flag = 0;
        uint8_t object_type, instance;
        uint8_t valid_area/*, valid_key*/;
   //     static uint8_t message_sup_flag = 0;
   //     uint8_t facesup_message_flag = 0;
        int8_t ret_checkcal;
   //     static uint8_t pre_message_sup_flag = 0;
   //     unsigned long flags = 0;

#ifdef _SUPPORT_MULTITOUCH_
  //      static int nPrevID= -1;
        uint8_t id;
        int bChangeUpDn= 0;
#endif

        int release = 1;
        static long cnt=0;
  //      int amplitude=0;        
        int sum =0;
        
        cnt++;
	dbg("get_message \n");
	/* Get the lock */
        mutex_lock(&qt602240->lock);
        
        if (driver_setup == DRIVER_SETUP_OK)
        {
                if(read_mem(message_processor_address, max_message_length, quantum_msg) == READ_MEM_OK)
                {
                        /* Call the main application to handle the message. */
                        dbg("[TSP] msg id =  %d, msg = %x\n", quantum_msg[0], quantum_msg[1]);
                        
                        object_type = report_id_to_type(quantum_msg[0], &instance);
                        
                        if (object_type == PROCI_GRIPFACESUPPRESSION_T20)
                        {
								dbg("%s : object_type=PROCI_GRIPFACESUPPRESSION_T20 \n",__func__);
                                if((quantum_msg[1]&0x01) == 0x00)   
                                { 
                                        dbg("[TSP] Palm released\n");
                                        clear_event();
                                        facesup_flag = 0;
                                }
                                else
                                {
                                        dbg("[TSP] Palm suppressed\n");
                                        clear_event();
                                        facesup_flag = 1;
                                        //touch_message_flag = 1;
                                }
                                
                                //enable_irq(qt602240->client->irq);
                                //return ;
                        }
                        else if (object_type == TOUCH_MULTITOUCHSCREEN_T9)
                        {
#ifdef _SUPPORT_MULTITOUCH_
                                id= quantum_msg[0] - 2;
#endif
								dbg("%s : object_type == TOUCH_MULTITOUCHSCREEN_T9 \n",__func__);
                                
                                //if((( quantum_msg[1] & 0xC0 ) == 0xC0) || (( quantum_msg[1] & 0x90 ) == 0x90) || (quantum_msg[1] & 0x20 )) 
                                if( (( quantum_msg[1] & 0xC0 ) == 0xC0))  //press                                
                                {
                                        touch_message_flag = 1;
                                        //dbg("[TSP] Down Interrupt : (msg id =  %d, msg = %x)\n", quantum_msg[0], quantum_msg[1]);
                                        dbg("[TSP] Dn Int\n");
                                }                   
                                
                                x = quantum_msg[2];
                                x = x << 2;
                                x = x | quantum_msg[4] >> 6;

								x=479-x;
									
                                y = quantum_msg[3];
                                y = y << 2;
                                y = y | ((quantum_msg[4] & 0x6)  >> 2);
        
                                size = quantum_msg[5];

                        dbg("[TSP] quantum_msg[1]: %02x, msg id =  %d, x: %ld, y: %ld, status: %d\n", 
					        quantum_msg[1], id, x, y, fingerInfo[id].status);							


#if 0//ndef USE_FACE_SUP 
                                fingerInfo[id].area = size;
                                if(quantum_msg[1] & 0x20) // Release
                                    fingerInfo[id].area = 0;
                                //dbg("fingerid[%d] size = %d\n", id, fingerInfo[id].area );

                                facesup_message_flag = 0;
                                for ( i= 0; i<MAX_NUM_FINGER; i++ )
                                {
                                    if(fingerInfo[i].area > 20){
                                        //dbg(" oversize detected! size = %d\n", fingerInfo[i].area );                                       
                                        facesup_message_flag =  1;
                                    }
                                }

                                if(pre_message_sup_flag != facesup_message_flag){
                                    //dbg("[TSP] facesup_message_flag = %d\n", facesup_message_flag );
                                    dbg("[TSP] facesup_flag=%d\n", facesup_message_flag );
                                }    
                                pre_message_sup_flag = facesup_message_flag;

                                if(cal_check_flag == 0){
                                    if(facesup_message_flag ==  1 && message_sup_flag == 0)
                                    {
                                        //dbg("[TSP] message_sup_flag = %d\n", message_sup_flag );
                                        
                                        clear_event();
                                        message_sup_flag = 1;
                                        //enable_irq(qt602240->client->irq);
                                        //return ;
                                        goto interrupt_return;
                                    }
                                    else if (facesup_message_flag ==  1)
                                    {
                                        //dbg("[TSP] message_sup_flag = %d\n", message_sup_flag );

                                        //enable_irq(qt602240->client->irq);
                                        //return;
                                        goto interrupt_return;
                                    }
                                    else
                                    {
                                        message_sup_flag = 0;
                                    }
                                }     
#endif                                
/*
                                if( size > 20 ) 
                                {
                                        
                                        //dbg(" oversize detected! size = %d\n", size );
                                        enable_irq(qt602240->client->irq);
                                        return ;
                                }
*/                                                            

                                /* Move LCD to Touch Key area */
                				if(fingerInfo[id].status == TSC_EVENT_WINDOW && (int16_t)y > SCREEN_RESOLUTION_Y)
				                	y= SCREEN_RESOLUTION_Y-1;

                                if((int16_t)y > SCREEN_RESOLUTION_Y)	// Key area
                                {
                    			    if (quantum_msg[1] & 0x20 )	// release
                    				{
                    					release = 0;

                    					if(fingerInfo[id].status == TSC_EVENT_KEY) // key release
                    					{
                    						dbg("Move Between Keys %d and release\n", fingerInfo[id].x);
                    						input_report_key(qt602240->input_dev, fingerInfo[id].x, release);
                    						input_sync(qt602240->input_dev);
                    						bChangeUpDn= 0;
                    					}
                    					else
                    						bChangeUpDn= 1;
                    						
                    				    fingerInfo[id].status = -1;
                    				}
                    				else
                    				{
                    					if(fingerInfo[id].status != TSC_EVENT_KEY)
                    						bChangeUpDn= 1;
                    					else
                    						bChangeUpDn= 0;
                    						
                    					fingerInfo[id].status = TSC_EVENT_KEY;	// key area
                    				}

                    				if(bChangeUpDn)
                    				{
                    					bChangeUpDn= 0;

                    					if(x < KEY_XMIN || x > KEY_XMAX || y < (SCREEN_RESOLUTION_Y+KEY_AREA_Y_GAP) )
                    					{
                    						dbg("Ignore Key : %ld\n",x);
                    					}	
                    					else if(x <= KEY_BACK_AREA_X_MAX && x >= KEY_BACK_AREA_X_MIN)
                    					{
                    						dbg("KEY_BACK %d\n", release);
                   							input_report_key(qt602240->input_dev, KEY_BACK, release);
                   							input_sync(qt602240->input_dev);
                   							fingerInfo[id].x = KEY_BACK;						
                   						}
                   						else if(x <= KEY_SEARCH_AREA_X_MAX && x > KEY_SEARCH_AREA_X_MIN)
                   						{
                   							dbg("KEY_SEARCH %d\n", release);
                   							input_report_key(qt602240->input_dev, KEY_SEARCH, release);					
                   							input_sync(qt602240->input_dev);
                   							fingerInfo[id].x = KEY_SEARCH;	
                   						}
										else if(x <= KEY_HOME_AREA_X_MAX && x >= KEY_HOME_AREA_X_MIN)
                    					{
                    						dbg("KEY_HOME %d\n", release);
                   							input_report_key(qt602240->input_dev, KEY_HOME, release);
                   							input_sync(qt602240->input_dev);
                   							fingerInfo[id].x = KEY_HOME;						
                   						}
                   						else if(x <= KEY_MENU_AREA_X_MAX && x > KEY_MENU_AREA_X_MIN)
                   						{
                   							dbg("KEY_MENU %d\n", release);
                   							input_report_key(qt602240->input_dev, KEY_MENU, release);					
                   							input_sync(qt602240->input_dev);
                   							fingerInfo[id].x = KEY_MENU;	
                   						}
                   						else
                   						{
                   							// null key area
                   							dbg("KEY_RESERVED %d\n", release);
                   							if(fingerInfo[id].status != TSC_EVENT_KEY)
                   								fingerInfo[id].x = KEY_RESERVED;
                   						}
                   					}
                   				}


#ifdef _SUPPORT_MULTITOUCH_
                                else if ((quantum_msg[1] & 0x80) == 0x00) 
				                {
						            fingerInfo[id].status= TSC_EVENT_NONE;
						            bChangeUpDn= 1;
				                }
                                else if ( (quantum_msg[1] & 0x20 ) )    // Release
                                {
                                        if(fingerInfo[id].status == TSC_EVENT_KEY) // previous key area but current move lcd area
						                {       
							                input_report_key(qt602240->input_dev, fingerInfo[id].x, 0);
							                input_sync(qt602240->input_dev);														
						                }
                                        fingerInfo[id].status= TSC_EVENT_NONE;
                                        bChangeUpDn= 1;
                                        dbg("\n##### Finger[%d] Up (%d,%d)!\n\n", id, fingerInfo[id].x, fingerInfo[id].y );
                                }
                                else if ( (quantum_msg[1] & 0x80) && (quantum_msg[1] & 0x40) )       // Detect & Press
                                {

                                        if(fingerInfo[id].status == TSC_EVENT_KEY) // previous key area but current move lcd area
						                {
							                input_report_key(qt602240->input_dev, fingerInfo[id].x, 0);
							                input_sync(qt602240->input_dev);														
						                }
                                        fingerInfo[id].id= id;
                                        fingerInfo[id].status= TSC_EVENT_WINDOW;
                                        fingerInfo[id].x= (int16_t)x;
                                        fingerInfo[id].y= (int16_t)y;
                                        bChangeUpDn= 1;
                                        dbg("\n##### Finger[%d] Down (%d,%d)!\n\n", id, fingerInfo[id].x, fingerInfo[id].y );
                                }
                                else if ( (quantum_msg[1] & 0x80) && (quantum_msg[1] & 0x10) )  // Detect & Move
                                {    
                                        if(fingerInfo[id].status == TSC_EVENT_KEY) // previous key area but current move lcd area
						                {
							                input_report_key(qt602240->input_dev, fingerInfo[id].x, 0);
							                input_sync(qt602240->input_dev);														
					     	            }
                                    
                                      //  fingerInfo[id].id= id;
                                        fingerInfo[id].status= TSC_EVENT_WINDOW;
                                        fingerInfo[id].x= (int16_t)x;
                                        fingerInfo[id].y= (int16_t)y;
                                        bChangeUpDn= 1;
                                        dbg("##### Finger[%d] Move (%d,%d)!\n", id, fingerInfo[id].x, fingerInfo[id].y );
                                }
                               
#else                    
                                if( ((quantum_msg[1] & 0x80) == 0x80 ) && ((quantum_msg[1] & 0x40) == 0x40) )    // case 11000000 -> DETECT & PRESS
                                {
                                        press = 1;
                                        btn_report = 1;
                                        
                                        //input_report_key(qt602240->input_dev, BTN_TOUCH, 1);
                                }
                                else if( ((quantum_msg[1] & 0x80) == 0x80 ) && ((quantum_msg[1] & 0x10) == 0x10) )    // case 10010000 -> DETECT & MOVE
                                {
                                        press = 1;
                                }       
        
                                else if( ((quantum_msg[1] & 0x20 ) == 0x20))   // case 00100000 -> RELEASE
                                {
                                        press = 0;
                                        
                                        input_report_key(qt602240->input_dev, BTN_TOUCH, 0);
                                }
#endif                  
                                else
                                {
                                        press = 3;
                                        dbg("[TSP] Unknown state ! status = %d \n",quantum_msg[1]);

                                        //return;
                                        goto interrupt_return;
                                }

                                ret_val = MESSAGE_READ_OK;
                        }
                        else if (object_type == GEN_COMMANDPROCESSOR_T6) 
                        {
								dbg("[TSP] object_type == GEN_COMMANDPROCESSOR_T6 \n");
                                
								//Reset, Overflow, SigErr, Calibration, CFG Err ...
                                dbg("[TSP] msg id =  %d, status = 0x%x \n", quantum_msg[0],quantum_msg[1]);
                                //enable_irq(qt602240->client->irq);
                                //return ;
                                if ( (quantum_msg[1] & 0x08))	// CFG Err
						        {
									dbg("%s ,  %d\n",__FUNCTION__, __LINE__ );
									cfg_error();
                                }

                                goto interrupt_return;
                        }
               /*         else if (object_type == TOUCH_KEYARRAY_T15)
                        {
                            keyInfo[TSC_KEY_LEFT].update = false;
                            keyInfo[TSC_KEY_RIGHT].update = false;
                            dbg("[TSP] TOUCH_KEYARRAY_T15 (quantum_msg[1]: %d, quantum_msg[3]: %d, quantum_msg[4] %d)!\n\n", quantum_msg[1], quantum_msg[3], quantum_msg[4]);
                            
                            if((quantum_msg[1] & 0x80) ||((quantum_msg[1] & 0x80) == 0)){
                                if(keyInfo[TSC_KEY_LEFT].status != TSC_KEY_PRESSED && quantum_msg[3] & 0x01 ){  
                                    keyInfo[TSC_KEY_LEFT].code = KEY_MENU;
                                    keyInfo[TSC_KEY_LEFT].status = TSC_KEY_PRESSED;
                                    keyInfo[TSC_KEY_LEFT].update = true;
                                    dbg("[TSP] TSC_KEY_LEFT PRESSED!!! , Line %d \n\r", __LINE__);
                                }
                                else if(keyInfo[TSC_KEY_LEFT].status == TSC_KEY_PRESSED && (quantum_msg[3] & 0x01) == 0){
                                    keyInfo[TSC_KEY_LEFT].code = KEY_MENU;
                                    keyInfo[TSC_KEY_LEFT].status = TSC_KEY_RELEASED;
                                    keyInfo[TSC_KEY_LEFT].update = true;
                                    dbg("[TSP] TSC_KEY_LEFT RELEASED!!! , Line %d \n\r", __LINE__);
                                }    

                                
                                if(keyInfo[TSC_KEY_RIGHT].status != TSC_KEY_PRESSED && quantum_msg[4] & 0x02){
                                    keyInfo[TSC_KEY_RIGHT].code = KEY_BACK;
                                    keyInfo[TSC_KEY_RIGHT].status = TSC_KEY_PRESSED;
                                    keyInfo[TSC_KEY_RIGHT].update = true;
                                    dbg("[TSP] TSC_KEY_RIGHT PRESSED!!! , Line %d \n\r", __LINE__);
                                }
                                else if(keyInfo[TSC_KEY_RIGHT].status == TSC_KEY_PRESSED && (quantum_msg[4] & 0x02) == 0){
                                    keyInfo[TSC_KEY_RIGHT].code = KEY_BACK;
                                    keyInfo[TSC_KEY_RIGHT].status = TSC_KEY_RELEASED;
                                    keyInfo[TSC_KEY_RIGHT].update = true;
                                    dbg("[TSP] TSC_KEY_RIGHT RELEASED!!! , Line %d \n\r", __LINE__);
                                }    
                            }
                        }               
                    */
                        else if (object_type == PROCG_NOISESUPPRESSION_T22)
                        {
                       	    dbg("[TSP] Noise~~ msg id =  %d, status = 0x%x \n", quantum_msg[0],quantum_msg[1]);
				            goto interrupt_return;
			            }
                        else    
                        {
                                dbg("[TSP] msg id =  %d, status = 0x%x \n", quantum_msg[0],quantum_msg[1]);
                                /*
                                dbg("[TSP] msg id =  %d ", quantum_msg[0]);

                                for(i = 1; i  < max_message_length;i++)
                                {
                                        dbg("%x ",quantum_msg[i]);

                                }
                                dbg("\n");
                                */
                                //enable_irq(qt602240->client->irq);
                                //return ;
                                goto interrupt_return;
                        }
                                 
                }
                else
                {
					dbg("[TSP] clear_event before \n");
				    clear_event();
                    goto interrupt_return;
                    //return;
                }
          /* touch_message_flag - set when tehre is a touch message
             facesup_message_flag - set when there is a face suppression message
          */
          /* Check if the cal_check_flag is enabled, and if there are any touch or face suppression messages, check the calibration */      
          if(cal_check_flag == 1)
          {
            
            /*
            if(facesup_flag)
            {  
                check_chip_calibration();
                #ifdef RECHECK_AFTER_CALIBRATION
                calibration_message = 0;
                #endif
                facesup_flag = 0; 

                                enable_irq(qt602240->client->irq);
                                return ;
                        }
            */
                      
             ret_checkcal = 0;
             if(touch_message_flag || facesup_flag /*|| facesup_message_flag*/) /* Check if the chip sent a face suppression message when in calibration */
             {
                ret_checkcal = check_chip_calibration();
                #ifdef RECHECK_AFTER_CALIBRATION
                calibration_message = 0;
                #endif
                                 
                if(facesup_flag){
                    clear_event();
                    goto interrupt_return;
                }    
             }
            #ifdef RECHECK_AFTER_CALIBRATION
            else
            {
                 if(quantum_msg[0] == 0x01 && quantum_msg[1] == 0x10)
                 {
                 	 calibration_message = 1;
                     ret_checkcal = -1;
                 }
                 if((quantum_msg[0] == 0x01 && quantum_msg[1] == 0x00) && calibration_message)
                 {             
                 	 calibration_message = 0;
                	 msleep(100); 
                	 ret_checkcal = check_chip_calibration();
                 }
            }
             #endif	

             if(ret_checkcal == -1){
                //enable_irq(qt602240->client->irq);
                //return;
                goto interrupt_return;
             }
           }

        }
       
		
        valid_area = 0;
#ifdef _SUPPORT_MULTITOUCH_
        if (bChangeUpDn)
        {
            if(nCheckCalInt == TOUCH_CAL_END)			
		    {
                for (i= 0; i<MAX_NUM_FINGER; ++i )		             	
			{
				pre_fingerInfo[i].id = fingerInfo[i].id;
				pre_fingerInfo[i].status = fingerInfo[i].status;
				pre_fingerInfo[i].x = fingerInfo[i].x;
				pre_fingerInfo[i].y = fingerInfo[i].y;
				CheckCalOk[i] =0;
			}
			
			nCheckCalInt = TOUCH_INT_COMING;
            }

            for ( i= 0; i<MAX_NUM_FINGER; i++ )
            {

                if ( fingerInfo[i].status == -1 || fingerInfo[i].status == TSC_EVENT_KEY) 
                   continue;

                
                        dbg("TSP_Fin=%d,Dn_WIN_XY(%d, %d)\n", i, fingerInfo[i].x, fingerInfo[i].y);

                        input_report_abs(qt602240->input_dev, ABS_MT_POSITION_X, fingerInfo[i].x);
                        input_report_abs(qt602240->input_dev, ABS_MT_POSITION_Y, fingerInfo[i].y);
                        input_report_abs(qt602240->input_dev, ABS_MT_TOUCH_MAJOR, fingerInfo[i].status);//pressure[i]); // 0이면 Release, 아니면 Press 상태(Down or Move)
                        input_report_abs(qt602240->input_dev, ABS_MT_WIDTH_MAJOR, fingerInfo[i].id);            // Size 대신 ID 전달
                        input_mt_sync(qt602240->input_dev);

                        dbg("[%d] ID: %d, x: %d, y: %d\n", fingerInfo[i].status, fingerInfo[i].id, fingerInfo[i].x, fingerInfo[i].y);

			            if ( fingerInfo[i].status == TSC_EVENT_NONE ) fingerInfo[i].status= -1;
				            valid_area = 1;

                        if(nCheckCalInt == TOUCH_INT_COMING)
                        {
  			            	if(fingerInfo[i].x != 0 || fingerInfo[i].y != 0)
  			            	{
  		            			if(fingerInfo[i].status != -1 && fingerInfo[i].status == pre_fingerInfo[i].status &&
  		            				abs(fingerInfo[i].x - pre_fingerInfo[i].x) <6 && abs(fingerInfo[i].y - pre_fingerInfo[i].y) < 6)
  			            			CheckCalOk[i]++;
        				    	else
        			    		{
        			    			pre_fingerInfo[i].id = fingerInfo[i].id;
        			    			pre_fingerInfo[i].status = fingerInfo[i].status;
        			    			pre_fingerInfo[i].x = fingerInfo[i].x;
        			    			pre_fingerInfo[i].y = fingerInfo[i].y;
        			    			CheckCalOk[i] =0;
        			    		}

                                if(CheckCalOk[i] > 10)
        					    {
        					    	dbg("Touch Cal Fail!, restart Calibrate!~~~!\n");
        					    	calibrate_chip();
        					    }
  			            	}
  		            	 }
                    
            }      

            if(valid_area != 0)
			    input_sync(qt602240->input_dev);

    		if(nCheckCalInt == TOUCH_INT_COMING)
    		{
    			for (i= 0; i<MAX_NUM_FINGER; ++i )
    				sum += CheckCalOk[i];
    
    			if(sum == 0)
    				CheckResumeCnt++;
    			else
    				CheckResumeCnt =0;
    
    			if(CheckResumeCnt > 30 && nCheckCalInt == TOUCH_INT_COMING)
    			{
    				dbg("Touch Cal Success!~~~! %d\n", CheckResumeCnt);
    				nCheckCalInt = TOUCH_CAL_SUCCESS;
    			}
    		}
            }   

    
#else

        if(press == 1)
        {
                input_report_abs(qt602240->input_dev, ABS_X, x);
                input_report_abs(qt602240->input_dev, ABS_Y, y);
                if( btn_report == 1)
                        input_report_key(qt602240->input_dev, BTN_TOUCH, 1);

                input_sync(qt602240->input_dev);
                amplitude = quantum_msg[6];
                dbg("[TSP]%s x=%3d, y=%3d, BTN=%d, size=%d, amp=%d\n",__FUNCTION__, x, y,press, size , amplitude);

#ifdef CONFIG_CPU_FREQ
#if USE_PERF_LEVEL_TS
                //set_dvfs_perf_level();
#endif
#endif

        }
        else if(press == 0)
        {
                input_sync(qt602240->input_dev);
                amplitude = quantum_msg[6];
                dbg("[TSP]%s x=%3d, y=%3d, BTN=%d, size=%d, amp=%d\n",__FUNCTION__, x, y,press, size , amplitude);
        }
#endif


        //if(readl(gpio_pend_mask_mem)&(0x1<<5))
        //      writel(readl(gpio_pend_mask_mem)|(0x1<<5), gpio_pend_mask_mem); 

        //enable_irq(qt602240->client->irq);
interrupt_return:
        mutex_unlock(&qt602240->lock);

        return ;
}




/*------------------------------ I2C Driver block -----------------------------------*/



#define I2C_M_WR 0 /* for i2c */
#define I2C_MAX_SEND_LENGTH     300
int qt602240_i2c_write(u16 reg, u8 *read_val, unsigned int len)
{
        struct i2c_msg wmsg;
        unsigned char data[I2C_MAX_SEND_LENGTH];
        int ret,i;

        address_pointer = reg;

        if(len+2 > I2C_MAX_SEND_LENGTH)
        {
                dbg("[TSP][ERROR] %s() data length error\n", __FUNCTION__);
                return -ENODEV;
        }

        wmsg.addr = qt602240->client->addr;
        wmsg.flags = I2C_M_WR;
        wmsg.len = len + 2;
        wmsg.buf = data;

        data[0] = reg & 0x00ff;
        data[1] = reg >> 8;

        for (i = 0; i < len; i++)
        {
                data[i+2] = *(read_val+i);
        }

        ret = i2c_transfer(qt602240->client->adapter, &wmsg, 1);
        return ret;
}

int boot_qt602240_i2c_write(u16 reg, u8 *read_val, unsigned int len)
{
        struct i2c_msg wmsg;
        unsigned char data[I2C_MAX_SEND_LENGTH];
        int ret,i;

        if(len+2 > I2C_MAX_SEND_LENGTH)
        {
                dbg("[TSP][ERROR] %s() data length error\n", __FUNCTION__);
                return -ENODEV;
        }

        wmsg.addr = 0x24;
        wmsg.flags = I2C_M_WR;
        wmsg.len = len;
        wmsg.buf = data;


        for (i = 0; i < len; i++)
        {
                data[i] = *(read_val+i);
        }

        ret = i2c_transfer(qt602240->client->adapter, &wmsg, 1);
        return ret;
}


int qt602240_i2c_read(u16 reg,unsigned char *rbuf, int buf_size)
{
        static unsigned char first_read=1;
        struct i2c_msg rmsg;
        int ret;
        unsigned char data[2];

        rmsg.addr = qt602240->client->addr;
		dbg("%s : qt602240_i2c_read \n",__func__);
    if(first_read == 1)
        {
                first_read = 0;
                address_pointer = reg+1;
        }

        if((address_pointer != reg) || (reg != message_processor_address))
        {
                address_pointer = reg;
				dbg("%s : qt602240_i2c_read  1 \n",__func__);
                rmsg.flags = I2C_M_WR;
                rmsg.len = 2;
                rmsg.buf = data;
                data[0] = reg & 0x00ff;
                data[1] = reg >> 8;
                ret = i2c_transfer(qt602240->client->adapter, &rmsg, 1);
        }
				dbg("%s : qt602240_i2c_read  2 \n",__func__);
                rmsg.flags = I2C_M_RD;
                rmsg.len = buf_size;
                rmsg.buf = rbuf;
                ret = i2c_transfer(qt602240->client->adapter, &rmsg, 1);

        return ret;
}
/* ------------------------- ????????????? -----------------*/
/*!
 * \brief Initializes the I2C interface.
 *
 * @param I2C_address_arg the touch chip I2C address.
 *
 */
U8 init_I2C(U8 I2C_address_arg)
{

        dbg("[QT] %s start\n",__func__);   

        return (I2C_INIT_OK);
}



/*! \brief Enables pin change interrupts on falling edge. */
void enable_changeline_int(void)
{
}

/*! \brief Returns the changeline state. */
U8 read_changeline(void)
{
        return 0;

}

/*! \brief Maxtouch Memory read by I2C bus */
U8 read_mem(U16 start, U8 size, U8 *mem)
{
        int ret;
		dbg("%s : read_mem \n",__func__);
        memset(mem,0xFF,size);
		
        ret = qt602240_i2c_read(start,mem,size);
        if(ret < 0) {
                dbg("%s : i2c read failed\n",__func__);
                return(READ_MEM_FAILED);
        } 

        return(READ_MEM_OK);
}

U8 boot_read_mem(U16 start, U8 size, U8 *mem)
{
        struct i2c_msg rmsg;
        int ret;

        rmsg.addr=0x24;
        rmsg.flags = I2C_M_RD;
        rmsg.len = size;
        rmsg.buf = mem;
        ret = i2c_transfer(qt602240->client->adapter, &rmsg, 1);
        
        return ret;
}

U8 read_U16(U16 start, U16 *mem)
{
        U8 status;

        status = read_mem(start, 2, (U8 *) mem);

        return (status);
}

U8 write_mem(U16 start, U8 size, U8 *mem)
{
        int ret;

        ret = qt602240_i2c_write(start,mem,size);
        if(ret < 0) 
                return(WRITE_MEM_FAILED);
        else
                return(WRITE_MEM_OK);
}

U8 boot_write_mem(U16 start, U16 size, U8 *mem)
{
        int ret;

        ret = boot_qt602240_i2c_write(start,mem,size);
        if(ret < 0){
                dbg("boot write mem fail: %d \n",ret);
                return(WRITE_MEM_FAILED);
        }
        else
                return(WRITE_MEM_OK);
}




/*****************************************************************************
*
*  FUNCTION
*  PURPOSE
*  INPUT
*  OUTPUT
*
* ***************************************************************************/

void write_message_to_usart(uint8_t msg[], uint8_t length)
{
    int i;
    for (i=0; i < length; i++)
    {
        dbg("0x%02x ", msg[i]);
    }
    dbg("\n\r");
}

/*
 * Message handler that is called by the touch chip driver when messages
 * are received.
 * 
 * This example message handler simply prints the messages as they are
 * received to USART1 port of EVK1011 board.
 */
void message_handler(U8 *msg, U8 length)
{  
}


irqreturn_t qt602240_irq_handler(int irq, void *dev_id)
{
        //disable_irq(qt602240->client->irq);

        queue_work(qt602240_wq, &qt602240->work);

        return IRQ_HANDLED;
}


static int qt602240_remove(struct i2c_client *client)
{
        free_irq(client->irq, 0);

#ifdef TOUCH_IO
    	misc_deregister(&touch_io);
#endif //TOUCH_IO
#ifdef SKY_PROCESS_CMD_KEY
	misc_deregister(&touch_event);
#endif    
        
        input_unregister_device(qt602240->input_dev);
        if (qt602240_wq)
                destroy_workqueue(qt602240_wq);
        kfree(qt602240);        
        return 0;
}

#ifdef CONFIG_PM
#ifndef CONFIG_HAS_EARLYSUSPEND
static int qt602240_suspend(struct i2c_client *client, pm_message_t message)
{
 

        ENTER_FUNC;
        disable_irq(client->irq);
        LEAVE_FUNC;

        hrtimer_cancel(&timer);
        timer_enabled = false;
        
#if 0//ndef USE_FACE_SUP 
        qt_Grip_Face_Suppression_Config_On();
#endif
#ifdef TOUCH_IO
        qt_charger_mode_config(BATTERY_PLUGGED_AC);
#endif
	qt_sleep_Init();

        clear_event();
    
        return 0;
}

static int qt602240_resume(struct i2c_client *client)
{
        int ret,i;
     
        touch_data_init();

#ifdef TOUCH_IO
        switch (charger_mode)
        {
            case BATTERY_PLUGGED_NONE:
                qt_charger_mode_config(BATTERY_PLUGGED_NONE);
                break;
            case BATTERY_PLUGGED_AC:
                qt_charger_mode_config(BATTERY_PLUGGED_AC);
                break;
            case BATTERY_PLUGGED_USB:
                qt_charger_mode_config(BATTERY_PLUGGED_USB);
                break;
            default:
                break;
        }
#endif        
        qt_Power_Config_Init();
        calibrate_chip();
        
        ENTER_FUNC;
        enable_irq(client->irq);
        LEAVE_FUNC;
 
        return 0;
}
#endif
#endif

#define GPIO_PIN_TOUCH_SCL	0
#define GPIO_PIN_TOUCH_SDA	1
static uint32_t touch_gpio_table[] = {
	GPIO_CFG(GPIO_PIN_TOUCH_SCL, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA ),
	GPIO_CFG(GPIO_PIN_TOUCH_SDA, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA ),
};
void I2C_ENABLE(void){
	int i,rtn;
	for (i = 0; i < ARRAY_SIZE(touch_gpio_table); ++i)
	{
		 rtn = gpio_tlmm_config(touch_gpio_table[i], GPIO_CFG_ENABLE);
		 if ( rtn ) {
			  dbg(KERN_ERR "%s: Failed to touch_gpio_table i2c gpio_tlmm_config(%d)=>(%d)\n", __func__, i ,rtn);
		 }
	}
}

/* I2C driver probe function */
static int __devinit qt602240_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
        int ret;
	    int result;
        
        qt602240 = kzalloc(sizeof(struct qt602240), GFP_KERNEL);
        if (qt602240 == NULL) {
                return -ENOMEM;
        }
        qt602240->client=client;
        ///////////////////////////////////////////////////////////////////////////////
        dbg("[QT] %s/%d\n",__func__,__LINE__);
		
        qt602240_wq = create_singlethread_workqueue("qt602240_wq");
        if (!qt602240_wq)
                return -ENOMEM;

        if(!touch_atmel_class)
                touch_atmel_class=class_create(THIS_MODULE, "touch_ateml");
        ts_dev = device_create(touch_atmel_class, NULL, 0, NULL, "ts");
        if (IS_ERR(ts_dev))
                pr_err("Failed to create device(ts)!\n");
        if (device_create_file(ts_dev, &dev_attr_gpio) < 0)
                pr_err("Failed to create device file(%s)!\n", dev_attr_gpio.attr.name);
        if (device_create_file(ts_dev, &dev_attr_i2c) < 0)
                pr_err("Failed to create device file(%s)!\n", dev_attr_i2c.attr.name);
        if (device_create_file(ts_dev, &dev_attr_setup) < 0)
                pr_err("Failed to create device file(%s)!\n", dev_attr_setup.attr.name);
        dbg("[QT] %s/%d, platform_driver_register!!\n",__func__,__LINE__);
        ////////////////////////////////////////////////////////////////////////////////////////
        
        dbg("+-----------------------------------------+\n");
        dbg("|  Quantum Touch Driver Probe!            |\n");
        dbg("+-----------------------------------------+\n");

        INIT_WORK(&qt602240->work, get_message );

        qt602240->input_dev = input_allocate_device();
        if (qt602240->input_dev == NULL) {
                ret = -ENOMEM;
                dbg("qt602240_probe: Failed to allocate input device\n");
                goto err_input_dev_alloc_failed;
        }
        qt602240->input_dev->name = "qt602240_ts_input";
        set_bit(EV_SYN, qt602240->input_dev->evbit);
        set_bit(EV_KEY, qt602240->input_dev->evbit);
        set_bit(BTN_TOUCH, qt602240->input_dev->keybit);
#if 1 //(BOARD_VER >= SP33_ES10)
        set_bit(KEY_HOME, qt602240->input_dev->keybit);
#endif
        set_bit(KEY_MENU, qt602240->input_dev->keybit);
        set_bit(KEY_BACK, qt602240->input_dev->keybit);
        set_bit(EV_ABS, qt602240->input_dev->evbit);
        set_bit(KEY_SEARCH, qt602240->input_dev->keybit);


#ifdef SKY_PROCESS_CMD_KEY
  	set_bit(KEY_0, qt602240->input_dev->keybit);
	set_bit(KEY_1, qt602240->input_dev->keybit);
	set_bit(KEY_2, qt602240->input_dev->keybit);
	set_bit(KEY_3, qt602240->input_dev->keybit);
	set_bit(KEY_4, qt602240->input_dev->keybit);
	set_bit(KEY_5, qt602240->input_dev->keybit);
	set_bit(KEY_6, qt602240->input_dev->keybit);
	set_bit(KEY_7, qt602240->input_dev->keybit);
	set_bit(KEY_8, qt602240->input_dev->keybit);
	set_bit(KEY_9, qt602240->input_dev->keybit);
	set_bit(KEY_KPASTERISK, qt602240->input_dev->keybit);
	set_bit(0x20b, qt602240->input_dev->keybit);

	set_bit(KEY_STAR, qt602240->input_dev->keybit);
	set_bit(KEY_SHARP, qt602240->input_dev->keybit);

	set_bit(KEY_LEFTSHIFT, qt602240->input_dev->keybit);
	set_bit(KEY_RIGHTSHIFT, qt602240->input_dev->keybit);


	set_bit(KEY_LEFT, qt602240->input_dev->keybit);
	set_bit(KEY_RIGHT, qt602240->input_dev->keybit);
	set_bit(KEY_UP, qt602240->input_dev->keybit);
	set_bit(KEY_DOWN, qt602240->input_dev->keybit);
	set_bit(KEY_ENTER, qt602240->input_dev->keybit);
	
	set_bit(KEY_SEND, qt602240->input_dev->keybit);
	set_bit(KEY_END, qt602240->input_dev->keybit);
	
	set_bit(KEY_VOLUMEUP, qt602240->input_dev->keybit);
	set_bit(KEY_VOLUMEDOWN, qt602240->input_dev->keybit);

	set_bit(KEY_CLEAR, qt602240->input_dev->keybit);

	set_bit(KEY_CAMERA, qt602240->input_dev->keybit);
	set_bit(KEY_FOCUS, qt602240->input_dev->keybit);
	set_bit(KEY_HOLD, qt602240->input_dev->keybit);
    set_bit(KEY_BACKSPACE, qt602240->input_dev->keybit);

    set_bit(KEY_A, qt602240->input_dev->keybit);
	set_bit(KEY_B, qt602240->input_dev->keybit);
	set_bit(KEY_C, qt602240->input_dev->keybit);
	set_bit(KEY_D, qt602240->input_dev->keybit);
	set_bit(KEY_E, qt602240->input_dev->keybit);
	set_bit(KEY_F, qt602240->input_dev->keybit);
	set_bit(KEY_G, qt602240->input_dev->keybit);
	set_bit(KEY_H, qt602240->input_dev->keybit);
	set_bit(KEY_I, qt602240->input_dev->keybit);
	set_bit(KEY_J, qt602240->input_dev->keybit);
	set_bit(KEY_K, qt602240->input_dev->keybit);
	set_bit(KEY_L, qt602240->input_dev->keybit);
	set_bit(KEY_M, qt602240->input_dev->keybit);
	set_bit(KEY_N, qt602240->input_dev->keybit);
	set_bit(KEY_O, qt602240->input_dev->keybit);
	set_bit(KEY_P, qt602240->input_dev->keybit);
	set_bit(KEY_Q, qt602240->input_dev->keybit);
	set_bit(KEY_R, qt602240->input_dev->keybit);
	set_bit(KEY_S, qt602240->input_dev->keybit);
	set_bit(KEY_T, qt602240->input_dev->keybit);
	set_bit(KEY_U, qt602240->input_dev->keybit);
	set_bit(KEY_V, qt602240->input_dev->keybit);
	set_bit(KEY_W, qt602240->input_dev->keybit);
	set_bit(KEY_X, qt602240->input_dev->keybit);
	set_bit(KEY_Y, qt602240->input_dev->keybit);
	set_bit(KEY_Z, qt602240->input_dev->keybit); 
	set_bit(KEY_COMMA, qt602240->input_dev->keybit); 
	set_bit(KEY_QUESTION, qt602240->input_dev->keybit); 
	set_bit(KEY_DOT, qt602240->input_dev->keybit);
	set_bit(KEY_LEFTALT, qt602240->input_dev->keybit);
	set_bit(KEY_LEFTSHIFT, qt602240->input_dev->keybit);
    set_bit(KEY_LEFTBRACE, qt602240->input_dev->keybit);
  //  set_bit(KEY_KPPLUS, qt602240->input_dev->keybit);
  //  set_bit(KEY_COMPOSE, qt602240->input_dev->keybit);
  	set_bit(KEY_SPACE, qt602240->input_dev->keybit);
	set_bit(KEY_MENU, qt602240->input_dev->keybit);
 	set_bit(KEY_HOME, qt602240->input_dev->keybit);
	set_bit(KEY_BACK, qt602240->input_dev->keybit);
    input_set_capability(qt602240->input_dev,EV_KEY, KEY_KPPLUS);
    input_set_capability(qt602240->input_dev,EV_KEY, KEY_COMPOSE);
#endif // SKY_PROCESS_CMD_KEY



        input_set_abs_params(qt602240->input_dev, ABS_X, 0, SCREEN_RESOLUTION_X-1, 0, 0);  // 480
        input_set_abs_params(qt602240->input_dev, ABS_Y, 0, SCREEN_RESOLUTION_Y-1, 0, 0);         // 800

        input_set_abs_params(qt602240->input_dev, ABS_PRESSURE, 0, 255, 0, 0);
        input_set_abs_params(qt602240->input_dev, ABS_TOOL_WIDTH, 0, 15, 0, 0);
#ifdef _SUPPORT_MULTITOUCH_
        input_set_abs_params(qt602240->input_dev, ABS_MT_POSITION_X, 0, SCREEN_RESOLUTION_X - 1, 0, 0);
        input_set_abs_params(qt602240->input_dev, ABS_MT_POSITION_Y, 0, SCREEN_RESOLUTION_Y - 1, 0, 0);
        input_set_abs_params(qt602240->input_dev, ABS_MT_TOUCH_MAJOR, 0, 255, 0, 0);
        input_set_abs_params(qt602240->input_dev, ABS_MT_WIDTH_MAJOR, 0, 15, 0, 0);
#endif  

        ret = input_register_device(qt602240->input_dev);
        if (ret) {
                dbg("qt602240_probe: Unable to register %s input device\n", qt602240->input_dev->name);
                goto err_input_register_device_failed;
        }

#ifdef TOUCH_IO  
        ret = misc_register(&touch_io);
	if (ret) 
    	{
        	dbg("qt602240_probe: misc_register failed.\n");
	}
#endif //TOUCH_IO

        mutex_init(&qt602240->lock);

#ifdef QT_FIRMUP_ENABLE
{
        msleep(1000);
        QT_reprogram();
}
#else
        quantum_touch_probe();
#endif

#ifdef NOINITCHIP
        if(NoInitChip)
        {
            TSP_Restart();
        quantum_touch_probe();

            NoInitChip = false;
        }
#endif

        qt602240->client->irq = IRQ_TOUCH_INT;
        ret = request_irq(qt602240->client->irq, qt602240_irq_handler, IRQF_TRIGGER_FALLING , "qt602240-irq", qt602240);
        if (ret == 0) {
                dbg("qt602240_probe: Start touchscreen %s\n", qt602240->input_dev->name);
        }
        else {
                dbg("request_irq failed\n");
        }


#ifdef CONFIG_PM
#ifdef CONFIG_HAS_EARLYSUSPEND
    qt602240->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN;
    qt602240->early_suspend.suspend = (void *)qt602240_early_suspend;
    qt602240->early_suspend.resume = (void *)qt602240_late_resume;
    register_early_suspend(&qt602240->early_suspend);
#endif
#endif


#ifdef SKY_PROCESS_CMD_KEY
	result = misc_register(&touch_event);
	if (result) {
		dbg("touch diag register failed\n");
	}
#endif    

    hrtimer_init(&timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    timer.function = (void *)check_chip_calibration_timer_func;

    nCheckCalInt = TOUCH_CAL_END;

    dbg("%s ,  %d\n",__FUNCTION__, __LINE__ );
    //      enable_irq(qt602240->client->irq);
    return 0;

err_input_register_device_failed:
        input_free_device(qt602240->input_dev);

err_input_dev_alloc_failed:
        kfree(qt602240);
        return ret;
}
        
#ifdef CONFIG_PM
#ifdef CONFIG_HAS_EARLYSUSPEND
static int  qt602240_early_suspend(struct early_suspend *h)
{
	struct qt602240 *ip;
	dbg_func_in();
	ip = container_of(h, struct qt602240, early_suspend);

    dbg("[TSP] qt602240_early_suspend\n");

 //   read_all_register();
     

    disable_irq(ip->client->irq);

    hrtimer_cancel(&timer);
    timer_enabled = false;

#if 0//ndef USE_FACE_SUP 
        qt_Grip_Face_Suppression_Config_On();
#endif

//        check_call_sleep = call_sleep;

#ifdef TOUCH_IO
    qt_charger_mode_config(BATTERY_PLUGGED_SLEEP);
#endif
    qt_sleep_Init();
        
    clear_event();

    CheckResumeCnt=0;

    return 0;
}

static int  qt602240_late_resume(struct early_suspend *h)
{
    struct qt602240 *ip;
	ip = container_of(h, struct qt602240, early_suspend);

    dbg("[TSP] qt602240_late_resume\n");

    init_hw_chg_reset();

    touch_data_init();
    qt_Power_Config_Resume();

  
      
#ifdef TOUCH_IO
#if 0
        switch (charger_mode)
        {
            case BATTERY_PLUGGED_NONE:
                qt_charger_mode_config(BATTERY_PLUGGED_NONE);
                break;
            case BATTERY_PLUGGED_AC:
                qt_charger_mode_config(BATTERY_PLUGGED_AC);
                break;
            case BATTERY_PLUGGED_USB:
                qt_charger_mode_config(BATTERY_PLUGGED_USB);
                break;
            default:
                break;
        }
#endif        
    qt_charger_mode_config(charger_mode);

#endif   

    msleep(20);

    nCheckCalInt = TOUCH_CAL_START;

    if(check_call_sleep == false)  // If user enter a call sleep, sometime....calibrate is wrong...so I skipped....
		calibrate_chip();
	else
        cal_check_flag = 0u;

    check_call_sleep = false;

    enable_irq(ip->client->irq);
    nCheckCalInt = TOUCH_CAL_END;


    return 0;
        
    }
#endif
#endif

//20100717 jjangu_device
void init_hw_chg_reset(void)
{
	
    	gpio_tlmm_config(GPIO_CFG(GPIO_TOUCH_CHG, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	    gpio_set_value(GPIO_TOUCH_CHG, 0);	
        msleep(10);
	    gpio_set_value(GPIO_TOUCH_CHG, 1);	 
    	gpio_tlmm_config(GPIO_CFG(GPIO_TOUCH_CHG, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);

        // HW Reset
    	TSP_Restart(); //20101105 TERRY ES6 : SYSTEM
}




void init_hw_setting(void)
{
        int rc; 
        unsigned gpioConfig;
       
        struct vreg *vreg_touch;
		struct vreg *vreg_touch1;
		
        vreg_touch = vreg_get(NULL, "gp11");
        
        rc = vreg_set_level(vreg_touch, 1800);
        dbg("[TERRY] gp11 Touch test\n" );
        if (rc) {
                dbg(KERN_ERR "%s: gp11 vreg set level failed (%d)\n",
                __func__, rc);
                return /*-EIO*/;
        }
        rc = vreg_enable(vreg_touch);
        if (rc) {
                dbg(KERN_ERR "%s: gp11 vreg enable failed (%d)\n",
                __func__, rc);
                return /*-EIO*/;
        }

//#if 0
         vreg_touch1 = vreg_get(NULL, "gp13");
        
        rc = vreg_set_level(vreg_touch1, 3000);
        dbg("[TERRY] gp13 Touch test\n" );
        if (rc) {
                dbg(KERN_ERR "%s: gp13 vreg set level failed (%d)\n",
                __func__, rc);
                return /*-EIO*/;
        }
        rc = vreg_enable(vreg_touch1);
        if (rc) {
                dbg(KERN_ERR "%s: gp13 vreg enable failed (%d)\n",
                __func__, rc);
                return /*-EIO*/;
        }
//#endif        
        
        rc = gpio_request(GPIO_TOUCH_RST_N, "touch_rst_n");
        if(rc != 0)
            dbg("[TERRY] GPIO_TOUCH_RST_N Failed \n" );
        
        gpioConfig = GPIO_CFG(GPIO_TOUCH_RST_N, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA);
        rc = gpio_tlmm_config(gpioConfig, GPIO_CFG_ENABLE);

         if(rc != 0)
            dbg("[TERRY] GPIO_TOUCH_RST_N tlmm Failed \n" );
        
        gpio_request(GPIO_TOUCH_CHG, "touch_chg_int");
        gpioConfig = GPIO_CFG(GPIO_TOUCH_CHG, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA);
        rc = gpio_tlmm_config(gpioConfig, GPIO_CFG_ENABLE);

         if(rc != 0)
            dbg("[TERRY] GPIO_TOUCH_RST_N tlmm Failed \n" );
        I2C_ENABLE();
        TSP_Restart();
        
        

        /*
        s3c_gpio_setpull(GPIO_TOUCH_INT, S3C_GPIO_PULL_UP); 
        set_irq_type(IRQ_TOUCH_INT, IRQ_TYPE_LEVEL_LOW);

        if (gpio_is_valid(GPIO_TOUCH_EN)) {
                if (gpio_request(GPIO_TOUCH_EN, "GPG3"))
                        dbg( "Failed to request GPIO_TOUCH_EN!\n");
                gpio_direction_output(GPIO_TOUCH_EN, 1);
        }

        s3c_gpio_setpull(GPIO_TOUCH_EN, S3C_GPIO_PULL_NONE); 
        gpio_free(GPIO_TOUCH_EN);
        
        */
        
        msleep(100);
		
}





static ssize_t i2c_show(struct device *dev, struct device_attribute *attr, char *buf)
{       
        int ret;
        unsigned char read_buf[5];

        ret = qt602240_i2c_read(0,read_buf, 5);
        if (ret < 0) {
                dbg("qt602240 i2c read failed.\n");
        }

        return sprintf(buf, "%s\n", buf);
}

static ssize_t i2c_store(
                struct device *dev, struct device_attribute *attr,
                const char *buf, size_t size)
{
        return size;
}

static ssize_t gpio_show(struct device *dev, struct device_attribute *attr, char *buf)
{
        dbg("qt602240 GPIO Status\n");

        return sprintf(buf, "%s\n", buf);
}

static ssize_t gpio_store(
                struct device *dev, struct device_attribute *attr,
                const char *buf, size_t size)
{
        /*
        if(strncmp(buf, "ENHIGH", 6) == 0 || strncmp(buf, "enhigh", 6) == 0) {
                gpio_set_value(GPIO_TOUCH_EN, 1);
                dbg("set TOUCH_EN High.\n");
                mdelay(100);
        }
        if(strncmp(buf, "ENLOW", 5) == 0 || strncmp(buf, "enlow", 5) == 0) {
                gpio_set_value(GPIO_TOUCH_EN, 0);
                dbg("set TOUCH_EN Low.\n");
                mdelay(100);
        }
        */
        if(strncmp(buf, "RSTHIGH", 7) == 0 || strncmp(buf, "rsthigh", 7) == 0) {
                gpio_set_value(GPIO_TOUCH_RST_N, 1);
                dbg("set TOUCH_RST High.\n");
                msleep(100);
        }
        if(strncmp(buf, "RSTLOW", 6) == 0 || strncmp(buf, "rstlow", 6) == 0) {
                gpio_set_value(GPIO_TOUCH_RST_N, 0);
                dbg("set TOUCH_RST Low.\n");
                msleep(100);
        }
        return size;
}

#ifdef QT_FIRMUP_ENABLE

uint8_t boot_unlock(void)
{

        int ret;
        unsigned char data[2];

        //   read_buf = (char *)kmalloc(size, GFP_KERNEL | GFP_ATOMIC);
        data[0] = 0xDC;
        data[1] = 0xAA;
        
        ret = boot_qt602240_i2c_write(0,data,2);
        if(ret < 0) {
                dbg("%s : i2c write failed\n",__func__);
                return(WRITE_MEM_FAILED);
        } 

        return(WRITE_MEM_OK);

}

void TSP_Restart(void)
{       
       // msleep(10);
        dbg("set TOUCH_RST High.\n");
        gpio_set_value(GPIO_TOUCH_RST_N, 0);
        msleep(10);
        dbg("set TOUCH_RST Low.\n");
        gpio_set_value(GPIO_TOUCH_RST_N, 1);
        msleep(50);
        
        /*
        if (gpio_is_valid(GPIO_TOUCH_EN)) {
                if (gpio_request(GPIO_TOUCH_EN, "GPG3"))
                        dbg("Failed to request GPIO_TOUCH_EN!\n");
                gpio_set_value(GPIO_TOUCH_EN, 0);
        }
        gpio_free(GPIO_TOUCH_EN);

        mdelay(300);

        if (gpio_is_valid(GPIO_TOUCH_EN)) {
                if (gpio_request(GPIO_TOUCH_EN, "GPG3"))
                        dbg("Failed to request GPIO_TOUCH_EN!\n");
                gpio_set_value(GPIO_TOUCH_EN, 1);
        }
        gpio_free(GPIO_TOUCH_EN);
        */
            
}

uint8_t QT_Boot(void)
{
                unsigned char boot_status;
                
                unsigned char retry_cnt;
                
                unsigned long int character_position;
                unsigned int frame_size;
                unsigned int next_frame;
                unsigned int crc_error_count;
                
                unsigned int size1,size2;
                
                uint8_t data = 0xA5;
        
                uint8_t reset_result = 0;
        
                unsigned char  *firmware_data ;
        
                firmware_data = QT602240_firmware;
        
                crc_error_count = 0;
                character_position = 0;
                next_frame = 0;
        
                reset_result = write_mem(command_processor_address + RESET_OFFSET, 1, &data);
        
        
                if(reset_result != WRITE_MEM_OK)
                {
                        for(retry_cnt =0; retry_cnt < 3; retry_cnt++)
                        {
                                msleep(100);
                                reset_result = write_mem(command_processor_address + RESET_OFFSET, 1, &data);
                                if(reset_result == WRITE_MEM_OK)
                                {
                                        break;
                                }
                        }
        
                }
                if (reset_result == WRITE_MEM_OK)
                        dbg("Boot reset OK\r\n");

                msleep(100);
        
                for(retry_cnt = 0; retry_cnt < 10; retry_cnt++)
                {
                        
                        if(boot_read_mem(0,1,&boot_status) == READ_MEM_OK)
                        {
                                retry_cnt  = 0;
                                dbg("TSP boot status is %x                stage 2 \n", boot_status);
                                if((boot_status & QT_WAITING_BOOTLOAD_COMMAND) == QT_WAITING_BOOTLOAD_COMMAND)
                                {
                                        if(boot_unlock() == WRITE_MEM_OK)
                                        {
                                                msleep(10);
        
                                                dbg("Unlock OK\n");
        
                                        }
                                        else
                                        {
                                                dbg("Unlock fail\n");
                                        }
                                }
                                else if((boot_status & 0xC0) == QT_WAITING_FRAME_DATA)
                                {
                                         /* Add 2 to frame size, as the CRC bytes are not included */
                                        size1 =  *(firmware_data+character_position);
                                        size2 =  *(firmware_data+character_position+1)+2;
                                        frame_size = (size1<<8) + size2;
        
                                        dbg("Frame size:%d\n", frame_size);
                                        dbg("Firmware pos:%ld\n", character_position);
                                        /* Exit if frame data size is zero */
                                        if( 0 == frame_size )
                                        {
                                                if(QT_i2c_address == I2C_BOOT_ADDR_0)
                                                {
                                                        QT_i2c_address = I2C_APPL_ADDR_0;
                                                }
                                                dbg("0 == frame_size\n");
                                                return 1;
                                        }
                                        next_frame = 1;
                                        boot_write_mem(0,frame_size, (firmware_data +character_position));
                                        msleep(10);
                                        dbg(".");
        
                                }
                                else if(boot_status == QT_FRAME_CRC_CHECK)
                                {
                                        dbg("CRC Check\n");
                                }
                                else if(boot_status == QT_FRAME_CRC_PASS)
                                {
                                        if( next_frame == 1)
                                        {
                                                dbg("CRC Ok\n");
                                                character_position += frame_size;
                                                next_frame = 0;
                                        }
                                        else {
                                                dbg("next_frame != 1\n");
                                        }
                                }
                                else if(boot_status  == QT_FRAME_CRC_FAIL)
                                {
                                        dbg("CRC Fail\n");
                                        crc_error_count++;
                                }
                                if(crc_error_count > 10)
                                {
                                        return QT_FRAME_CRC_FAIL;
                                }
        
                        }
                }
                dbg("QT_Boot end \n");
                return (0);
}

uint8_t QT_Boot_no_reset(void)
{
        unsigned char boot_status;
        
        unsigned char retry_cnt;
        unsigned short character_position;
        unsigned short frame_size;
        unsigned short next_frame;
        unsigned short crc_error_count;
        
        unsigned char size1,size2;
        
        

        

        unsigned char  *firmware_data ;

        firmware_data = QT602240_firmware;

        crc_error_count = 0;
        character_position = 0;
        next_frame = 0;


        for(retry_cnt = 0; retry_cnt < 30; retry_cnt++)
        {
                
                if(boot_read_mem(0,1,&boot_status) == READ_MEM_OK)
                {
                        retry_cnt  = 0;
                        dbg("TSP boot status is %x                stage 2 \n", boot_status);
                        if((boot_status & QT_WAITING_BOOTLOAD_COMMAND) == QT_WAITING_BOOTLOAD_COMMAND)
                        {
                                if(boot_unlock() == WRITE_MEM_OK)
                                {
                                        msleep(10);

                                        dbg("Unlock OK\n");

                                }
                                else
                                {
                                        dbg("Unlock fail\n");
                                }
                        }
                        else if((boot_status & 0xC0) == QT_WAITING_FRAME_DATA)
                        {
                                 /* Add 2 to frame size, as the CRC bytes are not included */
                                size1 =  *(firmware_data+character_position);
                                size2 =  *(firmware_data+character_position+1)+2;
                                frame_size = (size1<<8) + size2;

                                dbg("Frame size:%d\n", frame_size);
                                dbg("Firmware pos:%d\n", character_position);
                                /* Exit if frame data size is zero */
                                if( 0 == frame_size )
                                {
                                        if(QT_i2c_address == I2C_BOOT_ADDR_0)
                                        {
                                                QT_i2c_address = I2C_APPL_ADDR_0;
                                        }
                                        dbg("0 == frame_size\n");
                                        return 1;
                                }
                                next_frame = 1;
                                boot_write_mem(0,frame_size, (firmware_data +character_position));
                                msleep(10);
                                dbg(".");

                        }
                        else if(boot_status == QT_FRAME_CRC_CHECK)
                        {
                                dbg("CRC Check\n");
                        }
                        else if(boot_status == QT_FRAME_CRC_PASS)
                        {
                                if( next_frame == 1)
                                {
                                        dbg("CRC Ok\n");
                                        character_position += frame_size;
                                        next_frame = 0;
                                }
                                else {
                                        dbg("next_frame != 1\n");
                                }
                        }
                        else if(boot_status  == QT_FRAME_CRC_FAIL)
                        {
                                dbg("CRC Fail\n");
                                crc_error_count++;
                        }
                        if(crc_error_count > 10)
                        {
                                return QT_FRAME_CRC_FAIL;
                        }

                }
        }
        dbg("QT_Boot_no_reset end \n");
        return (0);
}

void QT_reprogram(void)
{
        uint8_t version=0, build=0;
        unsigned char rxdata;
        dbg("QT_reprogram check\n");

//20100525 TERRY ES6
//#if 0
        if(boot_read_mem(0,1,&rxdata) == READ_MEM_OK)
        {
                dbg("Enter to new firmware : boot mode\n");
                QT_Boot_no_reset();
                TSP_Restart();  //cte mode == 0
                quantum_touch_probe(); //cte mode ==3 
                TSP_Restart();
                dbg("Reprogram done : boot mode\n");       
        }
//#endif
        quantum_touch_probe();  /* find and initialise QT device */

          if (driver_setup != DRIVER_SETUP_OK)
            return;

        get_version(&version);
        get_build_number(&build);

#if defined(__VER_2_0__)
        #if	1//(BOARD_VER ==MOON_ES10)
        if((version < 0x20)||((version == 0x20)&&(build != 0xAA)))
        #else
          if((version < 0x21)||((version == 0x21)&&(build != 0xAA)))
        #endif
#else
       // if((version < 0x10)||((version == 0x10)&&(build != 0xAC)))
       if(!((version == 0x10)&&(build == 0xAC)))
#endif			
        {
                dbg("Enter to new firmware : ADDR = Other Version\n");
                if(QT_Boot())
                {
                    TSP_Restart();
                    quantum_touch_probe();
                    TSP_Restart(); 
                    
                }
                quantum_touch_probe();
                 TSP_Restart(); 
                dbg("Reprogram done : ADDR = Other Version\n");
        }
	
}
#endif

static ssize_t setup_show(struct device *dev, struct device_attribute *attr, char *buf)
{       
        dbg("qt602240 Setup Status\n");

        return 0;

}

static ssize_t setup_store(
                struct device *dev, struct device_attribute *attr,
                const char *buf, size_t size)
{
        return size;
}



//#if 1 /* (BOARD_VER >=SATURN_WS13)*/ 
int __init qt602240_init(void)
{
	int ret = 0;

    //20100819 TERRY ES6 =>
    #if	0//(BOARD_VER == MOON_ES20)||(BOARD_VER == MOON_ES10)||(BOARD_VER == MOON_WS20)
    if(touch_id_type)
        return ENOMEM;
    #endif
    //20100819 TERRY ES6 <=
    
	init_hw_setting();
	ret = i2c_add_driver(&qt602240_driver);
	return ret;
}
	
void __exit qt602240_exit(void)
{
	/*return*/ i2c_del_driver(&qt602240_driver);
}

//20101014 TERRY ES6 =>
int  qt602240_front_test_init(void)
{
	
   
	//init_hw_setting();
	//ret = i2c_add_driver(&qt602240_driver);

    //init_hw_chg_reset();
    init_hw_setting();
    quantum_touch_probe();
	return 0;
}

void  qt602240_front_test_exit(void)
{
	/*return*/ i2c_del_driver(&qt602240_driver);
}
//20101014 TERRY ES6 <=


#ifdef TOUCH_IO
static int open(struct inode *inode, struct file *file) 
{
	return 0;
}

static int release(struct inode *inode, struct file *file) 
{
	//dbg("qt602240 release\n");
	return 0; 
}

static int ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
    //dbg("terry pt_mxt140 cmd : [%d] value: %d \n", cmd, arg);
      
    switch (cmd)
    {
        case POWER_CONFIG_IDLEACQINT:
        	power_config.idleacqint=arg;
        	break;
        case POWER_CONFIG_ACTVACQINT:
        	power_config.actvacqint=arg;
        	break;
        case POWER_CONFIG_ACTV2IDLETO:        
        	power_config.actv2idleto=arg;
        	break;
        case ACQUISITION_CONFIG_CHRGTIME:
        	acquisition_config.chrgtime=arg;
        	break;
        case ACQUISITION_CONFIG_ATCHDRIFT:
        	break;
        case ACQUISITION_CONFIG_TCHDRIFT:
        	acquisition_config.tchdrift=arg;
        	break;
        case ACQUISITION_CONFIG_DRIFTST:
        	acquisition_config.driftst = arg;
        	break;
        case ACQUISITION_CONFIG_TCHAUTOCAL:
        	acquisition_config.tchautocal=arg;
        	break;
        case ACQUISITION_CONFIG_SYNC:
        	acquisition_config.sync =arg;
        	break;
        case ACQUISITION_CONFIG_ATCHCALST:
        	acquisition_config.atchcalst=arg;
        	break;
        case ACQUISITION_CONFIG_ATCHCALSTHR:   
        	acquisition_config.atchcalsthr=arg;
        	break;  

#if defined(__VER_2_0__)
        case ACQUISITION_CONFIG_ATCHFRCCALTHR:
        	acquisition_config.atchfrccalthr=arg;
        	break;  

        case ACQUISITION_CONFIG_ATCHFRCCALRATION:
        	acquisition_config.atchfrccalration=arg;
        	break;  
#endif

        case TOUCHSCREEN_CONFIG_CTRL:
        	touchscreen_config.ctrl = arg;
        	break;
    	case TOUCHSCREEN_CONFIG_XORIGIN:
	       	touchscreen_config.xorigin =arg;
		    break;
	    case TOUCHSCREEN_CONFIG_YORIGIN:
	    	touchscreen_config.yorigin = arg;
	    	break;
	    case TOUCHSCREEN_CONFIG_XSIZE:
		    touchscreen_config.xsize = arg;
		    break;
    	case TOUCHSCREEN_CONFIG_YSIZE:
    		touchscreen_config.ysize = arg;
    		break;
    	case TOUCHSCREEN_CONFIG_AKSCFG:
    		touchscreen_config.akscfg = arg;
    		break;
    	case TOUCHSCREEN_CONFIG_BLEN:
    		touchscreen_config.blen = arg;
    		break;
    	case TOUCHSCREEN_CONFIG_TCHTHR:
    		touchscreen_config.tchthr = arg;
    		break;
    	case TOUCHSCREEN_CONFIG_TCHDI:
    		touchscreen_config.tchdi = arg;
    		break;
    	case TOUCHSCREEN_CONFIG_ORIENT:
    		touchscreen_config.orient = arg;
    		break;
    	case TOUCHSCREEN_CONFIG_MRGTIMEOUT:
    		touchscreen_config.mrgtimeout = arg;
    		break;
    	case TOUCHSCREEN_CONFIG_MOVHYSTI:
    		touchscreen_config.movhysti = arg;
    		break;
    	case TOUCHSCREEN_CONFIG_MOVHYSTN:
    		touchscreen_config.movhystn = arg;
    		break;
    	case TOUCHSCREEN_CONFIG_MOVFILTER:
    		touchscreen_config.movfilter = arg;
    		break;
    	case TOUCHSCREEN_CONFIG_NUMTOUCH:
    		touchscreen_config.numtouch= arg;
    		break;
    	case TOUCHSCREEN_CONFIG_MRGHYST:
    		touchscreen_config.mrghyst = arg;
    		break;
    	case TOUCHSCREEN_CONFIG_MRGTHR:
    		touchscreen_config.mrgthr = arg;
    		break;
    	case TOUCHSCREEN_CONFIG_AMPHYST:
    		touchscreen_config.amphyst = arg;
    		break;
    	case TOUCHSCREEN_CONFIG_XRANGE:
    		touchscreen_config.xrange = arg;
    		break;
    	case TOUCHSCREEN_CONFIG_YRANGE:
    		touchscreen_config.yrange = arg;
    		break;
    	case TOUCHSCREEN_CONFIG_XLOCLIP:
    		touchscreen_config.xloclip = arg;
    		break;
    	case TOUCHSCREEN_CONFIG_XHICLIP:
    		touchscreen_config.xhiclip = arg;
    		break;
    	case TOUCHSCREEN_CONFIG_YLOCLIP:
    		touchscreen_config.yloclip =arg;
    		break;
    	case TOUCHSCREEN_CONFIG_YHICLIP:
    		touchscreen_config.yhiclip = arg;
    		break;
    	case TOUCHSCREEN_CONFIG_XEDGECTRL:
    		touchscreen_config.xedgectrl = arg;
    		break;
    	case TOUCHSCREEN_CONFIG_XEDGEDIST:
    		touchscreen_config.xedgedist = arg;
    		break;
    	case TOUCHSCREEN_CONFIG_YEDGECTRL:
    		touchscreen_config.yedgectrl = arg;
    		break;
    	case TOUCHSCREEN_CONFIG_YEDGEDIST:
    		touchscreen_config.yedgedist = arg;
    		break;
    	case TOUCHSCREEN_CONFIG_JUMPLIMIT:
    		touchscreen_config.jumplimit    = arg;
    		break;

#if defined(__VER_2_0__)
	case TOUCHSCREEN_CONFIG_TCHHYST:
		touchscreen_config.tchhyst    = arg;
		break;
#endif

      	case KEYARRAY_CONFIG_CTRL:
    		keyarray_config.ctrl = arg;
    		break;
    	case KEYARRAY_CONFIG_XORIGIN:
    		keyarray_config.xorigin = arg;
    		break;
    	case KEYARRAY_CONFIG_YORIGIN:
    		keyarray_config.yorigin = arg;
    		break;
    	case KEYARRAY_CONFIG_XSIZE:
    		keyarray_config.xsize = arg;
    		break;
    	case KEYARRAY_CONFIG_YSIZE:
    		keyarray_config.ysize = arg;
    		break;
    	case KEYARRAY_CONFIG_AKSCFG:
    		keyarray_config.akscfg = arg;
    		break;
    	case KEYARRAY_CONFIG_BLEN:
    		keyarray_config.blen = arg;
    		break;
    	case KEYARRAY_CONFIG_TCHTHR:
    		keyarray_config.tchthr = arg;
    		break;
    	case KEYARRAY_CONFIG_TCHDI:
    		keyarray_config.tchdi = arg;
    		break;
       	case COMC_CONFIG_CTRL:
    		comc_config.ctrl = arg;
    		break;
    	case COMC_CONFIG_COMMAND:
    		comc_config.cmd = arg;
    		break;	
       	case GPIOPWM_CONFIG_CTRL:
    		gpiopwm_config.ctrl = arg;
    		break;
    	case GPIOPWM_CONFIG_REPORTMASK:
    		gpiopwm_config.reportmask = arg;
    		break;
    	case GPIOPWM_CONFIG_DIR:
    		gpiopwm_config.dir = arg;
    		break;
    	case GPIOPWM_CONFIG_INTPULLUP:
    		gpiopwm_config.intpullup = arg;
    		break;
    	case GPIOPWM_CONFIG_OUT:
    		gpiopwm_config.out = arg;
    		break;
    	case GPIOPWM_CONFIG_WAKE:
    		gpiopwm_config.wake = arg;
    		break;
    	case GPIOPWM_CONFIG_PWM:
    		gpiopwm_config.pwm = arg;
    		break;
    	case GPIOPWM_CONFIG_PERIOD:
    		gpiopwm_config.period = arg;
    		break;
    	case GPIOPWM_CONFIG_DUTY0:
    		gpiopwm_config.duty[0] = arg;
    		break;
    	case GPIOPWM_CONFIG_DUTY1:
    		gpiopwm_config.duty[1] = arg;
    		break;
    	case GPIOPWM_CONFIG_DUTY2:
    		gpiopwm_config.duty[2] = arg;
    		break;
    	case GPIOPWM_CONFIG_DUTY3:
    		gpiopwm_config.duty[3] = arg;
    		break;
    	case GPIOPWM_CONFIG_TRIGGER0:
    	case GPIOPWM_CONFIG_TRIGGER1:
    	case GPIOPWM_CONFIG_TRIGGER2:
    	case GPIOPWM_CONFIG_TRIGGER3:
    		break;
    	
    	
    	case GRIPFACESUPPRESSION_CONFIG_CTRL:
    		gripfacesuppression_config.ctrl = arg;
    		break;
    	case GRIPFACESUPPRESSION_CONFIG_XLOGRIP:
    		gripfacesuppression_config.xlogrip = arg;
    		break;
    	case GRIPFACESUPPRESSION_CONFIG_XHIGRIP:
    		gripfacesuppression_config.xhigrip = arg;
    		break;
    	case GRIPFACESUPPRESSION_CONFIG_YLOGRIP:
    		gripfacesuppression_config.ylogrip = arg;
    		break;
    	case GRIPFACESUPPRESSION_CONFIG_YHIGRIP:
    		gripfacesuppression_config.yhigrip =arg;
    		break;
    	case GRIPFACESUPPRESSION_CONFIG_MAXTCHS:
    		gripfacesuppression_config.maxtchs = arg;
    		break;
    	case GRIPFACESUPPRESSION_CONFIG_RESERVED:
    		gripfacesuppression_config.reserved =arg;
    		break;
    	case GRIPFACESUPPRESSION_CONFIG_SZTHR1:
    		gripfacesuppression_config.szthr1 = arg;
    		break;
    	case GRIPFACESUPPRESSION_CONFIG_SZTHR2:
    		gripfacesuppression_config.szthr2 = arg;
    		break;
    	case GRIPFACESUPPRESSION_CONFIG_SHPTHR1:
    		gripfacesuppression_config.shpthr1 = arg;
    		break;
    	case GRIPFACESUPPRESSION_CONFIG_SHPTHR2:
    		gripfacesuppression_config.shpthr2 = arg;
    		break;
    	case GRIPFACESUPPRESSION_CONFIG_SUPEXTTO:
    		gripfacesuppression_config.supextto = arg;
    		break;
       	case NOISE_SUPPRESSION_CONFIG_CTRL:
    		noise_suppression_config.ctrl = arg;
    		break;
    	case NOISE_SUPPRESSION_CONFIG_RESERVED:
    		noise_suppression_config.reserved = arg;
    		break;
    	case NOISE_SUPPRESSION_CONFIG_RESERVED1:
    		noise_suppression_config.reserved1 = arg;
    		break;	
    	case NOISE_SUPPRESSION_CONFIG_GCAFUL:
    		noise_suppression_config.gcaful = arg;
    		break;
    	case NOISE_SUPPRESSION_CONFIG_GCAFLL:
    		noise_suppression_config.gcafll = arg;
    		break;
    	case NOISE_SUPPRESSION_CONFIG_ACTVGCAFVALID:
    		noise_suppression_config.actvgcafvalid = arg;
    		break;
    	case NOISE_SUPPRESSION_CONFIG_NOISETHR:
    		noise_suppression_config.noisethr = arg;
    		break;
    	case NOISE_SUPPRESSION_CONFIG_FREQHOPSCALE:
    		noise_suppression_config.freqhopscale = arg;
    		break;
    	case NOISE_SUPPRESSION_CONFIG_FREQ0:
    		noise_suppression_config.freq[0] = arg;
    		break;
    	case NOISE_SUPPRESSION_CONFIG_FREQ1:
    		noise_suppression_config.freq[1] = arg;
    		break;
    	case NOISE_SUPPRESSION_CONFIG_FREQ2:
    		noise_suppression_config.freq[2] = arg;
    		break;
    	case NOISE_SUPPRESSION_CONFIG_FREQ3:
    		noise_suppression_config.freq[3] = arg;
    		break;
    	case NOISE_SUPPRESSION_CONFIG_FREQ4:
    		noise_suppression_config.freq[4] = arg;
    		break;
    	case NOISE_SUPPRESSION_CONFIG_IDLEGCAFVALID:
    		noise_suppression_config.idlegcafvalid = arg;	
    		break;
       	case PROXIMITY_CONFIG_CTRL:
    		proximity_config.ctrl = arg;
    		break;
    	case PROXIMITY_CONFIG_XORIGIN:
    		proximity_config.xorigin =arg;
    		break;
    	case PROXIMITY_CONFIG_YORIGIN:
    		proximity_config.yorigin =arg;
    		break;
    	case PROXIMITY_CONFIG_XSIZE:
    		proximity_config.xsize = arg;
    		break;
    	case PROXIMITY_CONFIG_YSIZE:
    		proximity_config.ysize = arg;
    		break;
    	case PROXIMITY_CONFIG_BLEN:
    		proximity_config.blen = arg;
    		break;
      #if 0      
    	case PROXIMITY_CONFIG_TCHTHR:
    		proximity_config.tchthr = arg;
    		break;
    	case PROXIMITY_CONFIG_TCHDI:
    		proximity_config.tchdi = arg;
    		break;
      #endif      
    	case PROXIMITY_CONFIG_AVERAGE:
    		proximity_config.average = arg;
    		break;
      #if 0      
    	case PROXIMITY_CONFIG_RATE:
    		proximity_config.fxddthr =arg;
    		break;
      #endif
            
    	case PROXIMITY_CONFIG_FXDDTHR:
            proximity_config.fxddthr =arg;
            break;
    	case PROXIMITY_CONFIG_FXDDI:
            proximity_config.fxddi =arg;
            break;
    	case PROXIMITY_CONFIG_MVNULLRATE:
            proximity_config.mvnullrate =arg;
            break;
    	case PROXIMITY_CONFIG_MVDTHR:
            proximity_config.mvdthr =arg;
    		break;
      	case ONETOUCH_GESTURE_CONFIG_CTRL:
    		onetouch_gesture_config.ctrl = arg;
    		break;
    	case ONETOUCH_GESTURE_CONFIG_NUMGEST:
    		onetouch_gesture_config.numgest = arg;
    		break;
    	case ONETOUCH_GESTURE_CONFIG_GESTEN:
    		onetouch_gesture_config.gesten = arg;
    		break;
    	case ONETOUCH_GESTURE_CONFIG_PRESSPROC:
    		onetouch_gesture_config.pressproc = arg;
    		break;
    	case ONETOUCH_GESTURE_CONFIG_TAPTO:
    		onetouch_gesture_config.tapto = arg;
    		break;
    	case ONETOUCH_GESTURE_CONFIG_FLICKTO:
    		onetouch_gesture_config.flickto = arg;
    		break;
    	case ONETOUCH_GESTURE_CONFIG_DRAGTO:
    		onetouch_gesture_config.dragto = arg;
    		break;
    	case ONETOUCH_GESTURE_CONFIG_SPRESSTO:
    		onetouch_gesture_config.spressto = arg;
    		break;
    	case ONETOUCH_GESTURE_CONFIG_LPRESSTO:
    		onetouch_gesture_config.lpressto = arg;
    		break;
    	case ONETOUCH_GESTURE_CONFIG_REPPRESSTO:
    		onetouch_gesture_config.reppressto = arg;
    		break;
    	case ONETOUCH_GESTURE_CONFIG_FLICKTHR:
    		onetouch_gesture_config.flickthr = arg;
    		break;
    	case ONETOUCH_GESTURE_CONFIG_DRAGTHR:
    		onetouch_gesture_config.dragthr = arg;
    		break;
    	case ONETOUCH_GESTURE_CONFIG_TAPTHR:
    		onetouch_gesture_config.tapthr = arg;
    		break;
    	case ONETOUCH_GESTURE_CONFIG_THROWTHR:
    		onetouch_gesture_config.throwthr = arg;
    		break;
    	case ONETOUCH_GESTURE_CONFIG_PROCESS:
    		break;
    		
     	case SELFTEST_CONFIG_CTRL:
    		selftest_config.ctrl = arg;
    		break;
    	case SELFTEST_CONFIG_CMD:
    		selftest_config.cmd = arg;
    		break;
    	
       	case TWOTOUCH_GESTURE_CONFIG_CTRL:
    		twotouch_gesture_config.ctrl = arg;
    		break;
    	case TWOTOUCH_GESTURE_CONFIG_NUMGEST:
    		twotouch_gesture_config.numgest = arg;
    		break;
    	case TWOTOUCH_GESTURE_CONFIG_GESTEN:
    		twotouch_gesture_config.gesten = arg;
    		break;
    	case TWOTOUCH_GESTURE_CONFIG_ROTATETHR:
    		twotouch_gesture_config.rotatethr = arg;
    		break;
    	case TWOTOUCH_GESTURE_CONFIG_ZOOMTHR:
    		twotouch_gesture_config.zoomthr = arg;
    		break;
      	case CTE_CONFIG_CTRL:
    		cte_config.ctrl = arg;
    		break;
    	case CTE_CONFIG_CMD:
    		cte_config.cmd = arg;
    		break;
    	case CTE_CONFIG_MODE:
    		cte_config.mode = arg;
    		break;
    	case CTE_CONFIG_IDLEGCAFDEPTH:
    		cte_config.idlegcafdepth = arg;
    		break;
    	case CTE_CONFIG_ACTVGCAFDEPTH:
    		cte_config.actvgcafdepth = arg;
    		break;
    	case CTE_CONFIG_VOLTAGE:
    		cte_config.voltage = arg;
    		break;
    		
    	    	//apply touch config	
    	case APPLY_TOUCH_CONFIG:
    		apply_touch_config();
    		break;
    
    #ifdef TOUCH_IO
        case TOUCH_CHARGER_MODE:
           set_charger_mode(arg);
           break;
    #endif
    
    	//Factory Cmd
    	case TOUCH_IOCTL_READ_LASTKEY:
    		break;
    	case TOUCH_IOCTL_DO_KEY:
    	
            if(arg == KEY_MAX)
                input_event(qt602240->input_dev, EV_SW,SW_LID, 0);
            else if(arg == KEY_KPPLUS)
                input_report_key(qt602240->input_dev, (int)arg, 1);
            else if(arg == KEY_COMPOSE)
                input_report_key(qt602240->input_dev, (int)arg, 1);
            else
                input_report_key(qt602240->input_dev, (int)arg, 1);

            break;

    	case TOUCH_IOCTL_RELEASE_KEY:		
    	
        	if(arg == KEY_MAX)
                input_event(qt602240->input_dev, EV_SW,SW_LID, 1);
            else if(arg == KEY_KPPLUS)
                input_report_key(qt602240->input_dev, (int)arg, 0);
            else if(arg == KEY_COMPOSE)
                input_report_key(qt602240->input_dev, (int)arg, 0);
            else
                input_report_key(qt602240->input_dev, (int)arg, 0);

            break;
    	    		
    //20101014 TERRY ES6 =>
        case TOUCH_IOCTL_INIT:
         //   qt602240_front_test_exit();
            dbg("terry Touch init \n");
            qt602240_front_test_init();
           // gp2ap002s00f_restart();				// check_kyk
            break;
//        case TOUCH_IOCTL_EXIT:
//          break;
    //20101014 TERRY ES6 <=
    	    		
    	default:
    		break;
        }
        return 0;
}

static void apply_touch_config(void)
{
	dbg("xxxx power config \n");

    if (driver_setup != DRIVER_SETUP_OK)
        return;
    
    if (write_power_config(power_config) != CFG_WRITE_OK)
	{
		dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
	}

	
	dbg("xxxx acquisition config\n");
	if (write_acquisition_config(acquisition_config) != CFG_WRITE_OK)
	{
		dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
	}

	dbg("xxxx tchthr : %d",  touchscreen_config.tchthr);
	dbg("xxxx multitouchscreen config\n");
	if (write_multitouchscreen_config(0, touchscreen_config) != CFG_WRITE_OK)
	{
		dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
	}
	
	dbg("xxxx keyarray config \n");
	if (write_keyarray_config(0, keyarray_config) != CFG_WRITE_OK)
	{
		dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
	}
	
	dbg("xxxx comc config \n");
	if (get_object_address(SPT_COMCONFIG_T18, 0) != OBJECT_NOT_FOUND)
	{
		if (write_comc_config(0, comc_config) != CFG_WRITE_OK)
		{
		    dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
		}
	}
	
	dbg("xxxx gpio pwm config \n");
	if (write_gpio_config(0, gpiopwm_config) != CFG_WRITE_OK)
	{
		dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
	}	
	
	
	dbg("xxxx gripface suppression config \n");
	if (get_object_address(PROCI_GRIPFACESUPPRESSION_T20, 0) != OBJECT_NOT_FOUND)
	{
		if (write_gripsuppression_config(0, gripfacesuppression_config) !=CFG_WRITE_OK)
		{
    			dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
		}
	}
	
	dbg("xxxx noise suppression config \n");
	if (get_object_address(PROCG_NOISESUPPRESSION_T22, 0) != OBJECT_NOT_FOUND)
	{
		if (write_noisesuppression_config(0,noise_suppression_config) != CFG_WRITE_OK)
		{
    			dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
		}
	}
	
	dbg("xxxx proximity config \n");
	if (get_object_address(TOUCH_PROXIMITY_T23, 0) != OBJECT_NOT_FOUND)
	{
		if (write_proximity_config(0, proximity_config) != CFG_WRITE_OK)
		{
    			dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
		}
	}
	
	
	dbg("xxxx one touch gesture config \n");
	if (get_object_address(PROCI_ONETOUCHGESTUREPROCESSOR_T24, 0) !=OBJECT_NOT_FOUND)
	{
		if (write_onetouchgesture_config(0, onetouch_gesture_config) !=CFG_WRITE_OK)
		{
    			dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
		}
	}
	
	dbg("xxxx self test config \n");
	if (get_object_address(SPT_SELFTEST_T25, 0) != OBJECT_NOT_FOUND)
	{
		if (write_selftest_config(0,selftest_config) != CFG_WRITE_OK)
		{
    			dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
		}
	}


    // 2.0 remove
    #if 0 
	dbg("xxxx two touch config \n");
	if (get_object_address(PROCI_TWOTOUCHGESTUREPROCESSOR_T27, 0) !=OBJECT_NOT_FOUND)
	{
		if (write_twotouchgesture_config(0, twotouch_gesture_config) !=CFG_WRITE_OK)
		{
    			dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
		}
	}
    #endif
	
	dbg("xxxx cte config \n");
	if (get_object_address(SPT_CTECONFIG_T28, 0) != OBJECT_NOT_FOUND)
	{
		if (write_CTE_config(cte_config) != CFG_WRITE_OK)
		{
    			dbg("[TSP] Configuration Fail!!! , Line %d \n\r", __LINE__);
		}
	}
    
}

static uint8_t qt_charger_mode_config(unsigned long mode)
{
    uint8_t status = 0;
    uint16_t object_address_t9, object_address_t22, object_address_t7, object_address_t28;
    

   // dbg("[TSP] qt_charger_mode_config [charger_mode=%d, pre_charger_mode=%d] \n", mode, pre_charger_mode);

    if (driver_setup != DRIVER_SETUP_OK)
        return 0;

    if(pre_charger_mode == mode)
        return status;

    if(mode != BATTERY_PLUGGED_SLEEP)
        pre_charger_mode = mode;

    switch (mode)
    {
        case BATTERY_PLUGGED_NONE:
            touchscreen_config.tchthr = CHARGER_UNPLUGGED_TCHTHR;
            #if defined(__VER_2_0__)
            touchscreen_config.tchhyst = CHARGER_UNPLUGGED_TCHHYSYT;
            #endif
            touchscreen_config.movfilter = CHARGER_UNPLUGGED_MOVFILTER;		
            touchscreen_config.tchdi = CHARGER_UNPLUGGED_TCHDI;
			cte_config.actvgcafdepth = CHARGER_UNPLUGGED_ACTVGCAFDEPTH;
			cte_config.idlegcafdepth = CHARGER_UNPLUGGED_IDLEGCAFDEPTH;
			power_config.idleacqint = CHARGER_UNPLUGGED_IDLEACQINT;
            power_config.actvacqint = CHARGER_UNPLUGGED_ACTVACQINT;
			power_config.actv2idleto = CHARGER_UNPLUGGED_ACTV2IDLETO;
			noise_suppression_config.noisethr = CHARGER_UNPLUGGED_CONFIG_NOISETHR;
            acquisition_config.atchcalsthr = CHARGER_UNPLUGGED_TCHTHR;

         #if 0   
			noise_suppression_config.freq[0] = CHARGER_UNPLUGGED_CONFIG_FREQ0;
			noise_suppression_config.freq[1] = CHARGER_UNPLUGGED_CONFIG_FREQ1;
			noise_suppression_config.freq[2] = CHARGER_UNPLUGGED_CONFIG_FREQ2;
			noise_suppression_config.freq[3] = CHARGER_UNPLUGGED_CONFIG_FREQ3;
            noise_suppression_config.freq[4] = CHARGER_UNPLUGGED_CONFIG_FREQ4;
         #endif   
            break;
        case BATTERY_PLUGGED_AC:
        case BATTERY_PLUGGED_USB:
        case BATTERY_PLUGGED_SLEEP:
            touchscreen_config.tchthr = CHARGER_PLUGGED_TCHTHR;
            #if defined(__VER_2_0__)
            touchscreen_config.tchhyst = CHARGER_PLUGGED_TCHHYSYT;
            #endif
            touchscreen_config.movfilter = CHARGER_PLUGGED_MOVFILTER;
            touchscreen_config.tchdi = CHARGER_PLUGGED_TCHDI;
			cte_config.actvgcafdepth = CHARGER_PLUGGED_ACTVGCAFDEPTH;
			cte_config.idlegcafdepth = CHARGER_PLUGGED_IDLEGCAFDEPTH;	
			power_config.idleacqint = CHARGER_PLUGGED_IDLEACQINT;
            power_config.actvacqint = CHARGER_PLUGGED_ACTVACQINT;
			power_config.actv2idleto = CHARGER_PLUGGED_ACTV2IDLETO;
			noise_suppression_config.noisethr = CHARGER_PLUGGED_CONFIG_NOISETHR;	
            acquisition_config.atchcalsthr = CHARGER_PLUGGED_TCHTHR;
          #if 0  
			noise_suppression_config.freq[0] = CHARGER_PLUGGED_CONFIG_FREQ0;
			noise_suppression_config.freq[1] = CHARGER_PLUGGED_CONFIG_FREQ1;
			noise_suppression_config.freq[2] = CHARGER_PLUGGED_CONFIG_FREQ2;
			noise_suppression_config.freq[3] = CHARGER_PLUGGED_CONFIG_FREQ3;	
            noise_suppression_config.freq[3] = CHARGER_PLUGGED_CONFIG_FREQ4;
          #endif  
            
            break;
        default:
            break;
    }

    object_address_t9 = get_object_address(TOUCH_MULTITOUCHSCREEN_T9,0);
    if (object_address_t9 == 0)
        return(CFG_WRITE_FAILED);

    object_address_t28 = get_object_address(SPT_CTECONFIG_T28, 0);

	if (object_address_t28 == 0)
		return(CFG_WRITE_FAILED);

    object_address_t7 = get_object_address(GEN_POWERCONFIG_T7, 0);
	
	if (object_address_t7 == 0)
		return(CFG_WRITE_FAILED);

    
    object_address_t22 = get_object_address(PROCG_NOISESUPPRESSION_T22, 0);
    if (object_address_t22 == 0)
        return(CFG_WRITE_FAILED);
	
	if (object_address_t22 == 0)
		return(CFG_WRITE_FAILED);
    
   // dbg("[TSP] CHRGER MODE SET [%d]  \n", charger_mode);
   	status |= write_mem(object_address_t9+7, 1, &touchscreen_config.tchthr);
	status |= write_mem(object_address_t9+13, 1, &touchscreen_config.movfilter);	
	status |= write_mem(object_address_t28+3, 1, &cte_config.idlegcafdepth);
	status |= write_mem(object_address_t28+4, 1, &cte_config.actvgcafdepth);
	status |= write_mem(object_address_t7, 1, &power_config.idleacqint);
	status |= write_mem(object_address_t7+2, 1, &power_config.actv2idleto);
	status |= write_mem(object_address_t22+8, 1, &noise_suppression_config.noisethr);

   #if 0 
	status |= write_mem(object_address_t22+11, 1, &noise_suppression_config.freq[0]);
	status |= write_mem(object_address_t22+12, 1, &noise_suppression_config.freq[1]);
	status |= write_mem(object_address_t22+13, 1, &noise_suppression_config.freq[2]);
	status |= write_mem(object_address_t22+14, 1, &noise_suppression_config.freq[3]);	
   #endif 
      
    return(status);
}

/*static*/ void set_charger_mode(unsigned long arg)
{
    charger_mode = arg;
    qt_charger_mode_config(charger_mode);
//    calibrate_chip();
}
#endif //TOUCH_IO

late_initcall(qt602240_init);
module_exit(qt602240_exit);

EXPORT_SYMBOL(set_charger_mode);
MODULE_DESCRIPTION("Quantum Touchscreen Driver");
MODULE_LICENSE("GPL");

