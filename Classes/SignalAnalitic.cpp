//
//  SignalAnalitic.cpp
//  trapSound
//
//  Created by Дима on 25.02.15.
//
//

#include "SignalAnalitic.h"

std::vector<SignalDataType> SignalAnalitic::avarage(const std::vector<SignalDataType>& signal, float delta, float avDelta)
{
    float duration = signal.size()*delta;
    std::vector<SignalDataType> res(static_cast<size_t>(duration/avDelta + 0.5f), 0);
    
    float dInt = avDelta/delta;
    float idx = 0;
    long sum = 0;
    size_t sumCount = 0;
    size_t resIdx = 0;
    for(auto& item : signal)
    {
        if (idx < dInt)
        {
            sum += item;
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
    return std::move(res);
}