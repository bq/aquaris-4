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

#define FRAME_WIDTH  										(480)
#define FRAME_HEIGHT 										(800)//(800)
#define LCM_ID       (0x5510)//(0x55) //(0x55) // (0x69)
#define REGFLAG_DELAY             							0XFE
#define REGFLAG_END_OF_TABLE      							0xFd   // END OF REGISTERS MARKER


#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

static unsigned int lcm_esd_test = FALSE;      ///only for ESD test
#define LCM_DSI_CMD_MODE									1

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))

#define UDELAY(n) 											(lcm_util.udelay(n))
#define MDELAY(n) 											(lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	        lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)											lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)    

struct LCM_setting_table {
    unsigned char cmd;
    unsigned char count;
    unsigned char para_list[64];
};

#if 0
static struct LCM_setting_table lcm_initialization_setting[] = {
    // ENABLE FMARK
   {0x44,	2,	{((FRAME_HEIGHT/2)>>8), ((FRAME_HEIGHT/2)&0xFF)}},
   {0x35,	1,	{0x00}},    

//*************************************
// Select CMD2,Page 1
//*************************************
	{0xF0, 5, {0x55,0xAA,0x52,0x08,0x01}}, // LV2 Page 1 enable
	{0xB0, 3, {0x0D,0x0D,0x0D}}, //AVDD Set AVDD 5.2V
	{0xB6, 3, {0x34,0x34,0x34}}, //AVDD ratio
	{0xB1, 3, {0x0D,0x0D,0x0D}}, //AVEE -5.2V
	{0xB7, 3, {0x34,0x34,0x34}}, //AVEE ratio
	{0xB2, 3, {0x00,0x00,0x00}}, //VCL -2.5V
	{0xB8, 3, {0x24,0x24,0x24}}, //VCL ratio
	{0xBF, 1, {0x01}}, //VGH 15V (Free pump)
	{0xB3, 3, {0x0F,0x0F,0x0F}},
	{0xB9, 3, {0x34,0x34,0x34}}, //VGH ratio
	{0xB5, 3, {0x08,0x08,0x08}}, //VGL_REG -10V
	{0xC2, 1, {0x03}},
	{0xBA, 3, {0x24,0x24,0x24}}, //VGLX ratio
	{0xBC, 3, {0x00,0x78,0x00}}, //VGMP/VGSP 4.5V/0V
	{0xBD, 3, {0x00,0x78,0x00}}, //VGMN/VGSN -4.5V/0V
	{0xBE, 2, {0x00,0x64}}, //VCOM
//*************************************
// Gamma Code
//*************************************
// Positive Red Gamma

	{0xD1, 52, {0x00,0x33,0x00,0x34,0x00,0x3A,0x00,0x4A,0x00,0x5C,0x00,0x81,0x00,0xA6,0x00,0xE5,0x01,0x13,0x01,0x54,0x01,0x82,0x01,0xCA,0x02,0x00,0x02,0x01,0x02,0x34,0x02,0x67,0x02,0x84,0x02,0xA4,0x02,0xB7,0x02,0xCF,0x02,0xDE,0x02,0xF2,0x02,0xFE,0x03,0x10,0x03,0x33,0x03,0x6D}},
	{0xD2, 52, {0x00,0x33,0x00,0x34,0x00,0x3A,0x00,0x4A,0x00,0x5C,0x00,0x81,0x00,0xA6,0x00,0xE5,0x01,0x13,0x01,0x54,0x01,0x82,0x01,0xCA,0x02,0x00,0x02,0x01,0x02,0x34,0x02,0x67,0x02,0x84,0x02,0xA4,0x02,0xB7,0x02,0xCF,0x02,0xDE,0x02,0xF2,0x02,0xFE,0x03,0x10,0x03,0x33,0x03,0x6D}},
	{0xD3, 52, {0x00,0x33,0x00,0x34,0x00,0x3A,0x00,0x4A,0x00,0x5C,0x00,0x81,0x00,0xA6,0x00,0xE5,0x01,0x13,0x01,0x54,0x01,0x82,0x01,0xCA,0x02,0x00,0x02,0x01,0x02,0x34,0x02,0x67,0x02,0x84,0x02,0xA4,0x02,0xB7,0x02,0xCF,0x02,0xDE,0x02,0xF2,0x02,0xFE,0x03,0x10,0x03,0x33,0x03,0x6D}},
	{0xD4, 52, {0x00,0x33,0x00,0x34,0x00,0x3A,0x00,0x4A,0x00,0x5C,0x00,0x81,0x00,0xA6,0x00,0xE5,0x01,0x13,0x01,0x54,0x01,0x82,0x01,0xCA,0x02,0x00,0x02,0x01,0x02,0x34,0x02,0x67,0x02,0x84,0x02,0xA4,0x02,0xB7,0x02,0xCF,0x02,0xDE,0x02,0xF2,0x02,0xFE,0x03,0x10,0x03,0x33,0x03,0x6D}},
	{0xD5, 52, {0x00,0x33,0x00,0x34,0x00,0x3A,0x00,0x4A,0x00,0x5C,0x00,0x81,0x00,0xA6,0x00,0xE5,0x01,0x13,0x01,0x54,0x01,0x82,0x01,0xCA,0x02,0x00,0x02,0x01,0x02,0x34,0x02,0x67,0x02,0x84,0x02,0xA4,0x02,0xB7,0x02,0xCF,0x02,0xDE,0x02,0xF2,0x02,0xFE,0x03,0x10,0x03,0x33,0x03,0x6D}},
	{0xD6, 52, {0x00,0x33,0x00,0x34,0x00,0x3A,0x00,0x4A,0x00,0x5C,0x00,0x81,0x00,0xA6,0x00,0xE5,0x01,0x13,0x01,0x54,0x01,0x82,0x01,0xCA,0x02,0x00,0x02,0x01,0x02,0x34,0x02,0x67,0x02,0x84,0x02,0xA4,0x02,0xB7,0x02,0xCF,0x02,0xDE,0x02,0xF2,0x02,0xFE,0x03,0x10,0x03,0x33,0x03,0x6D}},



   {0xF0, 5, {0x55,0xAA,0x52,0x08,0x00}}, //LV2 Page 0 enable
   {0xB1, 2, {0xEC,0x00}}, //Display control
   {0xB6, 1, {0x05}}, //Source hold time
   {0xB7, 2, {0x70,0x70}}, //Gate EQ control
   {0xB8, 4, {0x01,0x03,0x03,0x03}}, //Source EQ control (Mode 2)
   {0xBC, 3, {0x02,0x00,0x00}}, // Inversion mode (2-dot)
   {0xc7, 1, {0x02}},
   {0xC9, 5, {0x11,0x00,0x00,0x00,0x00}}, //Timing control 4H w/ 4-delay
   {0x35, 1, {0x00}},
   {0x21, 0, {}},
   {0x3A, 1, {0x77}},
//*************************************
// Sleep Out
//*************************************
       {0x11, 0, {}},

       {REGFLAG_DELAY, 200, {}},

//*************************************
// Display On
//*************************************

   {0x29, 0, {}},
   {REGFLAG_DELAY, 200, {}},

   {REGFLAG_DELAY, 200, {}},
  // {0x21, 0, {}},
    {0x2C, 0, {}},

   // Note

   // Strongly recommend not to set Sleep out / Display On here. That will cause messed frame to be shown as later the backlight is on.

   // Setting ending by predefined flag

   {REGFLAG_END_OF_TABLE, 0x00, {}}

};
#else

static struct LCM_setting_table lcm_initialization_setting[] = {
    // ENABLE FMARK
   //{0x44,	2,	{((FRAME_HEIGHT/2)>>8), ((FRAME_HEIGHT/2)&0xFF)}},
   //{0x35,	1,	{0x00}},    
   
   {0xff,    4,      {0xaa, 0x55, 0x25, 0x01}},
   {0xf4,    21,    {0x00,0x00,0x04,0x00,0x00,0x55,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x55,0x24,0x03,0x21,0x43,0x10,0x34,0x52}},   
   {0xff,    4,      {0xaa, 0x55, 0x25, 0x00}},

   {0x11, 0, {}},
   {REGFLAG_DELAY, 124, {}},

//*************************************
// Select CMD2,Page 1
//*************************************
   {0xf0,    5,      {0x55, 0xaa, 0x52, 0x08, 0x01}},
   {0xB0,    3,      {0x0d,0x0d,0x0d}},   
   {0xB6,    3,      {0x44,0x44,0x44}},  
   {0xB1,    3,      {0x0d,0x0d,0x0d}},  
   {0xb7,    3,      {0x34,0x34,0x34}},   
   {0xB2,    3,      {0x00,0x00,0x00}},  
   {0xB8,    3,      {0x24,0x24,0x24}},    
   {0xBF,    1,      {0x01}}, 
   {0xB3,    3,      {0x08,0x08,0x08}}, 
   {0xB9,    3,      {0x34,0x34,0x34}},  
   {0xBA,    3,      {0x24,0x24,0x24}}, 
   {0xBC,    3,      {0x00,0x88,0x00}}, 
   {0xBD,    3,      {0x00,0x88,0x00}},  
   {0xBE,    2,      {0x00,0x94}}, //zhaoshaopeng ac
//*************************************
// Gamma Code
//*************************************
// Positive Red Gamma

       {0xD1,   52,   {
0x00,0x02,0x00,0x03,0x00,0x06,0x00,0x12,0x00,0x23,0x00,0x52,0x00,0x85,0x00,
0xD9,0x01,0x16,0x01,0x65,0x01,0x9E,0x01,0xE8,0x02,0x1D,0x02,0x1F,0x02,0x4B,0x02,0x76,
0x02,0x8E,0x02,0xA9,0x02,0xBA,0x02,0xCF,0x02,0xDD,0x02,0xF0,0x02,0xFC,0x03,0x0D,0x03,
0x2C,0x03,0xE8
}},

         {0xd2,   52,   {
0x00,0x02,0x00,0x03,0x00,0x06,0x00,0x12,0x00,0x23,0x00,0x52,0x00,0x85,0x00,
0xD9,0x01,0x16,0x01,0x65,0x01,0x9E,0x01,0xE8,0x02,0x1D,0x02,0x1F,0x02,0x4B,0x02,0x76,
0x02,0x8E,0x02,0xA9,0x02,0xBA,0x02,0xCF,0x02,0xDD,0x02,0xF0,0x02,0xFC,0x03,0x0D,0x03,
0x2C,0x03,0xE8
}},

            {0xd3,   52,   {
0x00,0x02,0x00,0x03,0x00,0x06,0x00,0x12,0x00,0x23,0x00,0x52,0x00,0x85,0x00,
0xD9,0x01,0x16,0x01,0x65,0x01,0x9E,0x01,0xE8,0x02,0x1D,0x02,0x1F,0x02,0x4B,0x02,0x76,
0x02,0x8E,0x02,0xA9,0x02,0xBA,0x02,0xCF,0x02,0xDD,0x02,0xF0,0x02,0xFC,0x03,0x0D,0x03,
0x2C,0x03,0xE8
}},

            {0xd4,   52,   {
0x00,0x02,0x00,0x03,0x00,0x06,0x00,0x12,0x00,0x23,0x00,0x52,0x00,0x85,0x00,
0xD9,0x01,0x16,0x01,0x65,0x01,0x9E,0x01,0xE8,0x02,0x1D,0x02,0x1F,0x02,0x4B,0x02,0x76,
0x02,0x8E,0x02,0xA9,0x02,0xBA,0x02,0xCF,0x02,0xDD,0x02,0xF0,0x02,0xFC,0x03,0x0D,0x03,
0x2C,0x03,0xE8
}},

            {0xd5,   52,   {
0x00,0x02,0x00,0x03,0x00,0x06,0x00,0x12,0x00,0x23,0x00,0x52,0x00,0x85,0x00,
0xD9,0x01,0x16,0x01,0x65,0x01,0x9E,0x01,0xE8,0x02,0x1D,0x02,0x1F,0x02,0x4B,0x02,0x76,
0x02,0x8E,0x02,0xA9,0x02,0xBA,0x02,0xCF,0x02,0xDD,0x02,0xF0,0x02,0xFC,0x03,0x0D,0x03,
0x2C,0x03,0xE8
}},

            {0xd6,   52,   {
0x00,0x02,0x00,0x03,0x00,0x06,0x00,0x12,0x00,0x23,0x00,0x52,0x00,0x85,0x00,
0xD9,0x01,0x16,0x01,0x65,0x01,0x9E,0x01,0xE8,0x02,0x1D,0x02,0x1F,0x02,0x4B,0x02,0x76,
0x02,0x8E,0x02,0xA9,0x02,0xBA,0x02,0xCF,0x02,0xDD,0x02,0xF0,0x02,0xFC,0x03,0x0D,0x03,
0x2C,0x03,0xE8
}},

   {0xf0,        5,      {0x55, 0xaa, 0x52, 0x08, 0x00}},
   {0xb5,       1,      {0x50}},   
   {0xb1,       2,      {0xfc, 0x00}},//from fc
   {0xb6,       1,      {0x04}},
   {0xb7,       2,      {0x70, 0x70}},
   {0xb8,       4,      {0x01,0x05,0x05,0x05}},

   {0xbc,       3,      {0x02,0x00,0x00}}, //zhaoshaopeng from 0x00  
   {0xbd,       5,      {0x01,0x84,0x1C,0x1C,0x00}},
   {0xc9,       5,      {0xD0,0x82,0x50,0x50,0x50}},
   {0x35,       1,      {0x00}},//from fc
   {0x36,       1,      {0x00}},
   {0x3a,       1,      {0x77}},
//*************************************
// Sleep Out
//*************************************
       {0x11, 0, {}},

       {REGFLAG_DELAY, 124, {}},

//*************************************
// Display On
//*************************************

   {0x29, 0, {}},

   //{REGFLAG_DELAY, 200, {}},

   // Note

   // Strongly recommend not to set Sleep out / Display On here. That will cause messed frame to be shown as later the backlight is on.

   // Setting ending by predefined flag

   {REGFLAG_END_OF_TABLE, 0x00, {}}

};



#endif

#if 0
static struct LCM_setting_table lcm_set_window[] = {
	{0x2A,	4,	{0x00, 0x00, (FRAME_WIDTH>>8), (FRAME_WIDTH&0xFF)}},
	{0x2B,	4,	{0x00, 0x00, (FRAME_HEIGHT>>8), (FRAME_HEIGHT&0xFF)}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
#endif

static struct LCM_setting_table lcm_sleep_out_setting[] = {
    // Sleep Out
   {0x11, 1, {0x00}},
   {REGFLAG_DELAY, 125, {}},
    // Display ON
   {0x29, 1, {0x00}},
   {REGFLAG_DELAY, 20, {}},
   {REGFLAG_END_OF_TABLE, 0x00, {}}

};


static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
	// Display off sequence
	{0x28, 1, {0x00}},
       {REGFLAG_DELAY, 200, {}},
    // Sleep Mode On
	{0x10, 1, {0x00}},
       {REGFLAG_DELAY, 200, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_compare_id_setting[] = {
	// Display off sequence
	{0xB9,	3,	{0xFF, 0x83, 0x69}},
	{REGFLAG_DELAY, 10, {}},

    // Sleep Mode On
//	{0xC3, 1, {0xFF}},

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
#if 1
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
		params->dbi.te_edge_polarity                 = LCM_POLARITY_RISING;
#if (LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
#else
		params->dsi.mode   = SYNC_PULSE_VDO_MODE;
#endif
		// DSI
		/* Command mode setting */

		params->dsi.LANE_NUM                          = LCM_TWO_LANE;
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

		params->dsi.vertical_sync_active=2; 
		params->dsi.vertical_backporch=2; 
		params->dsi.vertical_frontporch=2;
#ifdef MTK_WVGA_LCD_SUPPORT
            params->dsi.vertical_active_line=400;
#else
            params->dsi.vertical_active_line=854;
#endif
		params->dsi.line_byte=2180; 	// 2256 = 752*3
		params->dsi.horizontal_sync_active_byte=26;
		params->dsi.horizontal_backporch_byte=206;
		params->dsi.horizontal_frontporch_byte=206;   
		params->dsi.rgb_byte=(480*3+6); 	 
		params->dsi.horizontal_sync_active_word_count=20;	 
		params->dsi.horizontal_backporch_word_count=200;
		params->dsi.horizontal_frontporch_word_count=200;

		// Bit rate calculation
		params->dsi.pll_div1=38;              // fref=26MHz, fvco=fref*(div1+1)  (div1=0~63, fvco=500MHZ~1GHz)zhaoshaopeng from 38
		params->dsi.pll_div2=1;                    // div2=0~15: fout=fvo/(2*div2)


		params->dsi.HS_TRAIL    = 10;//10;//min max(n*8*UI, 60ns+n*4UI)  HS_TRAIL is low set 10
		params->dsi.HS_ZERO	   = 7;//8;//min 105ns+6*UI 
		params->dsi.HS_PRPR	   = 4;//min 40ns+4*UI; max 85ns+6UI 
		params->dsi.LPX		   = 12;//min 50ns 
		params->dsi.TA_SACK=0x01;	
		params->dsi.TA_GET=50;
		params->dsi.TA_SURE=15;  
		params->dsi.TA_GO	   = 12;//12;//4*LPX
		params->dsi.CLK_TRAIL   = 10;//5;//min 60ns   CLK_TRAIL is low  set 10
		params->dsi.CLK_ZERO    = 16;//18;//min 300ns-38ns
		params->dsi.LPX_WAIT    = 10;
		params->dsi.CONT_DET    = 0; 
		params->dsi.CLK_HS_PRPR = 4;//min 38ns; max 95ns

		// Non-continuous clock
		//params->dsi.noncont_clock = TRUE;
		//params->dsi.noncont_clock_period = 2;	// Unit : frames		

#else
            memset(params, 0, sizeof(LCM_PARAMS));
            params->type   = LCM_TYPE_DSI;
            params->width  = FRAME_WIDTH;
            params->height = FRAME_HEIGHT;

            // enable tearing-free
            params->dbi.te_mode                                 = LCM_DBI_TE_MODE_VSYNC_ONLY;
            params->dbi.te_edge_polarity                 = LCM_POLARITY_RISING;
#if (LCM_DSI_CMD_MODE)
            params->dsi.mode   = CMD_MODE;
#else
            params->dsi.mode   = SYNC_PULSE_VDO_MODE;
#endif


            // DSI

            /* Command mode setting */

            params->dsi.LANE_NUM                                     = LCM_TWO_LANE;
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

        #if 1//zhaoshaopeng old

            params->dsi.vertical_sync_active=3;
            params->dsi.vertical_backporch=12;
            params->dsi.vertical_frontporch=2;
#ifdef MTK_WVGA_LCD_SUPPORT
            params->dsi.vertical_active_line=400;
#else
            params->dsi.vertical_active_line=800;
#endif
            params->dsi.line_byte=2048;          // 2256 = 752*3
            params->dsi.horizontal_sync_active_byte=26;
            params->dsi.horizontal_backporch_byte=146;
            params->dsi.horizontal_frontporch_byte=146;      
            params->dsi.rgb_byte=(480*3+6); 

            params->dsi.horizontal_sync_active_word_count=20; 
            params->dsi.horizontal_backporch_word_count=140;
            params->dsi.horizontal_frontporch_word_count=140;


            // Bit rate calculation
            params->dsi.pll_div1=38;              // fref=26MHz, fvco=fref*(div1+1)  (div1=0~63, fvco=500MHZ~1GHz)zhaoshaopeng from 38
            params->dsi.pll_div2=1;                    // div2=0~15: fout=fvo/(2*div2)
            #else
            params->dsi.vertical_sync_active=2;
            params->dsi.vertical_backporch=2;
            params->dsi.vertical_frontporch=2;
            params->dsi.vertical_active_line=800;
            params->dsi.line_byte=2180;          // 2256 = 752*3

            params->dsi.horizontal_sync_active_byte=26;
            params->dsi.horizontal_backporch_byte=206;
            params->dsi.horizontal_frontporch_byte=206;      
            params->dsi.rgb_byte=(480*3+6); 
            params->dsi.horizontal_sync_active_word_count=20; 
            params->dsi.horizontal_backporch_word_count=200;
            params->dsi.horizontal_frontporch_word_count=200;

 

            // Bit rate calculation

            params->dsi.pll_div1=0x1a;//38;              // fref=26MHz, fvco=fref*(div1+1)  (div1=0~63, fvco=500MHZ~1GHz)zhaoshaopeng from 38
            params->dsi.pll_div2=1;                    // div2=0~15: fout=fvo/(2*div2)
            #endif
			#endif
}


static void lcm_init(void)
{


    SET_RESET_PIN(1);
    MDELAY(1 );
    SET_RESET_PIN(0);
    MDELAY(20);
    SET_RESET_PIN(1);
    MDELAY(120);
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);


   
}


static void lcm_suspend(void)
{
       #if 1
	push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
	#else
	char null = 0;
	dsi_set_cmdq_V2(0x28, 1, &null, 1);
	MDELAY(200);
       dsi_set_cmdq_V2(0x10, 1, &null, 1);
       MDELAY(200);	   
       //SET_RESET_PIN(0);	   
       //dsi_set_cmdq_V2(0x78, 1, &null, 1);
       //MDELAY(1);	   
       //SET_RESET_PIN(1);
       #endif
}

static void lcm_resume(void)
{
	//lcm_init();
	push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);


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
    static int err_count = 0;
    //static int test_count = 0;
    unsigned char buffer_1[12];
    unsigned int array[16];
    int i;
    unsigned char fResult;

        if(lcm_esd_test)
        {
            lcm_esd_test = FALSE;
            return TRUE;
        }
    //printk("\r\n lcm_esd_check enter!\n");
    for (i = 0; i < 12; i++)
    {
        buffer_1[i] = 0x00;
    }
    array[0] = 0x00013700;
    dsi_set_cmdq(array, 1, 1);
    read_reg_v2(0x0A, buffer_1, 7);

    //printk("\r\n lcm_esd_check read(0x0A)\n");
    for (i = 0; i < 7; i++)
    printk("\r\n buffer_1[%d]:%x\n", i, buffer_1[i]);

                
    if (buffer_1[3] == 0x02)
    {
        if (buffer_1[4] & 0x02)
            err_count++;
        else
            err_count = 0;
    }
    else
    {
        err_count = 0;
    }
    //printk("lcm_esd_check err_count=%d\n", err_count);

    if ((buffer_1[0] != 0x9c) || (err_count >= 2))
    {
        err_count = 0;
        //uncount++;
        //printk("lcm_esd_check end1\n");
        fResult = 1;
    }
    else
    {
        //count++;
        //printk("lcm_esd_check end2\n");
        fResult = 0;
    }
    array[0] = 0x00002200;
    dsi_set_cmdq(array, 1, 1);
                
    array[0] = 0x00033700;
    dsi_set_cmdq(array, 1, 1);

    read_reg_v2(0xBC, buffer_1, 12);
    //printk("lcm_esd_check: read(0xBC)\n");
    for (i = 0; i < 12; i++)
        {
        //printk("buffer_1[%d]:%x\n", i, buffer_1[i]);
    }
    if (fResult)
        {
        return 1;
        }
        else
        {            
        return 0;
        }
#endif
}

static unsigned int lcm_esd_recover(void)
{
    unsigned char para = 0;

    SET_RESET_PIN(1);
    MDELAY(1);
    SET_RESET_PIN(0);
    MDELAY(10);
    SET_RESET_PIN(1);
    MDELAY(120);
	  push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
    MDELAY(10);
    //push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
    //MDELAY(10);
    //dsi_set_cmdq_V2(0x35, 1, &para, 1);     ///enable TE
    //MDELAY(10);

    return TRUE;
}

static unsigned int lcm_compare_id(void)
{
	volatile unsigned int id = 0;
	volatile unsigned char buffer[4];
	volatile unsigned int array[16];  
	volatile unsigned char lcd_id =3;

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
	printk("### leanda otm8009 id = %x  lcd_id = %d  \r\n",id,lcd_id);
	//if((lcd_id == 0)&&(id!=0x8009))
	if(lcd_id == 0)		
        return 1;
        else
        return 0;

	//return ((lcd_id == 1)&&(LCM_ID_OTM8009A == id))?1:0;	
}

// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER nt35510_lead_lc_lcm_drv = 
{
    .name			= "nt35510_lead_lc",
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
	//.set_backlight	= lcm_setbacklight,
//	.set_pwm        = lcm_setpwm,
//	.get_pwm        = lcm_getpwm,
       //zhaoshaopeng 20130315 for esd
	.esd_check   = lcm_esd_check,
       .esd_recover   = lcm_esd_recover,
	.compare_id    = lcm_compare_id,
#endif
};
