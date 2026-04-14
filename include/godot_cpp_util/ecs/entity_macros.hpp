/**
 * This header provides macros that are intended to make it easier to define a entity class
 * (GDCLASS).
 *
 * Define GD_ECS_DO_NOT_GENERATE_SIGNAL_CODE before including this file to completely disable all
 * generated signal code.
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



#include <type_traits>

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/core/class_db.hpp"

#include "signal_macros.hpp"



//==================================================================================================
// has_gd_ecs_entity_tag
//==================================================================================================

template<typename, typename = void>
struct has_gd_ecs_entity_tag : std::false_type {};

template<typename T>
struct has_gd_ecs_entity_tag<T, std::void_t<typename T::gd_ecs_entity_tag>> : std::true_type {};

template<typename T>
inline constexpr bool has_gd_ecs_entity_tag_v = has_gd_ecs_entity_tag<T>::value;



//==================================================================================================
// GD_ECS_ENTITY
//==================================================================================================

/**
 * ECS entity node exposed to Godot.
 *
 * This node represents an ECS entity in the Godot scene tree.
 * When the node is constructed, a new entity is created in the ECS registry.
 * When the node is destroyed, the corresponding ECS entity is removed.
 * The lifetime of the ECS entity is therefore bound to the lifetime of this node.
 *
 * The node exposes arrays of components in the Godot editor.
 * Components can be added to this array and configured visually.
 *
 * When an array is set, each component in the array is processed in sequence,
 * calling emplace_or_replace on each component to transfer its data into the ECS world.
 *
 * Usage Example:
 *
 * #pragma once
 *
 * #include "godot_cpp_util/ecs/ecs.hpp"
 *
 * using ECSType = godot::ECS;
 * GD_ECS_ENTITY(ECSType, E_Entity, godot::Node)
 *
 * //// Do not forget to expose the new entity to Godot:
 * //ECSType::register_types();
 * //// ...
 * //E_Entity::register_types();
 */
#define GD_ECS_ENTITY(GD_ECS_SINGLETON_NAME, GD_ECS_ENTITY_NAME, GD_ECS_ENTITY_PARENT_TYPE)        \
class GD_ECS_ENTITY_NAME : public GD_ECS_ENTITY_PARENT_TYPE {                                      \
    GDCLASS(GD_ECS_ENTITY_NAME, GD_ECS_ENTITY_PARENT_TYPE)                                         \
                                                                                                   \
    static_assert(                                                                                 \
        !has_gd_ecs_entity_tag_v<GD_ECS_ENTITY_PARENT_TYPE>,                                       \
        "GD_ECS_ENTITY_PARENT_TYPE must NOT define gd_ecs_entity_tag!"                             \
    );                                                                                             \
                                                                                                   \
                                                                                                   \
                                                                                                   \
public:                                                                                            \
    using gd_ecs_entity_tag = void;                                                                \
                                                                                                   \
    GD_ECS_EMPTY_SIGNAL_STRUCT(                                                                    \
        Signal,                                                                                    \
        godot::GDTypedSignal::GDTypedSignalTypeOf<GD_ECS_ENTITY_PARENT_TYPE>::Type                 \
    )                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
private:                                                                                           \
    GD_ECS_SINGLETON_NAME::RegistryType::entity_type m_entity{};                                   \
                                                                                                   \
                                                                                                   \
                                                                                                   \
public:                                                                                            \
    static void register_types() {                                                                 \
        GDREGISTER_RUNTIME_CLASS(GD_ECS_ENTITY_NAME);                                              \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    GD_ECS_ENTITY_NAME() {                                                                         \
        auto &ecs = GD_ECS_SINGLETON_NAME::get_instance();                                         \
        auto &reg = ecs.get_registry();                                                            \
        m_entity = reg.create();                                                                   \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    virtual ~GD_ECS_ENTITY_NAME() override {                                                       \
        auto &ecs = GD_ECS_SINGLETON_NAME::get_instance();                                         \
        auto &reg = ecs.get_registry();                                                            \
        if(reg.valid(m_entity)) {                                                                  \
            reg.destroy(m_entity);                                                                 \
        }                                                                                          \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    GD_ECS_SINGLETON_NAME::RegistryType::entity_type& get_entity() {                               \
        return m_entity;                                                                           \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    const GD_ECS_SINGLETON_NAME::RegistryType::entity_type& get_entity() const {                   \
        return m_entity;                                                                           \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    void emplace_or_replace(const godot::Ref<GD_ECS_SINGLETON_NAME::ComponentType> &p_component) { \
        if (p_component.is_valid()) {                                                              \
            p_component->emplace_or_replace(*this, m_entity);                                      \
        }                                                                                          \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    void emplace_or_replace_many(const godot::Array &p_components) {                               \
        for (auto &variant : p_components) {                                                       \
            emplace_or_replace(variant);                                                           \
        }                                                                                          \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    void emplace_or_replace_extra(const godot::Array &p_extra_components) {                        \
        for (auto &variant : p_extra_components) {                                                 \
            if (variant.get_type() == godot::Variant::Type::ARRAY) {                               \
                godot::Array components = variant;                                                 \
                emplace_or_replace_many(components);                                               \
            }                                                                                      \
            else {                                                                                 \
                emplace_or_replace(variant);                                                       \
            }                                                                                      \
        }                                                                                          \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
protected:                                                                                         \
    static void _bind_methods() {                                                                  \
        godot::ClassDB::bind_method(                                                               \
            godot::D_METHOD("emplace_or_replace", "p_component"),                                  \
            &GD_ECS_ENTITY_NAME::emplace_or_replace                                                \
        );                                                                                         \
        godot::ClassDB::bind_method(                                                               \
            godot::D_METHOD("emplace_or_replace_many", "p_components"),                            \
            &GD_ECS_ENTITY_NAME::emplace_or_replace_many                                           \
        );                                                                                         \
        godot::ClassDB::bind_method(                                                               \
            godot::D_METHOD("emplace_or_replace_extra", "p_extra_components"),                     \
            &GD_ECS_ENTITY_NAME::emplace_or_replace_extra                                          \
        );                                                                                         \
        godot::ClassDB::bind_method(                                                               \
            godot::D_METHOD("get_empty_typed_array"),                                              \
            &GD_ECS_ENTITY_NAME::get_empty_typed_array                                             \
        );                                                                                         \
                                                                                                   \
        ADD_PROPERTY(                                                                              \
            godot::PropertyInfo(                                                                   \
                godot::Variant::ARRAY,                                                             \
                "components",                                                                      \
                godot::PROPERTY_HINT_RESOURCE_TYPE,                                                \
                GD_ECS_SINGLETON_NAME::ComponentType::get_class_static()                           \
            ),                                                                                     \
            "emplace_or_replace_many",                                                             \
            "get_empty_typed_array"                                                                \
        );                                                                                         \
        ADD_PROPERTY(                                                                              \
            godot::PropertyInfo(                                                                   \
                godot::Variant::ARRAY,                                                             \
                "extra_components",                                                                \
                godot::PROPERTY_HINT_ARRAY_TYPE,                                                   \
                godot::String::num_int64(godot::Variant::ARRAY) + "/"                              \
                + godot::String::num_int64(godot::PROPERTY_HINT_RESOURCE_TYPE) + ":"               \
                    + GD_ECS_SINGLETON_NAME::ComponentType::get_class_static()                     \
            ),                                                                                     \
            "emplace_or_replace_many",                                                             \
            "get_empty_typed_array"                                                                \
        );                                                                                         \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
private:                                                                                           \
    godot::Array get_empty_typed_array() {                                                         \
        return godot::Array{};                                                                     \
    }                                                                                              \
                                                                                                   \
};                                                                                                 \
