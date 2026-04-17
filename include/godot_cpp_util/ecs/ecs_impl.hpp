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
 *
 * public:
 *     using entity_type = std::uint32_t;
 *     using size_type = std::size_t;
 *
 * public:
 *     // @brief Checks if an identifier refers to a valid entity.
 *     // @param entt An identifier, either valid or not.
 *     // @return True if the identifier is valid, false otherwise.
 *     [[nodiscard]] bool valid([[maybe_unused]] const entity_type entt) const {
 *         return false;
 *     }
 *
 *     // @brief Creates a new entity or recycles a destroyed one.
 *     // @return A valid identifier.
 *     [[nodiscard]] entity_type create() { return 0; }
 *
 *     // @brief Destroys an entity and releases its identifier.
 *     // @return Type is void here since it is unused in this file, but may be customized
 *     //         by implementations.
 *     void destroy([[maybe_unused]] const entity_type entt) {}
 *
 *     // @brief Assigns or replaces the given element for an entity.
 *     // @return Type is void here since it is unused in this file, but may be customized
 *     //         by implementations.
 *     template<typename T, typename ...Args>
 *     void emplace_or_replace(
 *         [[maybe_unused]] const entity_type entt,
 *         [[maybe_unused]] Args &&...args
 *     ) {}
 *
 *     // @brief Removes the given element from an entity.
 *     // @tparam T Type of element to remove.
 *     // @param entt A valid identifier.
 *     // @return The number of elements actually removed.
 *     template<typename T>
 *     size_type remove([[maybe_unused]] const entity_type entt) {
 *         return 0;
 *     }
 *
 *     // @brief Check if an entity is part of all the given storage.
 *     // @tparam T Type of storage to check for.
 *     // @param entt A valid identifier.
 *     // @return True if the entity is part of all the storage, false otherwise.
 *     template<typename T>
 *     [[nodiscard]] bool all_of([[maybe_unused]] const entity_type entt) const {
 *         return false;
 *     }
 *
 *     // @brief Returns the pointer to the given element for an entity.
 *     //
 *     // @note
 *     // The registry retains ownership of the pointed-to element.
 *     //
 *     // @tparam T Type of element to get.
 *     // @param entt A valid identifier.
 *     // @return Pointer to the element owned by the entity.
 *     template<typename T>
 *     [[nodiscard]] T* try_get([[maybe_unused]] const entity_type entt) {
 *          return nullptr;
 *     }
 *
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



#include <cstddef>
#include <limits>
#include <functional>
#include <optional>

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/templates/hash_map.hpp"
#include "godot_cpp/templates/local_vector.hpp"

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
 *         auto &reg = ECSType::registry();
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
                                                                                                   \
                                                                                                   \
public:                                                                                            \
    using RegistryType = GD_ECS_REGISTRY_TYPE;                                                     \
    using ComponentType = GD_ECS_COMPONENT_NAME;                                                   \
    using ComponentIndex = std::uint64_t;                                                          \
                                                                                                   \
    using EmplaceOrReplaceFn =                                                                     \
        std::function<bool(const GD_ECS_REGISTRY_TYPE::entity_type&, const godot::Variant&)>;      \
    using RemoveFn = std::function<bool(const GD_ECS_REGISTRY_TYPE::entity_type&)>;                \
    using HasFn = std::function<bool(const GD_ECS_REGISTRY_TYPE::entity_type&)>;                   \
    using GetFn = std::function<godot::Variant(const GD_ECS_REGISTRY_TYPE::entity_type&)>;         \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    struct Entry final {                                                                           \
        ComponentIndex component_index{std::numeric_limits<ComponentIndex>::max()};                \
        EmplaceOrReplaceFn emplace_or_replace = nullptr;                                           \
        RemoveFn remove = nullptr;                                                                 \
        RemoveFn has = nullptr;                                                                    \
        GetFn get = nullptr;                                                                       \
                                                                                                   \
                                                                                                   \
                                                                                                   \
        template<typename T>                                                                       \
        void init_uninit(ComponentIndex p_component_index) {                                       \
            component_index = p_component_index;                                                   \
                                                                                                   \
            if (!emplace_or_replace) {                                                             \
                init_emplace_or_replace<T>();                                                      \
            }                                                                                      \
                                                                                                   \
            if (!remove) {                                                                         \
                init_remove<T>();                                                                  \
            }                                                                                      \
                                                                                                   \
            if (!has) {                                                                            \
                init_has<T>();                                                                     \
            }                                                                                      \
                                                                                                   \
            if (!get) {                                                                            \
                init_get<T>();                                                                     \
            }                                                                                      \
        }                                                                                          \
                                                                                                   \
                                                                                                   \
                                                                                                   \
        template<typename T>                                                                       \
        requires std::is_empty_v<T>                                                                \
        void init_emplace_or_replace() {                                                           \
            emplace_or_replace = [](                                                               \
                const GD_ECS_REGISTRY_TYPE::entity_type &p_entity,                                 \
                const godot::Variant &p_data                                                       \
            ) -> bool                                                                              \
            {                                                                                      \
                auto &reg = GD_ECS_SINGLETON_NAME::registry();                                     \
                reg.emplace_or_replace<T>(p_entity);                                               \
                                                                                                   \
                return true;                                                                       \
            };                                                                                     \
        }                                                                                          \
                                                                                                   \
                                                                                                   \
                                                                                                   \
        template<typename T>                                                                       \
        requires (!std::is_empty_v<T>)                                                             \
                 && godot::gd_ecs_has_component_descriptor<T>                                      \
        void init_emplace_or_replace() {                                                           \
            emplace_or_replace = [](                                                               \
                const GD_ECS_REGISTRY_TYPE::entity_type &p_entity,                                 \
                const godot::Variant &p_data                                                       \
            ) -> bool                                                                              \
            {                                                                                      \
                auto &reg = GD_ECS_SINGLETON_NAME::registry();                                     \
                auto &descriptor = T::descriptor();                                                \
                T instance{};                                                                      \
                descriptor.set(instance, p_data);                                                  \
                reg.emplace_or_replace<T>(p_entity, instance);                                     \
                                                                                                   \
                return true;                                                                       \
            };                                                                                     \
        }                                                                                          \
                                                                                                   \
                                                                                                   \
                                                                                                   \
        template<typename T>                                                                       \
        requires (!std::is_empty_v<T>)                                                             \
                 && (!godot::gd_ecs_has_component_descriptor<T>)                                   \
        void init_emplace_or_replace() {                                                           \
            emplace_or_replace = [](                                                               \
                const GD_ECS_REGISTRY_TYPE::entity_type&,                                          \
                const godot::Variant&                                                              \
            ) -> bool                                                                              \
            {                                                                                      \
                return false;                                                                      \
            };                                                                                     \
        }                                                                                          \
                                                                                                   \
                                                                                                   \
                                                                                                   \
        template<typename T>                                                                       \
        void init_remove() {                                                                       \
            remove = [](const GD_ECS_REGISTRY_TYPE::entity_type &p_entity) -> bool {               \
                auto &reg = GD_ECS_SINGLETON_NAME::registry();                                     \
                return (reg.remove<T>(p_entity) > 0);                                              \
            };                                                                                     \
        }                                                                                          \
                                                                                                   \
                                                                                                   \
                                                                                                   \
        template<typename T>                                                                       \
        void init_has() {                                                                          \
            has = [](const GD_ECS_REGISTRY_TYPE::entity_type &p_entity) -> bool {                  \
                auto &reg = GD_ECS_SINGLETON_NAME::registry();                                     \
                return reg.all_of<T>(p_entity);                                                    \
            };                                                                                     \
        }                                                                                          \
                                                                                                   \
                                                                                                   \
                                                                                                   \
        template<typename T>                                                                       \
        requires (!std::is_empty_v<T>)                                                             \
                 && godot::gd_ecs_has_component_descriptor<T>                                      \
        void init_get() {                                                                          \
            get = [](const GD_ECS_REGISTRY_TYPE::entity_type &p_entity) -> godot::Variant {        \
                auto &reg = GD_ECS_SINGLETON_NAME::registry();                                     \
                if (auto *instance = reg.try_get<T>(p_entity)) {                                   \
                    auto &descriptor = T::descriptor();                                            \
                    return descriptor.to_variant(*instance);                                       \
                }                                                                                  \
                return godot::Variant{};                                                           \
            };                                                                                     \
        }                                                                                          \
                                                                                                   \
                                                                                                   \
                                                                                                   \
        template<typename T>                                                                       \
        requires std::is_empty_v<T>                                                                \
                 || (!godot::gd_ecs_has_component_descriptor<T>)                                   \
        void init_get() {                                                                          \
            get = [](const GD_ECS_REGISTRY_TYPE::entity_type&) -> godot::Variant {                 \
                return godot::Variant{};                                                           \
            };                                                                                     \
        }                                                                                          \
                                                                                                   \
    };                                                                                             \
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
    static GD_ECS_REGISTRY_TYPE& registry() {                                                      \
        static GD_ECS_REGISTRY_TYPE registry{__VA_ARGS__};                                         \
        return registry;                                                                           \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    template<typename T>                                                                           \
    static void register_type(const godot::StringName &p_component) {                              \
        Entry entry{};                                                                             \
        register_type_and_entry<T>(p_component, entry);                                            \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    template<typename T>                                                                           \
    static void register_type_and_entry(const godot::StringName &p_component, Entry& p_entry) {    \
        static ComponentIndex component_index = component_names().size();                          \
                                                                                                   \
        p_entry.init_uninit<T>(component_index);                                                   \
                                                                                                   \
        component_name_to_entry_mut()[p_component] = p_entry;                                      \
                                                                                                   \
        if (component_index < component_names().size()) {                                          \
            component_names_mut()[component_index] = p_component;                                  \
        }                                                                                          \
        else {                                                                                     \
            component_names_mut().push_back(p_component);                                          \
        }                                                                                          \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    static const godot::HashMap<godot::StringName, Entry> &component_name_to_entry() {             \
        return component_name_to_entry_mut();                                                      \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    static const godot::LocalVector<godot::StringName, ComponentIndex> &component_names() {        \
        return component_names_mut();                                                              \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    static std::optional<godot::StringName> get_component_name(ComponentIndex component_index) {   \
        if (component_index < component_names().size()) {                                          \
            return component_names()[component_index];                                             \
        }                                                                                          \
                                                                                                   \
        return std::nullopt;                                                                       \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    static bool emplace_or_replace(                                                                \
        const GD_ECS_REGISTRY_TYPE::entity_type &p_entity,                                         \
        const godot::StringName &p_component,                                                      \
        const godot::Variant &p_data                                                               \
    ) {                                                                                            \
        if (component_name_to_entry().has(p_component)) {                                          \
            auto &entry = component_name_to_entry()[p_component];                                  \
            if (entry.emplace_or_replace) {                                                        \
                return entry.emplace_or_replace(p_entity, p_data);                                 \
            }                                                                                      \
        }                                                                                          \
                                                                                                   \
        return false;                                                                              \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    static bool remove(                                                                            \
        const GD_ECS_REGISTRY_TYPE::entity_type &p_entity,                                         \
        const godot::StringName &p_component                                                       \
    ) {                                                                                            \
        if (component_name_to_entry().has(p_component)) {                                          \
            auto &entry = component_name_to_entry()[p_component];                                  \
            if (entry.remove) {                                                                    \
                return entry.remove(p_entity);                                                     \
            }                                                                                      \
        }                                                                                          \
                                                                                                   \
        return false;                                                                              \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    static bool has(                                                                               \
        const GD_ECS_REGISTRY_TYPE::entity_type &p_entity,                                         \
        const godot::StringName &p_component                                                       \
    ) {                                                                                            \
        if (component_name_to_entry().has(p_component)) {                                          \
            auto &entry = component_name_to_entry()[p_component];                                  \
            if (entry.has) {                                                                       \
                return entry.has(p_entity);                                                        \
            }                                                                                      \
        }                                                                                          \
                                                                                                   \
        return false;                                                                              \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    static godot::Variant get(                                                                     \
        const GD_ECS_REGISTRY_TYPE::entity_type &p_entity,                                         \
        const godot::StringName &p_component                                                       \
    ) {                                                                                            \
        if (component_name_to_entry().has(p_component)) {                                          \
            auto &entry = component_name_to_entry()[p_component];                                  \
            if (entry.get) {                                                                       \
                return entry.get(p_entity);                                                        \
            }                                                                                      \
        }                                                                                          \
                                                                                                   \
        return godot::Variant{};                                                                   \
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
                                                                                                   \
                                                                                                   \
    static godot::HashMap<godot::StringName, Entry> &component_name_to_entry_mut() {               \
        static godot::HashMap<godot::StringName, Entry> map{};                                     \
        return map;                                                                                \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    static godot::LocalVector<godot::StringName, ComponentIndex> &component_names_mut() {          \
        static godot::LocalVector<godot::StringName, ComponentIndex> vec{};                        \
        return vec;                                                                                \
    }                                                                                              \
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
        auto &reg = GD_ECS_SINGLETON_NAME::registry();                                             \
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
