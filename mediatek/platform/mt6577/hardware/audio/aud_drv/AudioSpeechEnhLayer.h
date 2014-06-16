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

#ifndef _AUDIO_SPEECH_ENH_LAYER_H
#define _AUDIO_SPEECH_ENH_LAYER_H

#include <stdint.h>
#include <sys/types.h>
#include <utils/Vector.h>
#include <utils/threads.h>
#include <system/audio.h>
#include <hardware/audio.h>
#include <utils/SortedVector.h>
#include "AudioYusuDef.h"
#include <pthread.h>

extern "C" {
#include "enh_api.h"
}
#include "CFG_AUDIO_File.h"



namespace android {
/*
typedef unsigned short uWord16;
typedef signed short Word16;
typedef signed long Word32;
typedef unsigned long uWord32;

typedef struct {

    uWord32 enhance_pars[28];
	uWord32 error_flag;
	Word32 App_table;
    Word32 Fea_Cfg_table;
    Word32 MIC_DG;
	Word32 sample_rate;
	Word32 frame_rate;
	Word16 DMNR_cal_data[76];
    Word16 Compen_filter[270];
	Word16 ne_out[960];
    Word16 fe_out[960];
    Word16 PCM_buffer[1920];
    Word16 EPL_buffer[6080];
} SPH_ENH_ctrl_struct;
*/

#define EnhanceParasNum  28
#define DMNRCalDataNum  76
#define CompenFilterNum  270

#define RecBufSize20ms	960
#define RecBufSize10ms	480

#define EPLBufSize 4160

typedef enum
{
   SPE_MODE_NONE = 0,
   SPE_MODE_REC = 1,
   SPE_MODE_VOIP
} SPE_MODE;


typedef enum
{
   SPE_STATE_IDLE = 0,
   SPE_STATE_START = 1,
   SPE_STATE_RUNNING
} SPE_STATE;


typedef enum
{
   NB_VOIP = 0x1,
   WB_VOIP = 0x2,
   MONO_RECORD = 0x4,
   STEREO_RECORD = 0x8,
   SPEECH_RECOGNITION = 0x10
} SPE_APP_TABLE;

/*
typedef enum
{
   AGC = 0x1,
   AEC = 0x2,
   DMNR = 0x4
} SPE_FEA_TABLE;
*/

typedef enum
{
   UPLINK = 0,
   DOWNLINK
} SPE_DATA_DIRECTION;

typedef enum
{
    ROUTE_NORMAL	= 0,
    ROUTE_HEADSET	= 1,
    ROUTE_SPEAKER	= 2,
    ROUTE_BT		= 3
} SPE_ROUTE;


struct BufferInfo {
        short *pBufBase;
        int BufLen;
        short *pRead;	//short
        short *pWrite;	//short
        int BufLen4Delay;
        short *pRead4Delay;	//short
        short *pWrite4Delay;	//short
    };

    
class SPELayer
{
	public:
		SPELayer();
		~SPELayer();

		bool	LoadSPEParameter();
		////can remove below parameter setting function+++
		bool	SetEnhPara(SPE_MODE mode, unsigned long *pEnhance_pars);
		bool	SetDMNRPara(SPE_MODE mode, short *pDMNR_cal_data);
		bool	SetCompFilter(SPE_MODE mode, short *pCompen_filter);
		bool	SetPGAGain(SPE_MODE mode, long PGA_gain);
		////can remove below parameter setting function---
		bool	SetSampleRate(SPE_MODE mode, long sample_rate);
		bool	SetFrameRate(SPE_MODE mode, long frame_rate);
		bool	SetAPPTable(SPE_MODE mode, SPE_APP_TABLE App_table);
		bool	SetFeaCfgTable(SPE_MODE mode, long Fea_Cfg_table);
		bool	SetLatencyTime(int ms);
		bool	SetChannel(int channel);
//		bool	SetMode(SPE_MODE mode);
		bool	SetRoute(SPE_ROUTE route);
		bool	SetRecCompFilter(int leftnum, int rightnum, bool stereo);

		bool	Start(SPE_MODE mode);
		bool	Process(SPE_DATA_DIRECTION dir, short *inBuf, int  inBufLength,short *outBuf =0, int outBufLength=0);
		bool	Stop();		
		
		
		int	GetLatencyTime();
		SPE_MODE	GetMode();
		SPE_ROUTE	GetRoute();
		long	GetSampleRate(SPE_MODE mode);
		long	GetFrameRate(SPE_MODE mode);
		long	GetPGAGain(SPE_MODE mode);
		long	GetAPPTable(SPE_MODE mode);
		long	GetFeaCfgTable(SPE_MODE mode);
		int	GetChannel();
		bool	GetEnhPara(SPE_MODE mode, unsigned long *pEnhance_pars);
		bool	GetDMNRPara(SPE_MODE mode, short *pDMNR_cal_data);
		bool	GetCompFilter(SPE_MODE mode, short *pCompen_filter);
		bool	IsSPERunning();
	
	
		bool MutexLock(void);
    	bool MutexUnlock(void);
		bool DumpMutexLock(void);
    	bool DumpMutexUnlock(void);
    	void dump(void);
		
    	void Dump_PCM_In(SPE_DATA_DIRECTION dir, void* buffer, int bytes);
    	void Dump_PCM_Process(SPE_DATA_DIRECTION dir, void* buffer, int bytes);
    	void Dump_PCM_Out(SPE_DATA_DIRECTION dir, void* buffer, int bytes);
		void Dump_EPL(void* buffer, int bytes);
		bool CreateDumpThread();
		bool	HasBufferDump();

		Vector<BufferInfo *> mDumpDLInBufferQ, mDumpDLOutBufferQ, mDumpULOutBufferQ,mDumpULInBufferQ, mDumpEPLBufferQ;
#if defined(PC_EMULATION)
   HANDLE hDumpThread;
#else
   pthread_t hDumpThread;
#endif				

		FILE * mfpInDL;
		FILE * mfpInUL;
		FILE * mfpOutDL;
		FILE * mfpOutUL;
		FILE * mfpProcessedDL;
		FILE * mfpProcessedUL;
		FILE * mfpEPL;
	private:
		
		bool	SetSPHCtrlStruct(SPE_MODE mode);
		void	ReStart();
		void	Clear();
		void	LoadRecCustFIRParam();		

#ifdef	MTK_AP_SPEECH_ENHANCEMENT_VOIP
		AUDIO_SPH_ENH_Param_STRUCT	mAudioParamVoIP;	//load VoIP corresponding parameters
#endif		
		
		SPE_MODE mMode;
		SPE_ROUTE	mRoute;
		SPE_STATE mState;
		
		//Record settings
		Word32 mRecordSampleRate;
		Word32 mRecordFrameRate;
		Word32 mRecordPGAGain;	//MIC_DG
		uWord32 mRecordEnhanceParas[EnhanceParasNum];
		Word16 mRecordDMNRCalData[DMNRCalDataNum];
		Word16 mRecordCompenFilter[CompenFilterNum];
		Word32 mRecordApp_table;
		Word32 mRecordFea_Cfg_table;	 
	  		
		//VoIP settings
		int mLatencyTime;
		int mLatencySampleCount;
		Word32 mVoIPSampleRate;
		Word32 mVoIPFrameRate;
		Word32 mVoIPPGAGain;	//MIC_DG
		uWord32 mVoIPEnhanceParas[EnhanceParasNum];
		Word16 mVoIPDMNRCalData[DMNRCalDataNum];
		Word16 mVoIPCompenFilter[CompenFilterNum];
		Word32 mVoIPApp_table;
		Word32 mVoIPFea_Cfg_table;
		bool	mNeedDelayLatency;
		
		SPH_ENH_ctrl_struct mSph_Enh_ctrl;
		int *mSphCtrlBuffer;
		int mSPEProcessBufSize;
/*		BufferInfo *mpSPEBufferUL1;
		BufferInfo *mpSPEBufferUL2;
		BufferInfo *mpSPEBufferDL;
		BufferInfo *mpSPEBufferDLDelay;
*/
		short *mpSPEBufferUL1;
		short *mpSPEBufferUL2;
		short *mpSPEBufferDL;
		short *mpSPEBufferDLDelay;	
		short *mpSPEBufferNE;
		short *mpSPEBufferFE;
		
		int	mULInBufQLenTotal, mDLInBufQLenTotal,mULOutBufQLenTotal,mDLOutBufQLenTotal,mDLDelayBufQLenTotal;
		
		Vector<BufferInfo *> mDLInBufferQ, mDLOutBufferQ, mULOutBufferQ,mULInBufferQ, mDLDelayBufferQ;
		//Vector<BufferInfo>  mULInBufferQ;
		
		Mutex	mLock, mDumpLock;
		bool mLoadCustParams;
		bool mError;

};
}
#endif
