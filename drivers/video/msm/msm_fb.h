/* Copyright (c) 2008-2012, Code Aurora Forum. All rights reserved.
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
 */

#ifndef MSM_FB_H
#define MSM_FB_H

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/time.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include "linux/proc_fs.h"

#include <mach/hardware.h>
#include <linux/io.h>
#include <mach/board.h>

#include <asm/system.h>
#include <asm/mach-types.h>
#include <mach/memory.h>
#include <linux/semaphore.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>
#include <linux/hrtimer.h>

#include <linux/fb.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/msm_mdp.h>
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif

#include "msm_fb_panel.h"
#include "mdp.h"

/* lived, 2012.11.06 panel_power_on state check and set*/
#define F_SKYDISP_CHECK_AND_SET_PANEL_POWER_ON
/* lived, 2013.01.15 for fix underrun issue when using 4-layer bypass */
//#define PANTECH_LCD_FIX_UNDERRUN
/* lived, 2013.01.23 for fix mixer_info based on pipe_used not stage list
   because, when the framework reset occurred, the pipe clean does not clearly */
//#define PANTECH_LCD_PIPE_CLEAN_WHEN_FRAMEWORK_RESET
/* lived, 2013.02.13 bug fix for mdp bandwidth request */
#define PANTECH_LCD_BUG_FIX_MDP_BANDWIDTH_REQUEST
/* lived, 2013.05.08 bug fix for mdp composition with AOT */
#define PANTECH_LCD_BUG_FIX_QCPATCH_FOR_MDP_COMP_WITH_AOT
/* lived, 2013.03.05 support framework reset when lcd off*/
/* lived, 2013.03.20 add again by the following patch */
/* https://www.codeaurora.org/gitweb/quic/la/?p=kernel/msm.git;a=commit;h=548ff607d8d20792203260e0d74e013b43ea3781 */
#define PANTECH_LCD_SUPPORT_FRAMEWORK_RESET_WHEN_LCD_OFF

//#define CONFIG_LCD_DRIVER_STABILITY
#ifdef CONFIG_F_SKYDISP_SILENT_BOOT
extern int backlight_value;
#endif
/*20120523, kkcho, for cabc_on_off_ctrl*/ // 20120607 shkwak, cabc disable when fhd test
#if !defined(CONFIG_F_SKYDISP_CABC_CTRL) && !defined(CONFIG_FB_PANTECH_MIPI_DSI_RENESAS)
#define CONFIG_F_SKYDISP_CABC_CTRL
#endif

/* 2013.01.21, lived, mdp clk 200->266 when 4 Layer bypass for underrun*/
#if defined(CONFIG_MACH_APQ8064_EF51S) || defined(CONFIG_MACH_APQ8064_EF51K) || defined(CONFIG_MACH_APQ8064_EF51L)
//#define PANTECH_LCD_INCREASE_MDP_CLK_200_TO_266_WHEN_4_LAYER_BYPASS
#endif

/*20121116, kkcho,  need only sharp-ips for LCD-Flicker and burning*/
#if defined(CONFIG_MACH_APQ8064_EF48S) || defined(CONFIG_MACH_APQ8064_EF49K) || defined(CONFIG_MACH_APQ8064_EF50L) || defined(CONFIG_MACH_APQ8064_EF51S) || defined(CONFIG_MACH_APQ8064_EF51K) || defined(CONFIG_MACH_APQ8064_EF51L)
#define FEATURE_SKYDISP_DISPLAY_FLICKER_SHARP_IPS 
#endif

/*20121122, kkcho, Bug fix : Intermittently, LCD_BL_ON skip problem*/
//#define FEATURE_SKYDISP_BACKLIGHT_CONTROL_BUG_FIX

//shkwak 20121107, qcom patch for lcd boot up fail and vsync update problem
#if (defined(CONFIG_SKY_EF52S_BOARD)||defined(CONFIG_SKY_EF52K_BOARD)||defined(CONFIG_SKY_EF52L_BOARD)) && defined(CONFIG_FB_PANTECH_MIPI_SONY_CMD_HD_PANEL)
//#define QCOM_PATCH_VSYNC_MIPICMD
#endif

#if defined(CONFIG_MACH_APQ8064_EF48S) || defined(CONFIG_MACH_APQ8064_EF49K) || defined(CONFIG_MACH_APQ8064_EF50L)
/*20130103, kkcho, applied work-around code for JB
  Bug fix : kernel-panic by "Unable to handle kernel NULL pointer dereference at virtual address 00000000"
  call-stack : dma_cache_maint_page+0x24/0x10c <- arm_dma_unmap_page+0x5c/0xa0 <-mipi_dsi_cmd_dma_tx+0x214/0x254 <-mipi_dsi_cmds_tx
  Reproduce path : repeat lcd on/off by power-key
*/
//#define FEATURE_SKYDISP_RESET_FIX_TEMP
#define FEATURE_SKYDISP_RESET_FIX_SECOND_METHOD
#define PANTECH_LCD_FIX_ABOUT_HALT_WHEN_LCD_ONOFF_DURING_WFD
#endif

/* 130124, Get LCD Revision(2 or 3).
 * CONFIG_PANTECH_LCD_GET_LCD_REV is defined in below files.
 * kernel/drivers/video/msm/msm_fb.h
 * kernel/include/linux/msm_mdp.h
 * vendor/pantech/build/CUST_PANTECH_DISPLAY.h
 */
#if (defined(CONFIG_SKY_EF51S_BOARD)||defined(CONFIG_SKY_EF51K_BOARD)||defined(CONFIG_SKY_EF51L_BOARD))
#define CONFIG_PANTECH_LCD_GET_LCD_REV
#endif

#define MSM_FB_DEFAULT_PAGE_SIZE 2
#define MFD_KEY  0x11161126
#define MSM_FB_MAX_DEV_LIST 32

struct disp_info_type_suspend {
	boolean op_enable;
	boolean sw_refreshing_enable;
	boolean panel_power_on;
	boolean op_suspend;
};

struct msmfb_writeback_data_list {
	struct list_head registered_entry;
	struct list_head active_entry;
	void *addr;
	struct ion_handle *ihdl;
	struct file *pmem_file;
	struct msmfb_data buf_info;
	struct msmfb_img img;
	int state;
};


struct msm_fb_data_type {
	__u32 key;
	__u32 index;
	__u32 ref_cnt;
	__u32 fb_page;

	panel_id_type panel;
	struct msm_panel_info panel_info;

	DISP_TARGET dest;
	struct fb_info *fbi;

	struct delayed_work backlight_worker;
	boolean op_enable;
	uint32 fb_imgType;
	boolean sw_currently_refreshing;
	boolean sw_refreshing_enable;
	boolean hw_refresh;
#ifdef CONFIG_FB_MSM_OVERLAY
	int overlay_play_enable;
#endif

	MDPIBUF ibuf;
	boolean ibuf_flushed;
	struct timer_list refresh_timer;
	struct completion refresher_comp;

	boolean pan_waiting;
	struct completion pan_comp;

	/* vsync */
	boolean use_mdp_vsync;
	__u32 vsync_gpio;
	__u32 total_lcd_lines;
	__u32 total_porch_lines;
	__u32 lcd_ref_usec_time;
	__u32 refresh_timer_duration;

	struct hrtimer dma_hrtimer;

	boolean panel_power_on;
	struct work_struct dma_update_worker;
	struct semaphore sem;

	struct timer_list vsync_resync_timer;
	boolean vsync_handler_pending;
	struct work_struct vsync_resync_worker;

	ktime_t last_vsync_timetick;

	__u32 *vsync_width_boundary;

	unsigned int pmem_id;
	struct disp_info_type_suspend suspend;

	__u32 channel_irq;

	struct mdp_dma_data *dma;
	struct device_attribute dev_attr;
	void (*dma_fnc) (struct msm_fb_data_type *mfd);
	int (*cursor_update) (struct fb_info *info,
			      struct fb_cursor *cursor);
	int (*lut_update) (struct fb_info *info,
			      struct fb_cmap *cmap);
	int (*do_histogram) (struct fb_info *info,
			      struct mdp_histogram_data *hist);
	int (*start_histogram) (struct mdp_histogram_start_req *req);
	int (*stop_histogram) (struct fb_info *info, uint32_t block);
	void (*vsync_ctrl) (int enable);
	void (*vsync_init) (int cndx);
	void *vsync_show;
	void *cursor_buf;
	void *cursor_buf_phys;

	void *cmd_port;
	void *data_port;
	void *data_port_phys;

	__u32 bl_level;

	struct platform_device *pdev;

	__u32 var_xres;
	__u32 var_yres;
	__u32 var_pixclock;
	__u32 var_frame_rate;

#ifdef MSM_FB_ENABLE_DBGFS
	struct dentry *sub_dir;
#endif

#ifdef CONFIG_HAS_EARLYSUSPEND
	struct early_suspend early_suspend;
#ifdef CONFIG_FB_MSM_MDDI
	struct early_suspend mddi_early_suspend;
	struct early_suspend mddi_ext_early_suspend;
#endif
#endif
	u32 mdp_fb_page_protection;

	struct clk *ebi1_clk;
	boolean dma_update_flag;
	struct timer_list msmfb_no_update_notify_timer;
	struct completion msmfb_update_notify;
	struct completion msmfb_no_update_notify;
	struct mutex writeback_mutex;
	struct mutex unregister_mutex;
	struct list_head writeback_busy_queue;
	struct list_head writeback_free_queue;
	struct list_head writeback_register_queue;
	wait_queue_head_t wait_q;
	struct ion_client *iclient;
	unsigned long display_iova;
	unsigned long rotator_iova;
	struct mdp_buf_type *ov0_wb_buf;
	struct mdp_buf_type *ov1_wb_buf;
	u32 ov_start;
	u32 mem_hid;
	u32 mdp_rev;
	u32 writeback_state;
	bool writeback_active_cnt;
	int cont_splash_done;
	void *copy_splash_buf;
#ifdef CONFIG_F_SKYDISP_QCBUGFIX_CONTINUOUS_SPLASH_SCREEN_BUFFER_ALLOC_FOR_1080P
	dma_addr_t copy_splash_phys;
#else
	unsigned char *copy_splash_phys;
#endif
	void *cpu_pm_hdl;
	int vsync_sysfs_created;
	u32 acq_fen_cnt;
	struct sync_fence *acq_fen[MDP_MAX_FENCE_FD];
	int cur_rel_fen_fd;
	struct sync_pt *cur_rel_sync_pt;
	struct sync_fence *cur_rel_fence;
	struct sync_fence *last_rel_fence;
	struct sw_sync_timeline *timeline;
	int timeline_value;
};

struct dentry *msm_fb_get_debugfs_root(void);
void msm_fb_debugfs_file_create(struct dentry *root, const char *name,
				u32 *var);
void msm_fb_set_backlight(struct msm_fb_data_type *mfd, __u32 bkl_lvl);

struct platform_device *msm_fb_add_device(struct platform_device *pdev);
struct fb_info *msm_fb_get_writeback_fb(void);
int msm_fb_writeback_init(struct fb_info *info);
int msm_fb_writeback_start(struct fb_info *info);
int msm_fb_writeback_queue_buffer(struct fb_info *info,
		struct msmfb_data *data);
int msm_fb_writeback_dequeue_buffer(struct fb_info *info,
		struct msmfb_data *data);
int msm_fb_writeback_stop(struct fb_info *info);
int msm_fb_writeback_terminate(struct fb_info *info);
int msm_fb_detect_client(const char *name);
int calc_fb_offset(struct msm_fb_data_type *mfd, struct fb_info *fbi, int bpp);

#ifdef CONFIG_FB_BACKLIGHT
void msm_fb_config_backlight(struct msm_fb_data_type *mfd);
#endif

void fill_black_screen(void);
void unfill_black_screen(void);
int msm_fb_check_frame_rate(struct msm_fb_data_type *mfd,
				struct fb_info *info);

#ifdef CONFIG_FB_MSM_LOGO
#define INIT_IMAGE_FILE "/initlogo.rle"
int load_565rle_image(char *filename, bool bf_supported);
#endif

#endif /* MSM_FB_H */
