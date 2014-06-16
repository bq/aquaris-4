/*
 * Copyright (c) 2012, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of Google, Inc. nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <debug.h>
#include <dev/uart.h>
#include <arch/arm/mmu.h>
#include <arch/ops.h>

#include <platform/boot_mode.h>
#include <platform/mt_reg_base.h>
#include <mt_partition.h>
#include <platform/mt_pmic.h>
#include <platform/i2c.h>
#include <video.h>
#include <target/board.h>
#include <platform/mt_logo.h>
#include <platform/mt_gpio.h>

//#define LK_DL_CHECK

#ifdef LK_DL_CHECK
/*block if check dl fail*/
#undef LK_DL_CHECK_BLOCK_LEVEL
#endif

extern void platform_early_init_timer();

/* Transparent to DRAM customize */
int g_nr_bank;
int g_rank_size[4];
unsigned int g_fb_base;
unsigned int g_fb_size;
BOOT_ARGUMENT *g_boot_arg;
extern BOOT_ARGUMENT *g_boot_arg;
BI_DRAM bi_dram[MAX_NR_BANK];
extern void jump_da(u32 addr, u32 arg1, u32 arg2);

int dram_init(void)
{
    int i, index, num_record;
    unsigned int dram_rank_num;

    //FIXME: Hard coded temp version for LK porting
#if 0
    g_nr_bank = 2;
    bi_dram[0].start = RIL_SIZE;
    bi_dram[0].size = 0x10000000 - RIL_SIZE;
    bi_dram[1].start = 0x10000000;
    bi_dram[1].size = 0x10000000;
#endif
    //Get parameters from pre-loader. Get as early as possible
    g_boot_arg = (volatile BOOT_ARGUMENT *)(BOOT_ARGUMENT_LOCATION);

#if 1

    dram_rank_num = g_boot_arg->dram_rank_num;

    g_nr_bank = dram_rank_num;

    for (i = 0; i < g_nr_bank; i++)
    {
        g_rank_size[i] = g_boot_arg->dram_rank_size[i];
    }

    if (g_nr_bank == 1)
    {
        bi_dram[0].start = RIL_SIZE;
        bi_dram[0].size = g_rank_size[0] - RIL_SIZE;
    } else if (g_nr_bank == 2)
    {
        bi_dram[0].start = RIL_SIZE;
        bi_dram[0].size = g_rank_size[0] - RIL_SIZE;
        bi_dram[1].start = g_rank_size[0];
        bi_dram[1].size = g_rank_size[1];
    } else if (g_nr_bank == 3)
    {
        bi_dram[0].start = RIL_SIZE;
        bi_dram[0].size = g_rank_size[0] - RIL_SIZE;
        bi_dram[1].start = g_rank_size[0];
        bi_dram[1].size = g_rank_size[1];
        bi_dram[2].start = bi_dram[1].start + bi_dram[1].size;
        bi_dram[2].size = g_rank_size[2];
    } else if (g_nr_bank == 4)
    {
        bi_dram[0].start = RIL_SIZE;
        bi_dram[0].size = g_rank_size[0] - RIL_SIZE;
        bi_dram[1].start = g_rank_size[0];
        bi_dram[1].size = g_rank_size[1];
        bi_dram[2].start = bi_dram[1].start + bi_dram[1].size;
        bi_dram[2].size = g_rank_size[2];
        bi_dram[3].start = bi_dram[2].start + bi_dram[2].size;
        bi_dram[3].size = g_rank_size[3];
    } else
    {
        //dprintf(CRITICAL, "[LK ERROR] DRAM bank number is not correct!!!");
        while (1) ;
    }
#endif
}

/*******************************************************
 * Routine: memory_size
 * Description: return DRAM size to LCM driver
 ******************************************************/
u32 memory_size(void)
{
    int nr_bank = g_nr_bank;
    int i, size = 0;

    for (i = 0; i < nr_bank; i++)
        size += bi_dram[i].size;
    size += RIL_SIZE;

    return size;
}

void sw_env()
{
    int dl_status = 0;

#ifdef LK_DL_CHECK
#ifdef MTK_EMMC_SUPPORT
    dl_status = mmc_get_dl_info();
    printf("mt65xx_sw_env--dl_status: %d\n", dl_status);
    if (dl_status != 0)
    {
        video_printf("=> TOOL DL image Fail!\n");
        printf("TOOL DL image Fail\n");
#ifdef LK_DL_CHECK_BLOCK_LEVEL
        printf("uboot is blocking by dl info\n");
        while (1) ;
#endif
    }
#endif
#endif

#ifndef USER_BUILD
    switch (g_boot_mode)
    {
      case META_BOOT:
          video_printf(" => META MODE\n");
          break;
      case FACTORY_BOOT:
          video_printf(" => FACTORY MODE\n");
          break;
      case RECOVERY_BOOT:
          video_printf(" => RECOVERY MODE\n");
          break;
      case SW_REBOOT:
          //video_printf(" => SW RESET\n");
          break;
      case NORMAL_BOOT:
          video_printf(" => NORMAL BOOT\n");
          break;
      case ADVMETA_BOOT:
          video_printf(" => ADVANCED META MODE\n");
          break;
      case ATE_FACTORY_BOOT:
          video_printf(" => ATE FACTORY MODE\n");
          break;
      case ALARM_BOOT:
          video_printf(" => ALARM BOOT\n");
          break;
      case FASTBOOT:
          video_printf(" => FASTBOOT mode...\n");
          break;
      default:
          video_printf(" => UNKNOWN BOOT\n");
    }
    return 0;
#endif

}

void platform_init_mmu_mappings(void)
{
  /* configure available RAM banks */
  dram_init();
  
/* Enable D-cache  */
#if 1
  unsigned int addr;
  unsigned int i = 0;
  unsigned int dram_size = 0;

  dram_size = memory_size();

  /* do some memory map initialization */
  for (addr = bi_dram[0].start; addr < dram_size; addr += (1024*1024)) 
  {
    /*virtual to physical 1-1 mapping*/
    arm_mmu_map_section(addr, addr, MMU_MEMORY_TYPE_NORMAL_WRITE_BACK_ALLOCATE | MMU_MEMORY_AP_READ_WRITE);
  }
#endif  
}

void platform_early_init(void)
{
#ifdef LK_PROFILING
    unsigned int time_led_init;
    unsigned int time_pmic6329_init;
    unsigned int time_i2c_init;
    unsigned int time_disp_init;
    unsigned int time_platform_early_init;
    time_platform_early_init = get_timer(0);
#endif
    /* initialize the uart */
    uart_init_early();        

    /* initialize the frame buffet information */
    g_fb_size = mt_disp_get_vram_size();
    g_fb_base = memory_size() - g_fb_size;
    dprintf(INFO, "FB base = 0x%x, FB size = %d\n", g_fb_base, g_fb_size);

    platform_init_interrupts();
    platform_early_init_timer();
    mt_gpio_set_default();
#ifdef LK_PROFILING
    time_i2c_init = get_timer(0);
#endif
    i2c_v1_init();
#ifdef LK_PROFILING
    printf("[PROFILE] ------- i2c init takes %d ms -------- \n", get_timer(time_i2c_init));
#endif

    mtk_wdt_init();
#ifdef LK_PROFILING
    time_led_init = get_timer(0);
#endif
    leds_init();
#ifdef LK_PROFILING
      printf("[PROFILE] ------- led init takes %d ms -------- \n", get_timer(time_led_init));
#endif

    isink0_init();              //turn on PMIC6329 isink0
#ifdef LK_PROFILING
    time_disp_init = get_timer(0);
#endif
    mt_disp_init((void *)g_fb_base);
#ifdef LK_PROFILING
    printf("[PROFILE] ------- disp init takes %d ms -------- \n", get_timer(time_disp_init));
#endif

#ifdef CONFIG_CFB_CONSOLE
    drv_video_init();
#endif

#ifdef LK_PROFILING
    time_pmic6329_init = get_timer(0);
#endif
    pmic6329_init();
#ifdef LK_PROFILING
    printf("[PROFILE] ------- pmic6329_init takes %d ms -------- \n", get_timer(time_pmic6329_init));
    printf("[PROFILE] ------- platform_early_init takes %d ms -------- \n", get_timer(time_platform_early_init));
#endif
}

extern void mt65xx_bat_init(void); 
void platform_init(void)
{
    dprintf(INFO, "platform_init()\n");

#ifdef LK_PROFILING
    unsigned int time_nand_emmc;
    unsigned int time_load_logo;
    unsigned int time_bat_init;
    unsigned int time_backlight;
    unsigned int time_show_logo;
    unsigned int time_boot_mode;
    unsigned int time_sw_env;
    unsigned int time_platform_init;
    time_platform_init = get_timer(0);
    time_nand_emmc = get_timer(0);
#endif

#ifdef MTK_EMMC_SUPPORT
    mmc_legacy_init(1);
#else
    nand_init();
    nand_driver_test();
#endif

#ifdef LK_PROFILING
    printf("[PROFILE] ------- NAND/EMMC init takes %d ms -------- \n", get_timer(time_nand_emmc));
    time_load_logo = get_timer(0);
#endif
    mboot_common_load_logo((unsigned long)mt_get_logo_db_addr(), "logo");
    dprintf(INFO, "Show BLACK_PICTURE\n");
    mt_disp_fill_rect(0, 0, CFG_DISPLAY_WIDTH, CFG_DISPLAY_HEIGHT, 0x0);
    mt_disp_power(TRUE);
    mt_disp_update(0, 0, CFG_DISPLAY_WIDTH, CFG_DISPLAY_HEIGHT);
    mt_disp_power(1);           //power on display related modules
#ifdef LK_PROFILING
    printf("[PROFILE] ------- load_logo takes %d ms -------- \n", get_timer(time_load_logo));
    time_backlight = get_timer(0);
#endif

    mt65xx_backlight_on();
#ifdef LK_PROFILING
    printf("[PROFILE] ------- backlight takes %d ms -------- \n", get_timer(time_backlight));
    time_boot_mode = get_timer(0);
#endif

    boot_mode_select();
#ifdef LK_PROFILING
    printf("[PROFILE] ------- boot mode select takes %d ms -------- \n", get_timer(time_boot_mode));
#endif
    /*Show download logo & message on screen */
    if (g_boot_arg->boot_mode == DOWNLOAD_BOOT)
    {
        mt_disp_show_boot_logo();
        video_printf(" => Downloading...\n");
        mt65xx_backlight_on();
        mtk_wdt_disable();//Disable wdt before jump to DA
        platform_uninit();
#ifdef HAVE_CACHE_PL310
        l2_disable();
#endif		
        arch_disable_cache(UCACHE);
        arch_disable_mmu();
        jump_da(g_boot_arg->da_info.addr, g_boot_arg->da_info.arg1, g_boot_arg->da_info.arg2);
    }

#ifdef LK_PROFILING
    time_bat_init = get_timer(0);
#endif
    mt65xx_bat_init();
#ifdef LK_PROFILING
    printf("[PROFILE] ------- battery init takes %d ms -------- \n", get_timer(time_bat_init));
#endif

    /* NOTE: if define CFG_POWER_CHARGING, will rtc_boot_check() in mt65xx_bat_init() */
#ifndef CFG_POWER_CHARGING
    rtc_boot_check(false);
#endif

#ifdef LK_PROFILING
    time_show_logo = get_timer(0);
#endif
    if ((g_boot_mode != ALARM_BOOT) && (g_boot_mode != FASTBOOT))
    {
      mt_disp_show_boot_logo();
    }
#ifdef LK_PROFILING
    printf("[PROFILE] ------- show logo takes %d ms -------- \n", get_timer(time_show_logo));
    time_sw_env= get_timer(0);
#endif

    sw_env();
#ifdef LK_PROFILING
    printf("[PROFILE] ------- sw_env takes %d ms -------- \n", get_timer(time_sw_env));
#endif
#ifdef LK_PROFILING
    printf("[PROFILE] ------- platform_init takes %d ms -------- \n", get_timer(time_platform_init));
#endif

}

void platform_uninit(void)
{
    leds_deinit();
    platform_deinit_interrupts();
}
