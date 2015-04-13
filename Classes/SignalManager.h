//
//  SignalManager.h
//  trapSound
//
//  Created by Дима on 25.02.15.
//
//

#ifndef __trapSound__SignalManager__
#define __trapSound__SignalManager__

#include <AudioToolbox/AudioQueue.h>
#include <vector>
#include "SignalTypes.h"
#include "SignalAnalitic.h"

class SignalManager
{
public:
    SignalManager(size_t signalSize, float avrDt, float xTime);
    ~SignalManager();
    void init();
    
    void start();
    void pause();
    void audioCallback(
                       void *                          inUserData,
                       AudioQueueRef                   inAQ,
                       AudioQueueBufferRef             inBuffer,
                       const AudioTimeStamp *          inStartTime,
                       UInt32                          inNumberPacketDescriptions,
                       const AudioStreamPacketDescription *inPacketDescs);
    void setOnRecieveFunction(const std::function<void(long long)>& func);
    const Signal& getAvrSignal(size_t& startIdx) const;
    const Spectr& getSpectr() const;
    SignalDataType getMinY() const;
    SignalDataType getMaxY() const;
    float getXTime() const;
    void setSoundBuffTime(float timeSec);
    void setTimeInPoint(float pointTime);
    float getTimeInPoint() const;
    void setXTime(float xTime);
    
private:
    AudioQueueRef _queue = nullptr;
    int _bufferCount = 16;
    std::vector< Signal > _rawData;
    Signal _avrData;
    Spectr _spectr;
    SignalDataType _minY = 0;
    SignalDataType _maxY = 0;
    float _sndBuffLen = 0.05f;// sec
    float _dt = 0;
    float _pointTime = 0.001f;
    long long _recieveCount = 0;
    size_t _nextIdx = 0;
    std::function<void(long long)> _onRecieve;
    
    void addAvrData(const std::vector<SignalDataType>& source, int buffIdx);
    
};

#endif /* defined(__trapSound__SignalManager__) */
