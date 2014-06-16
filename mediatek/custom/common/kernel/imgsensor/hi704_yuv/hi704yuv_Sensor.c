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
/*****************************************************************************
 *
 * Filename:
 * ---------
 *   sensor.c
 *
 * Project:
 * --------
 *   DUMA
 *
 * Description:
 * ------------
 *   Source code of Sensor driver
 *
 *
 * Author:
 * -------
 *   PC Huang (MTK02204)
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by CC/CQ. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision:$
 * $Modtime:$
 * $Log:$
 *
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by CC/CQ. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <asm/atomic.h>
//#include <mach/mt6516_pll.h>

#include "kd_camera_hw.h"
#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_imgsensor_errcode.h"
#include "kd_camera_feature.h"

#include "hi704yuv_Sensor.h"
#include "hi704yuv_Camera_Sensor_para.h"
#include "hi704yuv_CameraCustomized.h"

#define HI704YUV_DEBUG
#ifdef HI704YUV_DEBUG
#define SENSORDB printk
#else
#define SENSORDB(x,...)
#endif


#if 0
extern int iReadReg(u16 a_u2Addr , u8 * a_puBuff , u16 i2cId);
extern int iWriteReg(u16 a_u2Addr , u32 a_u4Data , u32 a_u4Bytes , u16 i2cId);
static int sensor_id_fail = 0; 
#define HI704_write_cmos_sensor(addr, para) iWriteReg((u16) addr , (u32) para ,1,HI704_WRITE_ID)
#define HI704_write_cmos_sensor_2(addr, para, bytes) iWriteReg((u16) addr , (u32) para ,bytes,HI704_WRITE_ID)
kal_uint16 HI704_read_cmos_sensor(kal_uint32 addr)
{
kal_uint16 get_byte=0;
    iReadReg((u16) addr ,(u8*)&get_byte,HI704_WRITE_ID);
    return get_byte;
}
#endif
 
#ifndef CAMERA_SW_SCCB
extern int iReadRegI2C(u8 *a_pSendData , u16 a_sizeSendData, u8 * a_pRecvData, u16 a_sizeRecvData, u16 i2cId);
extern int iWriteRegI2C(u8 *a_pSendData , u16 a_sizeSendData, u16 i2cId);
#else
kal_uint16 SWSccbDelay = 38;

#define MAX_SW_SCCB_FAIL_TRY_NO                3
#define CAMERA_SET_SCCB_CLK_OUTPUT             mt_set_gpio_dir(GPIO_I2C1_SCA_PIN, GPIO_DIR_OUT);
#define CAMERA_SET_SCCB_DATA_OUTPUT            mt_set_gpio_dir(GPIO_I2C1_SDA_PIN, GPIO_DIR_OUT);
#define CAMERA_SET_SCCB_CLK_INPUT              mt_set_gpio_dir(GPIO_I2C1_SCA_PIN, GPIO_DIR_IN);
#define CAMERA_SET_SCCB_DATA_INPUT             mt_set_gpio_dir(GPIO_I2C1_SDA_PIN, GPIO_DIR_IN);
#define CAMERA_SET_SCCB_CLK_HIGH               mt_set_gpio_out(GPIO_I2C1_SCA_PIN, 1);
#define CAMERA_SET_SCCB_CLK_LOW                mt_set_gpio_out(GPIO_I2C1_SCA_PIN, 0);
#define CAMERA_SET_SCCB_DATA_HIGH              mt_set_gpio_out(GPIO_I2C1_SDA_PIN, 1);
#define CAMERA_SET_SCCB_DATA_LOW               mt_set_gpio_out(GPIO_I2C1_SDA_PIN, 0);
#define CAMERA_GET_SCCB_DATA_BIT               mt_get_gpio_in(GPIO_I2C1_SDA_PIN)

static void CameraSWSccbStartTransmission(void)
{
  volatile kal_uint16 j;
  
  CAMERA_SET_SCCB_CLK_OUTPUT;
  CAMERA_SET_SCCB_DATA_OUTPUT;
  CAMERA_SET_SCCB_DATA_HIGH;
  for (j = SWSccbDelay; j > 0; j--);
  CAMERA_SET_SCCB_CLK_HIGH;
  for (j = SWSccbDelay; j > 0; j--);
  CAMERA_SET_SCCB_DATA_LOW;
  for (j = SWSccbDelay; j > 0; j--);
  CAMERA_SET_SCCB_CLK_LOW;
  for (j = SWSccbDelay; j > 0; j--);
}

static void CameraSWSccbStopTransmission(void)
{ 
  volatile kal_uint16 j;
 
  CAMERA_SET_SCCB_CLK_LOW;
  CAMERA_SET_SCCB_DATA_LOW;
  for (j = SWSccbDelay; j > 0; j--);
  CAMERA_SET_SCCB_CLK_HIGH;
  for (j = SWSccbDelay; j > 0; j--);
  CAMERA_SET_SCCB_DATA_HIGH;
}

static kal_uint8 CameraSWSccbSendByte(kal_uint16 SendByte, kal_uint8 Size)
{
  volatile kal_uint16 j;
  kal_uint8 ack = 0;
  register kal_int8 i;

  while (Size--)
  {
    for (i = 7; i >= 1; i--)
    {
      if (SendByte&(1 << (i + (Size << 3))))
      {
        CAMERA_SET_SCCB_DATA_HIGH;
      }
      else
      {
        CAMERA_SET_SCCB_DATA_LOW;
      }
      for (j = SWSccbDelay; j > 0; j--);
      CAMERA_SET_SCCB_CLK_HIGH;
      for (j = SWSccbDelay; j > 0; j--);
      CAMERA_SET_SCCB_DATA_OUTPUT; /* just for delay */
      for (j = SWSccbDelay; j > 0; j--);
      CAMERA_SET_SCCB_CLK_LOW;
      for (j = SWSccbDelay; j > 0; j--);
    }

    {
        if (SendByte&(1 << (i + (Size << 3))))
        {
            CAMERA_SET_SCCB_DATA_HIGH;
        }
        else
        {
            CAMERA_SET_SCCB_DATA_LOW;
        }
          
        for (j = SWSccbDelay; j > 0; j--);
        CAMERA_SET_SCCB_CLK_HIGH;
        for (j = SWSccbDelay; j > 0; j--);
        CAMERA_SET_SCCB_DATA_OUTPUT; /* just for delay */
        for (j = SWSccbDelay; j > 0; j--);
        CAMERA_SET_SCCB_CLK_LOW;
       // for (j = SWSccbDelay; j > 0; j--);
    }
    
    CAMERA_SET_SCCB_DATA_INPUT;
    for (j = SWSccbDelay; j > 0; j--);
    CAMERA_SET_SCCB_CLK_HIGH;
    for (j = SWSccbDelay; j > 0; j--);
    ack |= CAMERA_GET_SCCB_DATA_BIT;
    for (j = SWSccbDelay; j > 0; j--);
    CAMERA_SET_SCCB_CLK_LOW;
    for (j = SWSccbDelay; j > 0; j--);
    CAMERA_SET_SCCB_DATA_OUTPUT;
  }
  return ack;
}

static kal_uint16 CameraSWSccbGetBytes(kal_uint8 Size)
{
  volatile kal_uint16 j;
  register kal_uint16 get_byte = 0;
  register kal_int8 i;
  
  while (Size--)
  {
    for (i = 7; i >= 1; i--)
    {
      CAMERA_SET_SCCB_DATA_INPUT;
      for (j = SWSccbDelay; j > 0; j--);
      CAMERA_SET_SCCB_CLK_HIGH;
      for (j = SWSccbDelay; j > 0; j--);
      get_byte <<= 1;
      if (CAMERA_GET_SCCB_DATA_BIT)
      {
        get_byte |= 1;
      }
      for (j = SWSccbDelay; j > 0; j--);
      CAMERA_SET_SCCB_CLK_LOW;
      for (j = SWSccbDelay; j > 0; j--);
    }

    {
        CAMERA_SET_SCCB_DATA_INPUT;
        for (j = SWSccbDelay; j > 0; j--);
        CAMERA_SET_SCCB_CLK_HIGH;
        for (j = SWSccbDelay; j > 0; j--);
        get_byte <<= 1; 
        if(CAMERA_GET_SCCB_DATA_BIT)
        {
            get_byte |= 1;
        }
        for (j = SWSccbDelay; j > 0; j--);
        CAMERA_SET_SCCB_CLK_LOW;
    }
    
    if (Size)
    {
      CAMERA_SET_SCCB_DATA_LOW; /* ACK */
    }
    else
    {
      CAMERA_SET_SCCB_DATA_HIGH; /* NACK */
    }
    
    CAMERA_SET_SCCB_DATA_OUTPUT;
    for (j = SWSccbDelay; j > 0; j--);
    CAMERA_SET_SCCB_CLK_HIGH;
    for (j = SWSccbDelay; j > 0; j--);
    CAMERA_SET_SCCB_DATA_OUTPUT; /* just for delay */
    for (j = SWSccbDelay; j > 0; j--);
    CAMERA_SET_SCCB_CLK_LOW;
    for (j = SWSccbDelay; j > 0; j--);
  }
  return get_byte;
}
#endif

kal_uint16 HI704_write_cmos_sensor(kal_uint8 addr, kal_uint8 para)
{
#ifndef CAMERA_SW_SCCB
    char puSendCmd[2] = {(char)(addr & 0xFF) , (char)(para & 0xFF)};

	  iWriteRegI2C(puSendCmd , 2,HI704_WRITE_ID);
#else
		kal_uint8 FailTryNo = MAX_SW_SCCB_FAIL_TRY_NO + 1;
    
    while (--FailTryNo > 0)
    {
      CameraSWSccbStartTransmission();
      if (CameraSWSccbSendByte(HI704_WRITE_ID, 1)) continue;
      if (CameraSWSccbSendByte(addr, 1)) continue;
      if (CameraSWSccbSendByte(para, 1)) continue;
      break;
    }
    CameraSWSccbStopTransmission();
    if (!FailTryNo) 
    {
      printk("SWI2C write %x, %x fail",addr, para);
    }
#endif
}
kal_uint16 HI704_read_cmos_sensor(kal_uint8 addr)
{
	kal_uint16 get_byte=0;
#ifndef CAMERA_SW_SCCB
  char puSendCmd = { (char)(addr & 0xFF) };
	iReadRegI2C(&puSendCmd , 1, (u8*)&get_byte,1,HI704_WRITE_ID);
#else
	kal_uint8 FailTryNo = MAX_SW_SCCB_FAIL_TRY_NO + 1;
    
	while (--FailTryNo > 0)
	{
      CameraSWSccbStartTransmission();
      if (CameraSWSccbSendByte(HI704_WRITE_ID, 1)) continue;
      if (CameraSWSccbSendByte(addr, 1)) continue;
     // if (!Info->SccbWREn) CameraSWSccbStopTransmission();
      CameraSWSccbStartTransmission();
      if (CameraSWSccbSendByte(HI704_READ_ID, 1)) continue;
      get_byte = CameraSWSccbGetBytes(1);
      break;
	}
	CameraSWSccbStopTransmission();
	if (!FailTryNo) 
 	{
		printk("SWI2C read %x fail", addr);
	}
#endif
	return get_byte;
}


/*******************************************************************************
* // Adapter for Winmo typedef 
********************************************************************************/
#define WINMO_USE 0

#define Sleep(ms) mdelay(ms)
#define RETAILMSG(x,...)
#define TEXT
kal_uint8 HI704_Sleep_Mode;

/*******************************************************************************
* follow is define by jun
********************************************************************************/
MSDK_SENSOR_CONFIG_STRUCT HI704SensorConfigData;

static struct HI704_sensor_STRUCT HI704_sensor;
static kal_uint32 HI704_zoom_factor = 0; 
static int sensor_id_fail = 0;	
static kal_uint32 Hi704_cp_exposure_lines=0x00afc8;
static int write_shutter_flag = 0;
static kal_bool Hi704_video_reset = KAL_FALSE;

const HI704_SENSOR_INIT_INFO HI704_Initial_Setting_Info[] =
{
  
    //PAGE 0
    //Image Size/Windowing/HSYNC/VSYNC[Type1]
    {0x03, 0x00},   //PAGEMODE(0x03)
    {0x01, 0xf1},
    {0x01, 0xf3},   //PWRCTL(0x01[P0])Bit[1]:Software Reset.
    {0x01, 0xf1},

  {0x11, 0x90},   //For No Fixed Framerate Bit[2]
    {0x12, 0x04},    //0x00
        
    {0x20, 0x00},
    {0x21, 0x04},
    {0x22, 0x00},
    {0x23, 0x04},

	{0x24, 0x01},
    {0x25, 0xe0},
    {0x26, 0x02},
    {0x27, 0x80},

	{0x40, 0x01},   //HBLANK: 0x70 = 112
    {0x41, 0x58},
    {0x42, 0x00},   //VBLANK: 0x40 = 64
    {0x43, 0x14},   //0x04 -> 0x40: For Max Framerate = 30fps
            
    //BLC
    {0x80, 0x2e},
    {0x81, 0x7e},
    {0x82, 0x90},
    {0x83, 0x30},
    {0x84, 0x2c},
    {0x85, 0x4b},
    {0x89, 0x48},
        
    {0x90, 0x0b},
    {0x91, 0x0b},    
    {0x92, 0x48},
    {0x93, 0x48},
    {0x98, 0x38},
    {0x99, 0x40},
    {0xa0, 0x00},
    {0xa8, 0x40},
    
    //PAGE 2
    //Analog Circuit
    {0x03, 0x02},      
    {0x13, 0x40},
    {0x14, 0x04},
    {0x1a, 0x00},
    {0x1b, 0x08},
        
    {0x20, 0x33},
    {0x21, 0xaa},
    {0x22, 0xa7},
    {0x23, 0x32},       //For Sun Pot b1->32 colin 20110627
        
    {0x3b, 0x48},
        
    {0x50, 0x21},
    {0x52, 0xa2},
    {0x53, 0x0a},
    {0x54, 0x30},
    {0x55, 0x10},
    {0x56, 0x0c},
    {0x59, 0x0F},
        
    {0x60, 0xca}, // 54-ca colin 20110627
    {0x61, 0xdb},
    {0x62, 0xca},
    {0x63, 0xda},
    {0x64, 0xca},
    {0x65, 0xda},
    {0x72, 0xcb},
    {0x73, 0xd8},
    {0x74, 0xcb},
    {0x75, 0xd8},
    {0x80, 0x02},
    {0x81, 0xbd},
    {0x82, 0x24},
    {0x83, 0x3e},
    {0x84, 0x24},
    {0x85, 0x3e},
    {0x92, 0x72},
    {0x93, 0x8c},
    {0x94, 0x72},
    {0x95, 0x8c},
    {0xa0, 0x03},
    {0xa1, 0xbb},
    {0xa4, 0xbb},
    {0xa5, 0x03},
    {0xa8, 0x44},
    {0xa9, 0x6a},
    {0xaa, 0x92},
    {0xab, 0xb7},
    {0xb8, 0xc9},
    {0xb9, 0xd0},
    {0xbc, 0x20},
    {0xbd, 0x28},
    {0xc0, 0xde},
    {0xc1, 0xec},
    {0xc2, 0xde},
    {0xc3, 0xec},
    {0xc4, 0xe0},
    {0xc5, 0xea},
    {0xc6, 0xe0},
    {0xc7, 0xea},
    {0xc8, 0xe1},
    {0xc9, 0xe8},
    {0xca, 0xe1},
    {0xcb, 0xe8},
    {0xcc, 0xe2},
    {0xcd, 0xe7},
    {0xce, 0xe2},
    {0xcf, 0xe7},
    {0xd0, 0xc8},
    {0xd1, 0xef},
     
    //PAGE 10
    //Image Format, Image Effect
    {0x03, 0x10},
    {0x10, 0x03},
    {0x11, 0x43},
    {0x12, 0x30},
        
    {0x40, 0x08},
    {0x41, 0x00},
    {0x48, 0x88},
        
    {0x50, 0x42},
           
    {0x60, 0x7f},
    {0x61, 0x00},
    {0x62, 0x8e},// sat blue
    {0x63, 0x8e},//sat red
    {0x64, 0x51},
    {0x66, 0x90},
    {0x67, 0x36}, //42
    
    //PAGE 11
    //Z-LPF
    {0x03, 0x11},
    {0x10, 0x25},   
    {0x11, 0x1f},   
        
    {0x20, 0x00},   
    {0x21, 0x38},   
    {0x23, 0x0a},
        
    {0x60, 0x10},   
    {0x61, 0x82},
    {0x62, 0x00},   
    {0x63, 0x83},   
    {0x64, 0x83},      
    {0x67, 0xF0},   
    {0x68, 0x30},   
    {0x69, 0x10},   
    
    //PAGE 12
    //2D
    {0x03, 0x12},
        
    {0x40, 0xe9},
    {0x41, 0x09},
        
    {0x50, 0x18},
    {0x51, 0x24},
        
    {0x70, 0x1f},
    {0x71, 0x00},
    {0x72, 0x00},
    {0x73, 0x00},
    {0x74, 0x10},
    {0x75, 0x10},
    {0x76, 0x20},
    {0x77, 0x80},
    {0x78, 0x88},
    {0x79, 0x18},
        
    {0xb0, 0x7d},

    //PAGE 13
    //Edge Enhancement
    {0x03, 0x13},
    {0x10, 0x01},   
    {0x11, 0x89},   
    {0x12, 0x14},   
    {0x13, 0x19},   
    {0x14, 0x08},
        
    {0x20, 0x05},
    {0x21, 0x04},
    {0x23, 0x30},
    {0x24, 0x33},
    {0x25, 0x08},
    {0x26, 0x18},
    {0x27, 0x00},
    {0x28, 0x08},
    {0x29, 0x50},
    {0x2a, 0xe0},
    {0x2b, 0x10},
    {0x2c, 0x28},
    {0x2d, 0x40},
    {0x2e, 0x00},
    {0x2f, 0x00},
    {0x30, 0x11},
    {0x80, 0x03},
    {0x81, 0x07},
    {0x90, 0x05},
    {0x91, 0x04},
    {0x92, 0x00},
    {0x93, 0x20},
    {0x94, 0x42},
    {0x95, 0x60},

    //PAGE 11
    {0x30, 0x11},
        
    {0x80, 0x03},
    {0x81, 0x07},
        
    {0x90, 0x04},
    {0x91, 0x02},
    {0x92, 0x00},
    {0x93, 0x20},
    {0x94, 0x42},
    {0x95, 0x60},
    
    //PAGE 14
    //Lens Shading Correction
    {0x03, 0x14},
    {0x10, 0x01},
        
    {0x20, 0x80},   //For Y decay
    {0x21, 0x80},   //For Y decay
    {0x22, 0x78},
    {0x23, 0x4d},
    {0x24, 0x46},
    
    //PAGE 15 
    //Color Correction
    {0x03, 0x15}, 
    {0x10, 0x03},         
    {0x14, 0x3c},
    {0x16, 0x2c},
    {0x17, 0x2f},
          
    {0x30, 0xcb},
    {0x31, 0x61},
    {0x32, 0x16},
    {0x33, 0x23},
    {0x34, 0xce},
    {0x35, 0x2b},
    {0x36, 0x01},
    {0x37, 0x34},
    {0x38, 0x75},
           
    {0x40, 0x87},
    {0x41, 0x18},
    {0x42, 0x91},
    {0x43, 0x94},
    {0x44, 0x9f},
    {0x45, 0x33},
    {0x46, 0x00},
    {0x47, 0x94},
    {0x48, 0x14},
    
    //PAGE 16
    //Gamma Correction
    {0x03,  0x16},
        
   {0x30,0x00},
{0x31, 0x08},
{0x32, 0x12},
{0x33, 0x2b},
{0x34, 0x58},
{0x35, 0x79},
{0x36, 0x95},
{0x37, 0xa7},
{0x38, 0xb6},
{0x39, 0xc1},
{0x3a, 0xca},
{0x3b, 0xd9},
{0x3c, 0xe8},
{0x3d, 0xf5},
{0x3e, 0xff},
    
    //PAGE 17 
    //Auto Flicker Cancellation 
    {0x03, 0x17},
        
    {0xc4, 0x3c},
    {0xc5, 0x32},
    
    //PAGE 20 
    //AE 
    {0x03, 0x20},
        
    {0x10, 0x0c},
    {0x11, 0x04},
           
    {0x20, 0x01},
    {0x28, 0x27},
    {0x29, 0xa1},   
    {0x2a, 0xf0},
    {0x2b, 0x34},
    {0x2c, 0x2b},
           
    {0x30, 0xf8},
    {0x39, 0x22},
    {0x3a, 0xde},
    {0x3b, 0x22},
    {0x3c, 0xde},
    
    {0x60, 0xd5}, // 95
    {0x68, 0x3c},
    {0x69, 0x64},
    {0x6A, 0x28},
    {0x6B, 0xc8},
    
    {0x70, 0x48},   //For Y decay   
    {0x76, 0x22},
    {0x77, 0x02},   
    {0x78, 0x12},
    {0x79, 0x27},
    {0x7a, 0x23},  
    {0x7c, 0x1d},
    {0x7d, 0x22},
    
    {0x83, 0x00},
    {0x84, 0xaf},
    {0x85, 0xc8}, 
        
    {0x86, 0x00},
    {0x87, 0xfa},
        
    {0x88, 0x03}, // 7fps
    {0x89, 0x34},
    {0x8a, 0x50},    
        
    {0x8b, 0x3a},
    {0x8c, 0x98},  
        
    {0x8d, 0x30},
    {0x8e, 0xd4},
    
    {0x91, 0x02},
    {0x92, 0xdc},
    {0x93, 0x6c},   
    {0x94, 0x01},
    {0x95, 0xb7},
    {0x96, 0x74},   
    {0x98, 0x8C},
    {0x99, 0x23},  
        
    {0x9c, 0x06},   //For Y decay: Exposure Time
    {0x9d, 0xd6},   //For Y decay: Exposure Time
    {0x9e, 0x00},
    {0x9f, 0xfa},
    
    {0xb1, 0x14},
    {0xb2, 0x50}, //50
    {0xb4, 0x14},
    {0xb5, 0x38},
    {0xb6, 0x26},
    {0xb7, 0x20},
    {0xb8, 0x1d},
    {0xb9, 0x1b},
    {0xba, 0x1a},
    {0xbb, 0x19},
    {0xbc, 0x19},
    {0xbd, 0x18},
    
    {0xc0, 0x16},   //0x1a->0x16
    {0xc3, 0x48},
    {0xc4, 0x48}, 
    
    //PAGE 22 
    //AWB
   //Page 22 AWB
{0x03, 0x22},
{0x11, 0x26},  // CMC adj 0x26->0x2e
{0x21, 0x40},
       
{0x30, 0x80},
{0x31, 0x80},//80
{0x38, 0x11},
{0x39, 0x33},
{0x40, 0xf0},
{0x41, 0x33},
{0x42, 0x33},
{0x43, 0xf3},
{0x44, 0x44},
{0x45, 0x44},
{0x46, 0x08},
       
    {0x80, 0x30},//3a
    {0x81, 0x20},
    {0x82, 0x33},//40
    {0x83, 0x52},
    {0x84, 0x1b},
    {0x85, 0x50},
    {0x86, 0x25},
    
    {0x87, 0x32},  //4d r 4d->3e
    {0x88, 0x23},  // 38 ->25
    {0x89, 0x30},  //3e b  4d->3e
    {0x8a, 0x25},  //29->25
    {0x8b, 0x02},
    {0x8d, 0x22},
    {0x8e, 0x71},  
    {0x8f, 0x63},
        
    {0x90, 0x60},
    {0x91, 0x58},//5c
    {0x92, 0x50},//56
    {0x93, 0x3f}, // 52->46
    {0x94, 0x38}, // ///  4c -> 3c
    {0x95, 0x32},
    {0x96, 0x2a},//31
    {0x97, 0x25},//2e
    {0x98, 0x20},//2a
    {0x99, 0x1c},//29
{0x9a, 0x18},//26
{0x9b, 0x09},//09
       
{0x03, 0x22},
{0x10, 0xfb},

{0x03, 0x20},
{0x10, 0x9c},
    
    {0x01, 0xf0},

    //PAGE 0
    {0x03, 0x00},
   // {0x01, 0x90},   //0xf1 ->0x41 : For Preview Green/Red Line.
	{0x01, 0xf0},  //jun debug for driver ability

    //{0x01, 0x90},   //0xf1 ->0x41 : For Preview Green/Red Line.

	{0xff, 0xff}    //End of Initial Setting

};
static void HI704_Initial_Setting(void)
{
	kal_uint32 iEcount;
	for(iEcount=0;(!((0xff==(HI704_Initial_Setting_Info[iEcount].address))&&(0xff==(HI704_Initial_Setting_Info[iEcount].data))));iEcount++)
	{	
		HI704_write_cmos_sensor(HI704_Initial_Setting_Info[iEcount].address, HI704_Initial_Setting_Info[iEcount].data);
	}

}

static void HI704_Init_Parameter(void)
{
    HI704_sensor.first_init = KAL_TRUE;
    HI704_sensor.pv_mode = KAL_TRUE;
    HI704_sensor.night_mode = KAL_FALSE;
    HI704_sensor.MPEG4_Video_mode = KAL_FALSE;
    
    HI704_sensor.cp_pclk = HI704_sensor.pv_pclk;
    
    HI704_sensor.pv_dummy_pixels = 0;
    HI704_sensor.pv_dummy_lines = 0;
	HI704_sensor.cp_dummy_pixels = 0;
	HI704_sensor.cp_dummy_lines = 0;

    HI704_sensor.wb = 0;
    HI704_sensor.exposure = 0;
    HI704_sensor.effect = 0;
    HI704_sensor.banding = AE_FLICKER_MODE_50HZ;

    HI704_sensor.pv_line_length = 640;
    HI704_sensor.pv_frame_height = 480;
    HI704_sensor.cp_line_length = 640;
    HI704_sensor.cp_frame_height = 480;
    
}

static kal_uint8 HI704_power_on(void)
{
    kal_uint8 HI704_sensor_id = 0;

	HI704_sensor.pv_pclk = 13000000;
    //Software Reset
    HI704_write_cmos_sensor(0x01,0xf1);
    HI704_write_cmos_sensor(0x01,0xf3);
    HI704_write_cmos_sensor(0x01,0xf1);

    /* Read Sensor ID  */
	HI704_sensor_id = HI704_read_cmos_sensor(0x04);
    SENSORDB("[HI704YUV]:read Sensor ID:%x\n",HI704_sensor_id);
	
	return HI704_sensor_id;
}


/*************************************************************************
* FUNCTION
*	HI704Open
*
* DESCRIPTION
*	This function initialize the registers of CMOS sensor
*
* PARAMETERS
*	None
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
UINT32 HI704Open(void)
{
//	mt_set_gpio_mode(GPIO_I2C1_SCA_PIN, GPIO_I2C1_SCA_PIN_M_GPIO);
//	mt_set_gpio_mode(GPIO_I2C1_SDA_PIN, GPIO_I2C1_SDA_PIN_M_GPIO);
   
	sensor_id_fail = 0; 
	   
	SENSORDB("[Enter]:HI704 Open func:");

	if (HI704_power_on() != HI704_SENSOR_ID) 
	{
	 	   SENSORDB("[HI704]Error:read sensor ID fail\n");
		   sensor_id_fail = 1;
		   return ERROR_SENSOR_CONNECT_FAIL;
	}
    
      
    /* Apply sensor initail setting*/
     HI704_Initial_Setting();
	 HI704_Init_Parameter(); 
	
	 SENSORDB("[Exit]:HI704 Open func\n");
     
	return ERROR_NONE;
}	/* HI704Open() */

/*************************************************************************
* FUNCTION
*	HI704_GetSensorID
*
* DESCRIPTION
*	This function get the sensor ID
*
* PARAMETERS
*	None
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
static kal_uint32 HI704_GetSensorID(kal_uint32 *sensorID)

{
	SENSORDB("[Enter]:HI704 Open func ");
	*sensorID = HI704_power_on() ;

     if (*sensorID != HI704_SENSOR_ID) 
 	{
 	   
	  SENSORDB("[HI704]Error:read sensor ID fail\n");
	  sensor_id_fail = 1;
	  return ERROR_SENSOR_CONNECT_FAIL;
   }
	   

    return ERROR_NONE;    
}   /* HI704Open  */


/*************************************************************************
* FUNCTION
*	HI704Close
*
* DESCRIPTION
*	This function is to turn off sensor module power.
*
* PARAMETERS
*	None
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
UINT32 HI704Close(void)
{

	return ERROR_NONE;
}	/* HI704Close() */


static void HI704_Set_Mirror_Flip(kal_uint8 image_mirror)
{
  /********************************************************
    * Page Mode 0: Reg 0x0011 bit[1:0] = [Y Flip : X Flip]
    * 0: Off; 1: On.
    *********************************************************/    
    SENSORDB("[Enter]:HI704 set Mirror_flip func:image_mirror=%d\n",image_mirror);
	
    HI704_write_cmos_sensor(0x03,0x00);     //Page 0	
    HI704_sensor.mirror = (HI704_read_cmos_sensor(0x11) & 0xfc); 
    
//	image_mirror=IMAGE_HV_MIRROR;

    switch (image_mirror) 
    {
//        case IMAGE_NORMAL:
		case IMAGE_HV_MIRROR:
		    HI704_sensor.mirror |= 0x00;
	        break;
//	    case IMAGE_H_MIRROR:
		case IMAGE_V_MIRROR:
		    HI704_sensor.mirror |= 0x01;
	        break;
//	    case IMAGE_V_MIRROR:
		case IMAGE_H_MIRROR:
		    HI704_sensor.mirror |= 0x02;
	        break;
//    	case IMAGE_HV_MIRROR:
		case IMAGE_NORMAL:
		    HI704_sensor.mirror |= 0x03;
	        break;
    	default:
	        HI704_sensor.mirror |= 0x00;
    }

    HI704_write_cmos_sensor(0x11, HI704_sensor.mirror);
		SENSORDB("[Exit]:HI704 set Mirror_flip func\n");

}

static void HI704_set_dummy(kal_uint16 dummy_pixels,kal_uint16 dummy_lines)
{	
	HI704_write_cmos_sensor(0x03, 0x00);                        //Page 0
	HI704_write_cmos_sensor(0x40,((dummy_pixels & 0x0F00))>>8);       //HBLANK
	HI704_write_cmos_sensor(0x41,(dummy_pixels & 0xFF));
	HI704_write_cmos_sensor(0x42,((dummy_lines & 0xFF00)>>8));       //VBLANK ( Vsync Type 1)
	HI704_write_cmos_sensor(0x43,(dummy_lines & 0xFF));
}  


// 640 * 480
static void HI704_Set_VGA_mode(void)
{
    HI704_write_cmos_sensor(0x01, HI704_read_cmos_sensor(0x01)|0x01);   //Sleep: For Write Reg

    HI704_write_cmos_sensor(0x03, 0x00);
    HI704_write_cmos_sensor(0x10, 0x00);        //VGA Size
    HI704_write_cmos_sensor(0x12, 0x04);
    HI704_write_cmos_sensor(0x20, 0x00);
    HI704_write_cmos_sensor(0x21, 0x04);

    HI704_write_cmos_sensor(0x40, 0x01);        //HBLANK: 0x70 = 112
    HI704_write_cmos_sensor(0x41, 0x58);
    HI704_write_cmos_sensor(0x42, 0x00);        //VBLANK: 0x04 = 4
    HI704_write_cmos_sensor(0x43, 0x63);

    HI704_write_cmos_sensor(0x03, 0x11);
    HI704_write_cmos_sensor(0x10, 0x25);  

    HI704_write_cmos_sensor(0x03, 0x20);

    HI704_write_cmos_sensor(0x10, HI704_read_cmos_sensor(0x10)&0x7f);   //Close AE
  //  HI704_write_cmos_sensor(0x18, HI704_read_cmos_sensor(0x18)|0x08);   //Reset AE
	
    //HI704_write_cmos_sensor(0x83, Hi704_cp_exposure_lines>> 16);
   // HI704_write_cmos_sensor(0x84, (Hi704_cp_exposure_lines >> 8) & 0x000000FF);
    //HI704_write_cmos_sensor(0x85, Hi704_cp_exposure_lines & 0x000000FF);
	
    HI704_write_cmos_sensor(0x86, 0x00);
    HI704_write_cmos_sensor(0x87, 0xfa);

    HI704_write_cmos_sensor(0x8b, 0x3a);
    HI704_write_cmos_sensor(0x8c, 0x98);
    HI704_write_cmos_sensor(0x8d, 0x30);
    HI704_write_cmos_sensor(0x8e, 0xd4);

    HI704_write_cmos_sensor(0x9c, 0x0b);
    HI704_write_cmos_sensor(0x9d, 0xb8);
    HI704_write_cmos_sensor(0x9e, 0x00);
    HI704_write_cmos_sensor(0x9f, 0xfa);

    HI704_write_cmos_sensor(0x01, HI704_read_cmos_sensor(0x01)&0xfe);   //Exit Sleep: For Write Reg

    HI704_write_cmos_sensor(0x03, 0x20);
    HI704_write_cmos_sensor(0x10, HI704_read_cmos_sensor(0x10)|0x80);   //Open AE
  //  HI704_write_cmos_sensor(0x18, HI704_read_cmos_sensor(0x18)&0xf7);   //Reset AE

}



// 320 * 240
static void HI704_Set_QVGA_mode(void)
{
    HI704_write_cmos_sensor(0x01, HI704_read_cmos_sensor(0x01)|0x01);   //Sleep: For Write Reg
    
    HI704_write_cmos_sensor(0x03, 0x00);
    HI704_write_cmos_sensor(0x10, 0x01);        //QVGA Size: 0x10 -> 0x01
    HI704_write_cmos_sensor(0x12, 0x04);
    HI704_write_cmos_sensor(0x20, 0x00);
    HI704_write_cmos_sensor(0x21, 0x02);

    HI704_write_cmos_sensor(0x40, 0x01);        //HBLANK:  0x0158 = 344
    HI704_write_cmos_sensor(0x41, 0x58);
    HI704_write_cmos_sensor(0x42, 0x00);        //VBLANK:  0x14 = 20
    HI704_write_cmos_sensor(0x43, 0x63);

    HI704_write_cmos_sensor(0x03, 0x11);        //QVGA Fixframerate
    HI704_write_cmos_sensor(0x10, 0x21);  

    HI704_write_cmos_sensor(0x03, 0x20);
    HI704_write_cmos_sensor(0x10, HI704_read_cmos_sensor(0x10)&0x7f);   //Close AE
    HI704_write_cmos_sensor(0x18, HI704_read_cmos_sensor(0x18)|0x08);   //Reset AE

    HI704_write_cmos_sensor(0x83, 0x00);
    HI704_write_cmos_sensor(0x84, 0xaf);
    HI704_write_cmos_sensor(0x85, 0xc8);
    HI704_write_cmos_sensor(0x86, 0x00);
    HI704_write_cmos_sensor(0x87, 0xfa);

    HI704_write_cmos_sensor(0x8b, 0x3a);
    HI704_write_cmos_sensor(0x8c, 0x98);
    HI704_write_cmos_sensor(0x8d, 0x30);
    HI704_write_cmos_sensor(0x8e, 0xd4);

    HI704_write_cmos_sensor(0x9c, 0x0b);
    HI704_write_cmos_sensor(0x9d, 0xb8);
    HI704_write_cmos_sensor(0x9e, 0x00);
    HI704_write_cmos_sensor(0x9f, 0xfa);

    HI704_write_cmos_sensor(0x01, HI704_read_cmos_sensor(0x01)&0xfe);   //Exit Sleep: For Write Reg

    HI704_write_cmos_sensor(0x03, 0x20);
    HI704_write_cmos_sensor(0x10, HI704_read_cmos_sensor(0x10)|0x80);   //Open AE
    HI704_write_cmos_sensor(0x18, HI704_read_cmos_sensor(0x18)&0xf7);   //Reset AE

}
void HI704_night_mode(kal_bool enable)
{

	  SENSORDB("[Enter]HI704 night mode func:enable = %d\n",enable);
	  SENSORDB("HI704_sensor.video_mode = %d\n",HI704_sensor.MPEG4_Video_mode); 
      SENSORDB("HI704_sensor.night_mode = %d\n",HI704_sensor.night_mode);

	HI704_sensor.night_mode = enable;

	if(enable)
	{
	    if(HI704_sensor.MPEG4_Video_mode == KAL_TRUE)
	    	{
	       Hi704_video_reset = KAL_TRUE;
		   
	    	HI704_write_cmos_sensor(0x03,0x00); 	
		HI704_Sleep_Mode = (HI704_read_cmos_sensor(0x01) & 0xfe);
		HI704_Sleep_Mode |= 0x01;
		HI704_write_cmos_sensor(0x01, HI704_Sleep_Mode);

    		HI704_write_cmos_sensor(0x11, HI704_read_cmos_sensor(0x11)&0xfb);
		
		HI704_write_cmos_sensor(0x03, 0x20);
		HI704_write_cmos_sensor(0x10, 0x1c);
		
		HI704_write_cmos_sensor(0x03, 0x20);
		

		HI704_write_cmos_sensor(0x88, 0x01);
		HI704_write_cmos_sensor(0x89, 0xd4);
		HI704_write_cmos_sensor(0x8a, 0xc0);
			
		HI704_write_cmos_sensor(0x03,0x00); 	
		HI704_Sleep_Mode = (HI704_read_cmos_sensor(0x01) & 0xfe);
		HI704_Sleep_Mode |= 0x00;
		HI704_write_cmos_sensor(0x01, HI704_Sleep_Mode);

		HI704_write_cmos_sensor(0x03, 0x20);
		HI704_write_cmos_sensor(0x10, 0x9c);
		
			msleep(10);
	    	}
		else
		{
			 //HI704_Cal_Min_Frame_Rate(HI704_MIN_FRAMERATE_5);        
		HI704_write_cmos_sensor(0x03,0x00); 	
		HI704_Sleep_Mode = (HI704_read_cmos_sensor(0x01) & 0xfe);
		HI704_Sleep_Mode |= 0x01;
		HI704_write_cmos_sensor(0x01, HI704_Sleep_Mode);
		
    		HI704_write_cmos_sensor(0x11, HI704_read_cmos_sensor(0x11)&0xfb);
		
		HI704_write_cmos_sensor(0x03, 0x20);
		HI704_write_cmos_sensor(0x10, 0x1c);
		
		HI704_write_cmos_sensor(0x03, 0x20);
		HI704_write_cmos_sensor(0x18, 0x38);

		HI704_write_cmos_sensor(0x83, Hi704_cp_exposure_lines>> 16);
    		HI704_write_cmos_sensor(0x84, (Hi704_cp_exposure_lines >> 8) & 0x000000FF);
    		HI704_write_cmos_sensor(0x85, Hi704_cp_exposure_lines & 0x000000FF);

		HI704_write_cmos_sensor(0x88, 0x04);
		HI704_write_cmos_sensor(0x89, 0x93);
		HI704_write_cmos_sensor(0x8a, 0xe0);
			
		HI704_write_cmos_sensor(0x03,0x00); 	
		HI704_Sleep_Mode = (HI704_read_cmos_sensor(0x01) & 0xfe);
		HI704_Sleep_Mode |= 0x00;
		HI704_write_cmos_sensor(0x01, HI704_Sleep_Mode);

		HI704_write_cmos_sensor(0x03, 0x20);
		HI704_write_cmos_sensor(0x10, 0x9c);

		HI704_write_cmos_sensor(0x18, 0x30);
		
	}
		}
	else
    {
    	    if(HI704_sensor.MPEG4_Video_mode == KAL_TRUE)
	    	{

			Hi704_video_reset = KAL_TRUE;
			
    	       HI704_write_cmos_sensor(0x03,0x00); 	
		HI704_Sleep_Mode = (HI704_read_cmos_sensor(0x01) & 0xfe);
		HI704_Sleep_Mode |= 0x01;
		HI704_write_cmos_sensor(0x01, HI704_Sleep_Mode);

    		HI704_write_cmos_sensor(0x11, HI704_read_cmos_sensor(0x11)&0xfb);
		
		HI704_write_cmos_sensor(0x03, 0x20);
		HI704_write_cmos_sensor(0x10, 0x1c);
		
		HI704_write_cmos_sensor(0x03, 0x20);
		

		HI704_write_cmos_sensor(0x88, 0x02);
		HI704_write_cmos_sensor(0x89, 0x0f);
		HI704_write_cmos_sensor(0x8a, 0x58);

		HI704_write_cmos_sensor(0xb2, 0x60);
			
		HI704_write_cmos_sensor(0x03,0x00); 	
		HI704_Sleep_Mode = (HI704_read_cmos_sensor(0x01) & 0xfe);
		HI704_Sleep_Mode |= 0x00;
		HI704_write_cmos_sensor(0x01, HI704_Sleep_Mode);

		HI704_write_cmos_sensor(0x03, 0x20);
		HI704_write_cmos_sensor(0x10, 0x9c);
	    	
		
	    	}
		else
		{
			//HI704_Cal_Min_Frame_Rate(HI704_MIN_FRAMERATE_10);      
		HI704_write_cmos_sensor(0x03,0x00); 	
		HI704_Sleep_Mode = (HI704_read_cmos_sensor(0x01) & 0xfe);
		HI704_Sleep_Mode |= 0x01;
		HI704_write_cmos_sensor(0x01, HI704_Sleep_Mode);

    		HI704_write_cmos_sensor(0x11, HI704_read_cmos_sensor(0x11)&0xfb);
		
		HI704_write_cmos_sensor(0x03, 0x20);
		HI704_write_cmos_sensor(0x10, 0x1c);
		
		HI704_write_cmos_sensor(0x03, 0x20);
		//HI704_write_cmos_sensor(0x18, 0x38);

		if(Hi704_video_reset == KAL_TRUE)
			{
			HI704_write_cmos_sensor(0x83, 0x03);
			HI704_write_cmos_sensor(0x84, 0x34);
			HI704_write_cmos_sensor(0x85, 0x50);
			Hi704_video_reset = KAL_FALSE;
			}

		HI704_write_cmos_sensor(0x88, 0x03);
		HI704_write_cmos_sensor(0x89, 0x34);
		HI704_write_cmos_sensor(0x8a, 0x50);

		HI704_write_cmos_sensor(0xb2, 0x50);
			
		HI704_write_cmos_sensor(0x03,0x00); 	
		HI704_Sleep_Mode = (HI704_read_cmos_sensor(0x01) & 0xfe);
		HI704_Sleep_Mode |= 0x00;
		HI704_write_cmos_sensor(0x01, HI704_Sleep_Mode);
		HI704_write_cmos_sensor(0x03, 0x20);
		HI704_write_cmos_sensor(0x10, 0x9c);

		//HI704_write_cmos_sensor(0x18, 0x30);
		}	
	}
}

/*************************************************************************
* FUNCTION
*	HI704Preview
*
* DESCRIPTION
*	This function start the sensor preview.
*
* PARAMETERS
*	*image_window : address pointer of pixel numbers in one period of HSYNC
*  *sensor_config_data : address pointer of line numbers in one period of VSYNC
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
static UINT32 HI704Preview(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
					  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	if(HI704_sensor.first_init == KAL_TRUE)
		HI704_sensor.MPEG4_Video_mode = HI704_sensor.MPEG4_Video_mode;
	else
		HI704_sensor.MPEG4_Video_mode = 0;
	SENSORDB("DBG_DH:HI704 preview func:");		
	SENSORDB("HI704_sensor.video_mode = %d\n",HI704_sensor.MPEG4_Video_mode); 

  HI704_sensor.first_init = KAL_FALSE;
	HI704_sensor.pv_mode = KAL_TRUE;		
 
	{   
	    SENSORDB("[HI704]preview set_VGA_mode\n");
		  HI704_Set_VGA_mode();
  }
   
	HI704_Set_Mirror_Flip(sensor_config_data->SensorImageMirror);

	SENSORDB("[Exit]:HI704 preview func\n");
    return TRUE; 
}	/* HI704_Preview */


UINT32 HI704Capture(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
					  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{	
	kal_uint8  iTemp;

	SENSORDB("DBG_DH:[Enter]HI704_capture_func\n");
	HI704_Set_Mirror_Flip(sensor_config_data->SensorImageMirror);  // fenggy add
	HI704_write_cmos_sensor(0x03, 0x20);
	iTemp = HI704_read_cmos_sensor(0x10) & 0x7F;
		HI704_write_cmos_sensor(0x10, iTemp);

	Hi704_cp_exposure_lines= (HI704_read_cmos_sensor(0x80) << 16) |
		(HI704_read_cmos_sensor(0x81) << 8)  |
		HI704_read_cmos_sensor(0x82);
	
	HI704_sensor.pv_mode = KAL_FALSE;	
		
	return ERROR_NONE;
}	/* HM3451Capture() */


UINT32 HI704GetResolution(MSDK_SENSOR_RESOLUTION_INFO_STRUCT *pSensorResolution)
{
    SENSORDB("[Enter]:HI704 get Resolution func\n");
	
	pSensorResolution->SensorFullWidth=HI704_IMAGE_SENSOR_FULL_WIDTH - 16;//12;  
	pSensorResolution->SensorFullHeight=HI704_IMAGE_SENSOR_FULL_HEIGHT - 12;//- 8;
	pSensorResolution->SensorPreviewWidth=HI704_IMAGE_SENSOR_PV_WIDTH - 16;//12;
	pSensorResolution->SensorPreviewHeight=HI704_IMAGE_SENSOR_PV_HEIGHT - 12;//8;

    SENSORDB("[Exit]:HI704 get Resolution func\n");
	
	return ERROR_NONE;
}	/* HI704GetResolution() */

UINT32 HI704GetInfo(MSDK_SCENARIO_ID_ENUM ScenarioId,
					  MSDK_SENSOR_INFO_STRUCT *pSensorInfo,
					  MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{
	kal_uint8 iTemp;
  SENSORDB("[Enter]:HI704 getInfo func:ScenarioId = %d\n",ScenarioId);
	if (MSDK_SCENARIO_ID_CAMERA_ZSD == ScenarioId) {
		if (write_shutter_flag) {
			HI704_write_cmos_sensor(0x03, 0x20);
			iTemp = HI704_read_cmos_sensor(0x10) & 0x7F;
			HI704_write_cmos_sensor(0x10, iTemp);

			Hi704_cp_exposure_lines= (HI704_read_cmos_sensor(0x80) << 16) |
																(HI704_read_cmos_sensor(0x81) << 8)  |
																HI704_read_cmos_sensor(0x82);
		}
		 HI704_Set_VGA_mode();	
		 write_shutter_flag = 0;
	}

	pSensorInfo->SensorPreviewResolutionX=HI704_IMAGE_SENSOR_PV_WIDTH;
	pSensorInfo->SensorPreviewResolutionY=HI704_IMAGE_SENSOR_PV_HEIGHT;
	pSensorInfo->SensorFullResolutionX=HI704_IMAGE_SENSOR_FULL_WIDTH;
	pSensorInfo->SensorFullResolutionY=HI704_IMAGE_SENSOR_FULL_HEIGHT;

	pSensorInfo->SensorCameraPreviewFrameRate=30;
	pSensorInfo->SensorVideoFrameRate=30;
	pSensorInfo->SensorStillCaptureFrameRate=30;
	pSensorInfo->SensorWebCamCaptureFrameRate=15;
	pSensorInfo->SensorResetActiveHigh=FALSE;//low is to reset 
	pSensorInfo->SensorResetDelayCount=4;  //4ms 
	pSensorInfo->SensorOutputDataFormat=SENSOR_OUTPUT_FORMAT_YUYV;
	pSensorInfo->SensorClockPolarity=SENSOR_CLOCK_POLARITY_LOW;	
	pSensorInfo->SensorClockFallingPolarity=SENSOR_CLOCK_POLARITY_LOW;
	pSensorInfo->SensorHsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;
	pSensorInfo->SensorVsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;
	pSensorInfo->SensorInterruptDelayLines = 1; 
	pSensorInfo->SensroInterfaceType=SENSOR_INTERFACE_TYPE_PARALLEL;

	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_100_MODE].MaxWidth=CAM_SIZE_VGA_WIDTH; //???
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_100_MODE].MaxHeight=CAM_SIZE_VGA_HEIGHT;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_100_MODE].ISOSupported=TRUE;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_100_MODE].BinningEnable=FALSE;

	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_200_MODE].MaxWidth=CAM_SIZE_VGA_WIDTH;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_200_MODE].MaxHeight=CAM_SIZE_VGA_HEIGHT;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_200_MODE].ISOSupported=TRUE;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_200_MODE].BinningEnable=FALSE;

	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_400_MODE].MaxWidth=CAM_SIZE_VGA_WIDTH;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_400_MODE].MaxHeight=CAM_SIZE_VGA_HEIGHT;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_400_MODE].ISOSupported=TRUE;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_400_MODE].BinningEnable=FALSE;

	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_800_MODE].MaxWidth=CAM_SIZE_VGA_WIDTH;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_800_MODE].MaxHeight=CAM_SIZE_VGA_HEIGHT;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_800_MODE].ISOSupported=TRUE;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_800_MODE].BinningEnable=TRUE;

	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_1600_MODE].MaxWidth=CAM_SIZE_VGA_WIDTH;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_1600_MODE].MaxHeight=CAM_SIZE_VGA_HEIGHT;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_1600_MODE].ISOSupported=TRUE;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_1600_MODE].BinningEnable=TRUE;

//	pSensorInfo->CaptureDelayFrame = 4; 
//	pSensorInfo->PreviewDelayFrame = 10; 
//	pSensorInfo->VideoDelayFrame = 0; 
	pSensorInfo->SensorMasterClockSwitch = 0; 
       pSensorInfo->SensorDrivingCurrent = ISP_DRIVING_8MA;   		
	switch (ScenarioId)
	{
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
		case MSDK_SCENARIO_ID_VIDEO_CAPTURE_MPEG4:
			pSensorInfo->SensorClockFreq=24;
			pSensorInfo->SensorClockDividCount=	3;
			pSensorInfo->SensorClockRisingCount= 0;
			pSensorInfo->SensorClockFallingCount= 2;
			pSensorInfo->SensorPixelClockCount= 3;
			pSensorInfo->SensorDataLatchCount= 2;
	        pSensorInfo->SensorGrabStartX = 1; 
	        pSensorInfo->SensorGrabStartY = 10;  			
			
		break;
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_MEM:
			pSensorInfo->SensorClockFreq=24;
			pSensorInfo->SensorClockDividCount=	3;
			pSensorInfo->SensorClockRisingCount= 0;
			pSensorInfo->SensorClockFallingCount= 2;
			pSensorInfo->SensorPixelClockCount= 3;
			pSensorInfo->SensorDataLatchCount= 2;
            pSensorInfo->SensorGrabStartX = 1; 
            pSensorInfo->SensorGrabStartY = 10;//1;     			
		break;
		default:
			pSensorInfo->SensorClockFreq=24;
			pSensorInfo->SensorClockDividCount=3;
			pSensorInfo->SensorClockRisingCount=0;
			pSensorInfo->SensorClockFallingCount=2;
			pSensorInfo->SensorPixelClockCount=3;
			pSensorInfo->SensorDataLatchCount=2;
            pSensorInfo->SensorGrabStartX = 1; 
            pSensorInfo->SensorGrabStartY = 10;//1;     			
		break;
	}
//	HI704_PixelClockDivider=pSensorInfo->SensorPixelClockCount;
	memcpy(pSensorConfigData, &HI704SensorConfigData, sizeof(MSDK_SENSOR_CONFIG_STRUCT));

	SENSORDB("[Exit]:HI704 getInfo func\n");
	
	return ERROR_NONE;
}	/* HI704GetInfo() */


UINT32 HI704Control(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *pImageWindow,
					  MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{
   SENSORDB("[Enter]:HI704 Control func:ScenarioId = %d\n",ScenarioId);

	switch (ScenarioId)
	{
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
		case MSDK_SCENARIO_ID_VIDEO_CAPTURE_MPEG4:
		//case MSDK_SCENARIO_ID_CAMERA_ZSD:
		 	 HI704Preview(pImageWindow, pSensorConfigData); 
			 break;
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_MEM:
			 HI704Capture(pImageWindow, pSensorConfigData); 
			 break;
		default:
		     break; 
	}

   SENSORDB("[Exit]:HI704 Control func\n");
	
	return TRUE;
}	/* HI704Control() */


/*************************************************************************
* FUNCTION
*	HI704_set_param_wb
*
* DESCRIPTION
*	wb setting.
*
* PARAMETERS
*	none
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
BOOL HI704_set_param_wb(UINT16 para)
{
	
    //This sensor need more time to balance AWB, 
    //we suggest higher fps or drop some frame to avoid garbage color when preview initial
   SENSORDB("[Enter]HI704 set_param_wb func:para = %d\n",para);
	
   if(HI704_sensor.wb == para) return KAL_TRUE;	

	HI704_sensor.wb = para;
	
	switch (para)
	{            
		case AWB_MODE_AUTO:
			{
	    HI704_write_cmos_sensor(0x03, 0x22);			
		HI704_write_cmos_sensor(0x10,0x6a); 
		HI704_write_cmos_sensor(0x80, 0x35);
	  HI704_write_cmos_sensor(0x82, 0x33);                                                                                                                
		HI704_write_cmos_sensor(0x83,0x52);                                 
		HI704_write_cmos_sensor(0x84, 0x18); // 2a
		HI704_write_cmos_sensor(0x85, 0x53); // 50
		HI704_write_cmos_sensor(0x86,0x25);                     
		HI704_write_cmos_sensor(0x03,0x22);                                                          
		HI704_write_cmos_sensor(0x10,0xfb);  			
            }                
		    break;
		case AWB_MODE_CLOUDY_DAYLIGHT:
			{
	        HI704_write_cmos_sensor(0x03, 0x22);
            HI704_write_cmos_sensor(0x10,0x6a);                                                          
	        HI704_write_cmos_sensor(0x80, 0x71);
	        HI704_write_cmos_sensor(0x82, 0x2b);
	        HI704_write_cmos_sensor(0x83, 0x72);
	        HI704_write_cmos_sensor(0x84, 0x70);
	        HI704_write_cmos_sensor(0x85, 0x2b);
            HI704_write_cmos_sensor(0x86, 0x28);
	        }			   
		    break;
		case AWB_MODE_DAYLIGHT:
		    {
            HI704_write_cmos_sensor(0x03, 0x22);
            HI704_write_cmos_sensor(0x10,0x6a);                                                          
            HI704_write_cmos_sensor(0x80, 0x59);
            HI704_write_cmos_sensor(0x82, 0x29);
            HI704_write_cmos_sensor(0x83, 0x60);
            HI704_write_cmos_sensor(0x84, 0x50);
            HI704_write_cmos_sensor(0x85, 0x2f);
            HI704_write_cmos_sensor(0x86, 0x23);
            }      
		    break;
		case AWB_MODE_INCANDESCENT:	
		    {
            HI704_write_cmos_sensor(0x03, 0x22);
            HI704_write_cmos_sensor(0x10,0x6a);                                                          
            HI704_write_cmos_sensor(0x80, 0x29);
            HI704_write_cmos_sensor(0x82, 0x54);
            HI704_write_cmos_sensor(0x83, 0x2e);
            HI704_write_cmos_sensor(0x84, 0x23);
            HI704_write_cmos_sensor(0x85, 0x58);
            HI704_write_cmos_sensor(0x86, 0x4f);
            }		
		    break;  
		case AWB_MODE_FLUORESCENT:
		    {
            HI704_write_cmos_sensor(0x03, 0x22);
            HI704_write_cmos_sensor(0x10,0x6a);                                                          
            HI704_write_cmos_sensor(0x80, 0x41);
            HI704_write_cmos_sensor(0x82, 0x42);
            HI704_write_cmos_sensor(0x83, 0x44);
            HI704_write_cmos_sensor(0x84, 0x34);
            HI704_write_cmos_sensor(0x85, 0x46);
            HI704_write_cmos_sensor(0x86, 0x3a);
            }	
		    break;  
		case AWB_MODE_TUNGSTEN:
		   {
            HI704_write_cmos_sensor(0x03, 0x22);
            HI704_write_cmos_sensor(0x10,0x6a);                                                          
            HI704_write_cmos_sensor(0x80, 0x24);
            HI704_write_cmos_sensor(0x82, 0x58);
            HI704_write_cmos_sensor(0x83, 0x27);
            HI704_write_cmos_sensor(0x84, 0x22);
            HI704_write_cmos_sensor(0x85, 0x58);
            HI704_write_cmos_sensor(0x86, 0x52);
           }
		    break;

		default:
			return FALSE;
	}

	return TRUE;
	
} /* HI704_set_param_wb */

/*************************************************************************
* FUNCTION
*	HI704_set_param_effect
*
* DESCRIPTION
*	effect setting.
*
* PARAMETERS
*	none
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
BOOL HI704_set_param_effect(UINT16 para)
{

   SENSORDB("[Enter]HI704 set_param_effect func:para = %d\n",para);
   
   if(HI704_sensor.effect == para) return KAL_TRUE;
    HI704_sensor.effect = para;
	
	switch (para)
	{
		case MEFFECT_OFF:
			{
	            HI704_write_cmos_sensor(0x03, 0x10);
	            HI704_write_cmos_sensor(0x11, 0x03);
	            HI704_write_cmos_sensor(0x12, 0x30);
	            HI704_write_cmos_sensor(0x13, 0x00);
	            HI704_write_cmos_sensor(0x44, 0x80);
	            HI704_write_cmos_sensor(0x45, 0x80);

	            HI704_write_cmos_sensor(0x47, 0x7f);
	            HI704_write_cmos_sensor(0x03, 0x13);
	            //HI704_write_cmos_sensor(0x20, 0x07);
	            //HI704_write_cmos_sensor(0x21, 0x07);
            }
	        break;
		case MEFFECT_SEPIA:
			{
	            HI704_write_cmos_sensor(0x03, 0x10);
	            HI704_write_cmos_sensor(0x11, 0x03);
	            HI704_write_cmos_sensor(0x12, 0x23);
	            HI704_write_cmos_sensor(0x13, 0x00);
	            HI704_write_cmos_sensor(0x44, 0x70);
	            HI704_write_cmos_sensor(0x45, 0x98);

	            HI704_write_cmos_sensor(0x47, 0x7f);
	            HI704_write_cmos_sensor(0x03, 0x13);
	            //HI704_write_cmos_sensor(0x20, 0x07);
	            //HI704_write_cmos_sensor(0x21, 0x07);
            }	
			break;  
		case MEFFECT_NEGATIVE:
			{
	            HI704_write_cmos_sensor(0x03, 0x10);
	            HI704_write_cmos_sensor(0x11, 0x03);
	            HI704_write_cmos_sensor(0x12, 0x08);
	            HI704_write_cmos_sensor(0x13, 0x00);
	            HI704_write_cmos_sensor(0x14, 0x00);
            }
			break; 
		case MEFFECT_SEPIAGREEN:		
			{
	            HI704_write_cmos_sensor(0x03, 0x10);
	            HI704_write_cmos_sensor(0x11, 0x03);
	            HI704_write_cmos_sensor(0x12, 0x03);
	            HI704_write_cmos_sensor(0x40, 0x00);
	            HI704_write_cmos_sensor(0x13, 0x00);
	            HI704_write_cmos_sensor(0x44, 0x30);
	            HI704_write_cmos_sensor(0x45, 0x50);
            }	
			break;
		case MEFFECT_SEPIABLUE:
			{
			  	HI704_write_cmos_sensor(0x03, 0x10);
				HI704_write_cmos_sensor(0x11, 0x03);
				HI704_write_cmos_sensor(0x12, 0x03);
				HI704_write_cmos_sensor(0x40, 0x00);
				HI704_write_cmos_sensor(0x13, 0x00);
				HI704_write_cmos_sensor(0x44, 0xb0);
				HI704_write_cmos_sensor(0x45, 0x40);
		    }     
			break;        
		case MEFFECT_MONO:			
			{
				HI704_write_cmos_sensor(0x03, 0x10);
				HI704_write_cmos_sensor(0x11, 0x03);
				HI704_write_cmos_sensor(0x12, 0x03);
				HI704_write_cmos_sensor(0x40, 0x00);
				HI704_write_cmos_sensor(0x44, 0x80);
				HI704_write_cmos_sensor(0x45, 0x80);
            }
			break;

		default:
			return KAL_FALSE;
	}

	return KAL_TRUE;

} /* HI704_set_param_effect */

/*************************************************************************
* FUNCTION
*	HI704_set_param_banding
*
* DESCRIPTION
*	banding setting.
*
* PARAMETERS
*	none
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
BOOL HI704_set_param_banding(UINT16 para)
{


	SENSORDB("[Enter]HI704 set_param_banding func:para = %d\n",para);

	
	if(HI704_sensor.banding == para) return KAL_TRUE;

	  HI704_sensor.banding = para;

	switch (para)
	{
		case AE_FLICKER_MODE_50HZ:
		    {
				HI704_write_cmos_sensor(0x03,0x20);
				HI704_write_cmos_sensor(0x10,0x9c);
		    }
			break;

		case AE_FLICKER_MODE_60HZ:
		    {
				HI704_write_cmos_sensor(0x03,0x20);
				HI704_write_cmos_sensor(0x10,0x8c);
		    }
			break;

	     default:
	          return KAL_FALSE;
	}


	return KAL_TRUE;
} /* HI704_set_param_banding */




/*************************************************************************
* FUNCTION
*	HI704_set_param_exposure
*
* DESCRIPTION
*	exposure setting.
*
* PARAMETERS
*	none
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
BOOL HI704_set_param_exposure(UINT16 para)
{


	SENSORDB("[Enter]HI704 set_param_exposure func:para = %d\n",para);

	
	if(HI704_sensor.exposure == para) return KAL_TRUE;

	  HI704_sensor.exposure = para;

	   HI704_write_cmos_sensor(0x03,0x10);
	   HI704_write_cmos_sensor(0x12,HI704_read_cmos_sensor(0x12)|0x10);
	switch (para)
	{
		case AE_EV_COMP_13:  //+4 EV
			HI704_write_cmos_sensor(0x40,0x60);
			break;  
		case AE_EV_COMP_10:  //+3 EV
			HI704_write_cmos_sensor(0x40,0x48);
			break;    
		case AE_EV_COMP_07:  //+2 EV
			HI704_write_cmos_sensor(0x40,0x30);
			break;    
		case AE_EV_COMP_03:	 //	+1 EV	
			HI704_write_cmos_sensor(0x40,0x18);	
			break;    
		case AE_EV_COMP_00:  // +0 EV
		    HI704_write_cmos_sensor(0x40,0x0a);
			break;    
		case AE_EV_COMP_n03:  // -1 EV
			HI704_write_cmos_sensor(0x40,0x98);
			break;    
		case AE_EV_COMP_n07:	// -2 EV		
			HI704_write_cmos_sensor(0x40,0xb0);	
			break;    
		case AE_EV_COMP_n10:   //-3 EV
			HI704_write_cmos_sensor(0x40,0xc8);
			break;
		case AE_EV_COMP_n13:  // -4 EV
			HI704_write_cmos_sensor(0x40,0xe0);
			break;
		default:
			return FALSE;
	}

	return TRUE;
	
} /* HI704_set_param_exposure */


UINT32 HI704YUVSensorSetting(FEATURE_ID iCmd, UINT32 iPara)
{
    SENSORDB("[Enter]HI704YUVSensorSetting func:cmd = %d\n",iCmd);
	
	switch (iCmd) {
	case FID_SCENE_MODE:	    //auto mode or night mode

		    if (iPara == SCENE_MODE_OFF)//auto mode
		    {
		        HI704_night_mode(FALSE); 
		    }
		    else if (iPara == SCENE_MODE_NIGHTSCENE)//night mode
		    {
	            HI704_night_mode(TRUE); 
		    }	
			
	     break; 	    
	case FID_AWB_MODE:
           HI704_set_param_wb(iPara);
	     break;
	case FID_COLOR_EFFECT:
           HI704_set_param_effect(iPara);
	     break;
	case FID_AE_EV:	    	    
           HI704_set_param_exposure(iPara);
	     break;
	case FID_AE_FLICKER:	    	    	    
           HI704_set_param_banding(iPara);
	     break;
	case FID_ZOOM_FACTOR:
	     HI704_zoom_factor = iPara; 		
	     break; 
	default:
	     break;
	}
	return TRUE;
}   /* HI704YUVSensorSetting */

UINT32 HI704YUVSetVideoMode(UINT16 u2FrameRate)
{
   HI704_sensor.MPEG4_Video_mode = KAL_TRUE;
    SENSORDB("[Enter]HI704 Set Video Mode:FrameRate= %d\n",u2FrameRate);
	SENSORDB("HI704_sensor.video_mode = %d\n",HI704_sensor.MPEG4_Video_mode);

//    if(u2FrameRate == 30) u2FrameRate = 25;
   
	
	//HI704_sensor.fix_framerate = u2FrameRate * 10;
    
    if(HI704_sensor.fix_framerate <= 300 )
    {
      //HI704_Fix_Video_Frame_Rate(HI704_sensor.fix_framerate); 
    }
    else 
    {
       // SENSORDB("Wrong Frame Rate"); 
    }
    
    
    return TRUE;
}

UINT32 HI704FeatureControl(MSDK_SENSOR_FEATURE_ENUM FeatureId,
							 UINT8 *pFeaturePara,UINT32 *pFeatureParaLen)
{
    UINT16 u2Temp = 0; 
	UINT16 *pFeatureReturnPara16=(UINT16 *) pFeaturePara;
	UINT16 *pFeatureData16=(UINT16 *) pFeaturePara;
	UINT32 *pFeatureReturnPara32=(UINT32 *) pFeaturePara;
	UINT32 *pFeatureData32=(UINT32 *) pFeaturePara;
	MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData=(MSDK_SENSOR_CONFIG_STRUCT *) pFeaturePara;
	MSDK_SENSOR_REG_INFO_STRUCT *pSensorRegData=(MSDK_SENSOR_REG_INFO_STRUCT *) pFeaturePara;

	switch (FeatureId)
	{
		case SENSOR_FEATURE_GET_RESOLUTION:
			*pFeatureReturnPara16++=HI704_IMAGE_SENSOR_FULL_WIDTH;
			*pFeatureReturnPara16=HI704_IMAGE_SENSOR_FULL_HEIGHT;
			*pFeatureParaLen=4;
		     break;
		case SENSOR_FEATURE_GET_PERIOD:
			*pFeatureReturnPara16++=HI704_IMAGE_SENSOR_PV_WIDTH;//+HI704_sensor.pv_dummy_pixels;
			*pFeatureReturnPara16=HI704_IMAGE_SENSOR_PV_HEIGHT;//+HI704_sensor.pv_dummy_lines;
			*pFeatureParaLen=4;
		     break;
		case SENSOR_FEATURE_GET_PIXEL_CLOCK_FREQ:
			//*pFeatureReturnPara32 = HI704_sensor_pclk/10;
			*pFeatureParaLen=4;
		     break;
		case SENSOR_FEATURE_SET_ESHUTTER:
	
		     break;
		case SENSOR_FEATURE_SET_NIGHTMODE:
			 HI704_night_mode((BOOL) *pFeatureData16);
		     break;
		case SENSOR_FEATURE_SET_GAIN:
			 break; 
		case SENSOR_FEATURE_SET_FLASHLIGHT:
		     break;
		case SENSOR_FEATURE_SET_ISP_MASTER_CLOCK_FREQ:
		     break;
		case SENSOR_FEATURE_SET_REGISTER:
			 HI704_write_cmos_sensor(pSensorRegData->RegAddr, pSensorRegData->RegData);
		     break;
		case SENSOR_FEATURE_GET_REGISTER:
			 pSensorRegData->RegData = HI704_read_cmos_sensor(pSensorRegData->RegAddr);
		     break;
		case SENSOR_FEATURE_GET_CONFIG_PARA:
			 memcpy(pSensorConfigData, &HI704SensorConfigData, sizeof(MSDK_SENSOR_CONFIG_STRUCT));
			 *pFeatureParaLen=sizeof(MSDK_SENSOR_CONFIG_STRUCT);
		     break;
		case SENSOR_FEATURE_SET_CCT_REGISTER:
		case SENSOR_FEATURE_GET_CCT_REGISTER:
		case SENSOR_FEATURE_SET_ENG_REGISTER:
		case SENSOR_FEATURE_GET_ENG_REGISTER:
		case SENSOR_FEATURE_GET_REGISTER_DEFAULT:
		case SENSOR_FEATURE_CAMERA_PARA_TO_SENSOR:
		case SENSOR_FEATURE_SENSOR_TO_CAMERA_PARA:
		case SENSOR_FEATURE_GET_GROUP_INFO:
		case SENSOR_FEATURE_GET_ITEM_INFO:
		case SENSOR_FEATURE_SET_ITEM_INFO:
		case SENSOR_FEATURE_GET_ENG_INFO:
		     break;
		case SENSOR_FEATURE_GET_GROUP_COUNT:
	               // *pFeatureReturnPara32++=0;
			//*pFeatureParaLen=4;
		     break; 
		
		case SENSOR_FEATURE_GET_LENS_DRIVER_ID:
			// get the lens driver ID from EEPROM or just return LENS_DRIVER_ID_DO_NOT_CARE
			// if EEPROM does not exist in camera module.
			*pFeatureReturnPara32=LENS_DRIVER_ID_DO_NOT_CARE;
			*pFeatureParaLen=4;
		     break;
		case SENSOR_FEATURE_SET_YUV_CMD:
			 HI704YUVSensorSetting((FEATURE_ID)*pFeatureData32, *(pFeatureData32+1));
		     break;	
		case SENSOR_FEATURE_SET_VIDEO_MODE:
		     HI704YUVSetVideoMode(*pFeatureData16);
		     break; 
		case SENSOR_FEATURE_CHECK_SENSOR_ID:
			HI704_GetSensorID(pFeatureData32); 
			break; 
		default:
			 break;			
	}
	return ERROR_NONE;
}	/* HI704FeatureControl() */


SENSOR_FUNCTION_STRUCT	SensorFuncHI704=
{
	HI704Open,
	HI704GetInfo,
	HI704GetResolution,
	HI704FeatureControl,
	HI704Control,
	HI704Close
};

UINT32 HI704_YUV_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc)
{
	/* To Do : Check Sensor status here */
	if (pfFunc!=NULL)
		*pfFunc=&SensorFuncHI704;

	return ERROR_NONE;
}	/* SensorInit() */

