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
class SignalManager
{
public:
    SignalManager(int bufferCount, size_t signalSize, float avrDt);
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
    const std::vector<SignalDataType>& getAvrSignal(size_t& startIdx) const;
    SignalDataType getMinY() const;
    SignalDataType getMaxY() const;
    float getXTime() const;
    
private:
    AudioQueueRef _queue = nullptr;
    int _bufferCount = 16;
    std::vector< std::vector<SignalDataType> > _rawData;
    std::vector<SignalDataType> _avrData;
    SignalDataType _minY = 0;
    SignalDataType _maxY = 0;
    float _sndBuffLen = 0.2f;// sec
    float _dt = 0;
    float _adrDt = 0.001f;
    long long _recieveCount = 0;
    size_t _nextIdx = 0;
    std::function<void(long long)> _onRecieve;
    
    void addAvrData(const std::vector<SignalDataType>& source, int buffIdx);
    
};

#endif /* defined(__trapSound__SignalManager__) */
