//
//  SignalAnalitic.cpp
//  trapSound
//
//  Created by Дима on 25.02.15.
//
//

#include "SignalAnalitic.h"
#include "FFT.h"
#include <math.h>
#include <algorithm>

void RoundBuff::addData(const Signal& source)
{
    size_t destSize = data.size();
    for(auto& item : source)
    {
        data[pos] = item;
        pos++;
        if (pos >= destSize)
            pos = 0;
    }
}

void RoundBuff::clearData()
{
    std::fill(data.begin(), data.end(), 0);
    pos = 0;
}

Signal SignalAnalitic::avarage(const Signal& signal, size_t dataCount, float delta, float avDelta)
{
    std::vector<SignalDataType> res;
    size_t avrCount = ceil(dataCount*(double)delta/(double)avDelta);
    res.reserve(avrCount);
    
    float idx = 0;
    long sum = 0;
    size_t sumCount = 0;
    auto itEnd = signal.begin() +  std::min(dataCount, signal.size());
    for(auto it = signal.begin(); it != itEnd; ++it)
    {
        idx += delta;
        if (idx < avDelta)
        {
            sum += *it;
            sumCount += 1;
        }
        else
        {
            auto dy = *it - *(it-1);
            idx -= avDelta;
            sum += static_cast<long>(dy*(1.f - idx));
            res.push_back(sum/sumCount);
            
            sumCount = idx;
            sum = static_cast<long>(dy*idx);
        }
    }
    if (sumCount != 0)
        res.push_back(sum/sumCount);
    return res;
}

void SignalAnalitic::avarage(const Signal& signal, size_t dataCount, float delta, float avDelta, RoundBuff& dest)
{
    //size_t avrCount = ceil(dataCount*(double)delta/(double)avDelta);
    size_t destIdx = dest.pos;
    auto itEnd = signal.begin() +  std::min(dataCount, signal.size());
    size_t destSize = dest.size();
    
    if (avDelta <= delta)
    {
        for(auto it = signal.begin(); it != itEnd; ++it)
        {
            dest.data[destIdx] = *it;
            destIdx++;
            if (destSize == destIdx)
                destIdx = 0;
        }
    }
    else
    {
        float idx = 0;
        long sum = 0;
        size_t sumCount = 0;

        for(auto it = signal.begin(); it != itEnd; ++it)
        {
            idx += delta;
            if (idx < avDelta)
            {
                sum += *it;
                sumCount += 1;
            }
            else
            {
                auto dy = *it - *(it-1);
                idx -= avDelta;
                sum += static_cast<long>(dy*(1.f - idx));
                dest.data[destIdx] = sum/sumCount;
                destIdx++;
                if (destSize == destIdx)
                    destIdx = 0;
                
                sumCount = idx;
                sum = static_cast<long>(dy*idx);
            }
        }
        if (sumCount != 0)
        {
            dest.data[destIdx] = sum/sumCount;
            destIdx++;
            if (destSize == destIdx)
                destIdx = 0;
        }
    }

    dest.pos = destIdx;

}

bool SignalAnalitic::getMinMax(const std::vector<SignalDataType>& signal, SignalDataType& minY, SignalDataType& maxY)
{
    if (signal.empty())
        return false;
    minY = maxY = signal[0];
    for(auto& item : signal)
    {
        if (item > maxY)
            maxY = item;
        else if (item < minY)
            minY = item;
    }
    return true;
}

void SignalAnalitic::FFT(const Signal& signal, size_t startIdx, float deltaTime, Spectr& spectr)
{
    int pow2 = 0;
    size_t sourceSize = signal.size();
    while(sourceSize > 1)
    {
        pow2++;
        sourceSize >>= 1;
    }
    if ( (1 << pow2) != spectr.data.size() )
    {
        spectr.data.resize(1 << pow2);
        spectr.amplituda.data.resize(1 << (pow2-1));
    }
    sourceSize = signal.size();
    auto it = signal.begin() + startIdx;
    for(auto& item : spectr.data)
    {
        item.re = *it;
        item.im = 0;
        it++;
        if (it >= signal.end())
            it = signal.begin();
    }
    fft(&spectr.data[0], pow2, false);
    
    size_t idx = 0;
    auto itE = spectr.data.begin();
    std::advance(itE, 1 << (pow2 - 1));
    
    pow2 = 1 << pow2;
    for(auto it = spectr.data.begin(); it != itE; ++it)
    {
        auto& item = *it;
        spectr.amplituda.data[idx] = sqrt(item.re*item.re + item.im*item.im)/pow2;
        idx++;
    }
    
    auto& spektrAmpl = spectr.amplituda.data;
    spectr.amplituda.minY = spectr.amplituda.maxY = spektrAmpl[0];
    for(auto& item : spectr.amplituda.data)
    {
        if (item > spectr.amplituda.maxY)
            spectr.amplituda.maxY = item;
        else if (item < spectr.amplituda.minY)
            spectr.amplituda.minY = item;
    }
    spectr.maxFreq = 1.f/(2*deltaTime);
    spectr.deltaFreq = spectr.maxFreq/pow2;
}

void SignalAnalitic::FFT(const Signal& source, size_t startIdx, float deltaTime, Signal& dest)
{

}

void SignalAnalitic::testSignal(int periodCount, float shiftX, float shiftY, Signal& signal)
{
    size_t size = signal.size();
    float dt =2*M_PI*periodCount/size;
    float ampl = ((sizeof(SignalDataType) << 8) - shiftY)/2.f;
    for(size_t i = 0; i < size; i++)
    {
        signal.at(i) = static_cast<SignalDataType>(ampl*sin(dt*i + shiftX) + shiftY);
    }
}