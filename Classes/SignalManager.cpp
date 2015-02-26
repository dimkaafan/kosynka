//
//  SignalManager.cpp
//  trapSound
//
//  Created by Дима on 25.02.15.
//
//

#include "SignalManager.h"

#include <set>
#include <CoreAudio/CoreAudioTypes.h>
#include "SignalAnalitic.h"


static std::set<void*> s_signals;

void audioDelegate(
                   void *                          inUserData,
                   AudioQueueRef                   inAQ,
                   AudioQueueBufferRef             inBuffer,
                   const AudioTimeStamp *          inStartTime,
                   UInt32                          inNumberPacketDescriptions,
                   const AudioStreamPacketDescription *inPacketDescs)
{
    auto it = s_signals.find(inUserData);
    if (it != s_signals.end())
    {
        SignalManager* pMan = reinterpret_cast<SignalManager*>(*it);
        pMan->audioCallback(inUserData, inAQ, inBuffer, inStartTime, inNumberPacketDescriptions, inPacketDescs);
    }
}

SignalManager::SignalManager(int bufferCount):
_queue(nullptr)
,_bufferCount(bufferCount)
,_onRecieve(nullptr)
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
    _dt = 1.f/mDataFormat.mSampleRate;
    
    AudioQueueRef queue = nullptr;
    AudioQueueNewInput(&mDataFormat, audioDelegate, this, NULL, kCFRunLoopCommonModes, 0, &queue);
    
    if (queue)
    {
        unsigned long frameSize = mDataFormat.mSampleRate * mDataFormat.mBytesPerFrame;
        std::vector<AudioQueueBufferRef> aBuffers(_bufferCount, nullptr);

        for (size_t i=0; i< aBuffers.size(); i++)
        {
            AudioQueueAllocateBuffer(queue, frameSize, &aBuffers[i]);
            AudioQueueEnqueueBuffer(queue, aBuffers[i], 0, NULL);
        }
        _queue = queue;
        _rawData.resize(mDataFormat.mSampleRate*mDataFormat.mChannelsPerFrame);
        s_signals.insert(this);
    }
}

void SignalManager::start()
{
    if (_queue)
        AudioQueueStart(_queue, NULL);
}

void SignalManager::pause()
{
    if (_queue)
        AudioQueuePause(_queue);//AudioQueueStop, true
}

void SignalManager::audioCallback(
                   void *                          inUserData,
                   AudioQueueRef                   inAQ,
                   AudioQueueBufferRef             inBuffer,
                   const AudioTimeStamp *          inStartTime,
                   UInt32                          inNumberPacketDescriptions,
                   const AudioStreamPacketDescription *inPacketDescs)
{
    if (this != inUserData)
        return;
    _recieveCount++;
    memcpy(&_rawData[0], inBuffer->mAudioData, inBuffer->mAudioDataByteSize);
    AudioQueueEnqueueBuffer(_queue, inBuffer, 0, NULL);
    SignalAnalitic::avarage(_rawData, _dt, 0.1f);
    if (_onRecieve)
        _onRecieve(_recieveCount);
}

void SignalManager::setOnRecieveFunction(const std::function<void(long long)>& func)
{
    _onRecieve = func;
}
