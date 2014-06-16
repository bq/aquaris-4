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
#define FRAME_HEIGHT 										(800)
#define LCM_ID_OMT       (0x40)
#define REGFLAG_DELAY             							0XFE
#define REGFLAG_END_OF_TABLE      							0xfd  // END OF REGISTERS MARKER
#define LCM_ID_OTM8009A		0x8009


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
#define dsi_set_cmdq_Vzsp(cmd, ppara,count)  dsi_set_cmdq_V2(cmd, count, ppara, 1)
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

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update);

static struct LCM_setting_table blockwrite_otm8009a_setting[] = {
	//DCS Write
	{0xB7,	2,	{0x10, 0x02}},
	//Set VC2=0
	{0xB8,	2,	{0x04, 0x00}},
	{0xbc,	2,	{0x05, 0x00}},
	{0xbd,	2,	{0x00, 0x00}},
	{0xbe,	2,	{0x05, 0x00}},
       {0xbf,	2,	{0x05, 0x00}},	

	
	{REGFLAG_DELAY, 200, {}},

    // Sleep Mode On
//	{0xC3, 1, {0xFF}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
unsigned char p_data[20] = {0};

/*
void BlockWrite_OTM8009A_MIPI(U32 XS,U32 YS,U32 XE,U32 YE)
{ 

    	XS=0;
       YS=0;
     	XE=480;
       YE=800;
    

	p_data[0] = 0x2A; p_data[1]=(XS>>8); p_data[2]=XS;p_data[3]=XE>>8;p_data[4]=XE;
       dsi_set_cmdq_Vzsp(0xbf, p_data, 5);
	   
	p_data[0] = 0x10; p_data[1]=0x02; 
       dsi_set_cmdq_Vzsp(0xB7, p_data, 2);
	p_data[0] = 0x04; p_data[1]=0x00; 
       dsi_set_cmdq_Vzsp(0xB8, p_data, 2);	   
	p_data[0] = 0x05; p_data[1]=0x00; 
       dsi_set_cmdq_Vzsp(0xbc, p_data, 2);	
	p_data[0] = 0x00; p_data[1]=0x00; 
       dsi_set_cmdq_Vzsp(0xBD, p_data, 2);	
	p_data[0] = 0x05; p_data[1]=0x00; 
       dsi_set_cmdq_Vzsp(0xbe, p_data, 2);	
	   
	p_data[0] = 0x2b; p_data[1]=YS>>8; p_data[2]=YS;p_data[3]=YE>>8;p_data[4]=YE;
       dsi_set_cmdq_Vzsp(0xbf, p_data, 5);

	p_data[0] = 0x50; p_data[1]=0x02; 
       dsi_set_cmdq_Vzsp(0xB7, p_data, 2);

	p_data[0] = 0x00; p_data[1]=0x94; 
       dsi_set_cmdq_Vzsp(0xbc, p_data, 2);	
	   
	p_data[0] = 0x11; p_data[1]=0x00; 
       dsi_set_cmdq_Vzsp(0xBD, p_data, 2);	
	p_data[0] = 0xc0; p_data[1]=0x03; 
       dsi_set_cmdq_Vzsp(0xbe, p_data, 2);	                     


        wrtie_cmd(0x2C);
        MDELAY(50);   
                    
 } 

*/
static struct LCM_setting_table Init_SSD2805_Initinal_setting[] = {
	{REGFLAG_DELAY, 50, {}},
	{0xB9,	2,	{0x00, 0x00}},
	{0xBa,	2,	{0x31, 0x00}},
	
	{0xbb,	2,	{0x66, 0x00}},
	{0xb9,	2,	{0x10, 0x00}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};    

static struct LCM_setting_table Init_SSD2805_Write_0P_setting[] = {
	{0xB7,	2,	{0x10, 0x02}},
	{0xB8,	2,	{0x04, 0x00}},
	
	{0xbc,	2,	{0x01, 0x00}},
	{0xbd,	2,	{0x00, 0x00}},
       {0xbe,	2,	{0x01, 0x00}},
       //{0xbf,	0,    {0x00}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};  
static void Init_SSD2805_Write_0P(void)
{
	push_table(Init_SSD2805_Write_0P_setting, sizeof(Init_SSD2805_Write_0P_setting) / sizeof(struct LCM_setting_table), 1);

}
static struct LCM_setting_table Init_SSD2805_Write_1P_setting[] = {
	{0xB7,	2,	{0x10, 0x02}},
	{0xB8,	2,	{0x04, 0x00}},
	
	{0xbc,	2,	{0x02, 0x00}},
	{0xbd,	2,	{0x00, 0x00}},
       {0xbe,	2,	{0x02, 0x00}},
       //{0xbf,	0,    {0x00}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};  
static void Init_SSD2805_Write_1P(void)
{
	push_table(Init_SSD2805_Write_1P_setting, sizeof(Init_SSD2805_Write_1P_setting) / sizeof(struct LCM_setting_table), 1);

}

static struct LCM_setting_table Init_SSD2805_Write_2P_setting[] = {
	{0xB7,	2,	{0x10, 0x02}},
	{0xB8,	2,	{0x04, 0x00}},
	
	{0xbc,	2,	{0x03, 0x00}},
	{0xbd,	2,	{0x00, 0x00}},
       {0xbe,	2,	{0x03, 0x00}},
       //{0xbf,	0,    {0x00}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};  
static void Init_SSD2805_Write_2P(void)
{
	push_table(Init_SSD2805_Write_2P_setting, sizeof(Init_SSD2805_Write_2P_setting) / sizeof(struct LCM_setting_table), 1);

}
static struct LCM_setting_table Init_SSD2805_Write_3P_setting[] = {
	{0xB7,	2,	{0x10, 0x02}},
	{0xB8,	2,	{0x04, 0x00}},
	
	{0xbc,	2,	{0x04, 0x00}},
	{0xbd,	2,	{0x00, 0x00}},
       {0xbe,	2,	{0x04, 0x00}},
       //{0xbf,	0,    {0x00}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};  
static void Init_SSD2805_Write_3P(void)
{
	push_table(Init_SSD2805_Write_3P_setting, sizeof(Init_SSD2805_Write_3P_setting) / sizeof(struct LCM_setting_table), 1);

}

static struct LCM_setting_table Init_SSD2805_Write_4P_setting[] = {
	{0xB7,	2,	{0x10, 0x02}},
	{0xB8,	2,	{0x04, 0x00}},
	
	{0xbc,	2,	{0x05, 0x00}},
	{0xbd,	2,	{0x00, 0x00}},
       {0xbe,	2,	{0x05, 0x00}},
       //{0xbf,	0,    {0x00}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};  
static void Init_SSD2805_Write_4P(void)
{
	push_table(Init_SSD2805_Write_4P_setting, sizeof(Init_SSD2805_Write_4P_setting) / sizeof(struct LCM_setting_table), 1);

}


static struct LCM_setting_table Init_SSD2805_Write_5P_setting[] = {
	{0xB7,	2,	{0x10, 0x02}},
	{0xB8,	2,	{0x04, 0x00}},
	
	{0xbc,	2,	{0x06, 0x00}},
	{0xbd,	2,	{0x00, 0x00}},
       {0xbe,	2,	{0x06, 0x00}},
       //{0xbf,	0,    {0x00}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};  
static void Init_SSD2805_Write_5P(void)
{
	push_table(Init_SSD2805_Write_5P_setting, sizeof(Init_SSD2805_Write_5P_setting) / sizeof(struct LCM_setting_table), 1);

}
static struct LCM_setting_table Init_SSD2805_Write_6P_setting[] = {
	{0xB7,	2,	{0x10, 0x02}},
	{0xB8,	2,	{0x04, 0x00}},
	
	{0xbc,	2,	{0x07, 0x00}},
	{0xbd,	2,	{0x00, 0x00}},
       {0xbe,	2,	{0x07, 0x00}},
       //{0xbf,	0,    {0x00}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};  
static void Init_SSD2805_Write_6P(void)
{
	push_table(Init_SSD2805_Write_6P_setting, sizeof(Init_SSD2805_Write_6P_setting) / sizeof(struct LCM_setting_table), 1);

}


static struct LCM_setting_table Init_SSD2805_Write_7P_setting[] = {
	{0xB7,	2,	{0x10, 0x02}},
	{0xB8,	2,	{0x04, 0x00}},
	
	{0xbc,	2,	{0x08, 0x00}},
	{0xbd,	2,	{0x00, 0x00}},
       {0xbe,	2,	{0x08, 0x00}},
       //{0xbf,	0,    {0x00}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};  
static void Init_SSD2805_Write_7P(void)
{
	push_table(Init_SSD2805_Write_7P_setting, sizeof(Init_SSD2805_Write_7P_setting) / sizeof(struct LCM_setting_table), 1);

}

static struct LCM_setting_table Init_SSD2805_Write_8P_setting[] = {
	{0xB7,	2,	{0x10, 0x02}},
	{0xB8,	2,	{0x04, 0x00}},
	
	{0xbc,	2,	{0x09, 0x00}},
	{0xbd,	2,	{0x00, 0x00}},
       {0xbe,	2,	{0x09, 0x00}},
       //{0xbf,	0,    {0x00}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};  
static void Init_SSD2805_Write_8P(void)
{
	push_table(Init_SSD2805_Write_8P_setting, sizeof(Init_SSD2805_Write_8P_setting) / sizeof(struct LCM_setting_table), 1);

}

static struct LCM_setting_table Init_SSD2805_Write_9P_setting[] = {
	{0xB7,	2,	{0x10, 0x02}},
	{0xB8,	2,	{0x04, 0x00}},
	
	{0xbc,	2,	{0x0a, 0x00}},
	{0xbd,	2,	{0x00, 0x00}},
       {0xbe,	2,	{0x0a, 0x00}},
       //{0xbf,	0,    {0x00}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};  
static void Init_SSD2805_Write_9P(void)
{
	push_table(Init_SSD2805_Write_9P_setting, sizeof(Init_SSD2805_Write_9P_setting) / sizeof(struct LCM_setting_table), 1);

}

static struct LCM_setting_table Init_SSD2805_Write_10P_setting[] = {
	{0xB7,	2,	{0x10, 0x02}},
	{0xB8,	2,	{0x04, 0x00}},
	
	{0xbc,	2,	{0x0b, 0x00}},
	{0xbd,	2,	{0x00, 0x00}},
       {0xbe,	2,	{0x0b, 0x00}},
       //{0xbf,	0,    {0x00}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};  	
static void Init_SSD2805_Write_10P(void)
{
	push_table(Init_SSD2805_Write_10P_setting, sizeof(Init_SSD2805_Write_10P_setting) / sizeof(struct LCM_setting_table), 1);

}


static struct LCM_setting_table Init_SSD2805_Write_12P_setting[] = {
	{0xB7,	2,	{0x10, 0x02}},
	{0xB8,	2,	{0x04, 0x00}},
	
	{0xbc,	2,	{0x0d, 0x00}},
	{0xbd,	2,	{0x00, 0x00}},
       {0xbe,	2,	{0x0d, 0x00}},
       //{0xbf,	0,    {0x00}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};  	
static void Init_SSD2805_Write_12P(void)
{
	push_table(Init_SSD2805_Write_12P_setting, sizeof(Init_SSD2805_Write_12P_setting) / sizeof(struct LCM_setting_table), 1);

}

static struct LCM_setting_table Init_SSD2805_Write_14P_setting[] = {
	{0xB7,	2,	{0x10, 0x02}},
	{0xB8,	2,	{0x04, 0x00}},
	
	{0xbc,	2,	{0x0f, 0x00}},
	{0xbd,	2,	{0x00, 0x00}},
       {0xbe,	2,	{0x0f, 0x00}},
       //{0xbf,	0,    {0x00}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};  
static void Init_SSD2805_Write_14P(void)
{
	push_table(Init_SSD2805_Write_14P_setting, sizeof(Init_SSD2805_Write_14P_setting) / sizeof(struct LCM_setting_table), 1);

}

static struct LCM_setting_table Init_SSD2805_Write_15P_setting[] = {
	{0xB7,	2,	{0x10, 0x02}},
	{0xB8,	2,	{0x04, 0x00}},
	
	{0xbc,	2,	{0x10, 0x00}},
	{0xbd,	2,	{0x00, 0x00}},
       {0xbe,	2,	{0x10, 0x00}},
       //{0xbf,	0,    {0x00}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};  
static void Init_SSD2805_Write_15P(void)
{
	push_table(Init_SSD2805_Write_15P_setting, sizeof(Init_SSD2805_Write_15P_setting) / sizeof(struct LCM_setting_table), 1);

}

static struct LCM_setting_table Init_SSD2805_Write_16P_setting[] = {
	{0xB7,	2,	{0x10, 0x02}},
	{0xB8,	2,	{0x04, 0x00}},
	
	{0xbc,	2,	{0x11, 0x00}},
	{0xbd,	2,	{0x00, 0x00}},
       {0xbe,	2,	{0x11, 0x00}},
       //{0xbf,	0,    {0x00}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};  
static void Init_SSD2805_Write_16P(void)
{
	push_table(Init_SSD2805_Write_16P_setting, sizeof(Init_SSD2805_Write_16P_setting) / sizeof(struct LCM_setting_table), 1);

}

static struct LCM_setting_table Init_SSD2805_Write_360P_1_setting[] = {
{0xD4,	360, {
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40,
0x00,
0x40
	}

	},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};  


static struct LCM_setting_table Init_SSD2805_Write_360P_2_setting[] = {
{0xD5,	360, {
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,
0x00,
0x60,

	}

	},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};  
static void Init_SSD2805_Write_360P_1(void)
{
	push_table(Init_SSD2805_Write_360P_1_setting, sizeof(Init_SSD2805_Write_360P_1_setting) / sizeof(struct LCM_setting_table), 1);

}


static void Init_SSD2805_Write_360P_2(void)
{
	push_table(Init_SSD2805_Write_360P_2_setting, sizeof(Init_SSD2805_Write_360P_2_setting) / sizeof(struct LCM_setting_table), 1);

}

void init_to_read_otm8009(void)
{
	p_data[0]=0x80; p_data[1]=0x09;p_data[2]=0x01;
       //Init_SSD2805_Write_3P();        
       dsi_set_cmdq_Vzsp(0xff, p_data, 3);
	
//{0xbf,	0,    {0x00}},
       //Init_SSD2805_Write_1P(); // enable Orise mode
       p_data[0]=0x80;
	dsi_set_cmdq_Vzsp(0x00, p_data, 1);
	

	//Init_SSD2805_Write_2P();
	p_data[0]=0x80; p_data[1]=0x09;
	dsi_set_cmdq_Vzsp(0xff, p_data, 2);
	MDELAY(200);
}

void Init_OTM8009A_BOE500_MIPI_MPU8bits(void)
{ 	
	p_data[0]=0x80; p_data[1]=0x09;p_data[2]=0x01;
       //Init_SSD2805_Write_3P();        
       dsi_set_cmdq_Vzsp(0xff, p_data, 3);
	
//{0xbf,	0,    {0x00}},
       //Init_SSD2805_Write_1P(); // enable Orise mode
       p_data[0]=0x80;
	dsi_set_cmdq_Vzsp(0x00, p_data, 1);
	

	//Init_SSD2805_Write_2P();
	p_data[0]=0x80; p_data[1]=0x09;
	dsi_set_cmdq_Vzsp(0xff, p_data, 2);

//**************************************************************************
       //Init_SSD2805_Write_1P(); 
       p_data[0]=0x82;
	dsi_set_cmdq_Vzsp(0x00, p_data, 1);
	
       //Init_SSD2805_Write_1P(); 
       p_data[0]=0x20;
	dsi_set_cmdq_Vzsp(0xb2, p_data, 1);	

       //Init_SSD2805_Write_1P(); 
       p_data[0]=0x80;
	dsi_set_cmdq_Vzsp(0x00, p_data, 1);	
	
       //Init_SSD2805_Write_1P(); 
       p_data[0]=0x03;
	dsi_set_cmdq_Vzsp(0xC5, p_data, 1);	
	
       //Init_SSD2805_Write_1P(); 
       p_data[0]=0x84;
	dsi_set_cmdq_Vzsp(0x00, p_data, 1);
	
       //Init_SSD2805_Write_1P(); 
       p_data[0]=0x08;
	dsi_set_cmdq_Vzsp(0xc0, p_data, 1);
	
       //Init_SSD2805_Write_1P(); 
       p_data[0]=0xB4;
	dsi_set_cmdq_Vzsp(0x00, p_data, 1);

       //Init_SSD2805_Write_1P(); 
       p_data[0]=0x00;
	dsi_set_cmdq_Vzsp(0xC0, p_data, 1);

       //Init_SSD2805_Write_1P(); 
       p_data[0]=0xA1;
	dsi_set_cmdq_Vzsp(0x00, p_data, 1);
	
       //Init_SSD2805_Write_1P(); 
       p_data[0]=0x08;
	dsi_set_cmdq_Vzsp(0xC1, p_data, 1);
	
       //Init_SSD2805_Write_1P(); 
       p_data[0]=0x90;
	dsi_set_cmdq_Vzsp(0x00, p_data, 1);
	

	//Init_SSD2805_Write_5P();
	p_data[0]=0x96;p_data[1] = 0x0F; p_data[2]=0x01;p_data[3] = 0x00; p_data[4]=0x33;
	dsi_set_cmdq_Vzsp(0xc5, p_data, 5);
	
       //Init_SSD2805_Write_1P(); 
       p_data[0]=0xA3;
	dsi_set_cmdq_Vzsp(0x00, p_data, 1);	

       //Init_SSD2805_Write_1P(); 
       p_data[0]=0x1b;
	dsi_set_cmdq_Vzsp(0xC0, p_data, 1);	

       //Init_SSD2805_Write_1P(); 
       p_data[0]=0x81;
	dsi_set_cmdq_Vzsp(0x00, p_data, 1);	
	
       //Init_SSD2805_Write_1P(); 
       p_data[0]=0x83;
	dsi_set_cmdq_Vzsp(0xC4, p_data, 1);		
	
       //Init_SSD2805_Write_1P(); 
       p_data[0]=0xB1;
	dsi_set_cmdq_Vzsp(0x00, p_data, 1);	

       //Init_SSD2805_Write_1P(); 
       p_data[0]=0x29;
	dsi_set_cmdq_Vzsp(0xC5, p_data, 1);		

	//Init_SSD2805_Write_2P();
	p_data[0]=0x70; p_data[0]=0x70;
	dsi_set_cmdq_Vzsp(0xD8, p_data, 2);
	
       //Init_SSD2805_Write_1P(); 
       p_data[0]=0x36;
	dsi_set_cmdq_Vzsp(0xD9, p_data, 1);

//*********************************CE***********************************     
       Init_SSD2805_Write_360P_1();

//D5 setting
       Init_SSD2805_Write_360P_2();


/*
    Init_SSD2805_Write_1P();
	WriteR(0x00); 
	WriteR(0x80);
	Init_SSD2805_Write_1P();
    WriteR(0xD6); 
	WriteR(0x08);
*/

       //p_data[0]=0x80;
//	dsi_set_cmdq_Vzsp(0x00, p_data, 1);
//       p_data[0]=0x08;
//	dsi_set_cmdq_Vzsp(0xd6, p_data, 1);	
//*********************************Gamma***********************************
// Positive

    //Init_SSD2805_Write_16P();
       p_data[0]=0x05;p_data[1] = 0x16; p_data[2]=0x17;
       p_data[3] = 0x12; p_data[4]=0x0B;p_data[5] = 0x1D; p_data[6]=0x0F;
       p_data[7] = 0x0E; p_data[8]=0x00;p_data[9] = 0x05; p_data[10]=0x02;
       p_data[11] = 0x06; p_data[12]=0x0E;p_data[13] = 0x20; p_data[14]=0x1D;p_data[15]=0x16;	 	   
	dsi_set_cmdq_Vzsp(0xE1, p_data, 16);

//NEGATIVE

    //Init_SSD2805_Write_16P();
       p_data[0]=0x05;p_data[1] = 0x16; p_data[2]=0x17;
       p_data[3] = 0x12; p_data[4]=0x0B;p_data[5] = 0x1D; p_data[6]=0x0F;
       p_data[7] = 0x0E; p_data[8]=0x00;p_data[9] = 0x05; p_data[10]=0x02;
       p_data[11] = 0x07; p_data[12]=0x0E;p_data[13] = 0x20; p_data[14]=0x1D;p_data[15]=0x16;	 	   
	dsi_set_cmdq_Vzsp(0xE2, p_data, 16);

/////////////////////////////////////////////////////////////////////
  /*  Init_SSD2805_Write_1P(); 
	WriteR(0x00); 
	WriteR(0xa6);
	Init_SSD2805_Write_1P();
	WriteR(0xb3); // reg_panel_zinv, reg_panel_zinv_pixel, reg_panel_zinv_odd, reg_panel_zigzag, reg_panel_zigzag_blue, reg_panel_zigzag_shift_r, reg_panel_zigzag_odd
	WriteR(0x20);


    Init_SSD2805_Write_1P();
	WriteR(0x00); 
	WriteR(0xa7);
	Init_SSD2805_Write_1P();
	WriteR(0xb3); //  panel_set[0] = 1
	WriteR(0x01);
*/	

       //Init_SSD2805_Write_1P(); 
       p_data[0]=0x80;
	dsi_set_cmdq_Vzsp(0x00, p_data, 1);

	//Init_SSD2805_Write_6P(); 
       p_data[0]=0x85;p_data[1] = 0x01; p_data[2]=0x00;p_data[3] = 0x84; p_data[4]=0x01;p_data[5]=0x00;
	dsi_set_cmdq_Vzsp(0xce, p_data, 6);

       //Init_SSD2805_Write_1P();
       p_data[0]=0xa0;
	dsi_set_cmdq_Vzsp(0x00, p_data, 1);	   

	//Init_SSD2805_Write_14P();
       p_data[0]=0x18;p_data[1] = 0x04; p_data[2]=0x03;
       p_data[3] = 0x21; p_data[4]=0x00;p_data[5] = 0x00; p_data[6]=0x00;
       p_data[7] = 0x18; p_data[8]=0x03;p_data[9] = 0x03; p_data[10]=0x22;
       p_data[11] = 0x00; p_data[12]=0x00;p_data[13] = 0x00; 
	dsi_set_cmdq_Vzsp(0xce, p_data, 14);
	
       //Init_SSD2805_Write_1P();
       p_data[0]=0xb0;
	dsi_set_cmdq_Vzsp(0x00, p_data, 1);		   

	//Init_SSD2805_Write_14P();
       p_data[0]=0x18;p_data[1] = 0x02; p_data[2]=0x03;
       p_data[3] = 0x23; p_data[4]=0x00;p_data[5] = 0x00; p_data[6]=0x00;
       p_data[7] = 0x18; p_data[8]=0x01;p_data[9] = 0x03; p_data[10]=0x24;
       p_data[11] = 0x00; p_data[12]=0x00;p_data[13] = 0x00; 
	dsi_set_cmdq_Vzsp(0xce, p_data, 14);
                     
	 
       //Init_SSD2805_Write_1P();
       p_data[0]=0xc0;
	dsi_set_cmdq_Vzsp(0x00, p_data, 1);		   

	//Init_SSD2805_Write_10P();
       p_data[0]=0x01;p_data[1] = 0x01; p_data[2]=0x20;
       p_data[3] = 0x20; p_data[4]=0x00;p_data[5] = 0x00; p_data[6]=0x01;
       p_data[7] = 0x00; p_data[8]=0x00;p_data[9] = 0x00; 
	dsi_set_cmdq_Vzsp(0xcf, p_data, 10);	
	  
	//Init_SSD2805_Write_1P();
       p_data[0]=0xd0;
	dsi_set_cmdq_Vzsp(0x00, p_data, 1);	

	//Init_SSD2805_Write_1P(); 
       p_data[0] = 0x00;
	dsi_set_cmdq_Vzsp(0xcf, p_data, 1);	

//--------------------------------------------------------------------------------
//		initial setting 3 < Panel setting >
//--------------------------------------------------------------------------------
/*
// CB8x

    Init_SSD2805_Write_1P();
	WriteR(0x00); 
	WriteR(0x80);
	Init_SSD2805_Write_10P();
	WriteR(0xcb);//cb81[7:0] : signal mode setting of  sig4,  sig3,  sig2,  sig1 ( sleep in )
  	WriteR(0x00); 
	//cb82[7:0] : signal mode setting of  sig8,  sig7,  sig6,  sig5 ( sleep in )
  	WriteR(0x00); 
	//cb83[7:0] : signal mode setting of sig12, sig11, sig10,  sig9 ( sleep in )
  	WriteR(0x00); 
	//cb84[7:0] : signal mode setting of sig16, sig15, sig14, sig13 ( sleep in )
  	WriteR(0x00); 
	//cb85[7:0] : signal mode setting of sig20, sig19, sig18, sig17 ( sleep in )
  	WriteR(0x00); 
	//cb86[7:0] : signal mode setting of sig24, sig23, sig22, sig21 ( sleep in )
  	WriteR(0x00); 
	//cb87[7:0] : signal mode setting of sig28, sig27, sig26, sig25 ( sleep in )
  	WriteR(0x00); 
	//cb88[7:0] : signal mode setting of sig32, sig31, sig30, sig29 ( sleep in )
  	WriteR(0x00); 
	//cb89[7:0] : signal mode setting of sig36, sig35, sig34, sig33 ( sleep in )
  	WriteR(0x00); 
	//cb8a[7:0] : signal mode setting of sig40, sig39, sig38, sig37 ( sleep in )
  	WriteR(0x00); 

// cb9x	 
    Init_SSD2805_Write_1P();
	WriteR(0x00); 
	WriteR(0x90);
	Init_SSD2805_Write_15P();
	WriteR(0xcb);//cb91[7:0] : enmode L-byte of sig1  (pwron_3, pwron_2, pwron_1, pwron_0 )	 
  	WriteR(0x00);
	//cb92[7:0] : enmode L-byte of sig2  (pwron_3, pwron_2, pwron_1, pwron_0 )
  	WriteR(0x00);
	//cb93[7:0] : enmode L-byte of sig3  (pwron_3, pwron_2, pwron_1, pwron_0 ) 
  	WriteR(0x00);
	//cb94[7:0] : enmode L-byte of sig4  (pwron_3, pwron_2, pwron_1, pwron_0 ) 
  	WriteR(0x00);
	//cb95[7:0] : enmode L-byte of sig5  (pwron_3, pwron_2, pwron_1, pwron_0 ) 
  	WriteR(0x00);
	//cb96[7:0] : enmode L-byte of sig6  (pwron_3, pwron_2, pwron_1, pwron_0 )  
  	WriteR(0x00);
	//cb97[7:0] : enmode L-byte of sig7  (pwron_3, pwron_2, pwron_1, pwron_0 ) 
  	WriteR(0x00);
	//cb98[7:0] : enmode L-byte of sig8  (pwron_3, pwron_2, pwron_1, pwron_0 ) 
  	WriteR(0x00);
	//cb99[7:0] : enmode L-byte of sig9  (pwron_3, pwron_2, pwron_1, pwron_0 ) 
  	WriteR(0x00);
	//cb9a[7:0] : enmode L-byte of sig10 (pwron_3, pwron_2, pwron_1, pwron_0 ) 
  	WriteR(0x00);
	//cb9b[7:0] : enmode L-byte of sig11 (pwron_3, pwron_2, pwron_1, pwron_0 ) 
  	WriteR(0x00);
	//cb9c[7:0] : enmode L-byte of sig12 (pwron_3, pwron_2, pwron_1, pwron_0 ) 
  	WriteR(0x00);
	//cb9d[7:0] : enmode L-byte of sig13 (pwron_3, pwron_2, pwron_1, pwron_0 ) 
  	WriteR(0x00);
	//cb9e[7:0] : enmode L-byte of sig14 (pwron_3, pwron_2, pwron_1, pwron_0 ) 
  	WriteR(0x00);
	//cb9f[7:0] : enmode L-byte of sig15 (pwron_3, pwron_2, pwron_1, pwron_0 ) 
  	WriteR(0x00); 

// cbax
    Init_SSD2805_Write_1P();
	WriteR(0x00); 
	WriteR(0xa0);
	Init_SSD2805_Write_15P();
	WriteR(0xcb);//cba1[7:0] : enmode L-byte of sig16 (pwron_3, pwron_2, pwron_1, pwron_0 )	  
  	WriteR(0x00); 
	//cba2[7:0] : enmode L-byte of sig17 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	WriteR(0x00); 
	//cba3[7:0] : enmode L-byte of sig18 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	WriteR(0x00); 
	//cba4[7:0] : enmode L-byte of sig19 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	WriteR(0x00); 
	//cba5[7:0] : enmode L-byte of sig20 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	WriteR(0x00); 
	//cba6[7:0] : enmode L-byte of sig21 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	WriteR(0x00); 
	//cba7[7:0] : enmode L-byte of sig22 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	WriteR(0x00); 
	//cba8[7:0] : enmode L-byte of sig23 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	WriteR(0x00); 
	//cba9[7:0] : enmode L-byte of sig24 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	WriteR(0x00); 
	//cbaa[7:0] : enmode L-byte of sig25 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	WriteR(0x00); 
	//cbab[7:0] : enmode L-byte of sig26 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	WriteR(0x00); 
	//cbac[7:0] : enmode L-byte of sig27 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	WriteR(0x00); 
	//cbad[7:0] : enmode L-byte of sig28 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	WriteR(0x00); 
	//cbae[7:0] : enmode L-byte of sig29 (pwron_3, pwron_2, pwron_1, pwron_0 )  
  	WriteR(0x00); 
	//cbaf[7:0] : enmode L-byte of sig30 (pwron_3, pwron_2, pwron_1, pwron_0 )  
  	WriteR(0x00);   

// cbbx	
    Init_SSD2805_Write_1P();
	WriteR(0x00); 
	WriteR(0xb0);
	Init_SSD2805_Write_10P(); 
	WriteR(0xcb);//cbb1[7:0] : enmode L-byte of sig31 (pwron_3, pwron_2, pwron_1, pwron_0 )	 
  	WriteR(0x00); 
	//cbb2[7:0] : enmode L-byte of sig32 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	WriteR(0x00); 
	//cbb3[7:0] : enmode L-byte of sig33 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	WriteR(0x00); 
	//cbb4[7:0] : enmode L-byte of sig34 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	WriteR(0x00); 
	//cbb5[7:0] : enmode L-byte of sig35 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	WriteR(0x00); 
	//cbb6[7:0] : enmode L-byte of sig36 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	WriteR(0x00); 
	//cbb7[7:0] : enmode L-byte of sig37 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	WriteR(0x00); 
	//cbb8[7:0] : enmode L-byte of sig38 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	WriteR(0x00); 
	//cbb9[7:0] : enmode L-byte of sig39 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	WriteR(0x00); 
	//cbba[7:0] : enmode L-byte of sig40 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	WriteR(0x00); 
  */
// cbcx	
       //Init_SSD2805_Write_1P();
       p_data[0]=0xc0;
	dsi_set_cmdq_Vzsp(0x00, p_data, 1);	

	//Init_SSD2805_Write_15P(); 
       p_data[0]=0x00;p_data[1] = 0x04; p_data[2]=0x04;
       p_data[3] = 0x04; p_data[4]=0x04;p_data[5] = 0x04; p_data[6]=0x00;
       p_data[7] = 0x00; p_data[8]=0x00;p_data[9] = 0x00; p_data[10]=0x00;
       p_data[11] = 0x00; p_data[12]=0x00;p_data[13] = 0x00; p_data[14] = 0x00;
	dsi_set_cmdq_Vzsp(0xcb, p_data,15);

// cbdx  
       //Init_SSD2805_Write_1P();
       p_data[0] = 0xd0;
	dsi_set_cmdq_Vzsp(0x00, p_data, 1);
	//Init_SSD2805_Write_15P();	
       p_data[0]=0x00;p_data[1] = 0x00; p_data[2]=0x00;
       p_data[3] = 0x00; p_data[4]=0x00;p_data[5] = 0x00; p_data[6]=0x04;
       p_data[7] = 0x04; p_data[8]=0x04;p_data[9] = 0x04; p_data[10]=0x04;
       p_data[11] = 0x00; p_data[12]=0x00;p_data[13] = 0x00; p_data[14] = 0x00;
	dsi_set_cmdq_Vzsp(0xcb, p_data,15);	
// cbex	 
       //Init_SSD2805_Write_1P();
       p_data[0] = 0xe0;
	dsi_set_cmdq_Vzsp(0x00, p_data, 1);

	//Init_SSD2805_Write_10P();
       p_data[0]=0x00; p_data[1]=0x00;p_data[2] = 0x00; p_data[3]=0x00;
       p_data[4] = 0x00; p_data[5]=0x00;p_data[6] = 0x00; p_data[7]=0x00;
       p_data[8] = 0x00; p_data[9]=0x00;
	dsi_set_cmdq_Vzsp(0xcb, p_data,10);		
// cbfx	 
       //Init_SSD2805_Write_1P();
       p_data[0] =0xf0;
	dsi_set_cmdq_Vzsp(0x00, p_data, 1);

	//Init_SSD2805_Write_10P();
       p_data[0] = 0xff; p_data[1]=0xFF;p_data[2] = 0xFF; p_data[3]=0xFF;
       p_data[4] = 0xFF; p_data[5]=0xFF;p_data[6] = 0xFF; p_data[7]=0xFF;
       p_data[8] = 0xFF; p_data[9]=0xFF;
	dsi_set_cmdq_Vzsp(0xf0, p_data,10);		
// cc8x	 
       //Init_SSD2805_Write_1P();
       p_data[0] = 0x80;
	dsi_set_cmdq_Vzsp(0x00, p_data, 1);
	//Init_SSD2805_Write_10P();
       p_data[0]=0x00;p_data[1] = 0x26; p_data[2]=0x09;
       p_data[3] = 0x0B; p_data[4]=0x01;p_data[5] = 0x25; p_data[6]=0x00;
       p_data[7] = 0x00; p_data[8]=0x00;p_data[9] = 0x00; 
       dsi_set_cmdq_Vzsp(0xcc, p_data, 10);	   
// cc9x
       //Init_SSD2805_Write_1P();
       p_data[0] = 0x90;
	dsi_set_cmdq_Vzsp(0x00, p_data, 1);

	//Init_SSD2805_Write_15P();	 
       p_data[0] = 0x00; p_data[1]=0x00;p_data[2] = 0x00; p_data[3]=0x00;
       p_data[4] = 0x00; p_data[5]=0x00;p_data[6] = 0x00; p_data[7]=0x00;
       p_data[8] = 0x00; p_data[9]=0x00;p_data[10] = 0x00; 
       p_data[11] = 0x26; p_data[12]=0x0A;p_data[13] = 0x0C; p_data[14] = 0x02;
	dsi_set_cmdq_Vzsp(0xcc, p_data,15);		
// ccax	 
       //Init_SSD2805_Write_1P();
       p_data[0] = 0xa0;
	dsi_set_cmdq_Vzsp(0x00, p_data, 1);

	//Init_SSD2805_Write_15P();
       p_data[0] = 0x25; p_data[1]=0x00;p_data[2] = 0x00; p_data[3]=0x00;
       p_data[4] = 0x00; p_data[5]=0x00;p_data[6] = 0x00; p_data[7]=0x00;
       p_data[8] = 0x00; p_data[9]=0x00;p_data[10] = 0x00; p_data[11]=0x00;
       p_data[12] = 0x00; p_data[13]=0x00;p_data[14] = 0x00;
	dsi_set_cmdq_Vzsp(0xcc, p_data,15);			
// ccbx
       //Init_SSD2805_Write_1P();
       p_data[0] = 0xb0;
	dsi_set_cmdq_Vzsp(0x00, p_data, 1);
	//Init_SSD2805_Write_10P();
       p_data[0]=0x00;p_data[1] = 0x25; p_data[2]=0x0C;
       p_data[3] = 0x0A; p_data[4]=0x02;p_data[5] = 0x26; p_data[6]=0x00;
       p_data[7] = 0x00; p_data[8]=0x00;p_data[9] = 0x00; 
	dsi_set_cmdq_Vzsp(0xcc, p_data,10);		
// cccx
       //Init_SSD2805_Write_1P();
       p_data[0] = 0xc0;
	dsi_set_cmdq_Vzsp(0x00, p_data, 1);

	//Init_SSD2805_Write_15P();
       p_data[0] = 0x00; p_data[1]=0x00;p_data[2] = 0x00; p_data[3]=0x00;
       p_data[4] = 0x00; p_data[5]=0x00;p_data[6] = 0x00; p_data[7]=0x00;
       p_data[8] = 0x00; p_data[9]=0x00;p_data[10] = 0x00; 
       p_data[11] = 0x25; p_data[12]=0x0B;p_data[13] = 0x09; p_data[14] = 0x01;
	dsi_set_cmdq_Vzsp(0xcc, p_data,15);		
// ccdx
       //Init_SSD2805_Write_1P();
       p_data[0] =0xd0;
	dsi_set_cmdq_Vzsp(0x00, p_data,1);

	//Init_SSD2805_Write_15P();
       p_data[0] = 0x26; p_data[1]=0x00;p_data[2] = 0x00; p_data[3]=0x00;
       p_data[4] = 0x00; p_data[5]=0x00;p_data[6] = 0x00; p_data[7]=0x00;
       p_data[8] = 0x00; p_data[9]=0x00;p_data[10] = 0x00; p_data[11]=0x00;
       p_data[12] = 0x00; p_data[13]=0x00;p_data[14] = 0x00; 
	dsi_set_cmdq_Vzsp(0xcc, p_data,15);			

       MDELAY(50);
        

	
	//Init_SSD2805_Write_1P();
       p_data[0] = 0xC7;
	dsi_set_cmdq_Vzsp(0x00, p_data, 1);

	//Init_SSD2805_Write_1P();
       p_data[0] = 0x02;
	dsi_set_cmdq_Vzsp(0xcf, p_data, 1);

	//Init_SSD2805_Write_1P();
       p_data[0] =0x81;
	dsi_set_cmdq_Vzsp(0x00, p_data, 1);	

	//Init_SSD2805_Write_1P();
       p_data[0] = 0x66;//0x77; //zhaoshaopeng for te
	dsi_set_cmdq_Vzsp(0xC1, p_data, 1);		
	
	//Init_SSD2805_Write_1P();
       p_data[0] = 0x77;
	dsi_set_cmdq_Vzsp(0x3A, p_data, 1);	

       p_data[0] = 0xd0;//zhaoshaopeng from 0xc0
	dsi_set_cmdq_Vzsp(0x36, p_data, 1);//flip and mirror		


	// ENABLE FMARK
	//p_data[0] = 0x03;p_data[1] = 0x20;//800
	//p_data[0] = 0x01;p_data[1] = 0x90;
	p_data[0] = 0x01;p_data[1] = 0xff;
	dsi_set_cmdq_Vzsp(0x44, p_data, 2);
	p_data[0]=0x00;
	dsi_set_cmdq_Vzsp(0x35, p_data, 1);	

      // p_data[0]=0x00;
	//dsi_set_cmdq_Vzsp(0x34, p_data, 0);	
	//Init_SSD2805_Write_4P();
       p_data[0]=0x00;p_data[1] = 0x00; p_data[2]=0x01;
       p_data[3] = 0xDF; 
	dsi_set_cmdq_Vzsp(0x2A, p_data,4);			
	
	
	//Init_SSD2805_Write_4P();
       p_data[0]=0x00;p_data[1] = 0x00; p_data[2]=0x03;
       p_data[3] = 0x1F; 
	dsi_set_cmdq_Vzsp(0x2B, p_data,4);		

       //Init_SSD2805_Write_0P();
       p_data[0] = 0x00;
	dsi_set_cmdq_Vzsp(0x11, p_data, 0);		   
	MDELAY(150);
	
	
   
   	//Init_SSD2805_Write_0P();
	p_data[0] = 0x00;
	dsi_set_cmdq_Vzsp(0x29, p_data, 0);	
	
	MDELAY(50);
	
	
	p_data[0] = 0x00;
	dsi_set_cmdq_Vzsp(0x2c, p_data, 0);
	
	MDELAY(50);   
       //kfree(p_data);
	
	}
	
	
#if 0
static struct LCM_setting_table lcm_initialization_setting[] = {
		
	/*
	Note :

	Data ID will depends on the following rule.
	
		count of parameters > 1	=> Data ID = 0x39
		count of parameters = 1	=> Data ID = 0x15
		count of parameters = 0	=> Data ID = 0x05

	Structure Format :

	{DCS command, count of parameters, {parameter list}}
	{REGFLAG_DELAY, milliseconds of time, {}},

	...

	Setting ending by predefined flag
	
	{REGFLAG_END_OF_TABLE, 0x00, {}}
	*/

	
	//TIANMA 4.46
	{0x00,	1,	{0x00}},
	{0xFF,	3,	{0x80, 0x09, 0x01}},
	{REGFLAG_DELAY, 10, {}},
	
	{0x00,	1,	{0x80}},
	{0xFF,	2,	{0x80, 0x09}},	
	{REGFLAG_DELAY, 10, {}},	
	
	//{0x00,	1,	{0x03}},
	//{0xff,	1,	{0x01}},
/*
	{0x00,	1,	{0x80}},
	{0xf5,	12,	{0x01,0x18,0x02,0x18,0x10,0x18,0x02,0x18,0x0e,0x18,0x0f,0x20}},
	{0x00,	1,	{0x90}},
	{0xf5,	10,	{0x02,0x18,0x08,0x18,0x06,0x18,0x0d,0x18,0x0b,0x18}},
	{0x00,	1,	{0xa0}},
	{0xf5,	8,	{0xf5,0x10,0x18,0x01,0x18,0x18,0x14,0x14,0x18}},
	{0x00,	1,	{0xb0}},
	{0xf5,	12,	{0x14,0x18,0x12,0x18,0x13,0x18,0x11,0x18,0x13,0x18,0x00,0x00}},
*/	
	{0x00,	1,	{0xb4}},
	{0xc0,	1,	{0x50}},
	{0x00,	1,	{0xa3}},
	{0xc0,	1,	{0x1b}},
	{0x00,	1,	{0x82}},
	{0xc5,	1,	{0xa3}},
	{0x00,	1,	{0x90}},
	{0xc5,	2,	{0xd6,0x76}},
	{0x00,	1,	{0x00}},
	{0xd8,	2,	{0x95,0x95}},
	{0x00,	1,	{0x00}},
	{0xd9,	1,	{0x6c}},
	{0x00,	1,	{0x81}},
	{0xc1,	1,	{0x66}},
	{0x00,	1,	{0xa1}},
	{0xc1,	1,	{0x08}},
	{0x00,	1,	{0xa3}},
	{0xc0,	1,	{0x1b}},
	{0x00,	1,	{0x81}},
	{0xc4,	1,	{0x83}},
	{0x00,	1,	{0x92}},
	{0xc5,	1,	{0x01}},
	{0x00,	1,	{0xb1}},
	{0xc5,	1,	{0xa9}},
	{0x00,	1,	{0x00}},
	
	{REGFLAG_DELAY, 10, {}},	
		
	{0xe1,	16,	{0x00,0x0b,0x0f,0x10,0x09,0x12,0x0a,0x06,0x07,0x07,0x0a,0x05,0x1d,0x17,0x0d,0x06}},		
	{0x00,	1,	{0x00}},
	{0xe2,	16,	{0x00,0x0b,0x0f,0x10,0x0a,0x11,0x0e,0x0e,0x01,0x05,0x0c,0x0a,0x13,0x0d,0x09,0x01}},		
	{0x00,	1,	{0x80}},
	{0xce,	6,	{0x84,0x03,0x00,0x83,0x03,0x00}},
	{0x00,	1,	{0x90}},
	{0xce,	6,	{0x33,0x27,0x00,0x33,0x28,0x00}},
	{0x00,	1,	{0xa0}},
	{0xce,	14,	{0x38,0x02,0x03,0x21,0x00,0x00,0x00,0x38,0x01,0x03,0x22,0x00,0x00,0x00}},
	{0x00,	1,	{0xb0}},
	{0xce,	14,	{0x38,0x00,0x03,0x23,0x00,0x00,0x00,0x30,0x00,0x03,0x24,0x00,0x00,0x00}},
	{0x00,	1,	{0xc0}},
	{0xce,	14,	{0x30,0x01,0x03,0x25,0x00,0x00,0x00,0x30,0x02,0x03,0x26,0x00,0x00,0x00}},
	{0x00,	1,	{0xd0}},
	{0xce,	14,	{0x30,0x03,0x03,0x27,0x00,0x00,0x00,0x30,0x04,0x03,0x28,0x00,0x00,0x00}},
	
	{0x00,	1,	{0xc7}},
	{0xcf,	1,	{0x00}},
	{0x00,	1,	{0xc0}},
	{0xcb,	15,	{0x00,0x00,0x00,0x00,0x54,0x54,0x54,0x54,0x00,0x54,0x00,0x54,0x00,0x00,0x00}},
	{0x00,	1,	{0xd0}},
	{0xcb,	15,	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x54,0x54,0x54,0x54,0x00,0x54}},
	{0x00,	1,	{0xe0}},
	{0xcb,	9,	{0x00,0x54,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

	{0x00,	1,	{0x80}},
	{0xcc,	10,	{0x00,0x00,0x00,0x00,0x0c,0x0a,0x10,0x0e,0x00,0x02}},
	{0x00,	1,	{0x90}},
	{0xcc,	15,	{0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0b}},
	{0x00,	1,	{0xa0}},
	{0xcc,	15,	{0x09,0x0f,0x0d,0x00,0x01,0x00,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
	{0x00,	1,	{0xb0}},
	{0xcc,	10,	{0x00,0x00,0x00,0x00,0x0d,0x0f,0x09,0x0b,0x00,0x05}},
	{0x00,	1,	{0xc0}},
	{0xcc,	15,	{0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0e}},
	{0x00,	1,	{0xd0}},
	{0xcc,	15,	{0x10,0x0a,0x0c,0x00,0x06,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

	{0x00,	1,	{0x00}},
	{0xFF,	3,	{0xff, 0xff, 0xff}},
	
	{0x00,	1,	{0x00}},
	{0x3a,	1,	{0x77}},
	{REGFLAG_DELAY, 10, {}},	
	
	{0x11, 1, {0x00}},
	{REGFLAG_DELAY, 25, {}},
	// Display ON
	{0x29, 1, {0x00}},
	//{REGFLAG_DELAY, 100, {}},
	
/*
	//backlight start
	{0x51,	1,	{0x00}},
	{REGFLAG_DELAY, 1, {}},

	{0x55,	1,	{0x02}},
	{REGFLAG_DELAY, 1, {}},
*/
	// ENABLE FMARK
	{0x44,	2,	{((FRAME_HEIGHT/2)>>8), ((FRAME_HEIGHT/2)&0xFF)}},
	{REGFLAG_DELAY, 1, {}},

	{0x35,	1,	{0x00}},
	{REGFLAG_DELAY, 1, {}},
/*
	{0x53,	1,	{0x2C}},
	{REGFLAG_DELAY, 1, {}},
	//backlight end
*/

	// Setting ending by predefined flag
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
#else//zhaoshaopeng for i8_sp ips
static struct LCM_setting_table lcm_initialization_setting[] = {
		
	/*
	Note :

	Data ID will depends on the following rule.
	
		count of parameters > 1	=> Data ID = 0x39
		count of parameters = 1	=> Data ID = 0x15
		count of parameters = 0	=> Data ID = 0x05

	Structure Format :

	{DCS command, count of parameters, {parameter list}}
	{REGFLAG_DELAY, milliseconds of time, {}},

	...

	Setting ending by predefined flag
	
	{REGFLAG_END_OF_TABLE, 0x00, {}}
	*/

	
	//TIANMA 4.46
	{0x00,	1,	{0x00}},
	{0xFF,	3,	{0x80, 0x09, 0x01}},
	{REGFLAG_DELAY, 10, {}},
	
	{0x00,	1,	{0x80}},
	{0xFF,	2,	{0x80, 0x09}},	
	{REGFLAG_DELAY, 10, {}},	
	
	{0x00,	1,	{0x03}},
	{0xff,	1,	{0x01}},
	{0x00,	1,	{0xa3}},
	{0xc0,	1,	{0x1b}},
	
/*
	{0x00,	1,	{0x80}},
	{0xf5,	12,	{0x01,0x18,0x02,0x18,0x10,0x18,0x02,0x18,0x0e,0x18,0x0f,0x20}},
	{0x00,	1,	{0x90}},
	{0xf5,	10,	{0x02,0x18,0x08,0x18,0x06,0x18,0x0d,0x18,0x0b,0x18}},
	{0x00,	1,	{0xa0}},
	{0xf5,	8,	{0xf5,0x10,0x18,0x01,0x18,0x18,0x14,0x14,0x18}},
	{0x00,	1,	{0xb0}},
	{0xf5,	12,	{0x14,0x18,0x12,0x18,0x13,0x18,0x11,0x18,0x13,0x18,0x00,0x00}},
*/	
	{0x00,	1,	{0xb4}},
	{0xc0,	1,	{0x50}},
	
	{0x00,	1,	{0x81}},
	{0xc1,	1,	{0x66}},
	
	{0x00,	1,	{0xa1}},
	{0xc1,	1,	{0x0e}},
	
	{0x00,	1,	{0x81}},
	{0xc4,	1,	{0x83}},
	
	{0x00,	1,	{0x82}},
	{0xc5,	1,	{0x83}},
	
	{0x00,	1,	{0x90}},
	{0xc5,	3,	{0x96,0x2B,0x01}},
	
	{0x00,	1,	{0x94}},
	{0xc5,	2,	{0x33, 0x34}},
	
	{0x00,	1,	{0xb1}},
	{0xc5,	1,	{0xa9}},
	
	{0x00,	1,	{0x80}},
	{0xce,	12,	{0x86,0x01,0x00,0x85,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
	
	{0x00,	1,	{0xa0}},
	{0xce,	14,	{0x18,0x04,0x03,0x21,0x00,0x00,0x00,0x18,0x03,0x03,0x22,0x00,0x00,0x00}},

	{0x00,	1,	{0xb0}},
	{0xce,	14,	{0x18,0x02,0x03,0x23,0x00,0x00,0x00,0x18,0x01,0x03,0x24,0x00,0x00,0x00}},

	{0x00,	1,	{0xc0}},
	{0xcf,	10,	{0x01,0x01,0x20,0x20,0x00,0x00,0x01,0x00,0x00,0x00}},
	
	{0x00,	1,	{0xd0}},
	{0xcf,	1,	{0x00}},
	
	{0x00,	1,	{0xc0}},
	{0xcb,	15,	{0x00,0x04,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

	{0x00,	1,	{0xd0}},
	{0xcb,	15,	{0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00}},

	{0x00,	1,	{0xe0}},
	{0xcb,	10,	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
	
	{0x00,	1,	{0x80}},
	{0xcc,	10,	{0x00,0x26,0x09,0x0B,0x01,0x25,0x00,0x00,0x00,0x00}},
	
	{0x00,	1,	{0x90}},
	{0xcc,	15,	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x26,0x0A,0x0C,0x02}},
	
	{0x00,	1,	{0xa0}},
	{0xcc,	15,	{0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
	
	{0x00,	1,	{0xb0}},
	{0xcc,	10,	{0x00,0x25,0x0C,0x0A,0x02,0x26,0x00,0x00,0x00,0x00}},
	
	{0x00,	1,	{0xc0}},
	{0xcc,	15,	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x0B,0x09,0x01}},
	
	{0x00,	1,	{0xd0}},
	{0xcc,	15,	{0x26,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
	
	{0x00,	1,	{0x00}},
	{0xd8,	2,	{0x79,0x79}},
	
	{0x00,	1,	{0x00}},
	{0xd9,	1,	{0x4f}},
	
	{0x00,	1,	{0x00}},
	{0xe1,	16,	{0x02,0x0A,0x10,0x0D,0x06,0x0E,0x0B,0x0A,0x03,0x06,0x0E,0x09,0x10,0x11,0x0B,0x03}},
	
	{0x00,	1,	{0x00}},
	{0xe2,	16,	{0x02,0x0A,0x10,0x0D,0x06,0x0E,0x0B,0x0A,0x04,0x07,0x0F,0x0A,0x10,0x11,0x0B,0x03}},

	{0x00,	1,	{0x00}},
	{0xff,	3,	{0xFF,0xFF,0xFF}},
	
	{0x00,	1,	{0x00}},
	{0x3a,	1,	{0x77}},

	{REGFLAG_DELAY, 10, {}},	
	
	{0x00,	1,	{0x00}},
	{0x11, 1, {0x00}},
	{REGFLAG_DELAY, 25, {}},
	// Display ON
	{0x00,	1,	{0x00}},	
	{0x29, 1, {0x00}},
	//{REGFLAG_DELAY, 100, {}},
	
/*
	//backlight start
	{0x51,	1,	{0x00}},
	{REGFLAG_DELAY, 1, {}},

	{0x55,	1,	{0x02}},
	{REGFLAG_DELAY, 1, {}},
*/
	{0x00,	1,	{0x00}},
	{0x35,	1,	{0x00}},
	{REGFLAG_DELAY, 1, {}},
/*
	{0x53,	1,	{0x2C}},
	{REGFLAG_DELAY, 1, {}},
	//backlight end
*/

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
	{0x00,	1,	{0x00}},
	{0xFF,	3,	{0x80, 0x09, 0x01}},
	{REGFLAG_DELAY, 10, {}},
	
	{0x00,	1,	{0x80}},
	{0xFF,	2,	{0x80, 0x09}},	
	{REGFLAG_DELAY, 10, {}},	

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
		params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

#if (LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
#else
		params->dsi.mode   = SYNC_PULSE_VDO_MODE;
#endif
	
		// DSI
		/* Command mode setting */
		params->dsi.LANE_NUM				= LCM_TWO_LANE;
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
		params->dsi.line_byte=2048;		// 2256 = 752*3
		params->dsi.horizontal_sync_active_byte=26;
		params->dsi.horizontal_backporch_byte=146;
		params->dsi.horizontal_frontporch_byte=146;	
		params->dsi.rgb_byte=(480*3+6);	
	
		params->dsi.horizontal_sync_active_word_count=20;	
		params->dsi.horizontal_backporch_word_count=140;
		params->dsi.horizontal_frontporch_word_count=140;

		// Bit rate calculation
		params->dsi.pll_div1=38;		// fref=26MHz, fvco=fref*(div1+1)	(div1=0~63, fvco=500MHZ~1GHz)//zhaoshaopeng from 38
		params->dsi.pll_div2=1;			// div2=0~15: fout=fvo/(2*div2)

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
	push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);

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
    unsigned char buffer_1[12];
    unsigned int array[16];

        if(lcm_esd_test)
        {
            lcm_esd_test = FALSE;
            return TRUE;
        }
    array[0] = 0x00013700;
    dsi_set_cmdq(array, 1, 1);
    read_reg_v2(0x0a, buffer_1, 1);
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

        if(buffer_1[0] == 0x9c)
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
    MDELAY(20);
    SET_RESET_PIN(1);
    MDELAY(120);
    push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
    MDELAY(1);
    //push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
    //MDELAY(1);
    //dsi_set_cmdq_V2(0x35, 1, &para, 1);     ///enable TE
    //MDELAY(1);

    return TRUE;
}

static unsigned int lcm_compare_id(void)
{
	volatile unsigned int id=0;
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

	return (LCM_ID_OTM8009A ==id)?1:0;

	//return ((lcd_id == 1)&&(LCM_ID_OTM8009A == id))?1:0;	

}
// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER otm8009a_lead_lcm_drv = 
{
    .name			= "otm8009a_tm",
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
	.esd_check   = lcm_esd_check,
       .esd_recover   = lcm_esd_recover,
	.compare_id    = lcm_compare_id,
#endif
};
