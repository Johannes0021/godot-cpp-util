// General helper macros.



#pragma once



#ifndef _HELPER_GD_ECS_HAS_INCLUDE
    #if defined(__has_include)
        #define _HELPER_GD_ECS_HAS_INCLUDE(x) __has_include(x)
    #else
        #define _HELPER_GD_ECS_HAS_INCLUDE(x) 0
    #endif
#endif
