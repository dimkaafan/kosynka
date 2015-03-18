//
//  SignalAnalitic.h
//  trapSound
//
//  Created by Дима on 25.02.15.
//
//

#ifndef __trapSound__SignalAnalitic__
#define __trapSound__SignalAnalitic__

#include "SignalTypes.h"

class SignalAnalitic
{
public:
    SignalAnalitic();
    ~SignalAnalitic();
    static std::vector<SignalDataType> avarage(const std::vector<SignalDataType>& signal, size_t dataCount, float delta, float avDelta);
    static bool getMinMax(const std::vector<SignalDataType>& signal, SignalDataType& minY, SignalDataType& maxY);
private:
    //std::vector<SignalDataType> _signal;
};

#endif /* defined(__trapSound__SignalAnalitic__) */
