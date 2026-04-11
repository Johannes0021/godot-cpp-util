/**
 * ECS bridge core header.
 *
 * For a default out-of-the-box implementation using EnTT, use the "ecs.hpp" header.
 *
 * This file provides the base implementation required to connect Godot data structures with an ECS
 * backend implementation.
 *
 * It defines the core classes responsible for:
 * - Representing ECS entities inside Godot.
 * - Exposing ECS components as Godot resources.
 * - Translating editor-side data into ECS registry data.
 * - Managing the lifecycle of ECS entities through Godot nodes.
 *
 * The behavior of this system is highly configurable through overrides of the default
 * implementations (see "Defaults" section).
 *
 * The design is intentionally kept as generic as possible in order to support multiple ECS
 * implementations or custom backends.
 *
 * If multiple implementations are used, refer to "clear_ecs_defines.hpp".
 *
 * For more information on how to use the ECS, refer to the other files in this directory.
 *
 * Usage example:
 *
 * #pragma once
 *
 * #include "godot_cpp_util/ecs/ecs_impl.hpp"
 *
 * // Register types to Godot:
 * GD_ECS_NAMESPACE::GD_ECS_SINGLETON_NAME::register_types();
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
 * // Then define GD_ECS_REGISTRY_TYPE
 * #define GD_ECS_REGISTRY_TYPE PlaceholderRegistry
 *
 * #include "godot_cpp_util/ecs/ecs_impl.hpp"
 */



#ifndef GODOT_CPP_UTIL_ECS_ECS_IMPL_HPP
#define GODOT_CPP_UTIL_ECS_ECS_IMPL_HPP



//==================================================================================================
// Defaults
//==================================================================================================

#ifndef GD_ECS_DO_NOT_DEFINE_DEFAULTS
    #ifndef GD_ECS_NAMESPACE
        #define GD_ECS_NAMESPACE godot
    #endif

    #ifndef GD_ECS_REGISTRY_TYPE
        #error "GD_ECS_REGISTRY_TYPE is not defined. Define it before including this header."
    #endif

    #ifndef GD_ECS_REGISTRY_CONSTRUCTION_ARGS
        #define GD_ECS_REGISTRY_CONSTRUCTION_ARGS
    #endif

    #ifndef GD_ECS_SINGLETON_NAME
        #define GD_ECS_SINGLETON_NAME ECS
    #endif

    #ifndef GD_ECS_ENTITY_NAME
        #define GD_ECS_ENTITY_NAME Entity
    #endif

    #ifndef GD_ECS_COMPONENT_NAME
        #define GD_ECS_COMPONENT_NAME Component
    #endif
#else
    #ifndef GD_ECS_NAMESPACE
        #error "GD_ECS_NAMESPACE is not defined. Define it before including this header."
    #endif

    #ifndef GD_ECS_REGISTRY_TYPE
        #error "GD_ECS_REGISTRY_TYPE is not defined. Define it before including this header."
    #endif

    #ifndef GD_ECS_REGISTRY_CONSTRUCTION_ARGS
        #error "GD_ECS_REGISTRY_CONSTRUCTION_ARGS is not defined. \
                Define it before including this header."
    #endif

    #ifndef GD_ECS_SINGLETON_NAME
        #error "GD_ECS_SINGLETON_NAME is not defined. Define it before including this header."
    #endif

    #ifndef GD_ECS_ENTITY_NAME
        #error "GD_ECS_ENTITY_NAME is not defined. Define it before including this header."
    #endif

    #ifndef GD_ECS_COMPONENT_NAME
        #error "GD_ECS_COMPONENT_NAME is not defined. Define it before including this header."
    #endif
#endif



//==================================================================================================
// include
//==================================================================================================

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"

#include "macros_impl.hpp"



//==================================================================================================
// GD_ECS_NAMESPACE namespace
//==================================================================================================

namespace GD_ECS_NAMESPACE {



class GD_ECS_ENTITY_NAME;
class GD_ECS_COMPONENT_NAME;



//==================================================================================================
// GD_ECS_SINGLETON_NAME class
//==================================================================================================

/**
 * Singleton instance that owns and manages the ECS registry.
 *
 * This class provides global access to the ECS registry used by the system.
 * It is responsible for holding the active registry instance and exposing it to all ECS-related
 * operations.
 */
class GD_ECS_SINGLETON_NAME final {

private:
    GD_ECS_REGISTRY_TYPE m_registry{GD_ECS_REGISTRY_CONSTRUCTION_ARGS};



public:
    // Registers ECS-related classes with the engine.
    static void register_types() {
        GDREGISTER_CLASS(GD_ECS_ENTITY_NAME);
        GDREGISTER_CLASS(GD_ECS_COMPONENT_NAME);
    }



    /**
     * Returns the singleton instance.
     *
     * This function is not thread safe. Any required synchronization must be handled by the caller.
     */
    static GD_ECS_SINGLETON_NAME& get_instance() {
        static GD_ECS_SINGLETON_NAME instance{};

        return instance;
    }



    // Provides access to the internal ECS registry instance.
    GD_ECS_REGISTRY_TYPE& get_registry() {
        return m_registry;
    }



    // Provides access to the internal ECS registry instance.
    const GD_ECS_REGISTRY_TYPE& get_registry() const {
        return m_registry;
    }



private:
    GD_ECS_SINGLETON_NAME() = default;
    ~GD_ECS_SINGLETON_NAME() = default;

    GD_ECS_SINGLETON_NAME(const GD_ECS_SINGLETON_NAME&) = delete;
    GD_ECS_SINGLETON_NAME& operator=(const GD_ECS_SINGLETON_NAME&) = delete;

    GD_ECS_SINGLETON_NAME(GD_ECS_SINGLETON_NAME&&) = delete;
    GD_ECS_SINGLETON_NAME& operator=(GD_ECS_SINGLETON_NAME&&) = delete;

};



//==================================================================================================
// GD_ECS_COMPONENT_NAME class
//==================================================================================================

/**
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
 * The Entity class exposes a property array of components. Components derived from this class can
 * be added to that array. When the property is set, the Entity iterates over the array and calls
 * emplace_or_replace on each component in sequence.
 *
 * Usage Example:
 *
 * #pragma once
 *
 * #include "godot_cpp_util/ecs/ecs.hpp"
 *
 * // Example ECS component stored in the registry.
 * struct Point {
 *     godot::Vector2 pos{};
 * };
 *
 * // Godot-facing component that exposes Point data to the editor and transfers it into the ECS
 * // registry.
 * class GDPoint : public GD_ECS_NAMESPACE::GD_ECS_COMPONENT_NAME {
 *     GDCLASS(GDPoint, GD_ECS_NAMESPACE::GD_ECS_COMPONENT_NAME)
 *
 * private:
 *     godot::Vector2 m_pos{};
 *
 * public:
 *     // Copies editor data into the ECS registry.
 *     virtual void emplace_or_replace(GD_ECS_NAMESPACE::GD_ECS_ENTITY_NAME &p_entity) override {
 *         // Derived implementations may call the base implementation to also apply base class
 *         // components.
 *         // GD_ECS_NAMESPACE::GD_ECS_COMPONENT_NAME::emplace_or_replace(p_entity);
 *
 *         auto &ecs = GD_ECS_NAMESPACE::GD_ECS_SINGLETON_NAME::get_instance();
 *         auto &reg = ecs.get_registry();
 *
 *         reg.emplace_or_replace<Point>(p_entity.get_entity(), m_pos);
 *
 *         // Alternatively, the Godot component itself can be stored in the registry, for example
 *         // wrapped in godot::Ref.
 *         //reg.emplace_or_replace<godot::Ref<GDPoint>>(p_entity.get_entity(), this);
 *     }
 *
 *     void set_pos(const godot::Vector2 pos) { m_pos = pos; }
 *     godot::Vector2 get_pos() const { return m_pos; }
 *
 * protected:
 *     static void _bind_methods() {
 *         godot::ClassDB::bind_method(godot::D_METHOD("set_pos", "p_pos"), &GDPoint ::set_pos);
 *         godot::ClassDB::bind_method(godot::D_METHOD("get_pos"), &GDPoint ::get_pos);
 *         ADD_PROPERTY(
 *             godot::PropertyInfo(godot::Variant::Type::VECTOR2, "pos"),
 *             "set_pos",
 *             "get_pos"
 *         );
 *     }
 *
 * };
 *
 * // Do not forget to expose the new component to Godot:
 * // ECS::register_types();
 * // // ...
 * // GDREGISTER_CLASS(GDPoint);
 */
class GD_ECS_COMPONENT_NAME : public godot::Resource {
    GDCLASS(GD_ECS_COMPONENT_NAME, godot::Resource)



public:
    /**
     * Transfers the component data from the Godot resource into the ECS registry.
     *
     * Implementations are expected to emplace or replace the corresponding ECS component for the
     * given entity.
     *
     * The default implementation only adds a godot::Ref<GD_ECS_COMPONENT_NAME> to the registry.
     */
    virtual void emplace_or_replace(GD_ECS_ENTITY_NAME &p_entity);



protected:
    static void _bind_methods() {}

};



//==================================================================================================
// GD_ECS_ENTITY_NAME class
//==================================================================================================

/**
 * ECS entity node exposed to Godot.
 *
 * This node represents an ECS entity in the Godot scene tree.
 * When the node is constructed, a new entity is created in the ECS registry.
 * When the node is destroyed, the corresponding ECS entity is removed.
 * The lifetime of the ECS entity is therefore bound to the lifetime of this node.
 *
 * The node exposes an array of components in the Godot editor.
 * Components can be added to this array and configured visually.
 *
 * When the array is set, each component in the array is processed in sequence,
 * calling emplace_or_replace on each component to transfer its data into the ECS world.
 */
class GD_ECS_ENTITY_NAME : public godot::Node {
    GDCLASS(GD_ECS_ENTITY_NAME, godot::Node)



private:
    GD_ECS_REGISTRY_TYPE::entity_type m_entity{};



public:
    GD_ECS_ENTITY_NAME() {
        auto &ecs = GD_ECS_SINGLETON_NAME::get_instance();
        auto &reg = ecs.get_registry();
        m_entity = reg.create();
    }



    virtual ~GD_ECS_ENTITY_NAME() override {
        auto &ecs = GD_ECS_SINGLETON_NAME::get_instance();
        auto &reg = ecs.get_registry();
        if(reg.valid(m_entity)) {
            reg.destroy(m_entity);
        }
    }



    // Provides access to the ECS entity handle.
    GD_ECS_REGISTRY_TYPE::entity_type& get_entity() {
        return m_entity;
    }



    // Provides access to the ECS entity handle.
    const GD_ECS_REGISTRY_TYPE::entity_type& get_entity() const {
        return m_entity;
    }


    // Processes a single component and applies it to this ECS entity.
    void emplace_or_replace(const godot::Ref<GD_ECS_COMPONENT_NAME> &p_component) {
        if (p_component.is_valid()) {
            p_component->emplace_or_replace(*this);
        }
    }


    // Processes an array of components and applies them in sequence.
    void emplace_or_replace_many(const godot::Array &p_components) {
        for (auto &variant : p_components) {
            emplace_or_replace(variant);
        }
    }



protected:
    static void _bind_methods() {
        // emplace_or_replace
        godot::ClassDB::bind_method(
            godot::D_METHOD("emplace_or_replace", "p_component"),
            &GD_ECS_ENTITY_NAME::emplace_or_replace
        );
        godot::ClassDB::bind_method(
            godot::D_METHOD("emplace_or_replace_many", "p_components"),
            &GD_ECS_ENTITY_NAME::emplace_or_replace_many
        );
        godot::ClassDB::bind_method(
            godot::D_METHOD("get_empty_typed_array"),
            &GD_ECS_ENTITY_NAME::get_empty_typed_array
        );

        ADD_PROPERTY(
            godot::PropertyInfo(
                godot::Variant::ARRAY,
                "emplace_or_replace_many",
                godot::PROPERTY_HINT_RESOURCE_TYPE,
                GD_ECS_COMPONENT_NAME::get_class_static()
            ),
            "emplace_or_replace_many",
            "get_empty_typed_array"
        );
    }



private:
    godot::Array get_empty_typed_array() {
        return godot::Array{};
    }

};



//==================================================================================================
// GD_ECS_COMPONENT_NAME class impl
//==================================================================================================

// Default implementation stores a reference to this component in the ECS registry.
inline void GD_ECS_COMPONENT_NAME::emplace_or_replace(GD_ECS_ENTITY_NAME &p_entity) {
    auto &ecs = GD_ECS_SINGLETON_NAME::get_instance();
    auto &reg = ecs.get_registry();
    reg.emplace_or_replace<godot::Ref<GD_ECS_COMPONENT_NAME>>(p_entity.get_entity(), this);
}



} // namespace GD_ECS_NAMESPACE



#endif // GODOT_CPP_UTIL_ECS_ECS_IMPL_HPP
