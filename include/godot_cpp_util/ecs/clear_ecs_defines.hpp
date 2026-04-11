/**
 * Overview:
 *
 * By default, "ecs_impl.hpp" provides a set of predefined names, types, and macros.
 * If you want to use different names, types, or namespaces, you can override these defaults before
 * including "ecs_impl.hpp".
 *
 * A common approach is to create your own wrapper header, for example "my_ecs.hpp". In this header,
 * you define the desired ECS configuration values and then include "ecs_impl.hpp". Your code should
 * include this custom header instead of including "ecs_impl.hpp" directly.
 *
 * This approach allows you to:
 * - Use custom namespaces, registry types, and naming conventions.
 * - Cleanly separate ECS configurations.
 * - Define and use multiple independent ECS instances by assigning different namespaces and
 *   identifiers.
 *
 * Usage Example:
 *
 * // This example shows how to define a fully custom ECS configuration using a dedicated wrapper
 * // header, for example "my_ecs.hpp".
 * // This pattern allows multiple independent ECS setups to coexist in the same project, each
 * // isolated by its own namespace and configuration.
 *
 * // "my_ecs.hpp"
 * #pragma once
 *
 * // Step 1. Clear existing ECS macro definitions.
 * // This ensures that no previously defined values leak into this configuration.
 * #include "godot_cpp_util/ecs/clear_ecs_defines.hpp"
 *
 * // Step 2. (Optional) Prevent "ecs_impl.hpp" from defining its own defaults.
 * // This forces you to explicitly define all required names and types, which avoids accidental
 * // reliance on implicit defaults.
 * #define GD_ECS_DO_NOT_DEFINE_DEFAULTS
 *
 * // Step 3. Define all required ECS macros.
 * // Here you specify the namespace, registry type, and naming conventions for entities,
 * // components, and the ECS singleton.
 * #define GD_ECS_NAMESPACE my_ecs
 * #define GD_ECS_REGISTRY_TYPE entt::registry
 * #define GD_ECS_REGISTRY_CONSTRUCTION_ARGS
 * #define GD_ECS_SINGLETON_NAME MyECS
 * #define GD_ECS_ENTITY_NAME MyEntity
 * #define GD_ECS_COMPONENT_NAME MyComponent
 *
 * // Step 4. Include "ecs_impl.hpp".
 * // At this point, the ECS code is generated using the values you defined above.
 * #include "godot_cpp_util/ecs/ecs_impl.hpp"
 *
 * // Step 5. (Optional) Re-enable default ECS behavior.
 * // By undefining the guard macro, you allow later includes of ecs_impl.hpp to use the default
 * // configuration again, if desired.
 * #undef GD_ECS_DO_NOT_DEFINE_DEFAULTS
 */



// NOTE:
// Do NOT add #pragma once here.
// This file must be processed on every include so that all ECS-related macros are undefined each
// time a new ECS configuration is set up.
//#pragma once

// Undefine header guards.
#undef GODOT_CPP_UTIL_ECS_ECS_IMPL_HPP
#undef GODOT_CPP_UTIL_ECS_MACROS_IMPL_HPP

// Undefine configurations.
#undef GD_ECS_NAMESPACE
#undef GD_ECS_REGISTRY_TYPE
#undef GD_ECS_REGISTRY_CONSTRUCTION_ARGS
#undef GD_ECS_SINGLETON_NAME
#undef GD_ECS_ENTITY_NAME
#undef GD_ECS_COMPONENT_NAME
