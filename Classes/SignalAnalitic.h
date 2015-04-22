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

class RoundBuff
{
public:
    size_t pos;
    Signal data;
    RoundBuff(size_t signalSize):pos(0), data(signalSize, 0){}
    size_t size() const{ return data.size();}
    void addData(const Signal& source);
    void clearData();
};

struct Spectr
{
    SpectrData data;
    double maxAmplutide;
    double minAmplutide;
    double deltaFreq;
    double maxFreq;
};

class SignalAnalitic
{
public:
    SignalAnalitic();
    ~SignalAnalitic();
    static Signal avarage(const Signal& signal, size_t dataCount, float delta, float avDelta);
    static void avarage(const Signal& signal, size_t dataCount, float deltaTime, float timeInPoint, RoundBuff& dest);
    static bool getMinMax(const Signal& signal, SignalDataType& minY, SignalDataType& maxY);
    static void FFT(const Signal& signal, size_t startIdx, float deltaTime, Spectr& spectr);
    static void testSignal(int periodCount, float shiftX, float shiftY, Signal& signal);
private:
    //std::vector<SignalDataType> _signal;
};

#endif /* defined(__trapSound__SignalAnalitic__) */
