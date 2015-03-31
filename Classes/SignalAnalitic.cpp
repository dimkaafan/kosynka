//
//  SignalAnalitic.cpp
//  trapSound
//
//  Created by Дима on 25.02.15.
//
//

#include "SignalAnalitic.h"

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

size_t SignalAnalitic::avarage(const std::vector<SignalDataType>& signal, size_t dataCount, float delta, float avDelta, std::vector<SignalDataType>& dest, size_t destIdx)
{
    size_t avrCount = ceil(dataCount*(double)delta/(double)avDelta);
    size_t destSize = dest.size();
    
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

Spectr SignalAnalitic::FFT(Signal& signal, float deltaTime)
{
    Spectr res;
    return res;
}