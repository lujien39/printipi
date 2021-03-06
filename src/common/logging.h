#ifndef COMMON_LOGGING_H
#define COMMON_LOGGING_H

/* 
 * Printipi/common/logging.h
 * (c) 2014 Colin Wallace
 *
 * This file provides some functions that allow for logging information to stdout.
 * Use LOG for information logging, LOGE to log an error, LOGW for warnings, LOGD for debug logging, and LOGV for verbose debug logging.
 */

#include <stdio.h>
#ifdef DNO_LOGGING
    #define DO_LOG 0
#else
    #define DO_LOG 1
#endif
#ifdef DNO_LOG_M105
    #define NO_LOG_M105 1
#else
    #define NO_LOG_M105 0
#endif

#if DO_LOG == 1

    #define LOGE(format, args...) \
        if (logging::isInfoEnabled()) { \
            fprintf(stderr, format, ## args); \
        }
    #define LOGW(format, args...) \
        if (logging::isInfoEnabled()) { \
            printf(format, ## args); \
        }
    #define LOG(format, args...) \
        if (logging::isInfoEnabled()) { \
            printf(format, ## args); \
        }
    #define LOGD(format, args...) \
        if (logging::isDebugEnabled()) { \
            printf(format, ## args); \
        }
    #define LOGV(format, args...) \
        if (logging::isVerboseEnabled()) { \
            printf(format, ## args); \
        }
    
#else

    #define LOGE(format, args...) {}
    #define LOGW(format, args...) {}
    #define LOG(format, args...) {}
    #define LOGD(format, args...) {}
    #define LOGV(format, args...) {}

#endif
    
namespace logging {

#if DO_LOG == 1

extern bool _info;
extern bool _debug;
extern bool _verbose;

inline bool isInfoEnabled() {
    return _info;
}
inline bool isDebugEnabled() {
    return _debug;
}
inline bool isVerboseEnabled() {
    return _verbose;
}

inline void disable() {
    _info = false;
    _debug = false;
    _verbose = false;
}

inline void enableDebug() {
    _debug = true;
    LOG("debug logging enabled\n");
}
inline void enableVerbose() {
    enableDebug();
    _verbose = true;
    LOG("verbose logging enabled\n");
}

#else

inline bool isInfoEnabled() {
    return false;
}
inline bool isVerboseEnabled() {
    return false;
}
inline bool isDebugEnabled() {
    return false;
}

inline void disable() {}
inline void enableVerbose() {}
inline void enableDebug() {}

#endif

}

#endif
