//
//  SignalTypes.h
//  trapSound
//
//  Created by Дима on 25.02.15.
//
//

#ifndef __trapSound__SignalTypes__
#define __trapSound__SignalTypes__

typedef short int SignalDataType;

template <class T>
struct SpectrItem
{
//    T frequency;
    T amplitude;
    T phase;
};

#endif /* defined(__trapSound__SignalTypes__) */
