/**
 * This header provides helpers that are intended to make it easier to define component classes.
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



#include <concepts>
#include <cstddef>
#include <tuple>
#include <utility>

#include "godot_cpp/classes/engine.hpp"

#include "godot_cpp_util/core/ptr.hpp"
#include "godot_cpp_util/core/object/export.hpp"

#include "signal_macros.hpp"



namespace godot {



//==================================================================================================
// gd_ecs_has_export_descriptor
//==================================================================================================

template<typename>
struct gd_ecs_is_export_descriptor : std::false_type {};



template<typename T, typename ...Ts>
struct gd_ecs_is_export_descriptor<const ExportDescriptor<T, Ts...>&> : std::true_type {};



template<typename T>
inline constexpr bool gd_ecs_is_export_descriptor_v = gd_ecs_is_export_descriptor<T>::value;



template <typename T>
concept gd_ecs_has_export_descriptor =
requires { T::export_descriptor(); }
&& gd_ecs_is_export_descriptor_v<decltype(T::export_descriptor())>;



//==================================================================================================
// C_SuperCallPolicy
//==================================================================================================

enum class C_SuperCallPolicy {
    Never,
    Before,
    After
};



//==================================================================================================
// gd_ecs_has_emplace_or_replace
//==================================================================================================

template <typename NodeType, typename EntityType, typename T>
concept gd_ecs_has_emplace_or_replace =
requires (NodeType &n, EntityType &e, T &t) {
    { T::emplace_or_replace(n, e, t) } -> std::same_as<void>;
};



//==================================================================================================
// Helpers
//==================================================================================================

template <typename RegistryType, typename EntityType, typename T>
requires std::is_empty_v<T>
void gd_ecs_emplace_or_replace_maybe_empty_type(
    RegistryType &p_registry,
    const EntityType &p_entity,
    const T&
) {
    p_registry.template emplace_or_replace<T>(p_entity);
}



template <typename RegistryType, typename EntityType, typename T>
requires (!std::is_empty_v<T>)
void gd_ecs_emplace_or_replace_maybe_empty_type(
    RegistryType &p_registry,
    const EntityType &p_entity,
    const T &p_data
) {
    p_registry.template emplace_or_replace<T>(p_entity, p_data);
}



} // namespace godot



//==================================================================================================
// GD_ECS_COMPONENT_EXPORT
//==================================================================================================

/**
 * Generates an empty component descriptor.
 *
 * #include "godot_cpp_util/ecs/ecs.hpp"
 *
 * struct Empty {
 *     GD_ECS_COMPONENT_EXPORT(Empty, "Empty")
 * };
 *
 * struct Data {
 *     int id{21};
 *     float length{21.21f};
 *     godot::String name{"SomeName"};
 *     godot::Dictionary meta{};
 *
 *     GD_ECS_COMPONENT_EXPORT(Data, "DataComponentName",
 *         godot::ExportByValue{&Data::id,     godot::Variant::Type::INT,        "id"},
 *         godot::ExportByValue{&Data::length, godot::Variant::Type::FLOAT,      "length"},
 *         godot::ExportByRef{&Data::name,     godot::Variant::Type::STRING,     "name"},
 *         godot::ExportByRef{&Data::meta,     godot::Variant::Type::DICTIONARY, "meta"}
 *     )
 * };
 */
#define GD_ECS_COMPONENT_EXPORT(ECS_COMPONENT_NAME, C_DESCRIPTOR_NAME, ...)                        \
static const auto& export_descriptor() {                                                           \
    static const auto descriptor = godot::ExportDescriptorOfType<ECS_COMPONENT_NAME>::make(        \
        C_DESCRIPTOR_NAME                                                                          \
        __VA_OPT__(,) __VA_ARGS__                                                                  \
    );                                                                                             \
                                                                                                   \
    return descriptor;                                                                             \
}



//==================================================================================================
// GD_ECS_COMPONENT_EMPLACE_OR_REPLACE_IMPL
//==================================================================================================

/**
 * Generates a default emplace_or_replace implementation.
 *
 * #include "godot_cpp_util/ecs/ecs.hpp"
 *
 * using ECSType = godot::ECS;
 *
 * struct Data {
 *     GD_ECS_COMPONENT_EMPLACE_OR_REPLACE_IMPL(ECSType, Data)
 * };
 */
#define GD_ECS_COMPONENT_EMPLACE_OR_REPLACE_IMPL(GD_ECS_SINGLETON_TYPE, ECS_COMPONENT_NAME)        \
static void emplace_or_replace(                                                                    \
    godot::Node&,                                                                                  \
    GD_ECS_SINGLETON_TYPE::RegistryType::entity_type &p_entity,                                    \
    ECS_COMPONENT_NAME &p_data                                                                     \
) {                                                                                                \
    auto &reg = GD_ECS_SINGLETON_TYPE::registry();                                                 \
    godot::gd_ecs_emplace_or_replace_maybe_empty_type(reg, p_entity, p_data);                      \
}



//==================================================================================================
// GD_ECS_COMPONENT_IMPL
//==================================================================================================

/**
 * Generates an empty component descriptor and a default emplace_or_replace implementation.
 *
 * #include "godot_cpp_util/ecs/ecs.hpp"
 *
 * using ECSType = godot::ECS;
 *
 * struct Empty {
 *     GD_ECS_COMPONENT_IMPL(ECSType, Empty, "Empty")
 * };
 *
 * struct Data {
 *     int id{21};
 *     godot::String name{"SomeName"};
 *     float length{21.21f};
 *     godot::Dictionary meta{};
 *
 *     GD_ECS_COMPONENT_IMPL(ECSType, Data, "ComponentName",
 *         godot::ExportByValue{&Data::id,     godot::Variant::Type::INT,        "id"},
 *         godot::ExportByRef{&Data::name,     godot::Variant::Type::STRING,     "name"},
 *         godot::ExportByValue{&Data::length, godot::Variant::Type::FLOAT,      "length"},
 *         godot::ExportByRef{&Data::meta,     godot::Variant::Type::DICTIONARY, "meta"}
 *     )
 * };
 */
#define GD_ECS_COMPONENT_IMPL(GD_ECS_SINGLETON_TYPE, ECS_COMPONENT_NAME, C_DESCRIPTOR_NAME, ...)   \
GD_ECS_COMPONENT_EXPORT(ECS_COMPONENT_NAME, C_DESCRIPTOR_NAME, __VA_ARGS__)                        \
GD_ECS_COMPONENT_EMPLACE_OR_REPLACE_IMPL(GD_ECS_SINGLETON_TYPE, ECS_COMPONENT_NAME)



//==================================================================================================
// GD_ECS_RES_COMPONENT_WITH_PARENT_AND_POLICY
//==================================================================================================

/**
 * #include "godot_cpp_util/ecs/ecs.hpp"
 *
 * using ECSType = godot::ECS;
 *
 * // A component that stores basic example data.
 * struct Data {
 *     int id{21};
 *     godot::String name{"SomeName"};
 *     float length{21.21f};
 *     godot::Dictionary meta{};
 *
 *     GD_ECS_COMPONENT_IMPL(ECSType, Data, "ComponentName",
 *         godot::ExportByValue{&Data::id,     godot::Variant::Type::INT,        "id"},
 *         godot::ExportByRef{&Data::name,     godot::Variant::Type::STRING,     "name"},
 *         godot::ExportByValue{&Data::length, godot::Variant::Type::FLOAT,      "length"},
 *         godot::ExportByRef{&Data::meta,     godot::Variant::Type::DICTIONARY, "meta"}
 *     )
 * };
 *
 * // Defines a Resource wrapper for the Data component.
 * // This allows the component to be created, stored, and edited as a Godot Resource.
 * // Entities such as E_Node can add and manage this component themselves.
 * GD_ECS_RES_COMPONENT_WITH_PARENT_AND_POLICY(
 *     ECSType,
 *     C_Data, Data, ECSType::ComponentType,
 *     godot::C_SuperCallPolicy::Never
 * )
 *
 * //// Do not forget to expose the new component to Godot:
 * //ECSType::register_types();
 * //// ...
 * //C_Data::register_types(); // This also calls ECSType::register_type<Data>();
 */
#define GD_ECS_RES_COMPONENT_WITH_PARENT_AND_POLICY(                                               \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_RES_COMPONENT_NAME,                                                                     \
    ECS_COMPONENT_NAME,                                                                            \
    GD_ECS_RES_COMPONENT_PARENT_TYPE,                                                              \
    SUPER_CALL_POLICY                                                                              \
)                                                                                                  \
class GD_ECS_RES_COMPONENT_NAME : public GD_ECS_RES_COMPONENT_PARENT_TYPE {                        \
    GDCLASS(GD_ECS_RES_COMPONENT_NAME, GD_ECS_RES_COMPONENT_PARENT_TYPE)                           \
                                                                                                   \
    static_assert(                                                                                 \
        godot::gd_ecs_has_export_descriptor<ECS_COMPONENT_NAME>                                    \
        && godot::gd_ecs_has_emplace_or_replace<                                                   \
            godot::Node,                                                                           \
            GD_ECS_SINGLETON_TYPE::RegistryType::entity_type,                                      \
            ECS_COMPONENT_NAME                                                                     \
        >,                                                                                         \
        "\n"                                                                                       \
        "Concept violation summary:\n"                                                             \
        #ECS_COMPONENT_NAME " is not a valid gd_ecs_has_export_descriptor or "                     \
        "gd_ecs_has_emplace_or_replace component type.\n"                                          \
        "\n"                                                                                       \
        "Expected interface:\n"                                                                    \
        "\n"                                                                                       \
        "struct " #ECS_COMPONENT_NAME " {\n"                                                       \
        "    godot::String example{\"default value\"};\n"                                          \
        "\n"                                                                                       \
        "    static const auto& export_descriptor() {\n"                                           \
        "        static const godot::ExportDescriptor descriptor{\n"                               \
        "            \"" #ECS_COMPONENT_NAME "ComponentName\",\n"                                  \
        "            godot::ExportByRef{&"                                                         \
                         #ECS_COMPONENT_NAME "::example, godot::Variant::STRING, \"example\"},\n"  \
        "        };\n"                                                                             \
        "\n"                                                                                       \
        "        return descriptor;\n"                                                             \
        "    }\n"                                                                                  \
        "\n"                                                                                       \
        "    static void emplace_or_replace(\n"                                                    \
        "        godot::Node &/* p_entity_node */,\n"                                              \
        "        " #GD_ECS_SINGLETON_TYPE "::RegistryType::entity_type &p_entity,\n"               \
        "        " #ECS_COMPONENT_NAME " &p_data\n"                                                \
        "    ) {\n"                                                                                \
        "        auto &reg = " #GD_ECS_SINGLETON_TYPE "::registry();\n"                            \
        "        reg.emplace_or_replace<" #ECS_COMPONENT_NAME ">(p_entity, p_data);\n"             \
        "    }\n"                                                                                  \
        "};\n\n"                                                                                   \
        "This function is functionally equivalent to:\n"                                           \
        "\n"                                                                                       \
        "struct " #ECS_COMPONENT_NAME " {\n"                                                       \
        "    godot::String example{\"default value\"};\n"                                          \
        "\n"                                                                                       \
        "    GD_ECS_COMPONENT_IMPL(" #GD_ECS_SINGLETON_TYPE ", " #ECS_COMPONENT_NAME ", \""        \
                 #ECS_COMPONENT_NAME "ComponentName\",\n"                                          \
        "        godot::ExportByRef{&"                                                             \
                     #ECS_COMPONENT_NAME "::example, godot::Variant::STRING, \"example\"}\n"       \
        "    )\n"                                                                                  \
        "};\n\n\n"                                                                                 \
    );                                                                                             \
                                                                                                   \
                                                                                                   \
                                                                                                   \
public:                                                                                            \
    GD_ECS_EMPTY_SIGNAL_STRUCT(Signal, GD_ECS_RES_COMPONENT_PARENT_TYPE::Signal)                   \
                                                                                                   \
                                                                                                   \
                                                                                                   \
public:                                                                                            \
    using ComponentType = ECS_COMPONENT_NAME;                                                      \
    using ExportDescriptorType =                                                                   \
        std::remove_reference_t<decltype(ECS_COMPONENT_NAME::export_descriptor())>;                \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    ECS_COMPONENT_NAME data{};                                                                     \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    static void register_types() {                                                                 \
        GDREGISTER_RUNTIME_CLASS(GD_ECS_RES_COMPONENT_NAME);                                       \
        GD_ECS_SINGLETON_TYPE::register_type<ECS_COMPONENT_NAME>();                                \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    template <std::size_t I, typename T>                                                           \
    void set(T p_value) {                                                                          \
        auto &descriptor = ECS_COMPONENT_NAME::export_descriptor();                                \
        descriptor.set<I>(data, p_value);                                                          \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    template <std::size_t I, typename T>                                                           \
    T get() const {                                                                                \
        auto &descriptor = ECS_COMPONENT_NAME::export_descriptor();                                \
        return descriptor.get<I>(data);                                                            \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    virtual void emplace_or_replace(                                                               \
        godot::Node &p_entity_node,                                                                \
        GD_ECS_SINGLETON_TYPE::RegistryType::entity_type &p_entity                                 \
    ) override {                                                                                   \
        if constexpr (SUPER_CALL_POLICY == godot::C_SuperCallPolicy::Before) {                     \
            GD_ECS_RES_COMPONENT_PARENT_TYPE::emplace_or_replace(p_entity_node, p_entity);         \
        }                                                                                          \
                                                                                                   \
        ECS_COMPONENT_NAME::emplace_or_replace(p_entity_node, p_entity, data);                     \
                                                                                                   \
        if constexpr (SUPER_CALL_POLICY == godot::C_SuperCallPolicy::After) {                      \
            GD_ECS_RES_COMPONENT_PARENT_TYPE::emplace_or_replace(p_entity_node, p_entity);         \
        }                                                                                          \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
protected:                                                                                         \
    static void _bind_methods() {                                                                  \
        bind_all_fields(                                                                           \
            std::make_index_sequence<                                                              \
                std::tuple_size_v<typename ExportDescriptorType::FieldTypeTuple>                   \
            >{}                                                                                    \
        );                                                                                         \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
private:                                                                                           \
    template <std::size_t I>                                                                       \
    static void bind_field() {                                                                     \
        using ExposedType =                                                                        \
            std::tuple_element_t<I, typename ExportDescriptorType::FieldTypeTuple>::ExposedType;   \
                                                                                                   \
        auto &descriptor = ECS_COMPONENT_NAME::export_descriptor();                                \
        auto &field = std::get<I>(descriptor.fields);                                              \
                                                                                                   \
        if (field.export_flags.all_of(ExportFlags::WithSet)) {                                     \
            godot::ClassDB::bind_method(                                                           \
                godot::D_METHOD(field.set_fn, "p_value"),                                          \
                &GD_ECS_RES_COMPONENT_NAME::set<I, ExposedType>                                    \
            );                                                                                     \
        }                                                                                          \
                                                                                                   \
        godot::ClassDB::bind_method(                                                               \
            godot::D_METHOD(field.get_fn),                                                         \
            &GD_ECS_RES_COMPONENT_NAME::get<I, ExposedType>                                        \
        );                                                                                         \
                                                                                                   \
        bool add_property = field.export_flags.all_of(                                             \
            ExportFlags::AddPropertyEditor | ExportFlags::AddPropertyRemote                        \
        );                                                                                         \
                                                                                                   \
        if (                                                                                       \
            !add_property                                                                          \
            && field.export_flags.any_of(                                                          \
                ExportFlags::AddPropertyEditor | ExportFlags::AddPropertyRemote                    \
            )                                                                                      \
        ) {                                                                                        \
            auto engine = Ptr<godot::Engine>{godot::Engine::get_singleton()};                      \
            bool is_editor_hint = engine && engine->is_editor_hint();                              \
            add_property =                                                                         \
                (                                                                                  \
                    is_editor_hint                                                                 \
                    && field.export_flags.all_of(ExportFlags::AddPropertyEditor)                   \
                )                                                                                  \
                || (                                                                               \
                    !is_editor_hint                                                                \
                    && field.export_flags.all_of(ExportFlags::AddPropertyRemote)                   \
                );                                                                                 \
        }                                                                                          \
                                                                                                   \
        if (add_property) {                                                                        \
            if (field.export_flags.all_of(ExportFlags::WithSet)) {                                 \
                ADD_PROPERTY(field.property_info, field.set_fn, field.get_fn);                     \
            }                                                                                      \
            else {                                                                                 \
                ADD_PROPERTY(field.property_info, "", field.get_fn);                               \
            }                                                                                      \
        }                                                                                          \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    template <std::size_t ...I>                                                                    \
    static void bind_all_fields(std::index_sequence<I...>) {                                       \
        (bind_field<I>(), ...);                                                                    \
    }                                                                                              \
                                                                                                   \
};



//==================================================================================================
// GD_ECS_RES_COMPONENT_WITH_PARENT
//==================================================================================================

 /**
 * #include "godot_cpp_util/ecs/ecs.hpp"
 *
 * using ECSType = godot::ECS;
 *
 * struct Empty {
 *     GD_ECS_COMPONENT_IMPL(ECSType, Empty, "Empty")
 * };
 *
 * GD_ECS_RES_COMPONENT_WITH_PARENT(ECSType, C_Empty, Empty, ECSType::ComponentType)
 *
 * //// Do not forget to expose the new component to Godot:
 * //ECSType::register_types();
 * //// ...
 * //C_Empty::register_types(); // This also calls ECSType::register_type<Empty>();
 */
#define GD_ECS_RES_COMPONENT_WITH_PARENT(                                                          \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_RES_COMPONENT_NAME,                                                                     \
    ECS_COMPONENT_NAME,                                                                            \
    GD_ECS_RES_COMPONENT_PARENT_TYPE                                                               \
)                                                                                                  \
GD_ECS_RES_COMPONENT_WITH_PARENT_AND_POLICY(                                                       \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_RES_COMPONENT_NAME,                                                                     \
    ECS_COMPONENT_NAME,                                                                            \
    GD_ECS_RES_COMPONENT_PARENT_TYPE,                                                              \
    godot::C_SuperCallPolicy::Never                                                                \
)



//==================================================================================================
// GD_ECS_RES_COMPONENT_WITH_POLICY
//==================================================================================================

 /**
 * #include "godot_cpp_util/ecs/ecs.hpp"
 *
 * using ECSType = godot::ECS;
 *
 * struct Empty {
 *     GD_ECS_COMPONENT_IMPL(ECSType, Empty, "Empty")
 * };
 *
 * GD_ECS_RES_COMPONENT_WITH_POLICY(ECSType, C_Empty, Empty, godot::C_SuperCallPolicy::After)
 *
 * //// Do not forget to expose the new component to Godot:
 * //ECSType::register_types();
 * //// ...
 * //C_Empty::register_types(); // This also calls ECSType::register_type<Empty>();
 */
#define GD_ECS_RES_COMPONENT_WITH_POLICY(                                                          \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_RES_COMPONENT_NAME,                                                                     \
    ECS_COMPONENT_NAME,                                                                            \
    SUPER_CALL_POLICY                                                                              \
)                                                                                                  \
GD_ECS_RES_COMPONENT_WITH_PARENT_AND_POLICY(                                                       \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_RES_COMPONENT_NAME,                                                                     \
    ECS_COMPONENT_NAME,                                                                            \
    GD_ECS_SINGLETON_TYPE::ComponentType,                                                          \
    SUPER_CALL_POLICY                                                                              \
)



//==================================================================================================
// GD_ECS_RES_COMPONENT
//==================================================================================================

 /**
 * #include "godot_cpp_util/ecs/ecs.hpp"
 *
 * using ECSType = godot::ECS;
 *
 * struct Empty {
 *     GD_ECS_COMPONENT_IMPL(ECSType, Empty, "Empty")
 * };
 *
 * GD_ECS_RES_COMPONENT(ECSType, C_Empty, Empty)
 *
 * //// Do not forget to expose the new component to Godot:
 * //ECSType::register_types();
 * //// ...
 * //C_Empty::register_types(); // This also calls ECSType::register_type<Empty>();
 */
#define GD_ECS_RES_COMPONENT(GD_ECS_SINGLETON_TYPE, GD_ECS_RES_COMPONENT_NAME, ECS_COMPONENT_NAME) \
GD_ECS_RES_COMPONENT_WITH_PARENT_AND_POLICY(                                                       \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_RES_COMPONENT_NAME,                                                                     \
    ECS_COMPONENT_NAME,                                                                            \
    GD_ECS_SINGLETON_TYPE::ComponentType,                                                          \
    godot::C_SuperCallPolicy::Never                                                                \
)
