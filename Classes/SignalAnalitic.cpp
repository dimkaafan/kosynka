//
//  SignalAnalitic.cpp
//  trapSound
//
//  Created by Дима on 25.02.15.
//
//

#include "SignalAnalitic.h"

static int s_maxZro = 0;

std::vector<SignalDataType> SignalAnalitic::avarage(const std::vector<SignalDataType>& signal, size_t dataCount, float delta, float avDelta)
{
    float duration = signal.size()*delta;
    std::vector<SignalDataType> res(static_cast<size_t>(duration/avDelta + 0.5f), 0);
    
    float dInt = avDelta/delta;
    float idx = 0;
    long sum = 0;
    size_t sumCount = 0;
    size_t resIdx = 0;
    size_t zeroCount = 0;
    std::vector<int> zeroStat;
    auto itEnd = signal.begin() +  std::max(dataCount, signal.size());
    for(auto it = signal.begin(); it != itEnd; ++it)
    {
        if (*it == 0)
            zeroCount++;
        else
        {
            if (zeroCount > 2)
            {
                zeroStat.push_back(zeroCount);
                zeroCount = 0;
            }
        }
        
        if (idx < dInt)
        {
            sum += *it;
            idx++;
            sumCount++;
        }
        else
        {
            res[resIdx] = sum/sumCount;
            idx -= dInt;
            resIdx++;
            sumCount = 0;
            sum = 0;
        }
    }
    if (sumCount != 0)
    {
        res[resIdx] = sum/sumCount;
    }
    
    if (!zeroStat.empty())
    {
        int all = 0;
        auto it = std::max_element(zeroStat.begin(), zeroStat.end());
        if (it != zeroStat.end() && *it > s_maxZro)
            s_maxZro = *it;
        for(auto& item : zeroStat)
            all += item;
        if (all*100.f/signal.size() > 80.f)
            return std::vector<SignalDataType>(0);
    }
    
    return std::move(res);
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