//
//  Utils.cpp
//  trapSound
//
//  Created by dima on 24.04.15.
//
//

#include "Utils.h"
#include <math.h>
    
std::string GameUtils::floatToStr(float val)
{
    std::string res = std::to_string(val);
    size_t pos = res.length();
    auto itr = res.rbegin();
    for(;itr != res.rend(); ++itr)
        if(*itr != '0')
            break;
        else
            pos--;
    if (pos != 0 && res.at(pos-1) == '.')
        pos--;
    res.erase(res.begin() + pos, res.end());
    pos = res.find('.');
    if (pos != std::string::npos)
    {
        int count = 2;
        auto it = res.begin() + pos + 1;
        for(;it != res.end(); ++it)
        {
            if (*it != '0')
            {
                it += count;
                break;
            }
        }
        if (it < res.end())
            res.erase(it, res.end());
    }
    return res;
}

int GameUtils::get10power(float value)
{
    float pow10 = ::log10f(value);
    return pow10 >= 0 ? static_cast<int>(pow10) : static_cast<int>(pow10 - 1.f);
}

float GameUtils::get10Factor(float value)
{
    return ::powf(10.f, get10power(value));
}



//float trancate(float value, int signAfterPoint)
//{
//    float factor10 = get10Factor(value);
//    float sign = :powf(10.f, signAfterPoint);
//    value = (sign*value/factor10 + 0.5);
//    value = ::roundf(value);
//    
//}