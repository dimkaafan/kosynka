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
    static std::vector<SignalDataType> avarage(const std::vector<SignalDataType>& signal, float delta, float avDelta);
private:
    std::vector<SignalDataType> _signal;
};

#endif /* defined(__trapSound__SignalAnalitic__) */
