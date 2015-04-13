//
//  SignalAnalitic.cpp
//  trapSound
//
//  Created by Дима on 25.02.15.
//
//

#include "SignalAnalitic.h"
#include "FFT.h"

std::vector<SignalDataType> SignalAnalitic::avarage(const std::vector<SignalDataType>& signal, size_t dataCount, float delta, float avDelta)
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

size_t SignalAnalitic::avarage(const Signal& signal, size_t dataCount, float delta, float avDelta, Signal& dest, size_t destIdx)
{
    //size_t avrCount = ceil(dataCount*(double)delta/(double)avDelta);
    auto itEnd = signal.begin() +  std::min(dataCount, signal.size());
    size_t destSize = dest.size();
    
    if (avDelta <= delta)
    {
        for(auto it = signal.begin(); it != itEnd; ++it)
        {
            dest[destIdx] = *it;
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
                dest[destIdx] = sum/sumCount;
                destIdx++;
                if (destSize == destIdx)
                    destIdx = 0;
                
                sumCount = idx;
                sum = static_cast<long>(dy*idx);
            }
        }
        if (sumCount != 0)
        {
            dest[destIdx] = sum/sumCount;
            destIdx++;
            if (destSize == destIdx)
                destIdx = 0;
        }
    }

    return destIdx;

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
        spectr.data.resize(1 << pow2); //setSize(1 << pow2);
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
    pow2 = 1 << pow2;
    for(auto& item : spectr.data)
    {
        item.re = sqrt(item.re*item.re + item.im*item.im)/pow2;
    }
    
    spectr.maxAmplutide = spectr.minAmplutide = spectr.data[0].re;
    for(auto& item : spectr.data)
    {
        if (item.re > spectr.maxAmplutide)
            spectr.maxAmplutide = item.re;
        else if (item.re < spectr.minAmplutide)
            spectr.minAmplutide = item.re;
    }
}