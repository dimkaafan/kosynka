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

SignalManager::SignalManager(int bufferCount, size_t signalSize, float avrDt):
_queue(nullptr)
,_bufferCount(bufferCount)
,_onRecieve(nullptr)
,_avrData(signalSize,0)
,_adrDt(avrDt)
,_rawData(bufferCount)
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
        unsigned long frameSize = ceil(_sndBuffLen*mDataFormat.mSampleRate * mDataFormat.mBytesPerFrame);
        std::vector<AudioQueueBufferRef> aBuffers(_bufferCount, nullptr);

        for (size_t i=0; i< aBuffers.size(); i++)
        {
            AudioQueueAllocateBuffer(queue, frameSize, &aBuffers[i]);
            aBuffers[i]->mUserData = reinterpret_cast<void*>(i);
            AudioQueueEnqueueBuffer(queue, aBuffers[i], 0, NULL);
            _rawData[i].resize(frameSize);
        }
        _queue = queue;
        
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
    if (inNumberPacketDescriptions == 0)
        return;
    _recieveCount++;
    long buffIdx = reinterpret_cast<long>(inBuffer->mUserData);
    auto& rawBuff = _rawData[buffIdx];
    memcpy(&rawBuff[0], inBuffer->mAudioData, inBuffer->mAudioDataByteSize);
    AudioQueueEnqueueBuffer(_queue, inBuffer, 0, NULL);
    
    auto avrData = SignalAnalitic::avarage(rawBuff, inNumberPacketDescriptions, _dt, _adrDt);
    //std::fill(_rawData.begin(), _rawData.begin() + inNumberPacketDescriptions , 0);

    addAvrData(avrData, buffIdx);
    
    if (_onRecieve)
        _onRecieve(_recieveCount);
}

void SignalManager::setOnRecieveFunction(const std::function<void(long long)>& func)
{
    _onRecieve = func;
}

const std::vector<SignalDataType>& SignalManager::getAvrSignal() const
{
    return _avrData;
}

SignalDataType SignalManager::getMinY() const
{
    return _minY;
}

SignalDataType SignalManager::getMaxY() const
{
    return _maxY;
}

float SignalManager::getXTime() const
{
    return _adrDt*_avrData.size();
}

void SignalManager::addAvrData(const std::vector<SignalDataType>& source, int buffIdx)
{
    size_t delta = _avrData.size() - _nextIdx;
    size_t sourceIdx = 0;
    if (source.size() > _avrData.size())
        sourceIdx = source.size() - _avrData.size();
    size_t deltaSource = source.size() - sourceIdx;
    
//    while (deltaSource > 0)
//    {
//        if (delta > deltaSource)
//        {
//            memcpy(&_avrData[_nextIdx], &source[sourceIdx], deltaSource*sizeof(SignalDataType));
//            _nextIdx += deltaSource;
//            sourceIdx += deltaSource;
//            deltaSource = source.size() - sourceIdx;
//        }
//    }
    
    int zeroCount = 0;
    std::vector<int> zeroStat;
    size_t destSize = _avrData.size();
    for(auto& item : source)
    {
        if (item == 0)
        {
            zeroCount++;
            //continue;
        }
        else
        {
            if (zeroCount > 0)
            {
                zeroStat.emplace_back(zeroCount);
                zeroCount = 0;
            }
        }
        
        _avrData[_nextIdx] = item;
        _nextIdx++;
        if (_nextIdx >= destSize)
            _nextIdx = 0;
    }
    if (!zeroStat.empty())
    {
        int all = 0;
        for(auto& item : zeroStat)
            all += item;
        if (all*100.f/source.size() > 80.f)
        {
            
        }
            
    }
    
    SignalAnalitic::getMinMax(_avrData, _minY, _maxY);
}
