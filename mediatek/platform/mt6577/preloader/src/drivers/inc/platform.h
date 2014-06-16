/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 * 
 * MediaTek Inc. (C) 2010. All rights reserved.
 * 
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#ifndef PLATFORM_H
#define PLATFORM_H

#include "print.h"
#include "string.h"
#include "boot_device.h"
#include "blkdev.h"
#include "preloader.h"

#include "mt6577.h"

#include "cust_bldr.h"
#include "cust_part.h"
#include "cust_rtc.h"
#include "cust_nand.h"
#include "cust_usb.h"
#include "sec_limit.h"

/*=======================================================================*/
/* Pre-Loader Internal Features                                          */
/*=======================================================================*/
#define CFG_HW_WATCHDOG                 (1)
#define CFG_BOOT_ARGUMENT               (1)
#define CFG_RAM_CONSOLE                 (1)
#define CFG_MDJTAG_SWITCH               (0)
#define CFG_MDMETA_DETECT               (0)
#define CFG_MDWDT_DISABLE               (1)
#define CFG_REBOOT_TEST                 (0)
#define CFG_SYS_STACK_SZ                (3*1024)

/* if not defined in cust_bldr.h, use default setting */
#if !defined(CFG_FPGA_PLATFORM)
#define CFG_FPGA_PLATFORM               (0)
#endif

/* if not defined in cust_bldr.h, use default setting */
#if !defined(CFG_BATTERY_DETECT)
#define CFG_BATTERY_DETECT              (1)
#endif

/* if not defined in cust_bldr.h, use default setting */
#if !defined(CFG_EMERGENCY_DL_SUPPORT)
#define CFG_EMERGENCY_DL_SUPPORT        (1)
#endif
#if !defined(CFG_EMERGENCY_DL_TIMEOUT_MS)
#define CFG_EMERGENCY_DL_TIMEOUT_MS     (1000 * 30)      /* 30s */
#endif
#if !defined(CFG_EMERGENCY_DL_MAX_TIMEOUT_MS)
#define CFG_EMERGENCY_DL_MAX_TIMEOUT_MS (1000 * 60 * 60) /* 1hour */
#endif

/* if not defined in cust_usb.h, use default setting */
#if !defined(CFG_USB_ENUM_TIMEOUT)
#define CFG_USB_ENUM_TIMEOUT            (8000)           /* 8s */
#endif

/* if not defined in cust_usb.h, use default setting */
#if !defined(CFG_USB_HANDSHAKE_TIMEOUT)
#define CFG_USB_HANDSHAKE_TIMEOUT       (2500)           /* 2.5s */
#endif

/* if not defined in cust_bldr.h, use default setting */
#if !defined(CFG_USBIF_COMPLIANCE)
#define CFG_USBIF_COMPLIANCE            (0)
#endif

/* support switch to modem com or not */
#if defined(MTK_DT_SUPPORT) & !defined(EVDO_DT_SUPPORT)
#define CFG_DT_MD_DOWNLOAD              (1)
#else
#define CFG_DT_MD_DOWNLOAD              (0)
#endif

/* for emmc, it doesn't support legacy usb download. */
#ifdef MTK_EMMC_SUPPORT
#define CFG_LEGACY_USB_DOWNLOAD         (0)
#endif

/* if not defined in cust_bldr.h, use default setting */
#if !defined(CFG_LEGACY_USB_DOWNLOAD)
#define CFG_LEGACY_USB_DOWNLOAD         (0)
#endif

#if CFG_LEGACY_USB_DOWNLOAD
#define FEATURE_DOWNLOAD_BOUNDARY_CHECK
#endif

#ifdef SLT_BOOTLOADER
#define CFG_LOAD_MD_ROM                 (1)
#define CFG_LOAD_DSP_ROM                (1)
#define CFG_LOAD_AP_ROM                 (1)
#define CFG_LOAD_UBOOT                  (0)

#undef  CFG_BOOT_ARGUMENT
#undef  CFG_BATTERY_DETECT
#undef  CFG_EMERGENCY_DL_SUPPORT
#undef  CFG_RAM_CONSOLE
#undef  CFG_UART_TOOL_HANDSHAKE
#undef  CFG_USB_TOOL_HANDSHAKE
#undef  CFG_USB_DOWNLOAD
#undef  CFG_PMT_SUPPORT

#define CFG_BOOT_ARGUMENT               (0)
#define CFG_BATTERY_DETECT              (0)
#define CFG_EMERGENCY_DL_SUPPORT        (0)
#define CFG_RAM_CONSOLE                 (0)
#define CFG_UART_TOOL_HANDSHAKE         (0)
#define CFG_USB_TOOL_HANDSHAKE          (0)
#define CFG_USB_DOWNLOAD                (0)
#define CFG_PMT_SUPPORT                 (0)
#else
#define CFG_LOAD_MD_ROM                 (0)
#define CFG_LOAD_DSP_ROM                (0)
#define CFG_LOAD_AP_ROM                 (0)
#define CFG_LOAD_UBOOT                  (1)
#endif

/*=======================================================================*/
/* Platform Setting                                                      */
/*=======================================================================*/
#if CFG_FPGA_PLATFORM
#define I2C_SRC_CLK_FRQ                 (12000000)
#define UART_SRC_CLK_FRQ                (12000000)
#define MSDC_SRC_CLK_FRQ                (12000000)

#undef CFG_MDWDT_DISABLE
#define CFG_MDWDT_DISABLE               (0)

#else /* !CFG_FPGA_PLATFORM */
#define UART_SRC_CLK_FRQ                (0)         /* use default */
#endif

/*=======================================================================*/
/* Image Address                                                         */	
/*=======================================================================*/
#define CFG_DA_RAM_ADDR                 (0x00000800)
#define CFG_UBOOT_MEMADDR               (0x01E00000)
#define CFG_AP_ROM_MEMADDR              (0x02200000)
#define CFG_MD_2G_ROM_MEMADDR           (0x00000000)
#define CFG_MD_3G_ROM_MEMADDR           (0x00000000)
#define CFG_DSP_2G_ROM_MEMADDR          (0x00700000)
#define CFG_DSP_3G_ROM_MEMADDR          (0x01300000)

/*=======================================================================*/
/* Common buffer                                                         */	
/*=======================================================================*/
#define BOOT_ARGUMENT_ADDR              (0x000A0000)   /* sync with u-boot */
#define BOOT_ARGUMENT_MAX_SIZE          (1024)

#ifdef SLT_BOOTLOADER
#define SEC_REGION_ADDR                 (0x08800000)
#define STORAGE_BUFFER_ADDR             (0x08000000)
#define COMMON_BUFFER_ADDR              (0x08010000)
#define NAND_BUFFER_START               (0x08020000)
#define LOG_BUFFER_START                (0x08030000)
#else
#define SEC_REGION_ADDR                 (0x00800000)
#define STORAGE_BUFFER_ADDR             (0x04000000)
#define COMMON_BUFFER_ADDR              (0x04010000)
#define NAND_BUFFER_START               (0x04020000)
#define LOG_BUFFER_START                (0x04030000)
#ifdef MTK_EMMC_SUPPORT
#define EMMC_PMT_BUFFER_START            (0x4040000)
#define EMMC_PMT_BUFFER_SIZE             (0x20000)
#endif
#endif
#define SEC_REGION_MAX_SIZE             (0x00800000)
#define STORAGE_BUFFER_MAX_SIZE         (65536)
#define COMMON_BUFFER_MAX_SIZE          (4500)
#define NAND_BUFFER_LENGTH              (0x10000)
#define LOG_BUFFER_MAX_SIZE             (0x10000)

#ifdef MT6575
#define RAM_CONSOLE_ADDR                (0xF000C000)
#define RAM_CONSOLE_MAX_SIZE            (0x4000-0xC)
#else
#define RAM_CONSOLE_ADDR                (0xF0006000)
#define RAM_CONSOLE_MAX_SIZE            (0x2400-0xC)
#endif

typedef enum {
    NORMAL_BOOT         = 0,
    META_BOOT           = 1,
    RECOVERY_BOOT       = 2,
    SW_REBOOT           = 3,
    FACTORY_BOOT        = 4,
    ADVMETA_BOOT        = 5,
    ATE_FACTORY_BOOT    = 6,
    ALARM_BOOT          = 7,

    DOWNLOAD_BOOT       = 100,
    UNKNOWN_BOOT
} boot_mode_t;

typedef enum {
    BR_POWER_KEY = 0,
    BR_USB,
    BR_RTC,
    BR_WDT,
    BR_WDT_BY_PASS_PWK,
    BR_TOOL_BY_PASS_PWK,
    BR_UNKNOWN
} boot_reason_t;

typedef enum {
    META_UNKNOWN_COM = 0,
    META_UART_COM,
    META_USB_COM
} meta_com_t;

/* boot argument magic */
#define BOOT_ARGUMENT_MAGIC             (0x504c504c)

typedef struct {
    u32 addr;   /* download agent address */
    u32 arg1;   /* download agent argument 1 */
    u32 arg2;   /* download agent argument 2 */
} da_info_t;

typedef struct {
    u32 magic;
    boot_mode_t mode;
    u32 e_flag;
    u32 log_port;
    u32 log_baudrate;
    u8  log_enable;
    u8  reserved[3];
    u32 dram_rank_num;
    u32 dram_rank_size[4];
    u32 boot_reason;
    u32 meta_com_type;
    u32 meta_com_id;
    u32 boot_time;
    da_info_t da_info;
    SEC_LIMIT sec_limit;
} boot_arg_t;

typedef enum {
    CHIP_VER_E1 = 0xca00,
    CHIP_VER_E2 = 0xcb00,
    CHIP_VER_E3 = 0xcc00, 
    CHIP_6575_E1 = CHIP_VER_E1,
    CHIP_6575_E2 = CHIP_VER_E2,
    CHIP_6575_E3 = CHIP_VER_E3,
    CHIP_6577_E1 = 0xcd00
} chip_ver_t;

typedef enum {
    COM_UART = 0,
    COM_USB
} comport_t;

struct comport_ops {
    int (*send)(u8 *buf, u32 len);
    int (*recv)(u8 *buf, u32 len, u32 tmo);
};

struct bldr_comport {
    u32 type;
    u32 tmo;
    struct comport_ops *ops;
};

struct bldr_command {
    u8  *data;
    u32  len;
};

struct bldr_command_handler;

typedef bool (*bldr_cmd_handler_t)(struct bldr_command_handler *handler, struct bldr_command *cmd, struct bldr_comport *comm);

struct bldr_command_handler {
    void *priv;
    u32   attr;
    bldr_cmd_handler_t cb;
};

#define CMD_HNDL_ATTR_COM_FORBIDDEN               (1<<0)

extern int usb_cable_in(void);

extern int  platform_get_mcp_id(u8 *id, u32 len);
extern void platform_vusb_on(void);
extern void platform_wdt_kick(void);
extern void platform_wdt_all_kick(void);
extern void platform_pre_init(void);
extern void platform_init(void);
extern void platform_post_init(void);
extern void platform_set_boot_args(void);
extern void platform_error_handler(void);
extern void platform_assert(char *file, int line, char *expr);
extern chip_ver_t platform_chip_ver(void);

extern void apmcu_dcache_clean_invalidate(void);
extern void apmcu_dsb(void);
extern void apmcu_dcache_invalidate(void);
extern void apmcu_disable_dcache(void);
extern void apmcu_disable_icache(void);    
extern void apmcu_icache_invalidate(void);
extern void jump(u32 addr, u32 arg1, u32 arg2);

extern boot_mode_t g_boot_mode;
extern boot_dev_t  g_boot_dev;
extern boot_reason_t g_boot_reason;
extern meta_com_t g_meta_com_type;
extern u32 g_meta_com_id;

#endif /* PLATFORM_H */
