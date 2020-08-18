//
// Created by Administrator on 2020/7/3 0003.
//

#ifndef FFMPEGOPENGLDEMO_VIDEOCONFIG_H
#define FFMPEGOPENGLDEMO_VIDEOCONFIG_H

#include "bx/debug.h"

#define SkASSERT(cond) static_cast<void>(0)
#define SkDebugf(fmt, ...)  bx::debugPrintf(fmt, __VA_ARGS__)


#define DESTROY_POINTER(q) if(q){ \
    delete q;  \
    q = nullptr; \
}

#define DESTROY_POINTER_FUNC(q, func_re) if(q){ \
    func_re(q); \
    delete q;  \
    q = nullptr; \
}

#define UN_REF_POINTER(p, func) if(p){ \
    func(p); \
    p = nullptr; \
}

//SIMPLE_DEMO only used simple
#ifdef ANDROID
    #ifdef SIMPLE_DEMO
    #else
        #ifndef USE_NATIVE_ACTIVITY
        #define USE_NATIVE_ACTIVITY 1
        #endif
    #endif
#endif
/**
 * @param str the full string
 * @param s the delimiter
 * @return the suffix string
 */
const char* getSuffixStr(const char* str, const char* s);

#define getEnumName(x) getSuffixStr(#x, ":")


#endif //FFMPEGOPENGLDEMO_VIDEOCONFIG_H