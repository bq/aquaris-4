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


/*******************************************************************************
 *
 * Filename:
 * ---------
 *   tst_main.c
 *
 * Project:
 * --------
 *   YUSU
 *
 * Description:
 * ------------
 *    driver main function
 *
 * Author:
 * -------
 *   Lu.Zhang (MTK80251) 09/11/2009
 *
 *------------------------------------------------------------------------------
 * $Revision:$
 * $Modtime:$
 * $Log:$
 *
 * 09 05 2010 siyang.miao
 * [ALPS00003981] Add reboot feature in meta
 * .
 *
 * 09 02 2010 sean.cheng
 * [ALPS00003477] [Need Patch] [Volunteer Patch] ALPS.W10.34 migrate camera function from 2.1 to 2.2
 * .Turn the CCAP / CCT Meta function
 *
 * 08 28 2010 qiuhuan.zhao
 * [ALPS00123522] [GPS] Android 2.2 porting
 * GPS META and FM porting.
 *
 * 08 28 2010 chunhui.li
 * [ALPS00123709] [Bluetooth] meta mode check in
 * for META mode check in.
 *
 * 08 14 2010 chipeng.chang
 * [ALPS00003297] [Need Patch] [Volunteer Patch] android 2.2 migration
 * add audio meta ft main.
 *
 * 07 16 2010 siyang.miao
 * [ALPS00122025]TST/FT for G-Sensor calibration
 * .
 *
 * 05 11 2010 lu.zhang
 * [ALPS00005327]CCAP
 * .
 *
 * 03 18 2010 lu.zhang
 * [ALPS00004362]CCAP APIs
 * for CCAP APIs
 *
 * 03 09 2010 ch.yeh
 * [ALPS00001276][BT]Migration to Android 2.1
 * [BT][meta]enable Bluetooth META function.
 *
 * 02 26 2010 lu.zhang
 * [ALPS00004332]Create META
 * .
 *
 * 01 20 2010 lu.zhang
 * [ALPS00004332]Create META
 * .
 *
 *
 *
 *
 *
 *******************************************************************************/

#include "ft_main.h"
#include "meta.h"
#include "FT_Cmd_Para.h"
#include "FT_Public.h"
#include <sys/types.h>
#include <sys/stat.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FT_PREFIX   "FT: "
//#define FT_LOG(fmt, arg ...) META_LOG(FT_PREFIX fmt, ##arg)
#define FT_LOG(fmt, arg...) printf(FT_PREFIX fmt, ##arg)

int g_fdUsbComPort = -1;

void FTMuxPrimitiveData(META_RX_DATA *pMuxBuf);

/********************************************************************************
//FUNCTION:
//		WriteDataToPC
//DESCRIPTION:
//		this function is called to init ft module when ft is loaded by device.exe. it will create ft task to recieve
//		data from tst, or recieve data from module and then send to test. it will init ft module too.
//
//PARAMETERS:
//		None
//
//RETURN VALUE:
//		TRUE is success, otherwise is fail
//
//DEPENDENCY:
//		None
//
//GLOBALS AFFECTED
//		None
********************************************************************************/

int FTT_Init(int dwContext)
{
    FT_Module_Init();
    g_fdUsbComPort = dwContext;
    FT_LOG("[FTT_Drv:] FT Init... ");
    return 1;
}

/********************************************************************************
//FUNCTION:
//		FTT_Deinit
//DESCRIPTION:
//		this function is called to deint ft module
//
//PARAMETERS:
//		None
//RETURN VALUE:
//		TRUE is success, otherwise is fail
//
//DEPENDENCY:
//		the FT module must have been loaded.
//
//GLOBALS AFFECTED
//		None
********************************************************************************/
int FTT_Deinit(int hDeviceContext)
{
    FT_Module_Deinit();
    return 1;
}

/********************************************************************************
//FUNCTION:
//		FT_DispatchMessage
//DESCRIPTION:
//		this function is called to switch the testcase, del the header of peer buf.
//
//PARAMETERS:
//		Local_buf:	[IN]	local buf (cnf cmd)
//		Local_len: 	[IN]	local buf size
//		pPeerBuf		[IN]	peer buff
//		Peer_len:		[IN]	peer buff size
//RETURN VALUE:
//		TRUE is success, otherwise is fail
//
//DEPENDENCY:
//		the FT module must have been loaded.
//
//GLOBALS AFFECTED
//		None
********************************************************************************/
void FT_DispatchMessage(void *pLocalBuf, void *pPeerBuf, int local_len, int peer_len)
{
    FT_H  *ft_header;
    ft_header =(FT_H *)pLocalBuf;
    kal_int16 ft_peer_len = peer_len -8;	//del the size of peer buf header
    char *pft_PeerBuf = (char *)pPeerBuf;
    
    pft_PeerBuf += 8; // skip the header of peer buffer
    
    FT_LOG("[FTT_Drv:] FTMainThread ID : %d ", ft_header->id);
    
    switch (ft_header->id)
    {

        case FT_IS_ALIVE_REQ_ID:				//test alive
            FT_TestAlive((FT_IS_ALIVE_REQ *)pLocalBuf);
            break;
        #ifdef FT_WIFI_FEATURE
        case FT_WIFI_REQ_ID:					//wifi test
            FT_WIFI_OP((FT_WM_WIFI_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        #endif
        case FT_SDCARD_REQ_ID:                  //sdcard test
            FT_SDcard_OP((SDCARD_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        case FT_NVRAM_READ_REQ_ID:				//nvram read
            FT_APEditorRead_OP((FT_AP_Editor_read_req *)pLocalBuf);
            break;
        case FT_NVRAM_WRITE_REQ_ID:				//nvram write
            FT_APEditorWrite_OP((FT_AP_Editor_write_req *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        case FT_NVRAM_RESET_REQ_ID:				//nvram reset
            FT_APEditorReset_OP((FT_AP_Editor_reset_req *)pLocalBuf);
            break;
	case FT_VER_INFO_REQ_ID:				//get version info
            FT_GetVersionInfo((FT_VER_INFO_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        case FT_GPIO_REQ_ID:
            FT_GPIO_OP((GPIO_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;

        /*
        case FT_CHECK_META_VER_REQ_ID:			//require meta dll version
            FT_CheckMetaDllVersion((FT_CHECK_META_VER_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        */
        case FT_POWER_OFF_REQ_ID:				//power off target
            FT_PowerOff();
            break;
        case FT_REG_WRITE_ID:					//baseband reg wirte
            FT_CPURegW_OP((FT_REG_WRITE_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        case FT_REG_READ_ID:					//baseband reg read
            FT_CPURegR_OP((FT_REG_READ_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        case FT_CCT_REQ_ID:
            FT_CCAP_OP((FT_CCT_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;

        case FT_UTILITY_COMMAND_REQ_ID:			//utility command
            FT_Peripheral_OP((FT_UTILITY_COMMAND_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
    #ifdef FT_BT_FEATURE 
        case FT_BT_REQ_ID:						//bt test
            FT_BT_OP((BT_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
	#endif
        case FT_ADC_GETMEADATA_ID:				//adc test
            FT_AUXADC_OP((AUXADC_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
	#ifdef FT_GPS_FEATURE		
        case FT_GPS_REQ_ID:
            FT_GPS_OP((GPS_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
    #endif
	#ifdef FT_FM_FEATURE 	
        case FT_FM_REQ_ID:
            FT_FM_OP((FM_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
	#endif
        case FT_L4AUD_REQ_ID:
            FT_L4AUDIO_OP((FT_L4AUD_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        case FT_NVRAM_BACKUP_REQ_ID:
            FT_NVRAM_Backup_OP((FT_NVRAM_BACKUP_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        case FT_NVRAM_RESTORE_REQ_ID:
            FT_NVRAM_Restore_OP((FT_NVRAM_RESTORE_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        case FT_GSENSOR_REQ_ID:
            FT_GSENSOR_OP((GS_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        case FT_META_MODE_LOCK_REQ_ID:
            FT_META_MODE_LOCK((FT_META_MODE_LOCK_REQ *)pLocalBuf);
            break;
        #ifdef FT_MATV_FEATURE		
        case FT_MATV_CMD_REQ_ID:
            FT_MATV_OP((FT_MATV_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        #endif		
        case FT_REBOOT_REQ_ID:
        FT_Reboot((FT_META_REBOOT_REQ *)pLocalBuf);
        break;
        case FT_CUSTOMER_REQ_ID:
            FT_CUSTOMER_OP((FT_CUSTOMER_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        case FT_GET_CHIPID_REQ_ID:
            FT_GET_CHIPID_OP((FT_GET_CHIPID_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        case FT_MSENSOR_REQ_ID:
            FT_MSENSOR_OP((FT_MSENSOR_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        case FT_ALSPS_REQ_ID:
            FT_ALSPS_OP((FT_ALSPS_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        case FT_GYROSCOPE_REQ_ID:
            FT_GYROSENSOR_OP((GYRO_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
		case FT_CTP_REQ_ID:
			FT_CTP_OP((Touch_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
		break;
    	case FT_VER_INFO_V2_REQ_ID:
        	FT_GetVersionInfoV2((FT_VER_INFO_V2_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
        	break;
#ifdef FT_CMMB_FEATURE
	case FT_CMMB_REQ_ID:
		FT_CMMB_OP((MetaCmmbReq *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
		break;
#endif
		case FT_BUILD_PROP_REQ_ID:
			FT_BUILD_PROP_OP((FT_BUILD_PROP_REQ *)pLocalBuf,(char *)pft_PeerBuf, ft_peer_len);
			break;
		#ifdef FT_EMMC_FEATURE
		case FT_EMMC_REQ_ID:
			FT_CLR_EMMC_OP((FT_EMMC_REQ *)pLocalBuf,(char *)pft_PeerBuf, ft_peer_len);
			break;
		case FT_CRYPTFS_REQ_ID:
			FT_CRYPTFS_OP((FT_CRYPTFS_REQ *)pLocalBuf,(char *)pft_PeerBuf, ft_peer_len);
			break;
		#endif
        default:
            FT_LOG("[FTT_Drv:] FTMainThread Error:!!! ID ");
        break;
    }
}

/********************************************************************************
//FUNCTION:
//		FTMuxPrimitiveData
//DESCRIPTION:
//		this function is called to add the header and add the escape for ap side before sending to PC
//
//PARAMETERS:
//		pMuxBuf: 	[IN]		data buffer including tst header
//
//RETURN VALUE:
//		None
//
//DEPENDENCY:
//		None
//
//GLOBALS AFFECTED
//		None
********************************************************************************/
void FTMuxPrimitiveData(META_RX_DATA *pMuxBuf)
{
    /* This primitive is logged by TST */
    unsigned char *pTempBuf = NULL;
    unsigned char *pTempDstBuf = NULL;
    unsigned char *pMamptrBase = NULL;
    unsigned char *pDestptrBase = NULL;
    int iCheckNum = 0;
    int dest_index=0;
    unsigned char cCheckSum = 0;
    int cbWriten = 0;
    int cbTxBuffer = 0;
    int i=0;
    
    if(pMuxBuf == NULL)
    {
        FT_LOG("Err: FTMuxPrimitiveData pMuxBuf is NULL");
        return;
    }
    
    cbTxBuffer = pMuxBuf->LocalLen + pMuxBuf->PeerLen + 9;
    if (cbTxBuffer>FrameMaxSize)
    {
        FT_LOG("[TST_Drv:] FTMuxPrimitiveData: error frame size is too big!! ");
        return;
    }
    else
        FT_LOG("[TST_Drv:] FTMuxPrimitiveData: Type-%d Local_len-%d, Peer_len-%d", pMuxBuf->eFrameType, pMuxBuf->LocalLen, pMuxBuf->PeerLen);
    
    FT_LOG("[TST_Drv:] FTMuxPrimitiveData: total size is -%d", cbTxBuffer);
    pMamptrBase = (unsigned char *)malloc(cbTxBuffer);
    //Wayne add MAX_TST_TX_BUFFER_LENGTH
    if(pMamptrBase == NULL)
    {
        FT_LOG("Err: FTMuxPrimitiveData malloc pMamptrBase Fail");
        return;
    }
    pDestptrBase = (unsigned char *)malloc(MAX_TST_TX_BUFFER_LENGTH);//2048);
    if(pDestptrBase == NULL)
    {
        FT_LOG("Err: FTMuxPrimitiveData malloc pDestptrBase Fail");
        free(pMamptrBase);
        return;
    }
    
    
    pTempDstBuf = pDestptrBase;
    pTempBuf = pMamptrBase;
    
    /* fill the frameheader */
    *pTempBuf++ = 0x55;
    *pTempBuf++=((pMuxBuf->LocalLen + pMuxBuf->PeerLen +5)&0xff00)>>8;
    *pTempBuf++= (pMuxBuf->LocalLen + pMuxBuf->PeerLen +5)&0xff;
    *pTempBuf++ = 0x60;
    
    /*fill the local and peer data u16Length and its data */
    *pTempBuf++ = ((pMuxBuf->LocalLen)&0xff); /// pMuxBuf->LocalLen ;
    *pTempBuf++ = ((pMuxBuf->LocalLen)&0xff00)>>8;
    *pTempBuf++ = (pMuxBuf->PeerLen )&0xff;   ///pMuxBuf->PeerLen ;
    *pTempBuf++ = ((pMuxBuf->PeerLen)&0xff00)>>8;
    
    memcpy((pTempBuf), pMuxBuf->uData, pMuxBuf->LocalLen + pMuxBuf->PeerLen);
    
    pTempBuf = pMamptrBase;
    
    /* 0x5a is start data, so we use 0x5a and 0x01 inidcate 0xa5, use 0x5a and 0x5a indicate 0x5a
    the escape is just for campatiable with feature phone */
    while (iCheckNum != (cbTxBuffer-1))
    {
        cCheckSum ^= *pTempBuf;
        *pTempDstBuf = *pTempBuf;
        iCheckNum++;
        
        if (*pTempBuf ==0xA5 )
        {
            *pTempDstBuf++ = 0x5A;
            *pTempDstBuf++ = 0x01;
            dest_index++;		//do the escape, dest_index should add for write to uart or usb
        }
        else if (*pTempBuf ==0x5A )
        {
            *pTempDstBuf++ = 0x5A;
            *pTempDstBuf++ = 0x5A;
            dest_index++;		//do the escape, dest_index should add for write to uart or usb
        }
        else
            pTempDstBuf++;
        
        dest_index++;
        pTempBuf++;
    }
    
    /* 0x5a is start data, so we use 0x5a and 0x01 inidcate 0xa5 for check sum, use 0x5a and 0x5a indicate 0x5a
    the escape is just for campatiable with feature phone */
    if ( cCheckSum ==0xA5 )
    {
        dest_index++;		//do the escape, dest_index should add for write to uart or usb
        //Wayne replace 2048 with MAX_TST_RECEIVE_BUFFER_LENGTH
        if ((dest_index) > MAX_TST_TX_BUFFER_LENGTH)//2048)
        {
            FT_LOG("[TST_Drv:] FTMuxPrimitiveData: Data is too big: index-%d cbTxBuffer-%d ",dest_index, cbTxBuffer);
            goto TSTMuxError;
        }
        
        *pTempDstBuf++= 0x5A;
        *pTempDstBuf = 0x01;
    }
    else if ( cCheckSum ==0x5A )
    {
        dest_index++;		//do the escape, dest_index should add for write to uart or usb
        if ((dest_index) > MAX_TST_TX_BUFFER_LENGTH)//2048)
        {
            FT_LOG("[TST_Drv:] FTMuxPrimitiveData: Data is too big: index-%d cbTxBuffer-%d ",dest_index, cbTxBuffer);
            goto TSTMuxError;
        }
        *pTempDstBuf++= 0x5A;
        *pTempDstBuf = 0x5A;
    }
    else
        *pTempDstBuf =(char )cCheckSum;
    
    dest_index++;
    
    //write to PC
    cbWriten = write(g_fdUsbComPort, (void *)pDestptrBase, dest_index);
    pTempDstBuf = pDestptrBase;
    /*
    for (i =0; i<cbWriten; i++)
    {
    FT_LOG("%2x ",*(pTempDstBuf+i));
    
    if ((i+1)%16 ==0)
    FT_LOG("");
    
    }
    */
    //FT_LOG("");
    
    FT_LOG("[TST_Drv:] FTMuxPrimitiveData: %d  %d %d  cChecksum: %d ",cbWriten, cbTxBuffer, dest_index,cCheckSum);
    
    TSTMuxError:
    
    free(pMamptrBase);
    free(pDestptrBase);
}





/********************************************************************************
//FUNCTION:
//		WriteDataToPC
//DESCRIPTION:
//		this function is called to send cnf data to PC side. the local_len + Peer Len must less than 2031 bytes
//		and peer len must less than 2000. so when it do not meet, module should divide the packet to
//		many small packet to sent.
//
//PARAMETERS:
//		Local_buf:	[IN]	local buf (cnf cmd)
//		Local_len: 	[IN]	local buf size
//		Peer_buf		[IN]	peer buff
//		Peer_len:		[IN]	peer buff size
//RETURN VALUE:
//		TRUE is success, otherwise is fail
//
//DEPENDENCY:
//		the FT module must have been loaded.
//
//GLOBALS AFFECTED
//		None
********************************************************************************/
int WriteDataToPC(void *Local_buf,unsigned short Local_len,void *Peer_buf,unsigned short Peer_len)
{
    META_RX_DATA sTempRxbuf ;
    unsigned char *cPeerbuf = &sTempRxbuf.uData[Local_len+8];
    
    memset(&sTempRxbuf, 0, sizeof(sTempRxbuf));
    
    //add the header of frame type
    sTempRxbuf.eFrameType = AP_FRAME;
    sTempRxbuf.LocalLen = Local_len ;
    
    //check the buffer size.
    if (Peer_len >0)
        sTempRxbuf.PeerLen =  Peer_len +8;
    else
        sTempRxbuf.PeerLen = 0;
    
    if (((Local_len + Peer_len)> FTMaxSize)||(Peer_len >PeerBufMaxlen))
    {
        FT_LOG("[FTT_Drv:] WriteDataToPC Error: Local_len-%hu Peer_len- %hu", Local_len,Peer_len);
        return 0;
    }
    
    if ((Local_len == 0) && (Local_buf == NULL))
    {
        FT_LOG("[FTT_Drv:] WriteDataToPC Error: Local_len-%hu Peer_len- %hu", Local_len,Peer_len);
        return 0;
    }
    
    // copy to the temp buffer, and send it to the tst task.
    memcpy(sTempRxbuf.uData, Local_buf, Local_len);
    if ((Peer_len >0)&&(Peer_buf !=NULL))
        memcpy(cPeerbuf, Peer_buf, Peer_len);
    
    FTMuxPrimitiveData(&sTempRxbuf);
    return 1;
}

#ifdef __cplusplus
}
#endif


