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
#include "SignalTypes.h"
class SignalManager
{
public:
    SignalManager(int bufferCount);
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
private:
    AudioQueueRef _queue = nullptr;
    int _bufferCount = 16;
    std::vector<SignalDataType> _rawData;
    
};

#endif /* defined(__trapSound__SignalManager__) */
