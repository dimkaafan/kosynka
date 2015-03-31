//
//  SignalAnalitic.h
//  trapSound
//
//  Created by Дима on 25.02.15.
//
//

#ifndef __trapSound__SignalAnalitic__
#define __trapSound__SignalAnalitic__

#include <vector>
#include "SignalTypes.h"

typedef std::vector<SignalDataType> Signal;
typedef std::vector<SpectrItem> Spectr;
class SignalAnalitic
{
public:
    SignalAnalitic();
    ~SignalAnalitic();
    static Signal avarage(const Signal& signal, size_t dataCount, float delta, float avDelta);
    static size_t avarage(const Signal& signal, size_t dataCount, float deltaTime, float avDelta, Signal& dest, size_t destIdx);
    static bool getMinMax(const Signal& signal, SignalDataType& minY, SignalDataType& maxY);
    static Spectr FFT(Signal& signal, float deltaTime);
private:
    //std::vector<SignalDataType> _signal;
};

#endif /* defined(__trapSound__SignalAnalitic__) */
