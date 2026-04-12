/*
 * Signal helper macros for optional ECS signal generation.
 *
 * Define GD_ECS_DO_NOT_GENERATE_SIGNAL_CODE before including this file to completely disable all
 * generated signal code.
 */



#pragma once



#include "general_helper_macros.hpp"



#if defined(GD_ECS_DO_NOT_GENERATE_SIGNAL_CODE)



#define GD_ECS_EMPTY_SIGNAL_STRUCT(SIGNAL_STRUCT_NAME, SIGNAL_STRUCT_PARENT_TYPE)



#else // defined(GD_ECS_DO_NOT_GENERATE_SIGNAL_CODE)



#if _HELPER_GD_ECS_HAS_INCLUDE("godot_cpp_util/core/variant/gd_typed_signal.hpp")

#include "godot_cpp_util/core/variant/gd_typed_signal.hpp"

#define GD_ECS_EMPTY_SIGNAL_STRUCT(SIGNAL_STRUCT_NAME, SIGNAL_STRUCT_PARENT_TYPE)                  \
struct SIGNAL_STRUCT_NAME : public SIGNAL_STRUCT_PARENT_TYPE {};

#else

#define GD_ECS_EMPTY_SIGNAL_STRUCT(SIGNAL_STRUCT_NAME, SIGNAL_STRUCT_PARENT_TYPE)                  \
struct SIGNAL_STRUCT_NAME {};

#endif



#endif // defined(GD_ECS_DO_NOT_GENERATE_SIGNAL_CODE)
