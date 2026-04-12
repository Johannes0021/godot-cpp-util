/**
 * This header provides a default ECS configuration based on the C++ ECS library EnTT
 * (https://github.com/skypjack/entt).
 *
 * If the default configuration is not suitable or EnTT is not used, refer to "ecs_impl.hpp" and
 * define a modified configuration it with the GD_ECS_IMPL macro.
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



#include "ecs_impl.hpp"



GD_ECS_IMPL(godot, ECS, Entity, Component, entt::registry)
