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
#include "FFT.h"

typedef std::vector<SignalDataType> Signal;
typedef std::vector<ShortComplex> SpectrData;

struct Spectr
{
    SpectrData data;
    double maxAmplutide;
    double minAmplutide;
    double deltaFreq;
};

class SignalAnalitic
{
public:
    SignalAnalitic();
    ~SignalAnalitic();
    static Signal avarage(const Signal& signal, size_t dataCount, float delta, float avDelta);
    static size_t avarage(const Signal& signal, size_t dataCount, float deltaTime, float avDelta, Signal& dest, size_t destIdx);
    static bool getMinMax(const Signal& signal, SignalDataType& minY, SignalDataType& maxY);
    static void FFT(const Signal& signal, size_t startIdx, float deltaTime, Spectr& spectr);
private:
    //std::vector<SignalDataType> _signal;
};

#endif /* defined(__trapSound__SignalAnalitic__) */
