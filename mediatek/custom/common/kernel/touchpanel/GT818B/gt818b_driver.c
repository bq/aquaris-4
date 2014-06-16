/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
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

#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/bitops.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/byteorder/generic.h>
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif 
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/rtpm_prio.h>

#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#ifdef MT6575
#include "tpd_custom_GT818B.h"
#include <mach/mt6575_pm_ldo.h>
#include <mach/mt6575_typedefs.h>
#include <mach/mt6575_boot.h>
#endif

#ifdef MT6577
#include "tpd_custom_GT818B.h"
#include <mach/mt_pm_ldo.h>
#include <mach/mt_typedefs.h>
#include <mach/mt_boot.h>
#endif

#include "tpd.h"
#include <cust_eint.h>
#include <linux/jiffies.h>

#ifndef TPD_NO_GPIO 
#include "cust_gpio_usage.h"
#endif

#include "gt818_fw.h"

extern struct tpd_device *tpd;

static int tpd_flag = 0;
static int tpd_halt=0;
static struct task_struct *thread = NULL;
static DECLARE_WAIT_QUEUE_HEAD(waiter);

#ifdef TPD_HAVE_BUTTON 
static int tpd_keys_local_tianma[TPD_KEY_COUNT] = TPD_KEYS_TIANMA;
static int tpd_keys_dim_local_tianma[TPD_KEY_COUNT][4] = TPD_KEYS_DIM_TIANMA;
static int tpd_keys_local_lianchuang[TPD_KEY_COUNT] = TPD_KEYS_LIANCHUANG;
static int tpd_keys_dim_local_lianchuang[TPD_KEY_COUNT][4] = TPD_KEYS_DIM_LIANCHUANG;
//zhaoshaopeng add for v_key 20120612 start
bool gt818_v_key_down = FALSE;
static int gt818b_point_num = 0;
//zhaoshaopeng add end
#endif
#if (defined(TPD_WARP_START) && defined(TPD_WARP_END))
static int tpd_wb_start_local[TPD_WARP_CNT] = TPD_WARP_START;
static int tpd_wb_end_local[TPD_WARP_CNT]   = TPD_WARP_END;
#endif
#if (defined(TPD_HAVE_CALIBRATION) && !defined(TPD_CUSTOM_CALIBRATION))
//static int tpd_calmat_local[8]     = TPD_CALIBRATION_MATRIX;
static int tpd_def_calmat_local[8] = TPD_CALIBRATION_MATRIX;
#endif

static void tpd_eint_interrupt_handler(void);
static int touch_event_handler(void *unused);
static int tpd_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id);
static int tpd_i2c_detect(struct i2c_client *client, struct i2c_board_info *info);
static int tpd_i2c_remove(struct i2c_client *client);
extern void mt65xx_eint_unmask(unsigned int line);
extern void mt65xx_eint_mask(unsigned int line);
extern void mt65xx_eint_set_hw_debounce(kal_uint8 eintno, kal_uint32 ms);
extern kal_uint32 mt65xx_eint_set_sens(kal_uint8 eintno, kal_bool sens);
extern void mt65xx_eint_registration(kal_uint8 eintno, kal_bool Dbounce_En,
                                     kal_bool ACT_Polarity, void (EINT_FUNC_PTR)(void),
                                     kal_bool auto_umask);

#ifdef CREATE_WR_NODE
extern s32 init_wr_node(struct i2c_client*);
extern void uninit_wr_node(void);
#endif

extern int  gt818_downloader( struct i2c_client *client, unsigned short ver, unsigned char * data );
//#define TPD_DITO_SENSOR
//#define TPD_CONDITION_SWITCH
#define TPD_RESET_ISSUE_WORKAROUND
#define TPD_MAX_RESET_COUNT 3
//zhaoshaopeng add to protect esd
#define ESD_PROTECT
#ifdef ESD_PROTECT
static struct delayed_work tpd_esd_check_work;
static struct workqueue_struct * tpd_esd_check_workqueue = NULL;
static void tpd_esd_check_func(struct work_struct *);
#define ESD_CHECK_CIRCLE 500
#define ESD_CHECK_DATA_LEN  6
#define ESD_CHECK_TIME      3
unsigned char esd_check_data[ESD_CHECK_TIME*ESD_CHECK_DATA_LEN];
int esd_checked_time = 0;
#endif
#define TPD_OK 0

#define TPD_CONFIG_REG_BASE           0x6A2
#define TPD_FREQ_CAL_RESULT           0x70F
#define TPD_TOUCH_INFO_REG_BASE       0x712
#define TPD_POINT_INFO_REG_BASE       0x722
#define TPD_VERSION_INFO_REG          0x713
#define TPD_VERSION_BASIC_REG         0x717
#define TPD_KEY_INFO_REG_BASE         0x721
#define TPD_POWER_MODE_REG            0x692
#define TPD_HANDSHAKING_START_REG     0xFFF
#define TPD_HANDSHAKING_END_REG       0x8000
#define TPD_FREQ_REG                  0x1522
#define TPD_SOFT_RESET_MODE     0x01
#define TPD_POINT_INFO_LEN      8
#define TPD_MAX_POINTS          5
#define MAX_TRANSACTION_LENGTH 8
#define I2C_DEVICE_ADDRESS_LEN 2
#define I2C_MASTER_CLOCK       200//400

#define TPD_KEY_INFO_REG_BASE       0x721
//zhaoshaopeng for fn00412 start 
#define GT818B_TPD_TOUCH_ID_INFO_REG_BASE    0x710
static u8 gt818_id_data;

//#define TPD_X_RES 480
//#define TPD_Y_RES 800

#ifdef MT6573
#define CHR_CON0        (0xF7000000+0x2FA00)
#endif
#ifdef MT6575
extern kal_bool upmu_is_chr_det(void);
#endif
 
#ifdef MT6577
extern kal_bool upmu_is_chr_det(void);
#endif 

#define MAX_I2C_TRANSFER_SIZE (MAX_TRANSACTION_LENGTH - I2C_DEVICE_ADDRESS_LEN)

#define GT818_CONFIG_PROC_FILE "gt818_config"
#define CONFIG_LEN (106)

struct tpd_info_t
{
    u8 vendor_id_1;
    u8 vendor_id_2;
    u8 product_id_1;
    u8 product_id_2;
    u8 version_1;
    u8 version_2;
};


//////////////////////////////////////KUUGA ADD KEY 11100801////////////////////////////////////////////////////////
//#define HAVE_TOUCH_KEY             //IF NO KEY CLOSE THIS  ;KUUGA zhaoshaopeng t zhaoshaopeng undefine this
#ifdef HAVE_TOUCH_KEY
    #define TPKEY_NUM 4
    bool gt818_key_down = FALSE;
    u16 touch_key_array[TPKEY_NUM]={0}; 
    //zhaoshaopeng for lianchuang/tianma com fn00412
    const u16 touch_key_array_lianchuang[TPKEY_NUM]={
		                                                   KEY_SEARCH,
									  KEY_BACK,				//CALL			
									  KEY_HOMEPAGE,				//HOME									  
									  KEY_MENU,				//MENU                                                                          
									 }; 
    const u16 touch_key_array_tianma[TPKEY_NUM]={
		                                                   KEY_MENU,
									  KEY_HOMEPAGE,				//CALL			
									  KEY_BACK,				//HOME									  
									  KEY_SEARCH,				//MENU                                                                          
									 }; 	
    static u8  last_key = 0;

#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef TPD_DITO_SENSOR
#else //SITO
static u8 *cfg_data_version_d = cfg_data_version_b;
static u8 *cfg_data_with_charger_version_d = cfg_data_with_charger_version_b;
#endif

static struct i2c_client *i2c_client = NULL;
static const struct i2c_device_id tpd_i2c_id[] = {{"gt818b",0},{}};
static unsigned short force[] = {0, 0xBA, I2C_CLIENT_END,I2C_CLIENT_END};
static const unsigned short * const forces[] = { force, NULL };
//static struct i2c_client_address_data addr_data = { .forces = forces,};
 static struct i2c_board_info __initdata i2c_tpd={ I2C_BOARD_INFO("gt818b", (0xBA>>1))};
static struct i2c_driver tpd_i2c_driver =
{                       
    .probe = tpd_i2c_probe,                                   
    .remove = tpd_i2c_remove,                           
    .detect = tpd_i2c_detect,                           
    .driver.name = "gt818b", 
    .id_table = tpd_i2c_id,                             
    .address_list = (const unsigned short*) forces,                        
}; 
struct tpd_info_t tpd_info;
static u8 *cfg_data = NULL;
static u8 *cfg_data_with_charger = NULL;

/* proc file system */
static int i2c_read_bytes( struct i2c_client *client, u16 addr, u8 *rxbuf, int len );
static int i2c_write_bytes( struct i2c_client *client, u16 addr, u8 *txbuf, int len );
static int i2c_write_dummy( struct i2c_client *client, u16 addr );
static struct proc_dir_entry *gt818_config_proc = NULL;

#define VELOCITY_CUSTOM_GT818B
#ifdef VELOCITY_CUSTOM_GT818B
#include <linux/device.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>

#ifndef TPD_VELOCITY_CUSTOM_X
#define TPD_VELOCITY_CUSTOM_X 10
#endif
#ifndef TPD_VELOCITY_CUSTOM_Y
#define TPD_VELOCITY_CUSTOM_Y 10
#endif

// for magnify velocity********************************************
#define TOUCH_IOC_MAGIC 'A'

#define TPD_GET_VELOCITY_CUSTOM_X _IO(TOUCH_IOC_MAGIC,0)
#define TPD_GET_VELOCITY_CUSTOM_Y _IO(TOUCH_IOC_MAGIC,1)

static int g_v_magnify_x =TPD_VELOCITY_CUSTOM_X;
static int g_v_magnify_y =TPD_VELOCITY_CUSTOM_Y;
static int tpd_misc_open(struct inode *inode, struct file *file)
{

	return nonseekable_open(inode, file);
}
/*----------------------------------------------------------------------------*/
static int tpd_misc_release(struct inode *inode, struct file *file)
{
	return 0;
}
/*----------------------------------------------------------------------------*/

static long tpd_unlocked_ioctl(struct file *file, unsigned int cmd,
       unsigned long arg)
{
	
	//char strbuf[256];
	void __user *data;
	
	long err = 0;
	
	if(_IOC_DIR(cmd) & _IOC_READ)
	{
		err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
	}
	else if(_IOC_DIR(cmd) & _IOC_WRITE)
	{
		err = !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
	}

	if(err)
	{
		printk("tpd: access error: %08X, (%2d, %2d)\n", cmd, _IOC_DIR(cmd), _IOC_SIZE(cmd));
		return -EFAULT;
	}

	switch(cmd)
	{
		case TPD_GET_VELOCITY_CUSTOM_X:
			data = (void __user *) arg;
			if(data == NULL)
			{
				err = -EINVAL;
				break;	  
			}			
			
			if(copy_to_user(data, &g_v_magnify_x, sizeof(g_v_magnify_x)))
			{
				err = -EFAULT;
				break;
			}				 
			break;

	   case TPD_GET_VELOCITY_CUSTOM_Y:
			data = (void __user *) arg;
			if(data == NULL)
			{
				err = -EINVAL;
				break;	  
			}			
			
			if(copy_to_user(data, &g_v_magnify_y, sizeof(g_v_magnify_y)))
			{
				err = -EFAULT;
				break;
			}				 
			break;


		default:
			printk("tpd: unknown IOCTL: 0x%08x\n", cmd);
			err = -ENOIOCTLCMD;
			break;
			
	}

	return err;
}


static struct file_operations tpd_fops = {
//	.owner = THIS_MODULE,
	.open = tpd_misc_open,
	.release = tpd_misc_release,
	.unlocked_ioctl = tpd_unlocked_ioctl,
};
/*----------------------------------------------------------------------------*/
static struct miscdevice tpd_misc_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "touch",
	.fops = &tpd_fops,
};

//**********************************************
#endif


static int gt818_config_read_proc(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    char *ptr = page;
    char temp_data[CONFIG_LEN] = {0};
    int i;

    ptr += sprintf( ptr, "==== GT818 config init value====\n" );

    for ( i = 0 ; i < CONFIG_LEN ; i++ )
    {
        ptr += sprintf( ptr, "0x%02X ", cfg_data[i] );

        if ( i%8 == 7 )
            ptr += sprintf( ptr, "\n" );
    }    
      
  ptr += sprintf( ptr, "\n" );
  ptr += sprintf( ptr, "==== GT818 charger init config ====\n" );
  for ( i = 0 ; i < CONFIG_LEN ; i++ )
  {
        ptr += sprintf( ptr, "0x%02X ", cfg_data_with_charger[i] );

        if ( i%8 == 7 )
    ptr += sprintf( ptr, "\n" );
  }
  ptr += sprintf( ptr, "\n" );
	ptr += sprintf( ptr, "==== GT818 config real value====\n" );
  i2c_write_dummy( i2c_client, TPD_HANDSHAKING_START_REG );	
	i2c_read_bytes( i2c_client, TPD_CONFIG_REG_BASE, temp_data, CONFIG_LEN);
  i2c_write_dummy( i2c_client, TPD_HANDSHAKING_END_REG );	
	for ( i = 0 ; i < CONFIG_LEN ; i++ )
  {
        ptr += sprintf( ptr, "0x%02X ", temp_data[i] );

        if ( i%8 == 7 )
            ptr += sprintf( ptr, "\n" );
  }    
  ptr += sprintf( ptr, "\n" );
  *eof = 1;
  return ( ptr - page );
}

static int gt818_config_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
    #ifdef MT6573
    u32 temp = *(volatile u32 *)CHR_CON0;
        
    temp &= (1<<13);
    #endif   
    #ifdef MT6575
    kal_bool temp = upmu_is_chr_det();
    #endif 

    #ifdef MT6577
    kal_bool temp = upmu_is_chr_det();
    #endif 

    TPD_DEBUG("write count %ld\n", count );

    if ( count != (CONFIG_LEN*2 ) )
    {
        TPD_DEBUG("size not match [%d:%ld]\n", CONFIG_LEN*2, count );
        return -EFAULT;
    }

    if (copy_from_user( cfg_data, buffer, count/2))
    {
        TPD_DEBUG("copy from user fail\n");
        return -EFAULT;
    }
    
    if (copy_from_user( cfg_data_with_charger, buffer + CONFIG_LEN, count/2))
    {
        TPD_DEBUG("copy from user fail\n");
        return -EFAULT;
    }
    i2c_write_dummy( i2c_client, TPD_HANDSHAKING_START_REG );

    if ( temp )
        i2c_write_bytes( i2c_client, TPD_CONFIG_REG_BASE, cfg_data_with_charger, CONFIG_LEN );
    else
        i2c_write_bytes( i2c_client, TPD_CONFIG_REG_BASE, cfg_data, CONFIG_LEN );

    i2c_write_dummy( i2c_client, TPD_HANDSHAKING_END_REG );
    return count;
}

#if 0 //#ifdef TPD_RESET_ISSUE_WORKAROUND
static int gt818_check_data(unsigned char *buffer, int count)
{
	unsigned char buf[128] = {0}; // need > sizeof(buffer)
	int i = 0, error = -1, ret = -1;

	do
	{
    i2c_write_dummy( i2c_client, TPD_HANDSHAKING_START_REG );		
		ret = i2c_read_bytes( i2c_client, TPD_CONFIG_REG_BASE, buf, CONFIG_LEN);
    i2c_write_dummy( i2c_client, TPD_HANDSHAKING_END_REG );		
		if(ret)
		{
			TPD_DMESG("read i2c error\n");
			break;
		}
		for(i = 0; i < CONFIG_LEN; i++) // the last one byte will be changed
		{
			if(buf[i] != cfg_data[i]) 
			{
				TPD_DMESG(TPD_DEVICE " fail to write touch panel config, %d bytes, expect:0x%x, real:0x%x\n", i,cfg_data[i], buf[i]);
				error = -1;
				break;
			}
		}
		if(i == CONFIG_LEN)
		{
			TPD_DMESG(TPD_DEVICE " write touch panel config OK, count:%d\n", count);
			error = 0;
			break;
		}

		if(error == -1)
		{
			for(i = 0; i < CONFIG_LEN - 1; i++)
			{
				printk("  0x%02X", buf[i]);
				if(i%8 == 7)
					printk("\n");
			}
      i2c_write_dummy( i2c_client, TPD_HANDSHAKING_START_REG );				
			i2c_write_bytes( i2c_client, TPD_CONFIG_REG_BASE, cfg_data, CONFIG_LEN );
      i2c_write_dummy( i2c_client, TPD_HANDSHAKING_END_REG );				
		}
	}while(count--);

	return error;
}

#endif
static int i2c_read_bytes( struct i2c_client *client, u16 addr, u8 *rxbuf, int len )
{
    u8 buffer[I2C_DEVICE_ADDRESS_LEN];
    u8 retry;
    u16 left = len;
    u16 offset = 0;

    struct i2c_msg msg[2] =
    {
        {
            .addr = ((client->addr&I2C_MASK_FLAG )|(I2C_ENEXT_FLAG )),
            .flags = 0,
            .buf = buffer,
            .len = I2C_DEVICE_ADDRESS_LEN,
            .timing = I2C_MASTER_CLOCK
        },
        {
            .addr = ((client->addr&I2C_MASK_FLAG )|(I2C_ENEXT_FLAG )),
            .flags = I2C_M_RD,
            .timing = I2C_MASTER_CLOCK
        },
    };

    if ( rxbuf == NULL )
        return -1;

    TPD_DEBUG("i2c_read_bytes to device %02X address %04X len %d\n", client->addr, addr, len );

    while ( left > 0 )
    {
        buffer[0] = ( ( addr+offset ) >> 8 ) & 0xFF;
        buffer[1] = ( addr+offset ) & 0xFF;

        msg[1].buf = &rxbuf[offset];

        if ( left > MAX_TRANSACTION_LENGTH )
        {
            msg[1].len = MAX_TRANSACTION_LENGTH;
            left -= MAX_TRANSACTION_LENGTH;
            offset += MAX_TRANSACTION_LENGTH;
        }
        else
        {
            msg[1].len = left;
            left = 0;
        }

        retry = 0;

        while ( i2c_transfer( client->adapter, &msg[0], 2 ) != 2 )
        {
            retry++;

            if ( retry == 20 )
            {
                TPD_DEBUG("I2C read 0x%X length=%d failed\n", addr + offset, len);
                TPD_DMESG("I2C read 0x%X length=%d failed\n", addr + offset, len);
                return -1;
            }
        }
    }

    return 0;
}

static int i2c_write_bytes( struct i2c_client *client, u16 addr, u8 *txbuf, int len )
{
    u8 buffer[MAX_TRANSACTION_LENGTH];
    u16 left = len;
    u16 offset = 0;
    u8 retry = 0;

    struct i2c_msg msg = 
    {
        .addr = ((client->addr&I2C_MASK_FLAG )|(I2C_ENEXT_FLAG )),
        .flags = 0,
        .buf = buffer,
        .timing = I2C_MASTER_CLOCK,
    };


    if ( txbuf == NULL )
        return -1;

    TPD_DEBUG("i2c_write_bytes to device %02X address %04X len %d\n", client->addr, addr, len );

    while ( left > 0 )
    {
        retry = 0;

        buffer[0] = ( (addr+offset) >> 8 ) & 0xFF;
        buffer[1] = ( addr+offset ) & 0xFF;

        if ( left > MAX_I2C_TRANSFER_SIZE )
        {
            memcpy( &buffer[I2C_DEVICE_ADDRESS_LEN], &txbuf[offset], MAX_I2C_TRANSFER_SIZE );
            msg.len = MAX_TRANSACTION_LENGTH;
            left -= MAX_I2C_TRANSFER_SIZE;
            offset += MAX_I2C_TRANSFER_SIZE;
        }
        else
        {
            memcpy( &buffer[I2C_DEVICE_ADDRESS_LEN], &txbuf[offset], left );
            msg.len = left + I2C_DEVICE_ADDRESS_LEN;
            left = 0;
        }

        TPD_DEBUG("byte left %d offset %d\n", left, offset );

        while ( i2c_transfer( client->adapter, &msg, 1 ) != 1 )
        {
            retry++;

            if ( retry == 20 )
            {
                TPD_DEBUG("I2C write 0x%X%X length=%d failed\n", buffer[0], buffer[1], len);
                TPD_DMESG("I2C write 0x%X%X length=%d failed\n", buffer[0], buffer[1], len);
                return -1;
            }
            else
                 TPD_DEBUG("I2C write retry %d addr 0x%X%X\n", retry, buffer[0], buffer[1]);

        }
    }

    return 0;
}

static int i2c_write_dummy( struct i2c_client *client, u16 addr )
{
    u8 buffer[MAX_TRANSACTION_LENGTH];

    struct i2c_msg msg =
    {
        .addr = client->addr,
        .flags = 0,
        .buf = buffer,
        .timing = I2C_MASTER_CLOCK,
        .len = 2
    };

    TPD_DEBUG("i2c_write_dummy to device %02X address %04X\n", client->addr, addr );

    buffer[0] = (addr >> 8) & 0xFF;
    buffer[1] = (addr) & 0xFF;

    i2c_transfer( client->adapter, &msg, 1 ); 

    return 0;
}

static int tpd_i2c_detect(struct i2c_client *client, struct i2c_board_info *info)
{
    strcpy(info->type, "mtk-tpd");
    return 0;
}

//
void tpd_reset_fuc(struct i2c_client *client)
{

    int err=0;
	TPD_DMESG("[mtk-tpd] tpd_reset_fuc: \n");
   //power off, need confirm with SA
    hwPowerDown(MT65XX_POWER_LDO_VGP2,  "TP");
    //hwPowerDown(MT65XX_POWER_LDO_VGP,  "TP");//zhaoshaopeng delete
	msleep(20);
	
   //power on, need confirm with SA
    hwPowerOn(MT65XX_POWER_LDO_VGP2, VOL_2800, "TP");
    //hwPowerOn(MT65XX_POWER_LDO_VGP, VOL_1800, "TP");//zhaoshaopeng delete

	
	// reset
	mt_set_gpio_mode(GPIO_CTP_RST_PIN, GPIO_CTP_RST_PIN_M_GPIO);
	mt_set_gpio_dir(GPIO_CTP_RST_PIN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_CTP_RST_PIN, GPIO_OUT_ZERO);
    msleep(1);
	
	mt_set_gpio_out(GPIO_CTP_RST_PIN, GPIO_OUT_ONE);
	msleep(20);

	
	i2c_write_dummy( client, TPD_HANDSHAKING_START_REG );
	err = i2c_read_bytes( client, TPD_VERSION_INFO_REG, (u8 *)&tpd_info, sizeof( struct tpd_info_t ) );
	i2c_write_dummy( client, TPD_HANDSHAKING_END_REG ); 	
	if ( err )
	{
			TPD_DMESG(TPD_DEVICE " fail to get tpd info %d\n", err );

			//return err;
	}
	else
	{
			TPD_DMESG( "TPD info\n");
			TPD_DMESG( "vendor %02X %02X\n", tpd_info.vendor_id_1, tpd_info.vendor_id_2 );
			TPD_DMESG( "product %02X %02X\n", tpd_info.product_id_1, tpd_info.product_id_2 );
			TPD_DMESG( "version %02X %02X\n", tpd_info.version_1, tpd_info.version_2 );
	
	}


	    // setting resolution, RES_X, RES_Y
#ifdef RES_AUTO_CONFIG
    cfg_data[59] = cfg_data_with_charger[59] = (TPD_X_RES&0xff);//(TPD_RES_X&0xff);
    cfg_data[60] = cfg_data_with_charger[60] = ((TPD_X_RES>>8)&0xff);//((TPD_RES_X>>8)&0xff);
    cfg_data[61] = cfg_data_with_charger[61] = (TPD_Y_RES&0xff);//(TPD_RES_Y&0xff);
    cfg_data[62] = cfg_data_with_charger[62] = ((TPD_Y_RES>>8)&0xff);//((TPD_RES_Y>>8)&0xff);    
#endif 
    i2c_write_dummy( client, TPD_HANDSHAKING_START_REG );
    err = i2c_write_bytes( client, TPD_CONFIG_REG_BASE, cfg_data, CONFIG_LEN );
    i2c_write_dummy( client, TPD_HANDSHAKING_END_REG );	

	TPD_DMESG("[mtk-tpd] tpd_reset_fuc: done\n");
}
//
static int tpd_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{             
    int err = 0;
    printk("\r\n zhaoshaopeng add to test gt818 tpd_i2c_probe\r\n ");
#ifdef HAVE_TOUCH_KEY
    int key;
#endif
#ifdef AUTOUPDATE_FIRMWARE
 
	u8 version_tmp[2] = {0};
	char cpf = 0;
	char i;
#endif	
	int ret = 0;
	char int_type = 0;
#ifdef TPD_RESET_ISSUE_WORKAROUND
    int reset_count = 0;

reset_proc:   
#ifdef MT6573	
    // power on CTP
    mt_set_gpio_mode(GPIO_CTP_EN_PIN, GPIO_CTP_EN_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_CTP_EN_PIN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_CTP_EN_PIN, GPIO_OUT_ONE);
#endif
#ifdef MT6575
    //power on, need confirm with SA
    hwPowerOn(MT65XX_POWER_LDO_VGP2, VOL_2800, "TP");
    //hwPowerOn(MT65XX_POWER_LDO_VGP, VOL_1800, "TP");   //zhaoshaopeng delete   
#endif
#ifdef MT6577
		//power on, need confirm with SA
		hwPowerOn(MT65XX_POWER_LDO_VGP2, VOL_2800, "TP");
		//hwPowerOn(MT65XX_POWER_LDO_VGP, VOL_1800, "TP");	  
#endif

    // set INT mode
    mt_set_gpio_mode(GPIO_CTP_EINT_PIN, GPIO_CTP_EINT_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_CTP_EINT_PIN, GPIO_DIR_IN);
    mt_set_gpio_pull_enable(GPIO_CTP_EINT_PIN, GPIO_PULL_DISABLE);

    // reset
    mt_set_gpio_mode(GPIO_CTP_RST_PIN, GPIO_CTP_RST_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_CTP_RST_PIN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_CTP_RST_PIN, GPIO_OUT_ONE);
    msleep(1);
    mt_set_gpio_out(GPIO_CTP_RST_PIN, GPIO_OUT_ZERO);
    msleep(10);

    mt_set_gpio_out(GPIO_CTP_RST_PIN, GPIO_OUT_ONE);
    msleep(20);

#else
#ifdef MT6577
		//power on, need confirm with SA
		hwPowerOn(MT65XX_POWER_LDO_VGP2, VOL_2800, "TP");
		//hwPowerOn(MT65XX_POWER_LDO_VGP, VOL_1800, "TP");  
#endif
#ifdef MT6575
    //power on, need confirm with SA
    hwPowerOn(MT65XX_POWER_LDO_VGP2, VOL_2800, "TP");
    //hwPowerOn(MT65XX_POWER_LDO_VGP, VOL_1800, "TP");   //zhaoshaopeng delete   
#endif    
    // set deep sleep off
    mt_set_gpio_mode(GPIO_CTP_RST_PIN, GPIO_CTP_RST_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_CTP_RST_PIN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_CTP_RST_PIN, GPIO_OUT_ONE);  
    msleep(10);  
#ifdef MT6573
    // power down CTP
    mt_set_gpio_mode(GPIO_CTP_EN_PIN, GPIO_CTP_EN_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_CTP_EN_PIN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_CTP_EN_PIN, GPIO_OUT_ZERO);
    msleep(10);

    // power on CTP
    mt_set_gpio_mode(GPIO_CTP_EN_PIN, GPIO_CTP_EN_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_CTP_EN_PIN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_CTP_EN_PIN, GPIO_OUT_ONE);
    msleep(10);
#endif    
#endif
     
  //zhaoshaopeng add for lianchuang/tianma tp fn00411
	i2c_write_dummy( client, TPD_HANDSHAKING_START_REG );
	ret = i2c_read_bytes( client, GT818B_TPD_TOUCH_ID_INFO_REG_BASE, (u8 *)&gt818_id_data, sizeof(gt818_id_data) );
	if (ret){
		TPD_DMESG( "TPD read error!\n");
		return ret;
	}
	i2c_write_dummy( client, TPD_HANDSHAKING_END_REG );
	//printk("\r\n zhaoshaopeng gt818_id_data = %x \r\n", gt818_id_data);
#ifdef HAVE_TOUCH_KEY
       if(gt818_id_data == 0)//tianma
       {
           memcpy(touch_key_array, touch_key_array_tianma,sizeof(touch_key_array_tianma));
           //printk("\r\n tianma touch_key_array=%d,%d,%d,%d \r\n", touch_key_array[0], touch_key_array[1],  touch_key_array[2], touch_key_array[3]);
       }
	else
       {
           memcpy(touch_key_array, touch_key_array_lianchuang,sizeof(touch_key_array_lianchuang));
           //printk("\r\n lianchuang touch_key_array=%d,%d,%d,%d \r\n", touch_key_array[0], touch_key_array[1],  touch_key_array[2], touch_key_array[3]);		   
	}
       for(key=0;key<TPKEY_NUM;key++)
              set_bit(touch_key_array[key],tpd->dev->keybit); 
#endif	   	
//zhaoshaopeng add end	


    memset( &tpd_info, 0, sizeof( struct tpd_info_t ) );
    i2c_write_dummy( client, TPD_HANDSHAKING_START_REG );
    err = i2c_read_bytes( client, TPD_VERSION_INFO_REG, (u8 *)&tpd_info, sizeof( struct tpd_info_t ) );
    i2c_write_dummy( client, TPD_HANDSHAKING_END_REG );		
    if ( err )
    {
        TPD_DMESG(TPD_DEVICE " fail to get tpd info %d\n", err );
#ifdef TPD_RESET_ISSUE_WORKAROUND
        if ( reset_count < TPD_MAX_RESET_COUNT )
        {
            reset_count++;
            goto reset_proc;
        }
#endif
        return err;
    }
    else
    {
        TPD_DMESG( "TPD info\n");
        TPD_DMESG( "vendor %02X %02X\n", tpd_info.vendor_id_1, tpd_info.vendor_id_2 );
        TPD_DMESG( "product %02X %02X\n", tpd_info.product_id_1, tpd_info.product_id_2 );
        TPD_DMESG( "version %02X %02X\n", tpd_info.version_1, tpd_info.version_2 );

#ifdef AUTOUPDATE_FIRMWARE
        //may be cause WCT time out if i2c ack error
        for(i = 0;i < 10;i++)
	      {
	
		     err = i2c_read_bytes( client, TPD_VERSION_BASIC_REG, version_tmp, 2 );
		
		     if(((tpd_info.version_1) !=version_tmp[0])||((tpd_info.version_2) != version_tmp[1]))
		     {
			     tpd_info.version_1 = version_tmp[0];
			     tpd_info.version_2 = version_tmp[1];
			     msleep(5);
			     break;
		     }
		     msleep(5);
		     cpf++;
	    }

	    if(cpf == 10)
	    {
		  TPD_DMESG( "after check version %02X %02X\n", tpd_info.version_1, tpd_info.version_2 );
	    }
	    else
	    {
		  tpd_info.version_1 = 0xff;
		  tpd_info.version_2 = 0xff;
		  TPD_DMESG( "after check version %02X %02X\n", tpd_info.version_1, tpd_info.version_2 );
	    }
      
        // check if upgradable
       gt818_downloader( client, tpd_info.version_2*256 + tpd_info.version_1, goodix_gt818_firmware );
#endif
    }

	#ifdef VELOCITY_CUSTOM_GT818B
	if((err = misc_register(&tpd_misc_device)))
	{
		printk("mtk_tpd: tpd_misc_device register failed\n");
		
	}
	#endif

    i2c_client = client;
   
    // Create proc file system
    gt818_config_proc = create_proc_entry( GT818_CONFIG_PROC_FILE, 0666, NULL);

    if ( gt818_config_proc == NULL )
    {
        TPD_DEBUG("create_proc_entry %s failed\n", GT818_CONFIG_PROC_FILE );
    }
    else 
    {
        gt818_config_proc->read_proc = gt818_config_read_proc;
        gt818_config_proc->write_proc = gt818_config_write_proc;
    }
  
#ifdef CREATE_WR_NODE
  	
    init_wr_node(client);
#endif  
//zhaoshaopeng add fn00411 for tianma/lianchuang lianchuang=2, tianma =0
    if(gt818_id_data == 0)//tianma
    {
        printk("\r\n zhaoshaopeng tianma config data \r\n");
    if ( tpd_info.version_1 < 0x7A ) // Chip version B    or Version C 
    {
           
        TPD_DMESG(TPD_DEVICE " read version %02X , use B version config\n", tpd_info.version_1 );
		TPD_DMESG(TPD_DEVICE " B version: 0x4B~0x59, C version 0x5A~0x79\n" );
            cfg_data = cfg_tianma_version_b;
            cfg_data_with_charger = cfg_tianma_with_charger_version_b;
    }
    else if ( tpd_info.version_1 < 0xA0 ) 
    {
            
            TPD_DMESG(TPD_DEVICE " read version %02X, use D version config\n", tpd_info.version_1 );
    		TPD_DMESG(TPD_DEVICE "  D version: 0x7A~0x99, E version 0x9A~0xB9\n" );
            cfg_data = cfg_tianma_version_d;
            cfg_data_with_charger = cfg_tianma_with_charger_version_d;
        }
        else
        {
            TPD_DMESG(TPD_DEVICE " unknow Chip version %02X ,use B version config\n", tpd_info.version_1 );
            cfg_data = cfg_tianma_version_b;
            cfg_data_with_charger = cfg_tianma_with_charger_version_b;
        }
    }
    else//lianchuang and others
    {
        printk("\r\n zhaoshaopeng lianchuang config data \r\n");
    if ( tpd_info.version_1 < 0x7A ) // Chip version B    or Version C 
    {
           
        TPD_DMESG(TPD_DEVICE " read version %02X , use B version config\n", tpd_info.version_1 );
		TPD_DMESG(TPD_DEVICE " B version: 0x4B~0x59, C version 0x5A~0x79\n" );
            cfg_data = cfg_lianchuang_version_b;
            cfg_data_with_charger = cfg_lianchuang_with_charger_version_b;
    }
    else if ( tpd_info.version_1 < 0xA0 ) 
    {
            
        TPD_DMESG(TPD_DEVICE " read version %02X, use D version config\n", tpd_info.version_1 );
		TPD_DMESG(TPD_DEVICE "  D version: 0x7A~0x99, E version 0x9A~0xB9\n" );
            cfg_data = cfg_lianchuang_version_d;
            cfg_data_with_charger = cfg_lianchuang_with_charger_version_d;
    }
    else
    {
        TPD_DMESG(TPD_DEVICE " unknow Chip version %02X ,use B version config\n", tpd_info.version_1 );
            cfg_data = cfg_lianchuang_version_b;
            cfg_data_with_charger = cfg_lianchuang_with_charger_version_b;
    }

    }
	
    // setting resolution, RES_X, RES_Y
#ifdef RES_AUTO_CONFIG
    cfg_data[59] = cfg_data_with_charger[59] = (TPD_X_RES&0xff);//(TPD_RES_X&0xff);
    cfg_data[60] = cfg_data_with_charger[60] = ((TPD_X_RES>>8)&0xff);//((TPD_RES_X>>8)&0xff);
    cfg_data[61] = cfg_data_with_charger[61] = (TPD_Y_RES&0xff);//(TPD_RES_Y&0xff);
    cfg_data[62] = cfg_data_with_charger[62] = ((TPD_Y_RES>>8)&0xff);//((TPD_RES_Y>>8)&0xff);    
#endif 
	int_type = ((cfg_data[55]>>3)&0x01);
  i2c_write_dummy( client, TPD_HANDSHAKING_START_REG );
  err = i2c_write_bytes( client, TPD_CONFIG_REG_BASE, cfg_data, CONFIG_LEN );
  i2c_write_dummy( client, TPD_HANDSHAKING_END_REG );	
#if 0 //#ifdef TPD_RESET_ISSUE_WORKAROUND
	err = gt818_check_data(cfg_data, TPD_MAX_RESET_COUNT); 
	if ( err )
	{
		TPD_DMESG(TPD_DEVICE " retry TPD_MAX_RESET_COUNT fail to write tpd cfg %d\n", err );
		return err;
	}
#endif

    thread = kthread_run(touch_event_handler, 0, TPD_DEVICE);

    if (IS_ERR(thread))
    { 
        err = PTR_ERR(thread);
        TPD_DMESG(TPD_DEVICE " failed to create kernel thread: %d\n", err);
    }
 

#ifndef TPD_RESET_ISSUE_WORKAROUND
    mt_set_gpio_out(GPIO_CTP_RST_PIN, GPIO_OUT_ZERO);
    msleep(10);
#endif

    // set INT mode
    mt_set_gpio_mode(GPIO_CTP_EINT_PIN, GPIO_CTP_EINT_PIN_M_EINT);
    mt_set_gpio_dir(GPIO_CTP_EINT_PIN, GPIO_DIR_IN);
    mt_set_gpio_pull_enable(GPIO_CTP_EINT_PIN, GPIO_PULL_DISABLE);
    //mt_set_gpio_pull_select(GPIO_CTP_EINT_PIN, GPIO_PULL_UP);

    msleep(50);

    mt65xx_eint_set_sens(CUST_EINT_TOUCH_PANEL_NUM, CUST_EINT_TOUCH_PANEL_SENSITIVE);
    mt65xx_eint_set_hw_debounce(CUST_EINT_TOUCH_PANEL_NUM, CUST_EINT_TOUCH_PANEL_DEBOUNCE_CN);
    //mt65xx_eint_registration(CUST_EINT_TOUCH_PANEL_NUM, CUST_EINT_TOUCH_PANEL_DEBOUNCE_EN, CUST_EINT_TOUCH_PANEL_POLARITY, tpd_eint_interrupt_handler, 1);
	if(int_type)
    {
    		mt65xx_eint_registration(CUST_EINT_TOUCH_PANEL_NUM, CUST_EINT_TOUCH_PANEL_DEBOUNCE_EN, CUST_EINT_POLARITY_HIGH, tpd_eint_interrupt_handler, 1);
    }
	else
    {
    		mt65xx_eint_registration(CUST_EINT_TOUCH_PANEL_NUM, CUST_EINT_TOUCH_PANEL_DEBOUNCE_EN, CUST_EINT_POLARITY_LOW, tpd_eint_interrupt_handler, 1);	
    }
    mt65xx_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);

#ifndef TPD_RESET_ISSUE_WORKAROUND
    mt_set_gpio_out(GPIO_CTP_RST_PIN, GPIO_OUT_ONE);
#endif

#ifdef ESD_PROTECT
	
tpd_esd_check_workqueue = create_workqueue("tpd_esd_check");
	
INIT_DELAYED_WORK(&tpd_esd_check_work, tpd_esd_check_func);
	
ret = queue_delayed_work(tpd_esd_check_workqueue, &tpd_esd_check_work,ESD_CHECK_CIRCLE);
#endif

    tpd_load_status = 1;
    
    return 0;
}

static void tpd_eint_interrupt_handler(void)
{ 
    //TPD_DMESG( "TPD interrupt!!!!!!\n" );
    TPD_DEBUG_PRINT_INT;
    tpd_flag=1;
    wake_up_interruptible(&waiter);
} 
static int tpd_i2c_remove(struct i2c_client *client)
{
#ifdef CREATE_WR_NODE
	uninit_wr_node();
#endif

#ifdef ESD_PROTECT
	destroy_workqueue(tpd_esd_check_workqueue);
#endif

    return 0;
}

#ifdef ESD_PROTECT
#if 0
static force_reset_guitar()
{
	int i;
	
#ifdef MT6575
//Power off TP
	hwPowerDown(MT65XX_POWER_LDO_VGP2, "TP");
	msleep(30);
//Power on TP
	hwPowerOn(MT65XX_POWER_LDO_VGP2, VOL_2800, "TP");
	msleep(30);
#endif

#ifdef MT6577
//Power off TP
	hwPowerDown(MT65XX_POWER_LDO_VGP2, "TP");
	msleep(30);
//Power on TP
	hwPowerOn(MT65XX_POWER_LDO_VGP2, VOL_2800, "TP");
	msleep(30);
#endif

	for ( i = 0; i < 5; i++)
	{ 
	//Reset Guitar
	mt_set_gpio_mode(GPIO_CTP_RST_PIN, GPIO_CTP_RST_PIN_M_GPIO);
	mt_set_gpio_dir(GPIO_CTP_RST_PIN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_CTP_RST_PIN, GPIO_OUT_ZERO);
	msleep(10);
	mt_set_gpio_out(GPIO_CTP_RST_PIN, GPIO_OUT_ONE);
	msleep(20);
	//Send config
	if (tpd_init_panel())
	{
		continue;
	}
	i2c_enable_commands(i2c_client, TPD_I2C_DISABLE_REG);
	break;
	}
}

#endif

static void tpd_esd_check_func(struct work_struct *work)
{   
	int ret = -1;
	int i;
	int err = 0;
	TPD_DMESG( "tpd_esd_check_func++\n");
	if (tpd_halt)
	{
	    TPD_DMESG( "tpd_esd_check_func return ..\n");
		return;
	}
	
	for (i = 0; i < 3; i++)
	{
		memset( &tpd_info, 0, sizeof( struct tpd_info_t ) );
		i2c_write_dummy( i2c_client, TPD_HANDSHAKING_START_REG );
		err = i2c_read_bytes( i2c_client, TPD_VERSION_INFO_REG, (u8 *)&tpd_info, sizeof( struct tpd_info_t ) );
		i2c_write_dummy( i2c_client, TPD_HANDSHAKING_END_REG );	

		if (err)
		{
			continue;
		}
		
		break;
	}
	
	if (i >= 3)
	{
		tpd_reset_fuc(i2c_client);
	}
	

	if(tpd_halt)
	{
		mt65xx_eint_mask(CUST_EINT_TOUCH_PANEL_NUM);
	}
	else	
	{
		queue_delayed_work(tpd_esd_check_workqueue, &tpd_esd_check_work, ESD_CHECK_CIRCLE);
	}
	TPD_DMESG( "tpd_esd_check_func--\n");
	return;
}
#endif

#if 0
static void tpd_down(int x, int y, int size, int id)
{
    //TPD_DMESG( "TPD donw id=%d\n",id-1);
    input_report_abs(tpd->dev, ABS_PRESSURE, size/100);
    input_report_key(tpd->dev, BTN_TOUCH, 1);
    input_report_abs(tpd->dev, ABS_MT_TOUCH_MAJOR, size/100);
    input_report_abs(tpd->dev, ABS_MT_WIDTH_MAJOR, 0);
    input_report_abs(tpd->dev, ABS_MT_POSITION_X, x);
    input_report_abs(tpd->dev, ABS_MT_POSITION_Y, y);
    /* track id Start 0 */
    input_report_abs(tpd->dev, ABS_MT_TRACKING_ID, id-1); 
    input_mt_sync(tpd->dev);
    //TPD_DEBUG_PRINT_POINT( x, y, 1 );
    TPD_EM_PRINT(x, y, x, y, id-1, 1);
    //printk("\r\n zhaoshaopeng add tpd_down x=%d, y=%d \r\n", x,  y);	
    if (FACTORY_BOOT == get_boot_mode()|| RECOVERY_BOOT == get_boot_mode())
    {   
        tpd_button(x, y, 1);  
    }
	if(y > TPD_RES_Y) //virtual key debounce to avoid android ANR issue
	{
         msleep(50);
		 printk("D virtual key \n");
	}
}

static void tpd_up(int x, int y, int id)
{
    //TPD_DMESG( "TPD up id=%d\n",id-1);
   
    //input_report_abs(tpd->dev, ABS_PRESSURE, 0);
    input_report_key(tpd->dev, BTN_TOUCH, 0);
    //input_report_abs(tpd->dev, ABS_MT_TOUCH_MAJOR, 0);
    //input_report_abs(tpd->dev, ABS_MT_WIDTH_MAJOR, 0);
    //input_report_abs(tpd->dev, ABS_MT_POSITION_X, x);
    //input_report_abs(tpd->dev, ABS_MT_POSITION_Y, y);
    /* track id Start 0 */
    //input_report_abs(tpd->dev, ABS_MT_TRACKING_ID, id-1);
    input_mt_sync(tpd->dev);
    TPD_EM_PRINT(x, y, x, y, id, 0);
    //printk("\r\n zhaoshaopeng add tpd_up x=%d, y=%d \r\n", x,  y);	
    if (FACTORY_BOOT == get_boot_mode()|| RECOVERY_BOOT == get_boot_mode())
    {   
        tpd_button(x, y, 0); 
    }       
    //TPD_DEBUG_PRINT_POINT( x, y, 0 );
}
#else
static  void tpd_down(int x, int y, int p) {
	// input_report_abs(tpd->dev, ABS_PRESSURE, p);
	 input_report_key(tpd->dev, BTN_TOUCH, 1);
	 input_report_abs(tpd->dev, ABS_MT_TOUCH_MAJOR, 1);
	 input_report_abs(tpd->dev, ABS_MT_POSITION_X, x);
	 input_report_abs(tpd->dev, ABS_MT_POSITION_Y, y);
	 //printk("D[%4d %4d %4d] ", x, y, p);
	 input_mt_sync(tpd->dev);
     if (FACTORY_BOOT == get_boot_mode()|| RECOVERY_BOOT == get_boot_mode())
     {   
       tpd_button(x, y, 1);  
     }
	 if(y > TPD_RES_Y) //virtual key debounce to avoid android ANR issue
	 {
         msleep(50);
		 printk("D virtual key \n");
	 }
	 TPD_EM_PRINT(x, y, x, y, p-1, 1);
 }
 
static  void tpd_up(int x, int y,int *count) {
	 //if(*count>0) {
		 //input_report_abs(tpd->dev, ABS_PRESSURE, 0);
		 input_report_key(tpd->dev, BTN_TOUCH, 0);
		 //input_report_abs(tpd->dev, ABS_MT_TOUCH_MAJOR, 0);
		 //input_report_abs(tpd->dev, ABS_MT_POSITION_X, x);
		 //input_report_abs(tpd->dev, ABS_MT_POSITION_Y, y);
		 //printk("U[%4d %4d %4d] ", x, y, 0);
		 input_mt_sync(tpd->dev);
		 TPD_EM_PRINT(x, y, x, y, 0, 0);
	//	 (*count)--;
     if (FACTORY_BOOT == get_boot_mode()|| RECOVERY_BOOT == get_boot_mode())
     {   
        tpd_button(x, y, 0); 
     }   		 

 }
#endif


#if 0//zhaoshaopeng start 20120613 for virtual_key
static int touch_event_handler(void *unused)
{
    struct sched_param param = { .sched_priority = RTPM_PRIO_TPD }; 
    int x, y, size, finger_num = 0;
	int wrap_x,wrap_y=0;
	int id=0;
    static u8 buffer[ TPD_POINT_INFO_LEN*TPD_MAX_POINTS ];
    static char buf_status;
    //static u8 id_mask = 0;
    u8 cur_mask;
	int i =0;
	char temp_data[CONFIG_LEN] = {0};
	//int err=0;
    int idx;
    static int x_history[TPD_MAX_POINTS+1];
    static int y_history[TPD_MAX_POINTS+1];
    
//////////////////////////////////////KUUGA ADD KEY 11100801////////////////////////////////////////////////////////
#ifdef HAVE_TOUCH_KEY      
    u8  key = 0;
    u8 key_return = 0;	
    unsigned int  count = 0;
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
#ifdef TPD_CONDITION_SWITCH
    u8 charger_plug = 0;
    u8 *cfg;
    u32 temp;    
#endif

    sched_setscheduler(current, SCHED_RR, &param); 

    do
    {
        set_current_state(TASK_INTERRUPTIBLE);

        while ( tpd_halt )
        {
            tpd_flag = 0;
            msleep(20);
        }

        wait_event_interruptible(waiter, tpd_flag != 0);
        tpd_flag = 0;
        TPD_DEBUG_SET_TIME;
        set_current_state(TASK_RUNNING); 
        
        i2c_write_dummy( i2c_client, TPD_HANDSHAKING_START_REG );

#ifdef TPD_CONDITION_SWITCH
        /* Workaround for common mode noise */
#ifdef MT6573        
        temp = *(volatile u32 *)CHR_CON0;
        temp &= (1<<13);
#endif
#ifdef MT6575
        temp = upmu_is_chr_det();
        //TPD_DMESG("check charge, status:%d \n", upmu_is_chr_det());
#endif     

#ifdef MT6577
        temp = upmu_is_chr_det();
        //TPD_DMESG("check charge, status:%d \n", upmu_is_chr_det());
#endif     
        cfg = NULL;

        if ( temp ) // charger is on
        {
            if ( charger_plug == 0 )
            {
                TPD_DEBUG("update configure for charger\n");
                //TPD_DMESG("update configure for charger\n");
                charger_plug = 1;
                cfg = cfg_data_with_charger;
            }
        }
        else
        {
            if ( charger_plug == 1 )
            {
                TPD_DEBUG("update configure for no charger\n");
                //TPD_DMESG("update configure for no charger\n");
                charger_plug = 0;
                cfg = cfg_data;
            }
        }

        if ( cfg )
        {
            TPD_DEBUG("[mtk-tpd] charger change  rewrite config \n");
            i2c_write_bytes( i2c_client, TPD_CONFIG_REG_BASE, cfg, CONFIG_LEN );
            i2c_write_dummy( i2c_client, TPD_HANDSHAKING_END_REG );
            continue;
        }
#endif

        i2c_read_bytes( i2c_client, TPD_TOUCH_INFO_REG_BASE, buffer, 1);
        TPD_DEBUG("[mtk-tpd] STATUS : %x\n", buffer[0]);
		
//////////////////////////////////////KUUGA ADD KEY 11100801////////////////////////////////////////////////////////
#ifdef HAVE_TOUCH_KEY      
        i2c_read_bytes( i2c_client, TPD_KEY_INFO_REG_BASE, &key, 1);
        TPD_DEBUG("[mtk-tpd] STATUS : %x\n", key);
	//printk("\r\n zhaoshaopeng for e500 key=%d \r\n", key);

        key = key&0x0f;
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        finger_num = buffer[0] & 0x0f;
        buf_status = buffer[0] & 0xf0;
     
        if ( tpd == NULL || tpd->dev == NULL )
        {
            i2c_write_dummy( i2c_client, TPD_HANDSHAKING_END_REG );  //kuuga  add 11082401
            TPD_DEBUG("[mtk-tpd] tpd=%x,  tpd->dev=%x\n",tpd, tpd->dev );
            continue;
        }
        if ( (buf_status&0x30) != 0x20 )        //data not ready£¬so return
        {
            TPD_DEBUG("[mtk-tpd] STATUS : %x\n", buffer[0]);
            TPD_DMESG("[mtk-tpd] data not ready return \n");
			tpd_reset_fuc(i2c_client);
            i2c_write_dummy( i2c_client, TPD_HANDSHAKING_END_REG );  //kuuga  add 11082401
            continue;
        }
/*
        if ( finger_num == 0x0f )        //ESD-TEST reset, send cfg again!!
        {
            TPD_DMESG("[mtk-tpd] error!! send config again\n");
            cfg = cfg_data;
            i2c_write_bytes( i2c_client, TPD_CONFIG_REG_BASE, cfg, CONFIG_LEN );
            i2c_write_dummy( i2c_client, TPD_HANDSHAKING_END_REG );  //kuuga  add 11082401
            continue;
        }
		*/
		if ( 0x0f == buffer[0] )        // send cfg again!!
        {
            TPD_DMESG("[mtk-tpd] STATUS error : %x\n", buffer[0]);
		    TPD_DMESG("[mtk-tpd] dumpt  error config: \n");
		   
		    i2c_write_dummy( i2c_client, TPD_HANDSHAKING_START_REG );	
	        i2c_read_bytes( i2c_client, TPD_CONFIG_REG_BASE, temp_data, CONFIG_LEN);
            i2c_write_dummy( i2c_client, TPD_HANDSHAKING_END_REG );	
	        for ( i = 0 ; i < CONFIG_LEN ; i++ )
            {
              printk( "0x%02X ", temp_data[i] );
              if ( i%8 == 7 )
              printk("\n" );
            }    
	
		    TPD_DMESG("[mtk-tpd] dumpt  error config done \n");
		   /*
            TPD_DMESG("[mtk-tpd]  send config again\n");
            cfg = cfg_data;
            i2c_write_bytes( i2c_client, TPD_CONFIG_REG_BASE, cfg, CONFIG_LEN );
            i2c_write_dummy( i2c_client, TPD_HANDSHAKING_END_REG );  //kuuga  add 11082401
            //
            */
            tpd_reset_fuc(i2c_client);
		   
            TPD_DMESG("[mtk-tpd] dumpt  config again : \n");
		    i2c_write_dummy( i2c_client, TPD_HANDSHAKING_START_REG );	
	        i2c_read_bytes( i2c_client, TPD_CONFIG_REG_BASE, temp_data, CONFIG_LEN);
            i2c_write_dummy( i2c_client, TPD_HANDSHAKING_END_REG );	
	        for ( i = 0 ; i < CONFIG_LEN ; i++ )
            {
              printk( "0x%02X ", temp_data[i] );
              if ( i%8 == 7 )
              printk("\n" );
            }    
		    TPD_DMESG("[mtk-tpd] dumpt config done \n");
		   
            continue;
        }
		    
        if ( finger_num > 5 )        //abnormal state £¬so return
        {
            TPD_DMESG("[mtk-tpd] finger_num =%d abnormal state  !\n",finger_num);
			TPD_DMESG("[mtk-tpd] STATUS  : %x\n", buffer[0]);
             i2c_write_dummy( i2c_client, TPD_HANDSHAKING_END_REG );  //kuuga  add 11082401
            continue;
        }
        if ( finger_num )
        {
            i2c_read_bytes( i2c_client, TPD_POINT_INFO_REG_BASE, buffer, finger_num*TPD_POINT_INFO_LEN);

        }
        else
        {
            //In order to release INT
            i2c_read_bytes( i2c_client, TPD_POINT_INFO_REG_BASE, buffer, 1);
        }
        
        cur_mask = 0;

        for ( idx = 0 ; idx < finger_num ; idx++ )
        {
            u8 *ptr = &buffer[ idx*TPD_POINT_INFO_LEN ];
            id = ptr[0];

            if ( id < TPD_MAX_POINTS+1 )
            {
                x = ptr[1] + (((int)ptr[2]) << 8);
                y = ptr[3] + (((int)ptr[4]) << 8);
                size = ptr[5] + (((int)ptr[6]) << 8);

                wrap_x = TPD_WARP_X(x);
				wrap_y = TPD_WARP_Y(y);
				
                if(TPD_X_RES== TPD_WARP_X(x))
                {
				  wrap_x = wrap_x-1;
                }
				if(0==TPD_WARP_X(x))
				{
				  wrap_x=wrap_x+1;
				}
				if(0==TPD_WARP_Y(y))
				{ 
				  wrap_y = wrap_y+1;
				}
				if(TPD_Y_RES==TPD_WARP_Y(y))
				{
				  wrap_y=wrap_y-1;
				}

                tpd_down( wrap_x, wrap_y, size, id);

                cur_mask |= ( 1 << id );
                x_history[id] = x;
                y_history[id] = y;
            }
            else
                TPD_DEBUG("Invalid id %d\n", id );
        }         
 #if 0
 // linux kernel update from 2.6.35 --> 3.0
        if ( cur_mask != id_mask )
        {
            u8 diff = cur_mask^id_mask;
            idx = 0;
            //TPD_DMESG("diff= %x\n", diff );
			//TPD_DMESG("cur_mask= %d\n", cur_mask );
			
            while ( diff )
            {
                if ( ( ( diff & 0x01 ) == 1 ) &&
                     ( ( cur_mask >> idx ) & 0x01 ) == 0 )
                {
                    // check if key release
                    tpd_up( TPD_WARP_X(x_history[idx]), TPD_WARP_Y(y_history[idx]), idx);                    
                }

                diff = ( diff >> 1 );
                idx++;
            }
            id_mask = cur_mask;
        }

		
		if ( tpd != NULL && tpd->dev != NULL )
			input_sync(tpd->dev);
#endif

#ifdef HAVE_TOUCH_KEY
/*
	if((last_key == 0)&&(key == 0))
	{
           printk("\r\n zhaoshaopeng no key \r\n");
	}
	else
	{
		for(count = 0; count < 4; count++)
		{
			//input_report_key(ts->input_dev, touch_key_array[count], !!(key&(0x01<<count)));
			input_report_key(tpd->dev, touch_key_array[count], !!(key&(0x01<<count)));
			printk("\r\n zhaoshaopeng what is this \r\n");
		}
	}		
	last_key = key;	
*/
//zhaoshaopeng add start

                     if(key)
                     {
                            if(key==8)
                            key_return = 3;
            			else if(key ==4)
            			key_return = 2;
            			else if(key ==2)
            			key_return = 1;
            			else if(key ==1)
            			key_return = 0;
            			else
            			key_return = 0xff;
                         last_key = key_return;
			    //printk("\r\n last_key = %d \r\n", last_key);
			    gt818_key_down = TRUE;
                         //printk("\r\n zhaoshaopeng what is this  key down last_key =%d, touch_key_array[key]=%d\r\n", last_key, touch_key_array[last_key]);
                         if(key_return!=0xff)
                         input_report_key(tpd->dev, touch_key_array[key_return], 1);
			}
		       else if((!key) && gt818_key_down)
		       {
		           //printk("\r\n zhaoshaopeng what is this  key up last_key =%d, touch_key_array[key]=%d\r\n", last_key, touch_key_array[last_key]);
			    if(last_key!=0xff)
                         input_report_key(tpd->dev, touch_key_array[last_key], 0);
			    gt818_key_down = FALSE;
			}
			else
			{
                        //printk("\r\n zhaoshaopeng do nothing\r\n");
			}	  
		
#endif

        // linux kernel update from 2.6.35 --> 3.0
        if(finger_num)
        {
             if ( tpd != NULL && tpd->dev != NULL )
                  input_sync(tpd->dev);
        }
		else
		{
		   if ( tpd != NULL && tpd->dev != NULL )
		   {
		      //input_mt_sync(tpd->dev);
		      tpd_up( TPD_WARP_X(x_history[idx]), TPD_WARP_Y(y_history[idx]), idx); 
		      input_sync(tpd->dev);
		   }
		}


        i2c_write_dummy( i2c_client, TPD_HANDSHAKING_END_REG );

    } while ( !kthread_should_stop() ); 

    return 0;
}
#else
struct gt818b_touch_info
{
	unsigned short y[5];
	unsigned short x[5];
	unsigned short p[5];
	unsigned short count;
};

static int touch_event_handler(void *unused)
{

    struct gt818b_touch_info cinfo;

    struct sched_param param = { .sched_priority = RTPM_PRIO_TPD }; 
    int x, y, size, finger_num = 0;
	int wrap_x,wrap_y=0;
	int id=0;
    static u8 buffer[ TPD_POINT_INFO_LEN*TPD_MAX_POINTS ];
    static char buf_status;
    //static u8 id_mask = 0;
    u8 cur_mask;
	int i =0;
	char temp_data[CONFIG_LEN] = {0};
	//int err=0;
    int idx;
    static int x_history[TPD_MAX_POINTS+1];
    static int y_history[TPD_MAX_POINTS+1];
    
//////////////////////////////////////KUUGA ADD KEY 11100801////////////////////////////////////////////////////////
#ifdef HAVE_TOUCH_KEY      
    u8  key = 0;
    u8 key_return = 0;	
    unsigned int  count = 0;
#endif
//zhaoshaopeng add 20120612 start
#ifdef TPD_HAVE_BUTTON
u8 virtual_key = 0;
#endif
//zhaoshaopeng end
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
#ifdef TPD_CONDITION_SWITCH
    u8 charger_plug = 0;
    u8 *cfg;
    u32 temp;    
#endif

    sched_setscheduler(current, SCHED_RR, &param); 

    do
    {
        set_current_state(TASK_INTERRUPTIBLE);

        while ( tpd_halt )
        {
            tpd_flag = 0;
            msleep(20);
        }

        wait_event_interruptible(waiter, tpd_flag != 0);
        tpd_flag = 0;
        TPD_DEBUG_SET_TIME;
        set_current_state(TASK_RUNNING); 
        
        i2c_write_dummy( i2c_client, TPD_HANDSHAKING_START_REG );

#ifdef TPD_CONDITION_SWITCH
        /* Workaround for common mode noise */
#ifdef MT6573        
        temp = *(volatile u32 *)CHR_CON0;
        temp &= (1<<13);
#endif
#ifdef MT6575
        temp = upmu_is_chr_det();
        //TPD_DMESG("check charge, status:%d \n", upmu_is_chr_det());
#endif        
#ifdef MT6577
		temp = upmu_is_chr_det();
		//TPD_DMESG("check charge, status:%d \n", upmu_is_chr_det());
#endif        

        cfg = NULL;

        if ( temp ) // charger is on
        {
            if ( charger_plug == 0 )
            {
                TPD_DEBUG("update configure for charger\n");
                //TPD_DMESG("update configure for charger\n");
                charger_plug = 1;
                cfg = cfg_data_with_charger;
            }
        }
        else
        {
            if ( charger_plug == 1 )
            {
                TPD_DEBUG("update configure for no charger\n");
                //TPD_DMESG("update configure for no charger\n");
                charger_plug = 0;
                cfg = cfg_data;
            }
        }

        if ( cfg )
        {
            TPD_DEBUG("[mtk-tpd] charger change  rewrite config \n");
            i2c_write_bytes( i2c_client, TPD_CONFIG_REG_BASE, cfg, CONFIG_LEN );
            i2c_write_dummy( i2c_client, TPD_HANDSHAKING_END_REG );
            continue;
        }
#endif

        i2c_read_bytes( i2c_client, TPD_TOUCH_INFO_REG_BASE, buffer, 1);
        TPD_DEBUG("[mtk-tpd] STATUS : %x\n", buffer[0]);
		
//////////////////////////////////////KUUGA ADD KEY 11100801////////////////////////////////////////////////////////
#ifdef HAVE_TOUCH_KEY      
        i2c_read_bytes( i2c_client, TPD_KEY_INFO_REG_BASE, &key, 1);
        TPD_DEBUG("[mtk-tpd] STATUS : %x\n", key);
	//printk("\r\n zhaoshaopeng for e500 key=%d \r\n", key);

        key = key&0x0f;
#endif

//zhaoshaopeng add 20120612 start
#ifdef TPD_HAVE_BUTTON
        i2c_read_bytes( i2c_client, TPD_KEY_INFO_REG_BASE, &virtual_key, 1);
        TPD_DEBUG("[mtk-tpd] STATUS : %x\n", virtual_key);
	//printk("\r\n zhaoshaopeng for e500 virtual_key=%d \r\n", virtual_key);
        virtual_key = virtual_key&0x0f;
//	printk("\r\n zhaoshaopeng for vk virtual_key = %d \r\n", virtual_key);
#endif
//zhaoshaopeng end
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        finger_num = buffer[0] & 0x0f;
        buf_status = buffer[0] & 0xf0;
     
        if ( tpd == NULL || tpd->dev == NULL )
        {
            i2c_write_dummy( i2c_client, TPD_HANDSHAKING_END_REG );  //kuuga  add 11082401
            TPD_DEBUG("[mtk-tpd] tpd=%x,  tpd->dev=%x\n",tpd, tpd->dev );
            continue;
        }
        if ( (buf_status&0x30) != 0x20 )        //data not ready£¬so return
        {
            TPD_DEBUG("[mtk-tpd] STATUS : %x\n", buffer[0]);
            TPD_DMESG("[mtk-tpd] data not ready return \n");
			tpd_reset_fuc(i2c_client);
            i2c_write_dummy( i2c_client, TPD_HANDSHAKING_END_REG );  //kuuga  add 11082401
            continue;
        }
/*
        if ( finger_num == 0x0f )        //ESD-TEST reset, send cfg again!!
        {
            TPD_DMESG("[mtk-tpd] error!! send config again\n");
            cfg = cfg_data;
            i2c_write_bytes( i2c_client, TPD_CONFIG_REG_BASE, cfg, CONFIG_LEN );
            i2c_write_dummy( i2c_client, TPD_HANDSHAKING_END_REG );  //kuuga  add 11082401
            continue;
        }
		*/
		if ( 0x0f == buffer[0] )        // send cfg again!!
        {
            TPD_DMESG("[mtk-tpd] STATUS error : %x\n", buffer[0]);
		    TPD_DMESG("[mtk-tpd] dumpt  error config: \n");
		   
		    i2c_write_dummy( i2c_client, TPD_HANDSHAKING_START_REG );	
	        i2c_read_bytes( i2c_client, TPD_CONFIG_REG_BASE, temp_data, CONFIG_LEN);
            i2c_write_dummy( i2c_client, TPD_HANDSHAKING_END_REG );	
	        for ( i = 0 ; i < CONFIG_LEN ; i++ )
            {
              printk( "0x%02X ", temp_data[i] );
              if ( i%8 == 7 )
              printk("\n" );
            }    
	
		    TPD_DMESG("[mtk-tpd] dumpt  error config done \n");
		   /*
            TPD_DMESG("[mtk-tpd]  send config again\n");
            cfg = cfg_data;
            i2c_write_bytes( i2c_client, TPD_CONFIG_REG_BASE, cfg, CONFIG_LEN );
            i2c_write_dummy( i2c_client, TPD_HANDSHAKING_END_REG );  //kuuga  add 11082401
            //
            */
            tpd_reset_fuc(i2c_client);
		   
            TPD_DMESG("[mtk-tpd] dumpt  config again : \n");
		    i2c_write_dummy( i2c_client, TPD_HANDSHAKING_START_REG );	
	        i2c_read_bytes( i2c_client, TPD_CONFIG_REG_BASE, temp_data, CONFIG_LEN);
            i2c_write_dummy( i2c_client, TPD_HANDSHAKING_END_REG );	
	        for ( i = 0 ; i < CONFIG_LEN ; i++ )
            {
              printk( "0x%02X ", temp_data[i] );
              if ( i%8 == 7 )
              printk("\n" );
            }    
		    TPD_DMESG("[mtk-tpd] dumpt config done \n");
		   
            continue;
        }
		    
        if ( finger_num > 5 )        //abnormal state £¬so return
        {
            TPD_DMESG("[mtk-tpd] finger_num =%d abnormal state  !\n",finger_num);
			TPD_DMESG("[mtk-tpd] STATUS  : %x\n", buffer[0]);
             i2c_write_dummy( i2c_client, TPD_HANDSHAKING_END_REG );  //kuuga  add 11082401
            continue;
        }
        if ( finger_num )
        {
            i2c_read_bytes( i2c_client, TPD_POINT_INFO_REG_BASE, buffer, finger_num*TPD_POINT_INFO_LEN);

        }
        else
        {
            //In order to release INT
            i2c_read_bytes( i2c_client, TPD_POINT_INFO_REG_BASE, buffer, 1);
        }
        
       //zhaoshaopeng add start
       if(virtual_key)
       {                   //we use tianma here because there is no defference
                      	if(virtual_key == 0x08)
				{
				     cinfo.x[0] = tpd_keys_dim_local_tianma[0][0];
				     cinfo.y[0] = tpd_keys_dim_local_tianma[0][1];
				     gt818b_point_num = 1;
				}
				else if(virtual_key == 0x04)
				{
				     cinfo.x[0] = tpd_keys_dim_local_tianma[1][0];
				     cinfo.y[0] = tpd_keys_dim_local_tianma[1][1];
				     gt818b_point_num = 1;
				}
				else if(virtual_key == 0x02)
				{
				     cinfo.x[0] = tpd_keys_dim_local_tianma[2][0];
				     cinfo.y[0] = tpd_keys_dim_local_tianma[2][1];
				     gt818b_point_num = 1;
				}
				else if(virtual_key == 0x01)
				{
				     cinfo.x[0] = tpd_keys_dim_local_tianma[3][0];
				     cinfo.y[0] = tpd_keys_dim_local_tianma[3][1];
				     gt818b_point_num = 1;
				}

	}
	else
	{
        cur_mask = 0;
        gt818b_point_num = finger_num;
        for ( idx = 0 ; idx < finger_num ; idx++ )
        {
            u8 *ptr = &buffer[ idx*TPD_POINT_INFO_LEN ];
            id = ptr[0];

            if ( id < TPD_MAX_POINTS+1 )
            {
                x = ptr[1] + (((int)ptr[2]) << 8);
                y = ptr[3] + (((int)ptr[4]) << 8);
                size = ptr[5] + (((int)ptr[6]) << 8);

                            wrap_x = TPD_WARP_X(x);
				wrap_y = TPD_WARP_Y(y);
				
                            if(TPD_X_RES== TPD_WARP_X(x))
                            {
				  wrap_x = wrap_x-1;
                            }
				if(0==TPD_WARP_X(x))
				{
				  wrap_x=wrap_x+1;
				}
				if(0==TPD_WARP_Y(y))
				{ 
				  wrap_y = wrap_y+1;
				}
				if(TPD_Y_RES==TPD_WARP_Y(y))
				{
				  wrap_y=wrap_y-1;
				}
                cinfo.x[idx]=wrap_x;
		  cinfo.y[idx]=wrap_y;	
                //tpd_down( wrap_x, wrap_y, size, id);

                //cur_mask |= ( 1 << id );
                //x_history[id] = x;
                //y_history[id] = y;
            }
            else
                TPD_DEBUG("Invalid id %d\n", id );
        }         
	} 
	#if 0
 // linux kernel update from 2.6.35 --> 3.0
        if ( cur_mask != id_mask )
        {
            u8 diff = cur_mask^id_mask;
            idx = 0;
            //TPD_DMESG("diff= %x\n", diff );
			//TPD_DMESG("cur_mask= %d\n", cur_mask );
			
            while ( diff )
            {
                if ( ( ( diff & 0x01 ) == 1 ) &&
                     ( ( cur_mask >> idx ) & 0x01 ) == 0 )
                {
                    // check if key release
                    tpd_up( TPD_WARP_X(x_history[idx]), TPD_WARP_Y(y_history[idx]), idx);                    
                }

                diff = ( diff >> 1 );
                idx++;
            }
            id_mask = cur_mask;
        }

		
		if ( tpd != NULL && tpd->dev != NULL )
			input_sync(tpd->dev);
#endif


        // linux kernel update from 2.6.35 --> 3.0
       {		  
           if(gt818b_point_num >0) {
                tpd_down(cinfo.x[0], cinfo.y[0], 1);
                if(gt818b_point_num>1)
             	{         tpd_down(TPD_WARP_X(cinfo.x[1]), cinfo.y[1], 2);
				if(gt818b_point_num >2)
             	{
			   	    tpd_down(TPD_WARP_X(cinfo.x[2]), cinfo.y[2], 3);
				    if(gt818b_point_num >3)
    			           {
    			   	        tpd_down(TPD_WARP_X(cinfo.x[3]), cinfo.y[3], 4);
					 if(gt818b_point_num >4)
    			              {
    			   	            tpd_down(TPD_WARP_X(cinfo.x[4]), cinfo.y[4], 5);
    			              }
    			           }
				}
             	}
                input_sync(tpd->dev);
				TPD_DEBUG("press --->\n");
				
            }else{
			    tpd_up(cinfo.x[0], cinfo.y[0], 0);
                //TPD_DEBUG("release --->\n"); 
                //input_mt_sync(tpd->dev);
                input_sync(tpd->dev);
            }
        }

        i2c_write_dummy( i2c_client, TPD_HANDSHAKING_END_REG );

    } while ( !kthread_should_stop() ); 

    return 0;
}
#endif
static int tpd_local_init(void) 
{

    if(i2c_add_driver(&tpd_i2c_driver)!=0)
    {
        TPD_DMESG("unable to add i2c driver.\n");
        return -1;
    }
    if(tpd_load_status == 0) //if(tpd_load_status == 0) // disable auto load touch driver for linux3.0 porting
    {
    	TPD_DMESG("add error touch panel driver.======gt818\n");
    	i2c_del_driver(&tpd_i2c_driver);
    	return -1;
    }

#if 0//def TPD_HAVE_BUTTON      zhaoshaopeng remove to probe
    tpd_button_setting(TPD_KEY_COUNT, tpd_keys_local, tpd_keys_dim_local);// initialize tpd button data
#endif   
#ifdef TPD_HAVE_BUTTON //     zhaoshaopeng add 20120613 for vk
       if(gt818_id_data == 0)//tianma
       {
           tpd_button_setting(TPD_KEY_COUNT, tpd_keys_local_tianma, tpd_keys_dim_local_tianma);// initialize tpd button data
           //printk("\r\n tianma touch_key_array=%d,%d,%d,%d \r\n", touch_key_array[0], touch_key_array[1],  touch_key_array[2], touch_key_array[3]);
       }
	else
       {
           tpd_button_setting(TPD_KEY_COUNT, tpd_keys_local_lianchuang, tpd_keys_dim_local_lianchuang);// initialize tpd button data
           //printk("\r\n lianchuang touch_key_array=%d,%d,%d,%d \r\n", touch_key_array[0], touch_key_array[1],  touch_key_array[2], touch_key_array[3]);		   
	}
#endif  

  
#if (defined(TPD_WARP_START) && defined(TPD_WARP_END))    
    TPD_DO_WARP = 1;
    memcpy(tpd_wb_start, tpd_wb_start_local, TPD_WARP_CNT*4);
    memcpy(tpd_wb_end, tpd_wb_start_local, TPD_WARP_CNT*4);
#endif 

#if (defined(TPD_HAVE_CALIBRATION) && !defined(TPD_CUSTOM_CALIBRATION))
    memcpy(tpd_calmat, tpd_def_calmat_local, 8*4);
    memcpy(tpd_def_calmat, tpd_def_calmat_local, 8*4);	
#endif  

    // set vendor string
    tpd->dev->id.vendor =  (tpd_info.vendor_id_2 << 8 ) | tpd_info.vendor_id_1;
    tpd->dev->id.product = (tpd_info.product_id_2 << 8 ) | tpd_info.product_id_1;
    tpd->dev->id.version = (tpd_info.version_2 << 8 ) | tpd_info.version_1;

    TPD_DMESG("end %s, %d\n", __FUNCTION__, __LINE__);  
    tpd_type_cap = 1;

    return 0;
}

/* Function to manage low power suspend */
//void tpd_suspend(struct i2c_client *client, pm_message_t message)
static void tpd_suspend( struct early_suspend *h )
{
    u8 mode = 0x01;
	TPD_DMESG("tpd_suspend\n");  
#if 0 // workaround for force tpd into sleep mode   
    mt_set_gpio_mode(GPIO_CTP_EINT_PIN, GPIO_CTP_EINT_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_CTP_EINT_PIN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_CTP_EINT_PIN, GPIO_OUT_ZERO);
#endif
    tpd_halt = 1;     
#ifdef ESD_PROTECT
	//cancel_delayed_work(tpd_esd_check_workqueue);
#endif

    msleep(1);     
    i2c_write_dummy( i2c_client, TPD_HANDSHAKING_START_REG );
    i2c_write_bytes( i2c_client, TPD_POWER_MODE_REG, &mode, 1 );
    i2c_write_dummy( i2c_client, TPD_HANDSHAKING_END_REG );    
    mt65xx_eint_mask(CUST_EINT_TOUCH_PANEL_NUM);
#if 0//#ifdef MT6575, using PMIC Power off leakage 0.3mA, sleep mode: 0.1mA    
    hwPowerDown(MT65XX_POWER_LDO_VGP2, "TP");
    hwPowerDown(MT65XX_POWER_LDO_VGP, "TP");
    /* Pull down EINT PIN and RST PIN */    
    mt_set_gpio_mode(GPIO_CTP_EINT_PIN, GPIO_CTP_EINT_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_CTP_EINT_PIN, GPIO_DIR_IN);
    mt_set_gpio_pull_enable(GPIO_CTP_EINT_PIN, GPIO_PULL_DISABLE);
    mt_set_gpio_pull_select(GPIO_CTP_EINT_PIN, GPIO_PULL_DOWN);  
    mt_set_gpio_mode(GPIO_CTP_RST_PIN, GPIO_CTP_EINT_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_CTP_RST_PIN, GPIO_DIR_IN);
    mt_set_gpio_pull_enable(GPIO_CTP_RST_PIN, GPIO_PULL_DISABLE);
    mt_set_gpio_pull_select(GPIO_CTP_RST_PIN, GPIO_PULL_DOWN);      
#endif    
    //mt_set_gpio_out(GPIO_CTP_RST_PIN, GPIO_OUT_ZERO);
    //return 0;
    TPD_DMESG("tpd_suspend ok\n"); 
}

/* Function to manage power-on resume */
//void tpd_resume(struct i2c_client *client)
static void tpd_resume( struct early_suspend *h )
{
#ifdef TPD_RESET_ISSUE_WORKAROUND
		struct tpd_info_t tpd_info;
		int err;
		
#endif
    TPD_DMESG(TPD_DEVICE " tpd_resume start \n"); 	
#if 0 //#ifdef MT6575, using PMIC Power off leakage 0.3mA, sleep mode: 0.1mA   
    hwPowerOn(MT65XX_POWER_LDO_VGP2, VOL_2800, "TP");
    hwPowerOn(MT65XX_POWER_LDO_VGP, VOL_1800, "TP"); 
#endif    
#ifdef TPD_RESET_ISSUE_WORKAROUND
    
#if 1 //#ifdef MT6573, using PMIC Power off leakage 0.3mA, sleep mode: 0.1mA      
    // use raising edge of INT to wakeup
    mt_set_gpio_mode(GPIO_CTP_EINT_PIN, GPIO_CTP_EINT_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_CTP_EINT_PIN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_CTP_EINT_PIN, GPIO_OUT_ZERO);
    msleep(1);
    mt_set_gpio_out(GPIO_CTP_EINT_PIN, GPIO_OUT_ONE);
    msleep(1);
    mt_set_gpio_out(GPIO_CTP_EINT_PIN, GPIO_OUT_ZERO);
    msleep(1);
    mt_set_gpio_mode(GPIO_CTP_EINT_PIN, GPIO_CTP_EINT_PIN_M_EINT);
    mt_set_gpio_dir(GPIO_CTP_EINT_PIN, GPIO_DIR_IN);
 #endif
#if 0 //#ifdef MT6575, using PMIC Power off leakage 0.3mA, sleep mode: 0.1mA 
     // set INT mode
    mt_set_gpio_mode(GPIO_CTP_EINT_PIN, GPIO_CTP_EINT_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_CTP_EINT_PIN, GPIO_DIR_IN);
    mt_set_gpio_pull_enable(GPIO_CTP_EINT_PIN, GPIO_PULL_DISABLE);
#endif//zhaoshaopeng add
    // reset
    mt_set_gpio_mode(GPIO_CTP_RST_PIN, GPIO_CTP_RST_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_CTP_RST_PIN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_CTP_RST_PIN, GPIO_OUT_ONE);
    msleep(1);
    mt_set_gpio_out(GPIO_CTP_RST_PIN, GPIO_OUT_ZERO);
    msleep(10);

    mt_set_gpio_out(GPIO_CTP_RST_PIN, GPIO_OUT_ONE);
    msleep(20);// msleep(20)
#if 0//zhaoshaopeng add     
        // set INT mode
    mt_set_gpio_mode(GPIO_CTP_EINT_PIN, GPIO_CTP_EINT_PIN_M_EINT);
    mt_set_gpio_dir(GPIO_CTP_EINT_PIN, GPIO_DIR_IN);
    mt_set_gpio_pull_enable(GPIO_CTP_EINT_PIN, GPIO_PULL_DISABLE);
    //mt_set_gpio_pull_select(GPIO_CTP_EINT_PIN, GPIO_PULL_UP);
#endif   
#endif

    mt65xx_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM); 
    //mt_set_gpio_out(GPIO_CTP_RST_PIN, GPIO_OUT_ONE);

#ifdef TPD_RESET_ISSUE_WORKAROUND

    i2c_write_dummy( i2c_client, TPD_HANDSHAKING_START_REG );    
    //TODO: Remove the code after the stabliity test is passed
    memset( &tpd_info, 0, sizeof( struct tpd_info_t ) );
    err = i2c_read_bytes( i2c_client, TPD_VERSION_INFO_REG, (u8 *)&tpd_info, sizeof( struct tpd_info_t ) );

    if ( err )
    {
        TPD_DMESG(TPD_DEVICE " fail to get tpd info %d\n", err ); 
	 tpd_reset_fuc(i2c_client);
    }
    else
    {
        TPD_DMESG( "TPD info\n");
        TPD_DMESG( "vendor %02X %02X\n", tpd_info.vendor_id_1, tpd_info.vendor_id_2 );
        TPD_DMESG( "product %02X %02X\n", tpd_info.product_id_1, tpd_info.product_id_2 );
        TPD_DMESG( "version %02X %02X\n", tpd_info.version_1, tpd_info.version_2 );
    }
      
    i2c_write_dummy( i2c_client, TPD_HANDSHAKING_END_REG );
#endif
    tpd_halt = 0;
#ifdef ESD_PROTECT
	queue_delayed_work(tpd_esd_check_workqueue, &tpd_esd_check_work,ESD_CHECK_CIRCLE);
#endif

TPD_DMESG(TPD_DEVICE " tpd_resume end \n" ); 	    
    //return 0;
}

static struct tpd_driver_t tpd_device_driver =
{
    .tpd_device_name = "gt818",
    .tpd_local_init = tpd_local_init,
    .suspend = tpd_suspend,
    .resume = tpd_resume,
#ifdef TPD_HAVE_BUTTON
    .tpd_have_button = 1,
#else
    .tpd_have_button = 0,
#endif		
};

/* called when loaded into kernel */
static int __init tpd_driver_init(void)
{
    TPD_DMESG("MediaTek gt818 touch panel driver init\n");
    i2c_register_board_info(0, &i2c_tpd, 1);
    if ( tpd_driver_add(&tpd_device_driver) < 0)
        TPD_DMESG("add generic driver failed\n");

    return 0;
}

/* should never be called */
static void __exit tpd_driver_exit(void)
{
    TPD_DMESG("MediaTek gt818 touch panel driver exit\n");
    //input_unregister_device(tpd->dev);
    tpd_driver_remove(&tpd_device_driver);
}

module_init(tpd_driver_init);
module_exit(tpd_driver_exit);

