/**
 * This header provides hepers that are intended to make it easier to define a entity class
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

#include <godot_cpp/classes/engine.hpp>
#include "godot_cpp/classes/node.hpp"
#include <godot_cpp/classes/os.hpp>
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/error_macros.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/string_name.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "godot_cpp_util/core/string/static_string_name.hpp"

#include "component.hpp"
#include "signal_macros.hpp"



//==================================================================================================
// Helpers
//==================================================================================================

namespace godot {

template<typename, typename = void>
struct has_gd_ecs_entity_tag : std::false_type {};



template<typename T>
struct has_gd_ecs_entity_tag<T, std::void_t<typename T::gd_ecs_entity_tag>> : std::true_type {};



template<typename T>
inline constexpr bool has_gd_ecs_entity_tag_v = has_gd_ecs_entity_tag<T>::value;



inline bool gd_ecs_is_valid_component_name_print_error(const godot::Variant& p_variant) {
    switch (p_variant.get_type()) {
        case godot::Variant::Type::STRING:
        case godot::Variant::Type::STRING_NAME:
            return true;
    }

    ERR_PRINT(godot::vformat(
        "Invalid component name. Expected String or StringName. Got: %s",
        p_variant
    ));

    return false;
}

} // namespace godot



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
 *
 * GD_ECS_ENTITY(ECSType, E_Node, godot::Node)
 *
 * // A component that stores basic example data.
 * struct Data {
 *     godot::String name{"SomeName"};
 *
 *     // Default constructor is required by the ECS.
 *     Data() = default;
 *
 *     // Required for:
 *     // - Resource wrapper components.
 *     // - Default components of entities.
 *     //
 *     // This function is functionally equivalent to:
 *     // GD_ECS_COMPONENT_EMPLACE_OR_REPLACE_IMPL(ECSType, Data)
 *     static void emplace_or_replace(
 *         [[maybe_unused]] godot::Node &p_entity_node,
 *         ECSType::RegistryType::entity_type &p_entity,
 *         Data &p_data
 *     ) {
 *         auto &reg = ECSType::registry();
 *         reg.emplace_or_replace<Data>(p_entity, p_data);
 *     }
 * };
 *
 * GD_ECS_ENTITY(ECSType, E_NodeWithDefaults, godot::Node, Data)
 *
 * //// Do not forget to expose the new entities to Godot:
 * //ECSType::register_types();
 * //// ...
 * //E_Node::register_types();
 * //E_NodeWithDefaults::register_types();
 */
#define GD_ECS_ENTITY(GD_ECS_SINGLETON_TYPE, GD_ECS_ENTITY_NAME, GD_ECS_ENTITY_PARENT_TYPE, ...)   \
class GD_ECS_ENTITY_NAME : public GD_ECS_ENTITY_PARENT_TYPE {                                      \
    GDCLASS(GD_ECS_ENTITY_NAME, GD_ECS_ENTITY_PARENT_TYPE)                                         \
                                                                                                   \
    static_assert(                                                                                 \
        !godot::has_gd_ecs_entity_tag_v<GD_ECS_ENTITY_PARENT_TYPE>,                                \
        #GD_ECS_ENTITY_PARENT_TYPE " must NOT define gd_ecs_entity_tag!"                           \
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
    GD_ECS_SINGLETON_TYPE::RegistryType::entity_type m_entity{};                                   \
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
        auto &reg = GD_ECS_SINGLETON_TYPE::registry();                                             \
        m_entity = reg.create();                                                                   \
        emplace_or_replace_defaults<__VA_ARGS__>();                                                \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    virtual ~GD_ECS_ENTITY_NAME() override {                                                       \
        auto &reg = GD_ECS_SINGLETON_TYPE::registry();                                             \
        if(reg.valid(m_entity)) {                                                                  \
            reg.destroy(m_entity);                                                                 \
        }                                                                                          \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    GD_ECS_SINGLETON_TYPE::RegistryType::entity_type& get_entity() {                               \
        return m_entity;                                                                           \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    const GD_ECS_SINGLETON_TYPE::RegistryType::entity_type& get_entity() const {                   \
        return m_entity;                                                                           \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    void set_res_component(const godot::Ref<GD_ECS_SINGLETON_TYPE::ComponentType> &p_component) {  \
        if (p_component.is_valid()) {                                                              \
            p_component->emplace_or_replace(*this, m_entity);                                      \
        }                                                                                          \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    void set_res_components(const godot::Array &p_components) {                                    \
        for (auto &variant : p_components) {                                                       \
            set_res_component(variant);                                                            \
        }                                                                                          \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    void set_component(const godot::StringName &p_component, const godot::Variant& p_data) const { \
        GD_ECS_SINGLETON_TYPE::emplace_or_replace(m_entity, p_component, p_data);                  \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    void components_diff(const godot::Dictionary &p_components) const {                            \
        godot::Array keys = p_components.keys();                                                   \
        for (int i = 0; i < keys.size(); ++i) {                                                    \
            auto &variant_c_name = keys[i];                                                        \
            if (!gd_ecs_is_valid_component_name_print_error(variant_c_name)) {                     \
                continue;                                                                          \
            }                                                                                      \
            godot::StringName c_name = variant_c_name;                                             \
            auto &data = p_components[c_name];                                                     \
                                                                                                   \
            if (                                                                                   \
                c_name == godot::StaticStringName<"+-">::get()                                     \
                || c_name == godot::StaticStringName<"-+">::get()                                  \
            ) {                                                                                    \
                if (data.get_type() == godot::Variant::Type::DICTIONARY) {                         \
                    godot::Dictionary inner_components = data;                                     \
                    godot::Array inner_keys = inner_components.keys();                             \
                    for (int i = 0; i < inner_keys.size(); ++i) {                                  \
                        auto &variant_inner_c_name = inner_keys[i];                                \
                        if (!gd_ecs_is_valid_component_name_print_error(variant_inner_c_name)) {   \
                            continue;                                                              \
                        }                                                                          \
                        godot::StringName inner_c_name = variant_inner_c_name;                     \
                        auto &inner_data = inner_components[inner_c_name];                         \
                        components_diff_flat(inner_c_name, inner_data);                            \
                    }                                                                              \
                }                                                                                  \
                else {                                                                             \
                    ERR_PRINT(godot::vformat(                                                      \
                        "The '+-' or '-+' command expects a Dictionary value. Got: %s",            \
                        data                                                                       \
                    ));                                                                            \
                }                                                                                  \
            }                                                                                      \
            else {                                                                                 \
                components_diff_flat(c_name, data);                                                \
            }                                                                                      \
        }                                                                                          \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    bool remove_component(const godot::StringName &p_component) const {                            \
        return GD_ECS_SINGLETON_TYPE::remove(m_entity, p_component);                               \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    bool has_component(const godot::StringName &p_component) const {                               \
        return GD_ECS_SINGLETON_TYPE::has(m_entity, p_component);                                  \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    godot::Variant get_component(const godot::StringName &p_component) const {                     \
        return GD_ECS_SINGLETON_TYPE::get(m_entity, p_component);                                  \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    godot::Dictionary get_all_components_DEBUG_SLOW() const {                                      \
        godot::Dictionary components{};                                                            \
                                                                                                   \
        for (auto &c_name : GD_ECS_SINGLETON_TYPE::component_names()) {                            \
            if (GD_ECS_SINGLETON_TYPE::has(m_entity, c_name)) {                                    \
                components[c_name] = GD_ECS_SINGLETON_TYPE::get(m_entity, c_name);                 \
            }                                                                                      \
        }                                                                                          \
                                                                                                   \
        return components;                                                                         \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
protected:                                                                                         \
    static void _bind_methods() {                                                                  \
        godot::ClassDB::bind_method(                                                               \
            godot::D_METHOD("set_res_component", "p_component"),                                   \
            &GD_ECS_ENTITY_NAME::set_res_component                                                 \
        );                                                                                         \
        godot::ClassDB::bind_method(                                                               \
            godot::D_METHOD("set_res_components", "p_components"),                                 \
            &GD_ECS_ENTITY_NAME::set_res_components                                                \
        );                                                                                         \
        godot::ClassDB::bind_method(                                                               \
            godot::D_METHOD("set_component", "p_component", "p_data"),                             \
            &GD_ECS_ENTITY_NAME::set_component                                                     \
        );                                                                                         \
        godot::ClassDB::bind_method(                                                               \
            godot::D_METHOD("components_diff", "p_components"),                                    \
            &GD_ECS_ENTITY_NAME::components_diff                                                   \
        );                                                                                         \
        godot::ClassDB::bind_method(                                                               \
            godot::D_METHOD("remove_component", "p_component"),                                    \
            &GD_ECS_ENTITY_NAME::remove_component                                                  \
        );                                                                                         \
        godot::ClassDB::bind_method(                                                               \
            godot::D_METHOD("has_component", "p_component"),                                       \
            &GD_ECS_ENTITY_NAME::has_component                                                     \
        );                                                                                         \
        godot::ClassDB::bind_method(                                                               \
            godot::D_METHOD("get_component", "p_component"),                                       \
            &GD_ECS_ENTITY_NAME::get_component                                                     \
        );                                                                                         \
        godot::ClassDB::bind_method(                                                               \
            godot::D_METHOD("get_all_components_DEBUG_SLOW"),                                      \
            &GD_ECS_ENTITY_NAME::get_all_components_DEBUG_SLOW                                     \
        );                                                                                         \
        godot::ClassDB::bind_method(                                                               \
            godot::D_METHOD("get_empty_typed_array"),                                              \
            &GD_ECS_ENTITY_NAME::get_empty_typed_array                                             \
        );                                                                                         \
                                                                                                   \
        ADD_PROPERTY(                                                                              \
            godot::PropertyInfo(                                                                   \
                godot::Variant::ARRAY,                                                             \
                "res_components",                                                                  \
                godot::PROPERTY_HINT_RESOURCE_TYPE,                                                \
                GD_ECS_SINGLETON_TYPE::ComponentType::get_class_static()                           \
            ),                                                                                     \
            "set_res_components",                                                                  \
            "get_empty_typed_array"                                                                \
        );                                                                                         \
                                                                                                   \
        {                                                                                          \
            godot::Engine *engine = godot::Engine::get_singleton();                                \
            godot::OS *os = godot::OS::get_singleton();                                            \
            if (engine && os && !engine->is_editor_hint() && os->is_debug_build()) {               \
                ADD_PROPERTY(                                                                      \
                    godot::PropertyInfo(                                                           \
                        godot::Variant::DICTIONARY,                                                \
                        "components",                                                              \
                        godot::PROPERTY_HINT_DICTIONARY_TYPE,                                      \
                        godot::String::num_int64(godot::Variant::STRING_NAME) + ":;"               \
                        + godot::String::num_int64(godot::Variant::NIL) + ":"                      \
                    ),                                                                             \
                    "components_diff",                                                             \
                    "get_all_components_DEBUG_SLOW"                                                \
                );                                                                                 \
            }                                                                                      \
        }                                                                                          \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
private:                                                                                           \
    template <typename T>                                                                          \
    void emplace_or_replace_one_default() {                                                        \
        static_assert(                                                                             \
            godot::gd_ecs_has_emplace_or_replace<                                                  \
                GD_ECS_ENTITY_NAME,                                                                \
                GD_ECS_SINGLETON_TYPE::RegistryType::entity_type,                                  \
                T                                                                                  \
            >,                                                                                     \
            "\n"                                                                                   \
            "Concept violation summary:\n"                                                         \
            "All default ComponentTypes must be a valid gd_ecs_has_emplace_or_replace component "  \
            "type.\n"                                                                              \
            "\n"                                                                                   \
            "Expected interface:\n"                                                                \
            "\n"                                                                                   \
            "struct ComponentType {\n"                                                             \
            "    godot::String example{\"default value\"};\n"                                      \
            "\n"                                                                                   \
            "    static void emplace_or_replace(\n"                                                \
            "        godot::Node &p_entity_node,\n"                                                \
            "        " #GD_ECS_SINGLETON_TYPE "::RegistryType::entity_type &p_entity,\n"           \
            "        ComponentType &p_data\n"                                                      \
            "    ) {\n"                                                                            \
            "        auto &reg = " #GD_ECS_SINGLETON_TYPE "::registry();\n"                        \
            "        reg.emplace_or_replace<ComponentType>(p_entity, p_data);\n"                   \
            "    }\n"                                                                              \
            "};\n\n\n"                                                                             \
        );                                                                                         \
                                                                                                   \
        T data{};                                                                                  \
        T::emplace_or_replace(*this, m_entity, data);                                              \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    template <typename ...ComponentTypes>                                                          \
    void emplace_or_replace_defaults() {                                                           \
        (emplace_or_replace_one_default<ComponentTypes>(), ...);                                   \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    godot::Array get_empty_typed_array() const {                                                   \
        return godot::Array{};                                                                     \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    void components_diff_flat(const godot::StringName &p_key, const godot::Variant &p_data) const {\
        if (p_key == godot::StaticStringName<"+">::get()) {                                        \
            switch (p_data.get_type()) {                                                           \
                case godot::Variant::Type::STRING:                                                 \
                case godot::Variant::Type::STRING_NAME: {                                          \
                    GD_ECS_SINGLETON_TYPE::emplace_or_replace(m_entity, p_data, godot::Variant{}); \
                    break;                                                                         \
                }                                                                                  \
                case godot::Variant::Type::PACKED_STRING_ARRAY: {                                  \
                    godot::PackedStringArray to_add = p_data;                                      \
                    for (auto &c_name : to_add) {                                                  \
                        GD_ECS_SINGLETON_TYPE::emplace_or_replace(                                 \
                            m_entity,                                                              \
                            c_name,                                                                \
                            godot::Variant{}                                                       \
                        );                                                                         \
                    }                                                                              \
                    break;                                                                         \
                }                                                                                  \
                case godot::Variant::Type::ARRAY: {                                                \
                    godot::Array to_add = p_data;                                                  \
                    for (auto &variant_c_name : to_add) {                                          \
                        if (gd_ecs_is_valid_component_name_print_error(variant_c_name)) {          \
                            GD_ECS_SINGLETON_TYPE::emplace_or_replace(                             \
                                m_entity,                                                          \
                                variant_c_name,                                                    \
                                godot::Variant{}                                                   \
                            );                                                                     \
                        }                                                                          \
                        else {                                                                     \
                            ERR_PRINT(godot::vformat(                                              \
                                "The '+' command expects an Array containing only String or "      \
                                "StringName values. Got: %s",                                      \
                                variant_c_name                                                     \
                            ));                                                                    \
                        }                                                                          \
                    }                                                                              \
                    break;                                                                         \
                }                                                                                  \
                case godot::Variant::Type::DICTIONARY: {                                           \
                    godot::Dictionary inner_components = p_data;                                   \
                    godot::Array inner_keys = inner_components.keys();                             \
                    for (int i = 0; i < inner_keys.size(); ++i) {                                  \
                        auto &variant_inner_c_name = inner_keys[i];                                \
                        if (!gd_ecs_is_valid_component_name_print_error(variant_inner_c_name)) {   \
                            continue;                                                              \
                        }                                                                          \
                        godot::StringName inner_c_name = variant_inner_c_name;                     \
                        auto &inner_data = inner_components[inner_c_name];                         \
                        switch (inner_data.get_type()) {                                           \
                            case godot::Variant::Type::STRING:                                     \
                            case godot::Variant::Type::STRING_NAME: {                              \
                                GD_ECS_SINGLETON_TYPE::emplace_or_replace(                         \
                                    m_entity,                                                      \
                                    inner_c_name,                                                  \
                                    inner_data                                                     \
                                );                                                                 \
                                break;                                                             \
                            }                                                                      \
                            case godot::Variant::Type::PACKED_STRING_ARRAY: {                      \
                                godot::PackedStringArray to_add = inner_data;                      \
                                for (auto &c_name : to_add) {                                      \
                                    GD_ECS_SINGLETON_TYPE::emplace_or_replace(                     \
                                        m_entity,                                                  \
                                        c_name,                                                    \
                                        inner_data                                                 \
                                    );                                                             \
                                }                                                                  \
                                break;                                                             \
                            }                                                                      \
                            case godot::Variant::Type::ARRAY: {                                    \
                                godot::Array to_add = inner_data;                                  \
                                for (auto &variant_c_name : to_add) {                              \
                                    if (gd_ecs_is_valid_component_name_print_error(variant_c_name))\
                                    {                                                              \
                                        GD_ECS_SINGLETON_TYPE::emplace_or_replace(                 \
                                            m_entity,                                              \
                                            variant_c_name,                                        \
                                            inner_data                                             \
                                        );                                                         \
                                    }                                                              \
                                    else {                                                         \
                                        ERR_PRINT(godot::vformat(                                  \
                                            "The '+' command expects an Array containing only "    \
                                            " String or StringName values. Got: %s",               \
                                            variant_c_name                                         \
                                        ));                                                        \
                                    }                                                              \
                                }                                                                  \
                                break;                                                             \
                            }                                                                      \
                            default: {                                                             \
                                ERR_PRINT(godot::vformat(                                          \
                                    "The '+' command expects a Dictionary containing only "        \
                                    "[String, StringName, PackedStringArray, Array]. "             \
                                    "Got: %s",                                                     \
                                     inner_data                                                    \
                                ));                                                                \
                                break;                                                             \
                            }                                                                      \
                        }                                                                          \
                    }                                                                              \
                    break;                                                                         \
                }                                                                                  \
                default: {                                                                         \
                    ERR_PRINT(godot::vformat(                                                      \
                        "The '+' command expects [String, StringName, PackedStringArray, Array, "  \
                        "Dictionary]. Got: %s",                                                    \
                        p_data                                                                     \
                    ));                                                                            \
                    break;                                                                         \
                }                                                                                  \
            }                                                                                      \
        }                                                                                          \
        else if (p_key == godot::StaticStringName<"-">::get()) {                                   \
            switch (p_data.get_type()) {                                                           \
                case godot::Variant::Type::STRING:                                                 \
                case godot::Variant::Type::STRING_NAME: {                                          \
                    GD_ECS_SINGLETON_TYPE::remove(m_entity, p_data);                               \
                    break;                                                                         \
                }                                                                                  \
                case godot::Variant::Type::PACKED_STRING_ARRAY: {                                  \
                    godot::PackedStringArray to_remove = p_data;                                   \
                    for (auto &c_name : to_remove) {                                               \
                        GD_ECS_SINGLETON_TYPE::remove(m_entity, c_name);                           \
                    }                                                                              \
                    break;                                                                         \
                }                                                                                  \
                case godot::Variant::Type::ARRAY: {                                                \
                    godot::Array to_remove = p_data;                                               \
                    for (auto &variant_c_name : to_remove) {                                       \
                        if (gd_ecs_is_valid_component_name_print_error(variant_c_name)) {          \
                            GD_ECS_SINGLETON_TYPE::remove(m_entity, variant_c_name);               \
                        }                                                                          \
                        else {                                                                     \
                            ERR_PRINT(godot::vformat(                                              \
                                "The '-' command expects an Array containing only String or "      \
                                "StringName values. Got: %s",                                      \
                                variant_c_name                                                     \
                            ));                                                                    \
                        }                                                                          \
                    }                                                                              \
                    break;                                                                         \
                }                                                                                  \
                default: {                                                                         \
                    ERR_PRINT(godot::vformat(                                                      \
                        "The '-' command expects [String, StringName, PackedStringArray, Array]. " \
                        "Got: %s",                                                                 \
                        p_data                                                                     \
                    ));                                                                            \
                    break;                                                                         \
                }                                                                                  \
            }                                                                                      \
        }                                                                                          \
        else {                                                                                     \
            GD_ECS_SINGLETON_TYPE::emplace_or_replace(m_entity, p_key, p_data);                    \
        }                                                                                          \
    }                                                                                              \
                                                                                                   \
};
