/**
 * This header provides a default ECS configuration based on the C++ ECS library EnTT
 * (https://github.com/skypjack/entt).
 *
 * If the default configuration is not suitable, EnTT is not used, or more than one ECS singleton
 * must coexist, see "ecs_impl.hpp" and provide a custom configuration by defining GD_ECS_IMPL.
 *
 * For more information on how to use the ECS, refer to the other files in this directory.
 *
 * // Register types to Godot:
 * godot::ECS::register_types();
 */



#pragma once



#include "helper_macros.hpp"



#if _HELPER_GD_ECS_HAS_INCLUDE("entt/entity/registry.hpp")
    #include "entt/entity/registry.hpp"
#elif _HELPER_GD_ECS_HAS_INCLUDE("entt/entt.hpp")
    #include "entt/entt.hpp"
#else
    #error "EnTT not found. Please make sure EnTT is available in your include paths."
#endif



#include "ecs_impl.hpp"



GD_ECS_IMPL(godot, ECS, C_Component, entt::registry)
