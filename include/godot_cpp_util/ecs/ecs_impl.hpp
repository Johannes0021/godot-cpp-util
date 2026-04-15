/**
 * ECS bridge core header.
 *
 * For a default out-of-the-box implementation using EnTT, use the "ecs.hpp" header.
 *
 * This file provides the macro GD_ECS_IMPL which defines a customizable implementation required to
 * connect Godot data structures with an ECS backend implementation.
 *
 * The behavior of this system is highly configurable through the macro parameters.
 *
 * The design is intentionally kept as generic as possible in order to support multiple ECS
 * implementations or custom backends.
 *
 * For more information on how to use the ECS, refer to the other files in this directory.
 *
 * Define GD_ECS_DO_NOT_GENERATE_SIGNAL_CODE before including this file to completely disable all
 * generated signal code.
 *
 * Usage example:
 *
 * #pragma once
 *
 * #include "entt/entity/registry.hpp"
 * #include "godot_cpp_util/ecs/ecs_impl.hpp"
 *
 * GD_ECS_IMPL(godot, ECS, C_Component, entt::registry)
 *
 * //// Register types to Godot:
 * //godot::ECS::register_types();
 *
 *
 *
 * -------------------------------------------------------------------------------------------------
 * ECS Registry Requirement
 * -------------------------------------------------------------------------------------------------
 *
 * The ECS backend is not fixed. Any implementation must provide a registry type that satisfies the
 * following interface.
 *
 * This interface is compatible with the EnTT C++ library. For other ECS backends, a wrapper type
 * may be used to match this API.
 *
 * Required interface:
 *
 * #pragma once
 *
 * #include <cstdint>
 * #include "godot_cpp_util/ecs/ecs_impl.hpp"
 *
 * class PlaceholderRegistry final {
 *     public:
 *         using entity_type = std::uint32_t;
 *
 *     public:
 *         // @brief Checks if an identifier refers to a valid entity.
 *         // @param entt An identifier, either valid or not.
 *         // @return True if the identifier is valid, false otherwise.
 *         [[nodiscard]] bool valid([[maybe_unused]] const entity_type entt) const {
 *             return false;
 *         }
 *
 *         // @brief Creates a new entity or recycles a destroyed one.
 *         // @return A valid identifier.
 *         [[nodiscard]] entity_type create() { return 0; }
 *
 *         // @brief Destroys an entity and releases its identifier.
 *         // @return Type is void here since it is unused in this file, but may be customized
 *         //         by implementations.
 *         void destroy([[maybe_unused]] const entity_type entt) {}
 *
 *         // @brief Assigns or replaces the given element for an entity.
 *         // @return Type is void here since it is unused in this file, but may be customized
 *         //         by implementations.
 *         template<typename T, typename... Args>
 *         void emplace_or_replace(
 *             [[maybe_unused]] const entity_type entt,
 *             [[maybe_unused]] Args &&...args
 *         ) {}
 * };
 *
 * GD_ECS_IMPL(godot, ECS, C_Component, PlaceholderRegistry)
 *
 *
 *
 * -------------------------------------------------------------------------------------------------
 * Note on macro arguments containing commas
 * -------------------------------------------------------------------------------------------------
 *
 * The C preprocessor treats commas as argument separators in macro calls.
 * This means template expressions like MyType<T, U> will be split into multiple macro arguments and
 * can cause errors.
 *
 * If you need to pass a templated type or any expression containing commas, wrap it using one of
 * the following approaches:
 *
 * 1. Type alias
 *     using MyCleanArg = MyArg<T, U>;
 *     MY_MACRO(MyCleanArg)
 *
 * 2. Helper macro
 *     #define MY_CLEAN_ARG MyArg<T, U>
 *     MY_MACRO(MY_CLEAN_ARG)
 */



#pragma once



#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"

#include "component_macros.hpp"
#include "entity_macros.hpp"
#include "signal_macros.hpp"



//==================================================================================================
// GD_ECS_IMPL
//==================================================================================================

/**
 * This macro defines the following classes:
 *
 * --- GD_ECS_SINGLETON_NAME class ---
 *
 * Singleton instance that owns and manages the ECS registry.
 *
 * This class provides global access to the ECS registry used by the system.
 * It is responsible for holding the active registry instance and exposing it to all ECS-related
 * operations.
 *
 * The singleton is not thread safe. Any required synchronization must be handled by the caller.
 *
 *
 *
 * --- GD_ECS_COMPONENT_NAME class ---
 *
 * Base class for all ECS components that are exposed to Godot.
 *
 * This class acts as the bridge between the Godot editor world and the ECS world.
 * Data configured in the editor exists as Godot resources and must be translated into ECS
 * components stored in the registry.
 *
 * The translation step is performed by emplace_or_replace, which must be overridden by derived
 * classes. Its responsibility is to read data from the Godot-side component and transfer it into
 * the ECS registry for the given entity.
 *
 * Each derived component defines how its editor-facing data maps to ECS data.
 *
 * The Entity class exposes a property arrays of components. Components derived from this class can
 * be added to that array. When the property is set, the Entity iterates over the array and calls
 * emplace_or_replace on each component in sequence.
 *
 * For macros that are intended to make it easier to define a component class(GDCLASS) see
 * "component_macros.hpp"
 *
 * Usage Example:
 *
 * #pragma once
 *
 * #include "godot_cpp_util/ecs/ecs.hpp"
 *
 * using ECSType = godot::ECS;
 *
 * // Example ECS component stored in the registry.
 * struct Point {
 *     godot::Vector2 pos{};
 * };
 *
 * // Godot-facing component that exposes Point data to the editor and transfers it into the ECS
 * // registry.
 * class C_Point : public ECSType::ComponentType {
 *     GDCLASS(C_Point, ECSType::ComponentType)
 *
 * public:
 *     // Define the Signal struct when signal generation is not disabled.
 *     GD_ECS_EMPTY_SIGNAL_STRUCT(Signal, ECSType::ComponentType::Signal)
 *
 * private:
 *     godot::Vector2 m_pos{};
 *
 * public:
 *     static void register_types() {
 *         GDREGISTER_RUNTIME_CLASS(C_Point);
 *     }
 *
 *     // Transfers the component data from the Godot resource into the ECS registry.
 *     //
 *     // Implementations are expected to emplace or replace the corresponding ECS component for the
 *     // given entity.
 *     //
 *     // The default implementation only adds a godot::Ref<ECSType::ComponentType> to the registry.
 *     virtual void emplace_or_replace(
 *         [[maybe_unused]] godot::Node &p_entity_node,
 *         ECSType::RegistryType::entity_type &p_entity
 *     ) override {
 *         // Derived implementations may call the base implementation to also apply base class
 *         // components.
 *         //ECSType::ComponentType::emplace_or_replace(p_entity_node, p_entity);
 *
 *         auto &reg = ECSType::get_registry();
 *
 *         reg.emplace_or_replace<Point>(p_entity, m_pos);
 *
 *         // Alternatively, the Godot component itself can be stored in the registry, for example
 *         // wrapped in godot::Ref.
 *         //reg.emplace_or_replace<godot::Ref<C_Point>>(p_entity, this);
 *     }
 *
 *     void set_pos(const godot::Vector2 pos) { m_pos = pos; }
 *     godot::Vector2 get_pos() const { return m_pos; }
 *
 * protected:
 *     static void _bind_methods() {
 *         godot::ClassDB::bind_method(godot::D_METHOD("set_pos", "p_pos"), &C_Point ::set_pos);
 *         godot::ClassDB::bind_method(godot::D_METHOD("get_pos"), &C_Point ::get_pos);
 *         ADD_PROPERTY(
 *             godot::PropertyInfo(godot::Variant::Type::VECTOR2, "pos"),
 *             "set_pos",
 *             "get_pos"
 *         );
 *     }
 *
 * };
 *
 * //// Do not forget to expose the new component to Godot:
 * //ECSType::register_types();
 * //// ...
 * //C_Point::register_types();
 *
 *
 *
 * -------------------------------------------------------------------------------------------------
 * Notes
 * -------------------------------------------------------------------------------------------------
 *
 * See "entity_macros.hpp" for helpers to define entities.
 * See "component_macros.hpp" for helpers to define components.
 */
#define GD_ECS_IMPL(                                                                               \
    GD_ECS_NAMESPACE,                                                                              \
    GD_ECS_SINGLETON_NAME,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    GD_ECS_REGISTRY_TYPE,                                                                          \
    ...                                                                                            \
)                                                                                                  \
namespace GD_ECS_NAMESPACE {                                                                       \
                                                                                                   \
                                                                                                   \
                                                                                                   \
class GD_ECS_ENTITY_NAME;                                                                          \
class GD_ECS_COMPONENT_NAME;                                                                       \
                                                                                                   \
                                                                                                   \
                                                                                                   \
class GD_ECS_SINGLETON_NAME final {                                                                \
                                                                                                   \
public:                                                                                            \
    using RegistryType = GD_ECS_REGISTRY_TYPE;                                                     \
    using ComponentType = GD_ECS_COMPONENT_NAME;                                                   \
                                                                                                   \
                                                                                                   \
                                                                                                   \
private:                                                                                           \
    GD_ECS_REGISTRY_TYPE m_registry{__VA_ARGS__};                                                  \
                                                                                                   \
                                                                                                   \
                                                                                                   \
public:                                                                                            \
    static void register_types() {                                                                 \
        GDREGISTER_RUNTIME_CLASS(GD_ECS_COMPONENT_NAME);                                           \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    static GD_ECS_SINGLETON_NAME& get_instance() {                                                 \
        static GD_ECS_SINGLETON_NAME instance{};                                                   \
                                                                                                   \
        return instance;                                                                           \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    static GD_ECS_REGISTRY_TYPE& get_registry() {                                                  \
        return get_instance().m_registry;                                                          \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
private:                                                                                           \
    GD_ECS_SINGLETON_NAME() = default;                                                             \
    ~GD_ECS_SINGLETON_NAME() = default;                                                            \
                                                                                                   \
    GD_ECS_SINGLETON_NAME(const GD_ECS_SINGLETON_NAME&) = delete;                                  \
    GD_ECS_SINGLETON_NAME& operator=(const GD_ECS_SINGLETON_NAME&) = delete;                       \
                                                                                                   \
    GD_ECS_SINGLETON_NAME(GD_ECS_SINGLETON_NAME&&) = delete;                                       \
    GD_ECS_SINGLETON_NAME& operator=(GD_ECS_SINGLETON_NAME&&) = delete;                            \
                                                                                                   \
};                                                                                                 \
                                                                                                   \
                                                                                                   \
                                                                                                   \
class GD_ECS_COMPONENT_NAME : public godot::Resource {                                             \
    GDCLASS(GD_ECS_COMPONENT_NAME, godot::Resource)                                                \
                                                                                                   \
                                                                                                   \
                                                                                                   \
public:                                                                                            \
    GD_ECS_EMPTY_SIGNAL_STRUCT(Signal, GDTypedSignal::Resource)                                    \
                                                                                                   \
                                                                                                   \
                                                                                                   \
public:                                                                                            \
    virtual void emplace_or_replace(                                                               \
        [[maybe_unused]] godot::Node &p_entity_node,                                               \
        GD_ECS_REGISTRY_TYPE::entity_type &p_entity                                                \
    ) {                                                                                            \
        auto &reg = GD_ECS_SINGLETON_NAME::get_registry();                                         \
        reg.emplace_or_replace<godot::Ref<GD_ECS_COMPONENT_NAME>>(p_entity, this);                 \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
protected:                                                                                         \
    static void _bind_methods() {}                                                                 \
                                                                                                   \
};                                                                                                 \
                                                                                                   \
                                                                                                   \
                                                                                                   \
} // namespace GD_ECS_NAMESPACE
