//
//  SignalManager.cpp
//  trapSound
//
//  Created by Дима on 25.02.15.
//
//

#include "SignalManager.h"
#include <CoreAudio/CoreAudioTypes.h>


void audioDelegate(
                   void *                          inUserData,
                   AudioQueueRef                   inAQ,
                   AudioQueueBufferRef             inBuffer,
                   const AudioTimeStamp *          inStartTime,
                   UInt32                          inNumberPacketDescriptions,
                   const AudioStreamPacketDescription *inPacketDescs)
{
    //g_scene->audioDelegate(inUserData, inAQ, inBuffer, inStartTime, inNumberPacketDescriptions, inPacketDescs);
}

SignalManager::SignalManager(int bufferCount):
_queue(nullptr)
,_bufferCount(bufferCount)
{
    
}

SignalManager::~SignalManager()
{
    if (_queue)
        AudioQueueDispose(_queue, true);
}

void SignalManager::init()
{
    AudioStreamBasicDescription mDataFormat;
    mDataFormat.mFormatID = kAudioFormatLinearPCM;
    mDataFormat.mSampleRate = 44100;
    mDataFormat.mChannelsPerFrame = 1;
    mDataFormat.mBitsPerChannel = 16;
    mDataFormat.mBytesPerPacket =
    mDataFormat.mBytesPerFrame = mDataFormat.mChannelsPerFrame * sizeof(SignalDataType);
    mDataFormat.mFramesPerPacket = 1;
    mDataFormat.mFormatFlags = kLinearPCMFormatFlagIsBigEndian | kLinearPCMFormatFlagIsSignedInteger | kLinearPCMFormatFlagIsPacked;
    AudioQueueRef queue;
    //AudioQueueInputCallback
    AudioQueueInputCallback AQInputCallback = [](
                                                 void *                          inUserData,
                                                 AudioQueueRef                   inAQ,
                                                 AudioQueueBufferRef             inBuffer,
                                                 const AudioTimeStamp *          inStartTime,
                                                 UInt32                          inNumberPacketDescriptions,
                                                 const AudioStreamPacketDescription *inPacketDescs)
    {
        //g_scene->audioCallback(inUserData, inAQ, inBuffer, inStartTime, inNumberPacketDescriptions, inPacketDescs);
    };
    
    AudioQueueNewInput(&mDataFormat, audioDelegate, NULL, NULL, kCFRunLoopCommonModes, 0, &queue);
    
    
    unsigned long frameSize = mDataFormat.mSampleRate * mDataFormat.mBytesPerFrame;
    std::vector<AudioQueueBufferRef> aBuffers(_bufferCount, nullptr);

    for (size_t i=0; i< aBuffers.size(); i++)
    {
        AudioQueueAllocateBuffer(queue, frameSize, &aBuffers[i]);
        AudioQueueEnqueueBuffer(queue, aBuffers[i], 0, NULL);
    }
    _queue = queue;
    _rawData.resize(mDataFormat.mSampleRate*mDataFormat.mChannelsPerFrame);
}

void SignalManager::start()
{
    if (_queue)
        AudioQueueStart(_queue, NULL);
}

void SignalManager::pause()
{
    
}