/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

//#include <linux/string.h>

//#include "lcm_drv.h"

////zhaoshaopeng add fn00410 lcd_id det
//#ifdef BUILD_UBOOT
//#include <asm/arch/mt6577_gpio.h>
//#else
//#include <mach/mt6577_gpio.h>
//#endif

#ifdef BUILD_LK
#include <platform/mt_gpio.h>
#include <platform/mt_pwm.h>

#include <platform/boot_mode.h>
#include <platform/mt_reg_base.h>
#include <platform/mt_typedefs.h>
#include <platform/disp_drv.h>
#include <platform/lcd_drv.h>
#include <platform/dpi_drv.h>
#include <platform/dsi_drv.h>
#include <platform/lcd_reg.h>
#include <platform/dpi_reg.h>
#include <platform/dsi_reg.h>
#include <platform/disp_assert_layer.h>
#include <platform/disp_drv_log.h>
#include <platform/mt_disp_drv.h>
#include "lcm_drv.h"

#ifdef LCD_DEBUG
#define LCM_DEBUG(format, ...)   printf("uboot ssd2825" format "\n", ## __VA_ARGS__)
#else
#define LCM_DEBUG(format, ...)
#endif

#elif defined(BUILD_UBOOT)
#if 1
#include <linux/types.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/concat.h>
#include <asm/io.h>
#endif
#include <asm/arch/mt6577_gpio.h>
#include <asm/arch/mt6577_pwm.h>
#ifdef LCD_DEBUG
#define LCM_DEBUG(format, ...)   printf("uboot ssd2825" format "\n", ## __VA_ARGS__)
#else
#define LCM_DEBUG(format, ...)
#endif
#else
#include <mach/mt_gpio.h>
#include <mach/mt_pwm.h>
#ifdef LCD_DEBUG
#define LCM_DEBUG(format, ...)   printk("kernel ssd2825" format "\n", ## __VA_ARGS__)
#else
#define LCM_DEBUG(format, ...)
#endif
#endif

#include "lcm_drv.h"


// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH                                                                             (480)
#define FRAME_HEIGHT                                                                            (800)
#define LCM_ID       (0x69)
#define REGFLAG_DELAY                                                                   0XFE
#define REGFLAG_END_OF_TABLE                                                            0xfd  // END OF REGISTERS MARKER


#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

static unsigned int lcm_esd_test = FALSE;      ///only for ESD test
#define LCM_DSI_CMD_MODE                                                                        1

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)                                                                (lcm_util.set_reset_pin((v)))

#define UDELAY(n)                                                                                       (lcm_util.udelay(n))
#define MDELAY(n)                                                                                       (lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)                lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)           lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)                                                                          lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)                                      lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)                                                                                   lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)                                   lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)    

struct LCM_setting_table {
    unsigned char cmd;
    unsigned char count;
    unsigned char para_list[64];
};

static struct LCM_setting_table lcm_ili_mode[] = {
    {0xFF, 3 ,{0xFF,0x98,0x06}},
    {REGFLAG_END_OF_TABLE, 0x00, {}}
};

#if 1
static struct LCM_setting_table lcm_initialization_setting[] = {
        
        /*
        Note :

        Data ID will depends on the following rule.
        
                count of parameters > 1 => Data ID = 0x39
                count of parameters = 1 => Data ID = 0x15
                count of parameters = 0 => Data ID = 0x05

        Structure Format :

        {DCS command, count of parameters, {parameter list}}
        {REGFLAG_DELAY, milliseconds of time, {}},

        ...

        Setting ending by predefined flag
        
        {REGFLAG_END_OF_TABLE, 0x00, {}}
        */
	{0xFF, 3 ,{0xFF,0x98,0x06}},
	{0xBA, 1 ,{0x60}},
	{0xBC, 21 ,{
0x01,
0x0E,
0x61,
0x34,
0x05,
0x05,
0x1B,
0x12,
0x31,
0x10,
0x00,
0x00,
0x05,
0x05,
0x03,
0x00,
0xF7,
0xF2,
0x01,
0x00,
0x40
	}},
	{0xBD, 8 ,{
0x01,
0x45,
0x23,
0x67,
0x01,
0x23,
0x45,
0x67
      }},
	{0xBE, 9 ,{0x01,
0x21,
0x06,
0x72,
0x2A,
0xCB,
0xD2,
0x22,
0x22}},
	{0xC7, 1 ,{0x47}},
	{0xED, 3 ,{0x7F,0x0F,0x00}},
	{0xC0, 3 ,{0x03,0x0B,0x00}},
	{0xFC, 1 ,{0x08}},
	{0xDF, 6 ,{0x00,0x00,0x00,0x00,0x00,0x20}},
	{0xF3, 1 ,{0x74}},
	//{0xF9, 4 ,{0x00,0xFD,0x80,0xC0}},
	{0xB4, 3 ,{0x00,0x00,0x00}},
	{0xF7, 1 ,{0x82}},
	{0xB1, 3 ,{0x00,0x13,0x13}},
	{0xF2, 4 ,{0x80,0x02,0x40,0x2a}},
	{0xC1, 4 ,{0x17,0x86,0x96,0x20}},
	{0xE0, 16 ,{0x00,
0x09,
0x1B,
0x10,
0x12,
0x1A,
0xCB,
0x09,
0x03,
0x09,
0x05,
0x0D,
0x0B,
0x2A,
0x27,
0x00}},
	{0xE1, 16 ,{0x00,
0x0C,
0x18,
0x11,
0x13,
0x17,
0x79,
0x08,
0x03,
0x07,
0x07,
0x0B,
0x0D,
0x24,
0x20,
0x00}},
	{0x35, 1 ,{0x00}},
	//{0x36, 1 ,{0x00}},//08                       
	//{0x3A, 1 ,{0x77}},//55
	{0x11, 0 ,{ }},
	{REGFLAG_DELAY, 150, {}},
	{0x29, 0 ,{ }},
	//{0x2C, 0 ,{ }},

        // Setting ending by predefined flag
        {REGFLAG_END_OF_TABLE, 0x00, {}}

};

#endif 
#if 0
static struct LCM_setting_table lcm_initialization_setting[] = {
        
        /*
        Note :

        Data ID will depends on the following rule.
        
                count of parameters > 1 => Data ID = 0x39
                count of parameters = 1 => Data ID = 0x15
                count of parameters = 0 => Data ID = 0x05

        Structure Format :

        {DCS command, count of parameters, {parameter list}}
        {REGFLAG_DELAY, milliseconds of time, {}},

        ...

        Setting ending by predefined flag
        
        {REGFLAG_END_OF_TABLE, 0x00, {}}
        */
	{0xff,  3,      {0xff, 0x98, 0x05}},
        {0xfd,  4,      {0x0f, 0x13, 0x44, 0x00}},      
        {0xf8, 15, {0x14,0x02,0x02,0x14,0x02,0x02,0x30,0x01,0x01,0x30,0x01,0x01,0x30,0x01,0x01}},
        {0xb8,  1,      {0x74}},
        {0xbb,  2,      {0x14, 0x55}},
        {0xf1,  1,      {0x00}},
        {0xf2,  3,      {0x00, 0x58, 0x41}},
        {0xfc,  3,      {0x04, 0x0f, 0x01}},
        {0xfe,  1,      {0x19}},
        {REGFLAG_DELAY, 360, {}},

        {0xeb,  2,      {0x08, 0x0f}},
        {0xe0, 16, {0x00,0x02,0x08,0x10,0x12,0x1f,0x0f,0x0a,0x00,0x05,0x04,0x0f,0x0f,0X30,0x2c,0x00}},
        {0xe1, 16, {0x00,0x02,0x08,0x10,0x12,0x12,0x0f,0x0c,0x00,0x05,0x04,0x0e,0x0f,0X30,0x2c,0x00}},
        {0xc1,  4,      {0x13, 0x26, 0x06, 0x26}},
        {0xc7,  1,      {0xb2}}, //vcom  0xa0
        {0xb1,  3,      {0x00, 0x12, 0x14}},
        {0xb4,  1,      {0x02}},
        {0x36,  1,      {0x0a}},
        {0x3a,  1,      {0x77}},
        {0x21,  0,      {0x00}},
        {0xb0,  1,      {0x00}},
        {0xb6,  1,      {0x01}},
        {0xc2,  1,      {0x11}},
        //*************************************
        // TE On
        //*************************************
        {0x35,  1,      {0x00}},
        //{0xb7,        2,      {0x50,0x02}},
        //{REGFLAG_DELAY, 200, {}},       
        //{0xbc,        2,      {0x00,0x00}},
        //{0xbd,        2,      {0x00,0x00}},
        //{0xbe,        2,      {0x02,0x00}},
        //*************************************
        // Sleep Out
        //*************************************
        {0x11, 0, {}},
        {REGFLAG_DELAY, 125, {}},
        //{0xb7,        2,      {0x50,0x02}},
        //{REGFLAG_DELAY, 200, {}},         
        //{0x2a,        4,      {0x00,0x00,0x02,0x1b}},
        //{0x2b,        4,      {0x00,0x00,0x03,0xbf}},
        //{0x36,        1,      {0x00}},

        //*************************************
        // Display On
        {0x29, 0, {}},
        {REGFLAG_DELAY, 20, {}},
        //{0x2c,        0,      {0x00}},

        // Setting ending by predefined flag
        {REGFLAG_END_OF_TABLE, 0x00, {}}
};
#endif

#if 0
static struct LCM_setting_table lcm_set_window[] = {
        {0x2A,  4,      {0x00, 0x00, (FRAME_WIDTH>>8), (FRAME_WIDTH&0xFF)}},
        {0x2B,  4,      {0x00, 0x00, (FRAME_HEIGHT>>8), (FRAME_HEIGHT&0xFF)}},
        {REGFLAG_END_OF_TABLE, 0x00, {}}
};
#endif

static struct LCM_setting_table lcm_sleep_out_setting[] = {
    // Sleep Out
        {0x11, 0, {0x00}},
       {REGFLAG_DELAY, 125, {}},

    // Display ON
        {0x29, 0, {0x00}},
        {REGFLAG_DELAY, 20, {}},
        {REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {

        // Display off sequence
        {0x28, 0, {0x00}},
       {REGFLAG_DELAY, 200, {}},
    // Sleep Mode On
        {0x10, 0, {0x00}},
       {REGFLAG_DELAY, 200, {}},
        {REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_compare_id_setting[] = {
        // Display off sequence
		{0xFF, 3 ,{0xFF,0x98,0x06}},
        {REGFLAG_DELAY, 10, {}},

    // Sleep Mode On
//      {0xC3, 1, {0xFF}},

        {REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_backlight_level_setting[] = {
        {0x51, 1, {0xFF}},
        {REGFLAG_END_OF_TABLE, 0x00, {}}
};

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
        unsigned int i;

    for(i = 0; i < count; i++) {
                
        unsigned cmd;
        cmd = table[i].cmd;
                
        switch (cmd) {
                        
            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;
                                
            case REGFLAG_END_OF_TABLE :
                break;
                                
            default:
                                dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
        }
    }
        
}


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
                memset(params, 0, sizeof(LCM_PARAMS));
        
                params->type   = LCM_TYPE_DSI;

                params->width  = FRAME_WIDTH;
                params->height = FRAME_HEIGHT;

                // enable tearing-free
#ifdef MTK_WVGA_LCD_SUPPORT
             params->dbi.te_mode                          = LCM_DBI_TE_MODE_DISABLED;
#else		
		params->dbi.te_mode                          = LCM_DBI_TE_MODE_VSYNC_ONLY;
#endif
                params->dbi.te_edge_polarity            = LCM_POLARITY_RISING;

#if (LCM_DSI_CMD_MODE)
                params->dsi.mode   = CMD_MODE;
#else
                params->dsi.mode   = SYNC_PULSE_VDO_MODE;
#endif
        
                // DSI
                /* Command mode setting */
                params->dsi.LANE_NUM                            = LCM_TWO_LANE;
                //The following defined the fomat for data coming from LCD engine.
                params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
                params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
                params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
                params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

                // Highly depends on LCD driver capability.
                // Not support in MT6573
                params->dsi.packet_size=256;

                // Video mode setting           
                params->dsi.intermediat_buffer_num = 2;

                params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
                params->dsi.word_count=480*3;   

                params->dsi.vertical_sync_active=3;
                params->dsi.vertical_backporch=12;
                params->dsi.vertical_frontporch=2;
#ifdef MTK_WVGA_LCD_SUPPORT
            params->dsi.vertical_active_line=400;
#else
            params->dsi.vertical_active_line=800;
#endif
        
                params->dsi.line_byte=2048;             // 2256 = 752*3
                params->dsi.horizontal_sync_active_byte=26;
                params->dsi.horizontal_backporch_byte=146;
                params->dsi.horizontal_frontporch_byte=146;     
                params->dsi.rgb_byte=(480*3+6); 
        
                params->dsi.horizontal_sync_active_word_count=20;       
                params->dsi.horizontal_backporch_word_count=140;
                params->dsi.horizontal_frontporch_word_count=140;

                // Bit rate calculation
                params->dsi.pll_div1=38;                // fref=26MHz, fvco=fref*(div1+1)       (div1=0~63, fvco=500MHZ~1GHz)
                params->dsi.pll_div2=1;                 // div2=0~15: fout=fvo/(2*div2)

}


static void lcm_init(void)
{

    //unsigned char lcd_id = 0;
    SET_RESET_PIN(1);
    MDELAY(5);  
    SET_RESET_PIN(0);
    MDELAY(50);
    SET_RESET_PIN(1);
    MDELAY(120);

        push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
	printk("### lanhong ili9608 lcm_init.....\n");
}


static void lcm_suspend(void)
{
        push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
}


static void lcm_resume(void)
{
        lcm_init();
        
       // push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
}

static void lcm_continue_update(void)
{
	unsigned int data_array[1];
	data_array[0]= 0x003c3909;

	dsi_set_cmdq(data_array, 1, 0);
}


static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
        unsigned int x0 = x;
        unsigned int y0 = y;
        unsigned int x1 = x0 + width - 1;
        unsigned int y1 = y0 + height - 1;

        unsigned char x0_MSB = ((x0>>8)&0xFF);
        unsigned char x0_LSB = (x0&0xFF);
        unsigned char x1_MSB = ((x1>>8)&0xFF);
        unsigned char x1_LSB = (x1&0xFF);
        unsigned char y0_MSB = ((y0>>8)&0xFF);
        unsigned char y0_LSB = (y0&0xFF);
        unsigned char y1_MSB = ((y1>>8)&0xFF);
        unsigned char y1_LSB = (y1&0xFF);

        unsigned int data_array[16];

        data_array[0]= 0x00053902;
        data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
        data_array[2]= (x1_LSB);
        data_array[3]= 0x00053902;
        data_array[4]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
        data_array[5]= (y1_LSB);
        data_array[6]= 0x002c3909;

        dsi_set_cmdq(data_array, 7, 0);

}


static void lcm_setbacklight(unsigned int level)
{
        unsigned int default_level = 145;
        unsigned int mapped_level = 0;

        //for LGE backlight IC mapping table
        if(level > 255) 
                        level = 255;

        if(level >0) 
                        mapped_level = default_level+(level)*(255-default_level)/(255);
        else
                        mapped_level=0;

        // Refresh value of backlight level.
        lcm_backlight_level_setting[0].para_list[0] = mapped_level;

        push_table(lcm_backlight_level_setting, sizeof(lcm_backlight_level_setting) / sizeof(struct LCM_setting_table), 1);
}

static unsigned int lcm_esd_check(void)
{
#ifndef BUILD_UBOOT
        if(lcm_esd_test)
        {
            lcm_esd_test = FALSE;
            return TRUE;
        }

        /// please notice: the max return packet size is 1
        /// if you want to change it, you can refer to the following marked code
        /// but read_reg currently only support read no more than 4 bytes....
        /// if you need to read more, please let BinHan knows.
        /*
                unsigned int data_array[16];
                unsigned int max_return_size = 1;
                
                data_array[0]= 0x00003700 | (max_return_size << 16);    
                
                dsi_set_cmdq(&data_array, 1, 1);
        */

        if(read_reg(0xB6) == 0x42)
        {
            return FALSE;
        }
        else
        {            
            return TRUE;
        }
#endif
}

static unsigned int lcm_esd_recover(void)
{
    unsigned char para = 0;

    SET_RESET_PIN(1);
    SET_RESET_PIN(0);
    MDELAY(1);
    SET_RESET_PIN(1);
    MDELAY(100);//120
/*
    SET_RESET_PIN(1);
    MDELAY(5);  
    SET_RESET_PIN(0);
    MDELAY(50);
    SET_RESET_PIN(1);
    MDELAY(120);
*/
          push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
    MDELAY(10);
          push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
    MDELAY(10);
    dsi_set_cmdq_V2(0x35, 1, &para, 1);     ///enable TE
    MDELAY(10);

    return TRUE;
}

static unsigned int lcm_compare_id(void)
{
#if 1
      volatile unsigned char lcd_id = 4;
	volatile unsigned int id = 0;
	volatile unsigned char buffer[4];
	unsigned int data_array[16];
	
	SET_RESET_PIN(1);  //NOTE:should reset LCM firstly
	MDELAY(2);
	SET_RESET_PIN(0);
	MDELAY(20);
	SET_RESET_PIN(1);
	MDELAY(50); 

	push_table(lcm_compare_id_setting, sizeof(lcm_compare_id_setting) / sizeof(struct LCM_setting_table), 1);
	data_array[0] = 0x00043700;
       dsi_set_cmdq(data_array, 1, 1);

	//data_array[0] = 0x00023700;// read id return two byte,version and id
	//dsi_set_cmdq(data_array, 1, 1);
	MDELAY(10); 
       memset(buffer,0x0,4);
	read_reg_v2(0xD3, buffer, 4);  //Read  2 bytes 5510 means NT35510 from IC Spec
	printk("### zhaoshaopeng  DA lcm_compare_id buffer[0] = 0x%x,0x%x,0x%x,0x%x\r\n",buffer[0], buffer[1], buffer[2], buffer[3]);
	id = buffer[1]<<8 | buffer[2];
	lcd_id =  mt_get_gpio_in(GPIO_LCD_ID);
	printk("### zhaoshaopeng  DC lcm_compare_id id=0x%x,lcd_id=%d\r\n",id,lcd_id);
//	printk("### leanda lide lcd_id = %d \r\n",lcd_id);
//	if((buffer[0]==0x80))
	if(id == 0x9806)
        return 1;
        else
        return 0;

#else
	unsigned int id = 0, id2 = 0;
	unsigned char buffer[2];
	unsigned char lcd_id = 0;
//	unsigned int data_array[16];
	unsigned int array[16]; 
	 

	SET_RESET_PIN(1);
	SET_RESET_PIN(0);
	MDELAY(25);
	SET_RESET_PIN(1);
	MDELAY(50);

	array[0]=0x00043902;
	array[1]=0x010980ff; 
	array[2]=0x80001500;
	array[3]=0x00033902;
	array[4]=0x010980ff;
	dsi_set_cmdq(array, 5, 1);
	MDELAY(10);

	array[0] = 0x00023700;// set return byte number
	dsi_set_cmdq(array, 1, 1);

	array[0] = 0x02001500;
	dsi_set_cmdq(array, 1, 1);

	read_reg_v2(0xD2, &buffer, 2);

	id = buffer[0]<<8 |buffer[1]; 

	lcd_id =  mt_get_gpio_in(GPIO_LCD_ID);

	return (lcd_id == 0)?1:0;
#endif

}
// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER ili9806_lead_dsi_lcm_drv = 
{
    .name                       = "ili9806_lead_dsi",
        .set_util_funcs = lcm_set_util_funcs,
        .get_params     = lcm_get_params,
        .init           = lcm_init,
        .suspend        = lcm_suspend,
        .resume         = lcm_resume,
#if (LCM_DSI_CMD_MODE)
        .update         = lcm_update,
#ifdef MTK_WVGA_LCD_SUPPORT
	.continue_update         = lcm_continue_update,
#endif
        //.set_backlight        = lcm_setbacklight,
//      .set_pwm        = lcm_setpwm,
//      .get_pwm        = lcm_getpwm,
        //.esd_check   = lcm_esd_check,
        //.esd_recover   = lcm_esd_recover,
        .compare_id    = lcm_compare_id,
#endif
};
