/**
 * This header provides a default ECS configuration based on the C++ ECS library EnTT
 * (https://github.com/skypjack/entt).
 *
 * If the default configuration is not suitable, refer to "clear_ecs_defines.hpp".
 *
 * If EnTT is not used, refer to "clear_ecs_defines.hpp" and the comment regarding the Registry
 * interface in "ecs_impl.hpp".
 *
 * For more information on how to use the ECS, refer to the other files in this directory.
 *
 * Usage Example:
 *
 * #pragma once
 *
 * #include "godot_cpp_util/ecs/ecs.hpp"
 *
 * // Register types to Godot:
 * ECS::register_types();
 */



#pragma once



// __has_include might not exist on very old compilers.
#if !defined(__has_include)
  #define __has_include(x) 0
#endif



#if __has_include("entt/entity/registry.hpp")
    #include "entt/entity/registry.hpp"
#elif __has_include("entt/entt.hpp")
    #include "entt/entt.hpp"
#else
    #error "EnTT not found. Please make sure EnTT is available in your include paths."
#endif



#include "godot_cpp_util/ecs/clear_ecs_defines.hpp"



#undef GD_ECS_DO_NOT_DEFINE_DEFAULTS



#define GD_ECS_REGISTRY_TYPE entt::registry



#include "ecs_impl.hpp"



#undef GD_ECS_DO_NOT_DEFINE_DEFAULTS
