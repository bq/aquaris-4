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

/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef A2DP_AUDIO_HARDWARE_H
#define A2DP_AUDIO_HARDWARE_H

#include <stdint.h>
#include <sys/types.h>

#include <utils/threads.h>

#include <hardware_legacy/AudioHardwareBase.h>


namespace android_audio_legacy {
    using android::Mutex;

class A2dpAudioInterface : public AudioHardwareBase
{
    class A2dpAudioStreamOut;

public:
                        A2dpAudioInterface(AudioHardwareInterface* hw);
    virtual             ~A2dpAudioInterface();
    virtual status_t    initCheck();


    /////////////////////////////////////////////////////////////////////////
    //    for PCMxWay Interface API ...  
    /////////////////////////////////////////////////////////////////////////
    virtual int xWayPlay_Start(int sample_rate);
    virtual int xWayPlay_Stop(void);
    virtual int xWayPlay_Write(void *buffer, int size_bytes);
    virtual int xWayPlay_GetFreeBufferCount(void);
    virtual int xWayRec_Start(int sample_rate);
    virtual int xWayRec_Stop(void);
    virtual int xWayRec_Read(void *buffer, int size_bytes);


    virtual status_t    setVoiceVolume(float volume);
    virtual status_t    setMasterVolume(float volume);

    virtual status_t    setMode(int mode);

    // mic mute
    virtual status_t    setMicMute(bool state);
    virtual status_t    getMicMute(bool* state);

    virtual status_t    setParameters(const String8& keyValuePairs);
    virtual String8     getParameters(const String8& keys);


    virtual status_t SetEMParameter(void *ptr , int len);
    virtual status_t GetEMParameter(void *ptr , int len);
    virtual status_t SetAudioCommand(int par1 , int par2);
    virtual status_t GetAudioCommand(int par1);
    virtual status_t SetAudioData(int par1,size_t len,void *ptr);
    virtual status_t GetAudioData(int par1,size_t len,void *ptr);

    // item needs to do when mode change
    virtual void A2dpAudiosetMode(int mode);

    virtual status_t SetACFPreviewParameter(void *ptr , int len);
    virtual status_t SetHCFPreviewParameter(void *ptr , int len);

    virtual size_t      getInputBufferSize(uint32_t sampleRate, int format, int channelCount);

    // create I/O streams
    virtual AudioStreamOut* openOutputStream(
                                uint32_t devices,
                                int *format=0,
                                uint32_t *channels=0,
                                uint32_t *sampleRate=0,
                                status_t *status=0);
    virtual    void        closeOutputStream(AudioStreamOut* out);

    virtual AudioStreamIn* openInputStream(
                                uint32_t devices,
                                int *format,
                                uint32_t *channels,
                                uint32_t *sampleRate,
                                status_t *status,
                                AudioSystem::audio_in_acoustics acoustics);
    virtual    void        closeInputStream(AudioStreamIn* in);
    static AudioHardwareInterface* createA2dpInterface();

protected:
    virtual status_t    dump(int fd, const Vector<String16>& args);

private:
    class A2dpAudioStreamOut : public AudioStreamOut {
    public:
                            A2dpAudioStreamOut();
        virtual             ~A2dpAudioStreamOut();
                status_t    set(uint32_t device,
                                int *pFormat,
                                uint32_t *pChannels,
                                uint32_t *pRate);
        virtual uint32_t    sampleRate() const { return 44100; }
        // SBC codec wants a multiple of 512
        virtual size_t      bufferSize() const { return 512 * 20; }
        virtual uint32_t    channels() const { return AudioSystem::CHANNEL_OUT_STEREO; }
        virtual int         format() const { return AudioSystem::PCM_16_BIT; }
        virtual uint32_t    latency() const { return ((1000*bufferSize())/frameSize())/sampleRate(); }
        virtual status_t    setVolume(float left, float right) { return INVALID_OPERATION; }
        virtual status_t    setMuteModeNormal(int Mutecount);
        virtual ssize_t     write(const void* buffer, size_t bytes);
                status_t    standby();
        virtual status_t    dump(int fd, const Vector<String16>& args);
        virtual status_t    setParameters(const String8& keyValuePairs);
        virtual String8     getParameters(const String8& keys);
        virtual status_t    getRenderPosition(uint32_t *dspFrames);

    private:
        friend class A2dpAudioInterface;
                status_t    init();
                status_t    close();
                status_t    close_l();
                status_t    setAddress(const char* address);
                status_t    setBluetoothEnabled(bool enabled);
                status_t    setSuspended(bool onOff);
                status_t    standby_l();

    private:
                int         mFd;
                bool        mStandby;
                int         mStartCount;
                int         mRetryCount;
                char        mA2dpAddress[20];
                void*       mData;
                Mutex       mLock;
                bool        mBluetoothEnabled;
                uint32_t    mDevice;
                bool        mClosing;
                bool        mSuspended;
                nsecs_t     mLastWriteTime;
                uint32_t    mBufferDurationUs;

        uint32_t WriteMuteCounter;
        bool     mIncallSuspened;
#ifdef DUMP_A2DPSTREAMOUT
        FILE  *pA2dpinputFile;
#endif
    };

    friend class A2dpAudioStreamOut;

    A2dpAudioStreamOut*     mOutput;
    AudioHardwareInterface  *mHardwareInterface;
    char        mA2dpAddress[20];
    bool        mBluetoothEnabled;
    bool        mSuspended;

    bool        mIncallSuspened;
};

// ----------------------------------------------------------------------------

extern "C" AudioHardwareInterface* createA2DPAudioHardware(void);

}; // namespace android

#endif // A2DP_AUDIO_HARDWARE_H
