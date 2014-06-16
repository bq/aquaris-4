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

/* //device/servers/AudioFlinger/AudioHardwareStub.h
**
** Copyright 2007, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#ifndef ANDROID_AUDIO_HARDWARE_STUB_H
#define ANDROID_AUDIO_HARDWARE_STUB_H

#include <stdint.h>
#include <sys/types.h>

#include <hardware_legacy/AudioHardwareBase.h>

namespace android_audio_legacy {

// ----------------------------------------------------------------------------

class AudioStreamOutStub : public AudioStreamOut {
public:
    virtual status_t    set(int *pFormat, uint32_t *pChannels, uint32_t *pRate);
    virtual uint32_t    sampleRate() const { return 44100; }
    virtual size_t      bufferSize() const { return 4096; }
    virtual uint32_t    channels() const { return AudioSystem::CHANNEL_OUT_STEREO; }
    virtual int         format() const { return AudioSystem::PCM_16_BIT; }
    virtual uint32_t    latency() const { return 0; }
    virtual status_t    setVolume(float left, float right) { return NO_ERROR; }
    virtual ssize_t     write(const void* buffer, size_t bytes);
    virtual status_t    standby();
    virtual status_t    dump(int fd, const Vector<String16>& args);
    virtual status_t    setParameters(const String8& keyValuePairs) { return NO_ERROR;}
    virtual String8     getParameters(const String8& keys);
    virtual status_t    getRenderPosition(uint32_t *dspFrames);
};

class AudioStreamInStub : public AudioStreamIn {
public:
    virtual status_t    set(int *pFormat, uint32_t *pChannels, uint32_t *pRate, AudioSystem::audio_in_acoustics acoustics);
    virtual uint32_t    sampleRate() const { return 8000; }
    virtual size_t      bufferSize() const { return 320; }
    virtual uint32_t    channels() const { return AudioSystem::CHANNEL_IN_MONO; }
    virtual int         format() const { return AudioSystem::PCM_16_BIT; }
    virtual status_t    setGain(float gain) { return NO_ERROR; }
    virtual ssize_t     read(void* buffer, ssize_t bytes);
    virtual status_t    dump(int fd, const Vector<String16>& args);
    virtual status_t    standby() { return NO_ERROR; }
    virtual status_t    setParameters(const String8& keyValuePairs) { return NO_ERROR;}
    virtual String8     getParameters(const String8& keys);
    virtual unsigned int  getInputFramesLost() const { return 0; }
    #ifndef ANDROID_DEFAULT_CODE
    virtual status_t    addAudioEffect(effect_handle_t effect) { return NO_ERROR; }
    virtual status_t    removeAudioEffect(effect_handle_t effect) { return NO_ERROR; }
    #endif
};

class AudioHardwareStub : public  AudioHardwareBase
{
public:
                        AudioHardwareStub();
    virtual             ~AudioHardwareStub();
    virtual status_t    initCheck();
    virtual status_t    setVoiceVolume(float volume);
    virtual status_t    setMasterVolume(float volume);

    // mic mute
    virtual status_t    setMicMute(bool state) { mMicMute = state;  return  NO_ERROR; }
    virtual status_t    getMicMute(bool* state) { *state = mMicMute ; return NO_ERROR; }
#ifndef ANDROID_DEFAULT_CODE
    virtual status_t SetEMParameter(void *ptr, int len)
    {
        return NO_ERROR;
    }
    virtual status_t GetEMParameter(void *ptr, int len)
    {
        return NO_ERROR;
    }
    virtual status_t SetAudioCommand(int par1, int par2)
    {
        return NO_ERROR;
    }
    virtual status_t GetAudioCommand(int par1)
    {
        return NO_ERROR;
    }
    virtual status_t SetAudioData(int par1,size_t len,void *ptr)
    {
        return NO_ERROR;
    }
    virtual status_t GetAudioData(int par1,size_t len,void *ptr)
    {
        return NO_ERROR;
    }

    virtual status_t SetACFPreviewParameter(void *ptr, int len)
    {
        return NO_ERROR;
    }
    virtual status_t SetHCFPreviewParameter(void *ptr, int len)
    {
        return NO_ERROR;
    }

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


    /////////////////////////////////////////////////////////////////////////
    //    for PCMxWay Interface API ...   
    /////////////////////////////////////////////////////////////////////////
    virtual int xWayPlay_Start(int sample_rate)
    {
        return NO_ERROR;
    }
    virtual int xWayPlay_Stop(void)
    {
        return NO_ERROR;
    }
    virtual int xWayPlay_Write(void *buffer, int size_bytes)
    {
        return NO_ERROR;
    }
    virtual int xWayPlay_GetFreeBufferCount(void)
    {
        return NO_ERROR;
    }
    virtual int xWayRec_Start(int sample_rate)
    {
        return NO_ERROR;
    }
    virtual int xWayRec_Stop(void)
    {
        return NO_ERROR;
    }
    virtual int xWayRec_Read(void *buffer, int size_bytes)
    {
        return NO_ERROR;
    }
#endif
protected:
    virtual status_t    dump(int fd, const Vector<String16>& args);

            bool        mMicMute;
private:
    status_t            dumpInternals(int fd, const Vector<String16>& args);
};

// ----------------------------------------------------------------------------

}; // namespace android

#endif // ANDROID_AUDIO_HARDWARE_STUB_H
