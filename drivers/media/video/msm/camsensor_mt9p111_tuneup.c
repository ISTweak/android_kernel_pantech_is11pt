/* 
================================================================================
TUNEUP Interface for PixelPlus PO4010N CMOS CIF sensor

DESCRIPTION
    This file contains the definitions needed for the camera tuneup interface.

Copyright (c) 2007 by PANTECH, Incorporated. All Rights Reserved.
================================================================================
*/

/*
================================================================================
INCLUDE FILES
================================================================================
*/

//#include "customer.h"
//#include "comdef.h"

#if defined(F_SKYCAM_SENSOR_TUNEUP)
//nclude <stdio.h>
#include <linux/string.h>
//#include "camera_dbg.h"
#include <linux/types.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>

#include "camsensor_parser.h"
#include "camsensor_mt9p111_tuneup.h"



/*
================================================================================
LOCAL FEATURES
================================================================================
*/

/*
================================================================================
LOCAL CONSTANTS
================================================================================
*/

/*
================================================================================
LOCAL MACROS
================================================================================
*/
#ifdef F_SKYCAM_TUP_PRINT_LOAD_VALUE
#define CAM_INFO(fmt, arg...) printk(KERN_ERR "Camera: " fmt "\n" , ## arg)
#else
#define CAM_INFO(fmt, arg...) do{}while(0)
#endif	
#define CAM_ERR(fmt, arg...)  printk(KERN_ERR  "%s: " fmt "\n" , \
						__func__ , ## arg)
#define CAM_WAIT(fmt, args...) do { } while (0)	

/*
================================================================================
LOCAL DATA TYPES
================================================================================
*/

/*
================================================================================
LOCAL VARIABLES
================================================================================
*/

static int32_t mt9p111_tup_init_num_params = 0;
static int32_t mt9p111_tup_wb_num_params[MT9P111_CFG_WB_MAX];
static int32_t mt9p111_tup_bright_num_params[MT9P111_CFG_BRIGHT_MAX];

static mt9p111_cfg_param_t mt9p111_tup_init_params[MT9P111_TUP_INIT_MAX_PARAMS];
static mt9p111_cfg_param_t mt9p111_tup_wb_params[MT9P111_CFG_WB_MAX][MT9P111_TUP_WB_MAX_PARAMS];
static mt9p111_cfg_param_t mt9p111_tup_bright_params[MT9P111_CFG_BRIGHT_MAX][MT9P111_TUP_BRIGHT_MAX_PARAMS];

static u_int8_t pdata_init[MT9P111_TUP_BURST_MODE_MAX_PARAMS];
static u_int32_t pdata_init_count = 0;

/*
================================================================================
LOCAL API DECLARATIONS 
================================================================================
*/

static void mt9p111_tup_init (void);
static void mt9p111_tup_load_params (const int8_t *stream);
static void mt9p111_tup_load_init_params (const int8_t *stream);
static void mt9p111_tup_load_wb_params (const int8_t *stream);
static void mt9p111_tup_load_bright_params (const int8_t *stream);

static int32_t mt9p111_tup_parse_params (const int8_t *stream, mt9p111_cfg_param_t *params);

/*
================================================================================
IMPORTED VARIABLES, API DECLARATIONS
================================================================================
*/

/*
================================================================================
GLOBAL API DEFINITIONS
================================================================================
*/

/*
================================================================================
FUNCTION
================================================================================
*/
void camsensor_mt9p111_tup_init (int8_t *stream)
{
#if 0
    int8_t *stream = NULL;
#endif
    CAM_INFO(">>%s ()", __func__);

    mt9p111_tup_init();
#if 0
    stream = mt9p111_tup_load_file();
#endif
    if (stream == NULL)
    {
        CAM_INFO("<<%s ()", __func__);
        return;
    }
    CAM_INFO("TUNEUP file is loaded!");

    mt9p111_tup_load_params((const int8_t *)stream);
    CAM_INFO("TUNEUP params are loaded!");

    kfree(stream);

    CAM_INFO("<<%s ()", __func__);
    return;
}

/*
================================================================================
FUNCTION
================================================================================
*/
u_int32_t camsensor_mt9p111_tup_get_init_params
(
    mt9p111_cfg_param_t **init_params
)
{
    CAM_INFO(">>%s ()", __func__);
    CAM_INFO("- *init_params = 0x%08x", *init_params, 0);

    *init_params = &mt9p111_tup_init_params[0];
    CAM_INFO("<<%s (%d) (0x%08x)", __func__, mt9p111_tup_init_num_params, *init_params);

    return mt9p111_tup_init_num_params;
}

/*
================================================================================
FUNCTION
================================================================================
*/
u_int32_t camsensor_mt9p111_tup_get_wb_params
(
    mt9p111_cfg_wb_e wb,
    mt9p111_cfg_param_t **wb_params
)
{
    CAM_INFO(">>%s ()", __func__);
    CAM_INFO("- wb = %d, *wb_params = 0x%08x", wb, *wb_params, 0);

    if ((wb < MT9P111_CFG_WB_AUTO) || (wb >= MT9P111_CFG_WB_MAX))
    {
        CAM_INFO("Invalid param! [wb = %d]", wb, 0, 0);
        return 0;
    }

    *wb_params = &mt9p111_tup_wb_params[wb][0];
    CAM_INFO("<<%s (%d) (0x%08x)", __func__, mt9p111_tup_wb_num_params[wb], *wb_params);

    return mt9p111_tup_wb_num_params[wb];
}

/*
================================================================================
FUNCTION
================================================================================
*/
u_int32_t camsensor_mt9p111_tup_get_bright_params
(
    mt9p111_cfg_bright_e bright,
    mt9p111_cfg_param_t **bright_params
)
{
    CAM_INFO(">>%s ()", __func__);
    CAM_INFO("- bright = %d, *bright_params = 0x%08x", bright, *bright_params, 0);

    if ((bright < MT9P111_CFG_BRIGHT_M4) || (bright >= MT9P111_CFG_BRIGHT_MAX))
    {
        CAM_INFO("Invalid param! [bright = %d]", bright, 0, 0);
        return 0;
    }

    *bright_params = &mt9p111_tup_bright_params[bright][0];
    CAM_INFO("<<%s (%d) (0x%08x)", __func__, mt9p111_tup_bright_num_params[bright], *bright_params);

    return mt9p111_tup_bright_num_params[bright];
}

/*
================================================================================
FUNCTION
================================================================================
*/
u_int32_t camsensor_mt9p111_tup_get_all_params
(
    mt9p111_tup_params *params_struct_prt
)
{
    CAM_INFO(">>%s ()", __func__);
    CAM_INFO("- *params_struct = 0x%08x", params_struct_prt, 0, 0);
    
    params_struct_prt->mt9p111_tup_init_params = &mt9p111_tup_init_params[0];
    params_struct_prt->mt9p111_tup_wb_params = &mt9p111_tup_wb_params[0][0];
    params_struct_prt->mt9p111_tup_bright_params = &mt9p111_tup_bright_params[0][0];

    CAM_INFO("<<%s (0x%08x) (0x%08x)", __func__, params_struct_prt->mt9p111_tup_init_params, &mt9p111_tup_init_params[0]);
    return mt9p111_tup_init_num_params;
}

/*
================================================================================
LOCAL API DEFINITIONS
================================================================================
*/

/*
================================================================================
FUNCTION
================================================================================
*/
static void mt9p111_tup_init (void)
{
    CAM_INFO(">>%s ()", __func__);

    mt9p111_tup_init_num_params = 0;
    pdata_init_count = 0;	 // for burst mode initialize 
    memset(mt9p111_tup_init_params, 0, sizeof(mt9p111_tup_init_params));

    memset(mt9p111_tup_wb_num_params, 0, sizeof(mt9p111_tup_wb_num_params));
    memset(mt9p111_tup_wb_params, 0, sizeof(mt9p111_tup_wb_params));

    memset(mt9p111_tup_bright_num_params, 0, sizeof(mt9p111_tup_bright_num_params));
    memset(mt9p111_tup_bright_params, 0, sizeof(mt9p111_tup_bright_params));

    CAM_INFO("<<%s ()", __func__);
    return;
}


/*
================================================================================
FUNCTION
================================================================================
*/
static void mt9p111_tup_load_params (const int8_t *stream)
{
    CAM_INFO(">>%s ()", __func__);
    CAM_INFO("- stream = 0x%08x", stream, 0, 0);

    mt9p111_tup_load_init_params(stream);
    mt9p111_tup_load_wb_params(stream);
    mt9p111_tup_load_bright_params(stream);

    CAM_INFO("<<%s ()", __func__);
    return;
}

/*
================================================================================
FUNCTION
================================================================================
*/
static void mt9p111_tup_load_init_params (const int8_t *stream)
{
    int8_t *my_ptr = NULL;

    CAM_INFO(">>%s ()", __func__);
    CAM_INFO("- stream = 0x%08x", stream, 0, 0);

    my_ptr = strstr(stream, MT9P111_TUP_INIT_MARK);
    if (my_ptr == NULL)
    {
        CAM_INFO("Can't find INIT mark! [stream = 0x%08x]", stream, 0, 0);
        return;
    }

    mt9p111_tup_init_num_params = mt9p111_tup_parse_params(my_ptr, &mt9p111_tup_init_params[0]);    
    CAM_INFO("Number of params in %s = %d", MT9P111_TUP_INIT_MARK, mt9p111_tup_init_num_params, 0);

    CAM_INFO("========== %s ==========", MT9P111_TUP_INIT_MARK, 0, 0);
#ifdef F_SKYCAM_TUP_PRINT_LOAD_VALUE		
    mt9p111_tup_print_params(&mt9p111_tup_init_params[0], mt9p111_tup_init_num_params);
#endif

    CAM_INFO("<<%s ()", __func__);
    return;
}

/*
================================================================================
FUNCTION
================================================================================
*/
static void mt9p111_tup_load_wb_params (const int8_t *stream)
{
    int8_t *my_ptr = NULL;
    int8_t *my_mark = NULL;
    int32_t i = 0;

    CAM_INFO(">>%s ()", __func__);
    CAM_INFO("- stream = 0x%08x", stream, 0, 0);

    for (i = MT9P111_CFG_WB_AUTO; i < MT9P111_CFG_WB_MAX; i++)
    {
        switch (i)
        {
            case MT9P111_CFG_WB_AUTO: my_mark = (int8_t *)MT9P111_TUP_WB_AUTO_MARK; break;
            case MT9P111_CFG_WB_CLOUDY: my_mark = (int8_t *)MT9P111_TUP_WB_CLOUDY_MARK; break;
            case MT9P111_CFG_WB_DAYLIGHT: my_mark = (int8_t *)MT9P111_TUP_WB_DAYLIGHT_MARK; break;
            case MT9P111_CFG_WB_FLUORESCENT: my_mark = (int8_t *)MT9P111_TUP_WB_FLUORESCENT_MARK; break;
            case MT9P111_CFG_WB_INCANDESCENT: my_mark = (int8_t *)MT9P111_TUP_WB_INCANDESCENT_MARK; break;
        }
        
        my_ptr = strstr(stream, my_mark);
        if (my_ptr == NULL)
        {
            CAM_INFO("Can't find WB mark! [stream = 0x%08x][i = %d]", stream, i, 0);
            return;
        }
        mt9p111_tup_wb_num_params[i] = mt9p111_tup_parse_params(my_ptr, &mt9p111_tup_wb_params[i][0]);
        CAM_INFO("Number of params in %s = %d", my_mark, mt9p111_tup_wb_num_params[i], 0);

        CAM_INFO("========== %s ==========", my_mark, 0, 0);
#ifdef F_SKYCAM_TUP_PRINT_LOAD_VALUE			
        mt9p111_tup_print_params(&mt9p111_tup_wb_params[i][0], mt9p111_tup_wb_num_params[i]);
#endif
    }

    CAM_INFO("<<%s ()", __func__);
    return;
}

/*
================================================================================
FUNCTION
================================================================================
*/
static void mt9p111_tup_load_bright_params (const int8_t *stream)
{
    int8_t *my_ptr = NULL;
    int8_t *my_mark = NULL;
    int32_t i = 0;

    CAM_INFO(">>%s ()", __func__);
    CAM_INFO("- stream = 0x%08x", stream, 0, 0);

    for (i = MT9P111_CFG_BRIGHT_M4; i < MT9P111_CFG_BRIGHT_MAX; i++)
    {
        switch (i)
        {			
            case MT9P111_CFG_BRIGHT_M4: my_mark = (int8_t *)MT9P111_TUP_BRIGHT_M4_MARK; break;
            case MT9P111_CFG_BRIGHT_M3: my_mark = (int8_t *)MT9P111_TUP_BRIGHT_M3_MARK; break;
            case MT9P111_CFG_BRIGHT_M2: my_mark = (int8_t *)MT9P111_TUP_BRIGHT_M2_MARK; break;
            case MT9P111_CFG_BRIGHT_M1: my_mark = (int8_t *)MT9P111_TUP_BRIGHT_M1_MARK; break;
            case MT9P111_CFG_BRIGHT_0: my_mark = (int8_t *)MT9P111_TUP_BRIGHT_0_MARK; break;
            case MT9P111_CFG_BRIGHT_P1: my_mark = (int8_t *)MT9P111_TUP_BRIGHT_P1_MARK; break;
            case MT9P111_CFG_BRIGHT_P2: my_mark = (int8_t *)MT9P111_TUP_BRIGHT_P2_MARK; break;
            case MT9P111_CFG_BRIGHT_P3: my_mark = (int8_t *)MT9P111_TUP_BRIGHT_P3_MARK; break;
            case MT9P111_CFG_BRIGHT_P4: my_mark = (int8_t *)MT9P111_TUP_BRIGHT_P4_MARK; break;			
        }
        
        my_ptr = strstr(stream, my_mark);
        if (my_ptr == NULL)
        {
            CAM_INFO("Can't find BRIGHT mark! [stream = 0x%08x][i = %d]", stream, i, 0);
            return;
        }
        mt9p111_tup_bright_num_params[i] = mt9p111_tup_parse_params(my_ptr, &mt9p111_tup_bright_params[i][0]);
        CAM_INFO("Number of params in %s = %d", my_mark, mt9p111_tup_bright_num_params[i], 0);

        CAM_INFO("========== %s ==========", my_mark, 0, 0);
#ifdef F_SKYCAM_TUP_PRINT_LOAD_VALUE			
        mt9p111_tup_print_params(&mt9p111_tup_bright_params[i][0], mt9p111_tup_bright_num_params[i]);
#endif
    }

    CAM_INFO("<<%s ()", __func__);
    return;
}


/*
================================================================================
FUNCTION
================================================================================
*/
static int32_t mt9p111_tup_parse_params
(
    const int8_t *stream, 
    mt9p111_cfg_param_t *params
)
{
    int8_t *my_ptr = NULL;
    u_int16_t addr = 0;
    u_int16_t data = 0;
    int32_t cnt = 0;

    CAM_INFO(">>%s ()", __func__);
    CAM_INFO("- stream = 0x%08x, params = 0x%08x", stream, params, 0);

    my_ptr = strstr(stream, MT9P111_TUP_BEGIN_MARK);
    if (my_ptr == NULL)
    {
        CAM_INFO("Can't find BEGIN mark! [stream = 0x%08x]", stream, 0, 0);
        return 0;
    }

    my_ptr = camsensor_parser_find_line(my_ptr);
    if (my_ptr == NULL)
    {
        CAM_INFO("Can't find BEGIN mark! [stream = 0x%08x]", stream, 0, 0);
        return 0;
    }

    while (TRUE)
    {
        if (strncmp(my_ptr, MT9P111_TUP_REG_MARK, strlen(MT9P111_TUP_REG_MARK)) == 0)
        {
            my_ptr = (int8_t *)camsensor_parser_find_addr((u_int8_t *)(&my_ptr[4]), &addr);
            if (my_ptr != NULL)
            {
                my_ptr = (int8_t *)camsensor_parser_find_data((u_int8_t *)my_ptr, &data);
                if (my_ptr != NULL)
                {
                    (params+cnt)->op   = CAMOP_WRITE;
                    (params+cnt)->addr = addr;
                    (params+cnt)->data = data;
                    cnt++;
                }
            }
        }
	else if (strncmp(my_ptr, MT9P111_TUP_REG_DATA_1BYTE_MARK, strlen(MT9P111_TUP_REG_DATA_1BYTE_MARK)) == 0)
        {
            my_ptr = (int8_t *)camsensor_parser_find_addr((u_int8_t *)(&my_ptr[7]), &addr);
            if (my_ptr != NULL)
            {
                my_ptr = (int8_t *)camsensor_parser_find_data((u_int8_t *)my_ptr, &data);
                if (my_ptr != NULL)
                {
                    (params+cnt)->op   = CAMOP_WRITE_DATA1;
                    (params+cnt)->addr = addr;
                    (params+cnt)->data = data;
                    cnt++;
                }
            }
        }
        else if (strncmp(my_ptr, MT9P111_TUP_NOP_MARK, strlen(MT9P111_TUP_NOP_MARK)) == 0)
        {
            (params+cnt)->op   = CAMOP_NOP;
            (params+cnt)->addr = 0;
            (params+cnt)->data = 0;
            cnt++;
        }
        else if (strncmp(my_ptr, MT9P111_TUP_DELAY_MARK, strlen(MT9P111_TUP_DELAY_MARK)) == 0)
        {
            my_ptr = (int8_t *)camsensor_parser_find_data((u_int8_t *)&my_ptr[6], &data);
            if (my_ptr != NULL)
            {
                (params+cnt)->op   = CAMOP_DELAY;
                (params+cnt)->addr = 0;
                (params+cnt)->data = data;
                cnt++;
            }
        }
        else if (strncmp(my_ptr, MT9P111_TUP_POLL_REG_MARK, strlen(MT9P111_TUP_POLL_REG_MARK)) == 0)
        {
            my_ptr = (int8_t *)camsensor_parser_find_addr((u_int8_t *)&my_ptr[9], &addr);
            if (my_ptr != NULL)
            {
                my_ptr = (int8_t *)camsensor_parser_find_data(my_ptr, &data);
                if (my_ptr != NULL)
                {
                    (params+cnt)->op   = CAMOP_POLL_REG;
                    (params+cnt)->addr = addr;
                    (params+cnt)->data = data;
                    cnt++;
                }
            }
        }
        else if (strncmp(my_ptr, MT9P111_TUP_POLL_MCU_VAR_MARK, strlen(MT9P111_TUP_POLL_MCU_VAR_MARK)) == 0)
        {
            my_ptr = (int8_t *)camsensor_parser_find_addr((u_int8_t *)&my_ptr[13], &addr);
            if (my_ptr != NULL)
            {
                my_ptr = (int8_t *)camsensor_parser_find_data((u_int8_t *)my_ptr, &data);
                if (my_ptr != NULL)
                {
                    (params+cnt)->op   = CAMOP_POLL_MCU_VAR;
                    (params+cnt)->addr = addr;
                    (params+cnt)->data = data;
                    cnt++;
                }
            }
        }
        else if (strncmp(my_ptr, MT9P111_TUP_REG_BURST_MARK, strlen(MT9P111_TUP_REG_BURST_MARK)) == 0)
        {
			u_int16_t temp = 0;

            my_ptr = (int8_t *)camsensor_parser_find_addr((u_int8_t *)&my_ptr[10], &temp);
			pdata_init[pdata_init_count++] = (u_int8_t)(temp >> 8);
			pdata_init[pdata_init_count++] = (u_int8_t)(temp & 0xFF);
			
			if (my_ptr != NULL)
			{
				do {
					my_ptr = (int8_t *)camsensor_parser_find_data((u_int8_t *)my_ptr, &temp);
					pdata_init[pdata_init_count++] = (u_int8_t)(temp >> 8);
					pdata_init[pdata_init_count++] = (u_int8_t)(temp & 0xFF);
				} while( (*(my_ptr+8) == ',') && (*(my_ptr+9) == 0x20) ); /* Comma and Space */

				my_ptr = (int8_t *)camsensor_parser_find_data((u_int8_t *)my_ptr, &temp);
				pdata_init[pdata_init_count++] = (u_int8_t)(temp >> 8);
				pdata_init[pdata_init_count++] = (u_int8_t)(temp & 0xFF);

				pdata_init[pdata_init_count+1] = (u_int8_t)0xAB;
				pdata_init[pdata_init_count+2] = (u_int8_t)0xCD;

				if (my_ptr != NULL)
				{
					if (pdata_init_count == 18)
					{
						(params+cnt)->op   = CAMOP_REG_BURST_MODE;
						(params+cnt)->addr = ( ((int)&pdata_init[0]) & 0x0000ffff );
						(params+cnt)->data = ( ((int)&pdata_init[0]) >> 16 );
						cnt++;
					}
				}
			}
        }
        else if (strncmp(my_ptr, MT9P111_TUP_OTP_MODULE_ZONE_DETECT_MARK, strlen(MT9P111_TUP_OTP_MODULE_ZONE_DETECT_MARK)) == 0)
        {
			(params+cnt)->op   = CAMOP_REG_OTP_MODULE_ZONE_DETECT_MODE;
			(params+cnt)->addr = 0;
			(params+cnt)->data = 0;
			cnt++;
        }		  
        else if (strncmp(my_ptr, MT9P111_TUP_END_MARK, strlen(MT9P111_TUP_END_MARK)) == 0)
        {
            break;
        }

        my_ptr = camsensor_parser_find_line(my_ptr);
        if (my_ptr == NULL)
        {
            CAM_INFO("Can't find BEGIN mark! [stream = 0x%08x]", stream, 0, 0);
            return 0;
        }
    }

    CAM_INFO("<<%s (%d)", __func__, cnt, 0);
    return cnt;
}


#endif /* F_SKYCAM_SENSOR_TUNEUP */
