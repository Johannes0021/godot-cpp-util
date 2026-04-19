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

#include "godot_cpp/core/property_info.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/string_name.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "signal_macros.hpp"



namespace godot {



//==================================================================================================
// C_Field
//==================================================================================================

template<typename StructT, typename FieldT>
struct C_Field final {
    using StructType = StructT;
    using FieldType = FieldT;



    FieldType StructType:: *member = nullptr;
    godot::PropertyInfo property_info{};
    godot::StringName set_fn{};
    godot::StringName get_fn{};



    C_Field(
        FieldType StructType:: *p_member,
        const godot::PropertyInfo &p_property_info,
        const godot::StringName &p_set_fn,
        const godot::StringName &p_get_fn
    )
        : member(p_member)
        , property_info(p_property_info)
        , set_fn(p_set_fn)
        , get_fn(p_get_fn)
    {}



    C_Field(
        FieldType StructType:: *p_member,
        godot::Variant::Type p_type,
        const godot::StringName &p_name,
        const godot::StringName &p_set_fn,
        const godot::StringName &p_get_fn
    )
        : member(p_member)
        , property_info(godot::PropertyInfo(p_type, p_name))
        , set_fn(p_set_fn)
        , get_fn(p_get_fn)
    {}



    C_Field(
        FieldType StructType:: *p_member,
        const godot::PropertyInfo &p_property_info
    )
        : member(p_member)
        , property_info(p_property_info)
        , set_fn(godot::String{"set_"} + p_property_info.name)
        , get_fn(godot::String{"get_"} + p_property_info.name)
    {}



    C_Field(
        FieldType StructType:: *p_member,
        godot::Variant::Type p_type,
        const godot::StringName &p_name
    )
        : member(p_member)
        , property_info(godot::PropertyInfo(p_type, p_name))
        , set_fn(godot::String{"set_"} + p_name)
        , get_fn(godot::String{"get_"} + p_name)
    {}

};



//==================================================================================================
// C_Descriptor
//==================================================================================================

template <typename StructT, typename ...Ts>
struct C_Descriptor final {
    using StructType = StructT;
    using FieldTypeTuple = std::tuple<C_Field<StructType, Ts>...>;



    godot::StringName name{};
    FieldTypeTuple fields{};



    C_Descriptor(const godot::StringName &p_name, const C_Field<StructType, Ts> ...p_fields)
        : name(p_name)
        , fields(p_fields...)
    {}



    C_Descriptor(const C_Field<StructType, Ts> ...p_fields)
        : fields(p_fields...)
    {}



    template <std::size_t I>
    void set(StructType& p_instance, const auto &p_value) const {
        auto &field = std::get<I>(fields);

        if (field.member) {
            p_instance.*(field.member) = p_value;
            return;
        }

        ERR_PRINT(godot::vformat(
            "C_Descriptor.set()%s: Missing member pointer for field '%s'. Tried to set: %s",
            name.is_empty() ? "" : godot::vformat(" (%s)", name),
            field.property_info.name,
            p_value
        ));
    }



    void set(StructType& p_instance, const godot::Variant &p_variant) const {
        if constexpr (std::tuple_size_v<FieldTypeTuple> == 1) {
            using FieldType = std::tuple_element_t<0, FieldTypeTuple>::FieldType;
            set<0>(p_instance, FieldType{p_variant});
        }
        else if constexpr (std::tuple_size_v<FieldTypeTuple> > 0) {
            const godot::Dictionary dictionary = p_variant;

            set_from_dictionary(
                p_instance,
                dictionary,
                std::make_index_sequence<std::tuple_size_v<FieldTypeTuple>>{}
            );
        }
    }



    template <std::size_t I>
    auto* try_get(StructType& p_instance) const {
        auto &field = std::get<I>(fields);

        if (field.member) {
            return &(p_instance.*(field.member));
        }

        ERR_PRINT(godot::vformat(
            "C_Descriptor.try_get()%s: Missing member pointer for field '%s'.",
            name.is_empty() ? "" : godot::vformat(" (%s)", name),
            field.property_info.name
        ));

        using FieldType = std::tuple_element_t<I, FieldTypeTuple>::FieldType;
        return static_cast<FieldType*>(nullptr);
    }



    template <std::size_t I>
    const auto* try_get(const StructType& p_instance) const {
        const auto &field = std::get<I>(fields);

        if (field.member) {
            return &(p_instance.*(field.member));
        }

        ERR_PRINT(godot::vformat(
            "C_Descriptor.try_get()%s: Missing member pointer for field '%s'.",
            name.is_empty() ? "" : godot::vformat(" (%s)", name),
            field.property_info.name
        ));

        using FieldType = std::tuple_element_t<I, FieldTypeTuple>::FieldType;
        return static_cast<FieldType*>(nullptr);
    }



    godot::Variant to_variant(const StructType& p_instance) const {
        if constexpr (std::tuple_size_v<FieldTypeTuple> == 1) {
            if (auto *value = try_get<0>(p_instance)) {
                return godot::Variant{*value};
            }
        }
        else if constexpr (std::tuple_size_v<FieldTypeTuple> > 0) {
            return try_to_dictionary(
                p_instance,
                std::make_index_sequence<std::tuple_size_v<FieldTypeTuple>>{}
            );
        }

        return godot::Variant{};
    }



private:
    template <std::size_t ...Is>
    void set_from_dictionary(
        StructType &p_instance,
        const godot::Dictionary &p_dictionary,
        std::index_sequence<Is...>
    ) const {
        (set_field_from_dictionary_if_present<Is>(p_instance, p_dictionary), ...);
    }



    template <std::size_t I>
    void set_field_from_dictionary_if_present(
        StructType &p_instance,
        const godot::Dictionary &p_dictionary
    ) const {
        auto &field = std::get<I>(fields);

        const godot::String key = field.property_info.name;
        if (p_dictionary.has(key)) {
            using FieldType = std::tuple_element_t<I, FieldTypeTuple>::FieldType;
            set<I>(p_instance, FieldType{p_dictionary[key]});
        }
    }



    template <std::size_t I>
    void try_insert_field_to_dictionary(
        const StructType &p_instance,
        godot::Dictionary &p_dictionary
    ) const {
        auto &field = std::get<I>(fields);
        if (auto *value = try_get<I>(p_instance)) {
            p_dictionary[field.property_info.name] = godot::Variant{*value};
        }
    }



    template <std::size_t ...Is>
    godot::Dictionary try_to_dictionary(
        const StructType& p_instance,
        std::index_sequence<Is...>
    ) const {
        godot::Dictionary dictionary;

        (try_insert_field_to_dictionary<Is>(p_instance, dictionary), ...);

        return dictionary;
    }

};



template<typename>
struct gd_ecs_is_component_descriptor : std::false_type {};



template<typename T, typename ...Ts>
struct gd_ecs_is_component_descriptor<const C_Descriptor<T, Ts...>&> : std::true_type {};



template<typename T>
inline constexpr bool gd_ecs_is_component_descriptor_v = gd_ecs_is_component_descriptor<T>::value;



template <typename T>
concept gd_ecs_has_component_descriptor =
requires { T::descriptor(); }
&& gd_ecs_is_component_descriptor_v<decltype(T::descriptor())>;



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
// GD_ECS_EMPTY_COMPONENT_DESCRIPTOR_IMPL
//==================================================================================================

/**
 * Generates an empty component descriptor.
 *
 * struct Empty {
 *     // The component name defaults to an empty string unless explicitly specified.
 *     // When registered via a resource, this name is used directly. If it is empty, the name is
 *     // taken from GD_ECS_COMPONENT, for example GD_ECS_COMPONENT(ECS, C_Empty, Empty) assigns the
 *     // name "Empty".
 *     GD_ECS_EMPTY_COMPONENT_DESCRIPTOR_IMPL(Empty)
 *
 *     // Alternative with a custom component name.
 *     //GD_ECS_EMPTY_COMPONENT_DESCRIPTOR_IMPL(Empty, "ComponentName")
 * };
 */
#define GD_ECS_EMPTY_COMPONENT_DESCRIPTOR_IMPL(ECS_COMPONENT_NAME, ...)                            \
static const auto& descriptor() {                                                                  \
    static godot::C_Descriptor<ECS_COMPONENT_NAME> descriptor{__VA_ARGS__};                        \
    return descriptor;                                                                             \
}



//==================================================================================================
// GD_ECS_COMPONENT_EMPLACE_OR_REPLACE_IMPL
//==================================================================================================

/**
 * Generates a default emplace_or_replace implementation.
 *
 * struct ComponentType {
 *     GD_ECS_COMPONENT_EMPLACE_OR_REPLACE_IMPL(ComponentType)
 * };
 */
#define GD_ECS_COMPONENT_EMPLACE_OR_REPLACE_IMPL(ECS_COMPONENT_NAME)                               \
static void emplace_or_replace(Node&, entt::entity &p_entity, ECS_COMPONENT_NAME &p_data) {        \
    auto &reg = ECS::registry();                                                                   \
    godot::gd_ecs_emplace_or_replace_maybe_empty_type(reg, p_entity, p_data);                      \
}



//==================================================================================================
// GD_ECS_EMPTY_COMPONENT_IMPL
//==================================================================================================

/**
 * Generates an empty component descriptor and a default emplace_or_replace implementation.
 *
 * struct Empty {
 *     // The component name defaults to an empty string unless explicitly specified.
 *     // When registered via a resource, this name is used directly. If it is empty, the name is
 *     // taken from GD_ECS_COMPONENT, for example GD_ECS_COMPONENT(ECS, C_Empty, Empty) assigns the
 *     // name "Empty".
 *     GD_ECS_EMPTY_COMPONENT_IMPL(Empty)
 *
 *     // Alternative with a custom component name.
 *     //GD_ECS_EMPTY_COMPONENT_IMPL(Empty, "ComponentName")
 * };
 */
#define GD_ECS_EMPTY_COMPONENT_IMPL(ECS_COMPONENT_NAME, ...)                                       \
GD_ECS_EMPTY_COMPONENT_DESCRIPTOR_IMPL(ECS_COMPONENT_NAME, __VA_ARGS__)                            \
GD_ECS_COMPONENT_EMPLACE_OR_REPLACE_IMPL(ECS_COMPONENT_NAME)



//==================================================================================================
// GD_ECS_COMPONENT_WITH_PARENT_EMPLACE_OR_REPLACE
//==================================================================================================

/**
 * using ECSType = godot::ECS;
 *
 * // A component that stores basic example data.
 * struct Data {
 *     int id{21};
 *     godot::String name{"SomeName"};
 *     float length{21.21f};
 *     godot::Dictionary meta{};
 *
 *     // Default constructor is required by the ECS.
 *     Data() = default;
 *
 *     // Required for:
 *     // - Translating C++ ECS components to editor-compatible variants.
 *     // - Resource wrapper components.
 *     //
 *     // Descriptor used by the ECS to expose this component to the Godot editor.
 *     // It defines how fields are interpreted, serialized, and edited.
 *     static const auto& descriptor() {
 *         static const auto descriptor = godot::C_Descriptor{
 *             // "ComponentName", // Defaults to an empty string if not explicitly specified.
 *             // When registered via a resource, this name is used directly. If it is empty, the
 *             // name is taken from GD_ECS_COMPONENT_WITH_PARENT_EMPLACE_OR_REPLACE, for example
 *             // GD_ECS_COMPONENT_WITH_PARENT_EMPLACE_OR_REPLACE(ECSType, C_Data, Data, ...)
 *             // assigns the name "Data".
 *
 *             // Field with explicit PropertyInfo and setter and getter.
 *             godot::C_Field{
 *                 &Data::id,
 *                 godot::PropertyInfo(godot::Variant::Type::INT, "id"),
 *                 "set_id",
 *                 "get_id"
 *              },
 *
 *             // Field using a simplified constructor with Variant type.
 *             godot::C_Field{
 *                 &Data::name,
 *                 godot::Variant::Type::STRING, "name",
 *                 "set_name",
 *                 "get_name"
 *             },
 *
 *             // Field with:
 *             // - PropertyInfo
 *             // - "set_length"
 *             // - "get_length"
 *             godot::C_Field{
 *                 &Data::length,
 *                 godot::PropertyInfo(godot::Variant::Type::FLOAT, "length")
 *             },
 *
 *             // Field with:
 *             // - PropertyInfo(Variant::Type::DICTIONARY, "meta")
 *             // - "set_meta"
 *             // - "get_meta"
 *             godot::C_Field{&Data::meta, godot::Variant::Type::DICTIONARY, "meta"}
 *         };
 *
 *         return descriptor;
 *     }
 *
 *     // Required for:
 *     // - Resource wrapper components.
 *     // - Default components of entities.
 *     //
 *     // This function is functionally equivalent to:
 *     // GD_ECS_COMPONENT_EMPLACE_OR_REPLACE_IMPL(Data)
 *     static void emplace_or_replace(
 *         [[maybe_unused]] godot::Node &p_entity_node,
 *         ECSType::RegistryType::entity_type &p_entity,
*          Data &p_data
*      ) {
 *         auto &reg = ECS::registry();
 *         reg.emplace_or_replace<Data>(p_entity, p_data);
 *     }
 * };
 *
 * // Defines a Resource wrapper for the Data component.
 * // This allows the component to be created, stored, and edited as a Godot Resource.
 * // Entities such as E_Node can add and manage this component themselves.
 * GD_ECS_COMPONENT_WITH_PARENT_AND_POLICY(
 *     ECSType,
 *     C_Data, Data, ECSType::ComponentType,
 *     C_SuperCallPolicy::Never
 * )
 *
 * //// Do not forget to expose the new component to Godot:
 * //ECSType::register_types();
 * //// ...
 * //C_Data::register_types(); // This also calls ECS::register_type<Data>(...);
 */
#define GD_ECS_COMPONENT_WITH_PARENT_AND_POLICY(                                                   \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    ECS_COMPONENT_NAME,                                                                            \
    GD_ECS_COMPONENT_PARENT_TYPE,                                                                  \
    SUPER_CALL_POLICY                                                                              \
)                                                                                                  \
class GD_ECS_COMPONENT_NAME : public GD_ECS_COMPONENT_PARENT_TYPE {                                \
    GDCLASS(GD_ECS_COMPONENT_NAME, GD_ECS_COMPONENT_PARENT_TYPE)                                   \
                                                                                                   \
    static_assert(                                                                                 \
        godot::gd_ecs_has_component_descriptor<ECS_COMPONENT_NAME>                                 \
        && gd_ecs_has_emplace_or_replace<                                                          \
            godot::Node,                                                                           \
            GD_ECS_SINGLETON_TYPE::RegistryType::entity_type,                                      \
            ECS_COMPONENT_NAME                                                                     \
        >,                                                                                         \
        "\n"                                                                                       \
        "Concept violation summary:\n"                                                             \
        #ECS_COMPONENT_NAME " is not a valid gd_ecs_has_component_descriptor or "                  \
        "gd_ecs_has_emplace_or_replace component type.\n"                                          \
        "\n"                                                                                       \
        "Expected interface:\n"                                                                    \
        "\n"                                                                                       \
        "struct " #ECS_COMPONENT_NAME " {\n"                                                       \
        "    godot::String example{\"default value\"};\n"                                          \
        "\n"                                                                                       \
        "    static const auto& descriptor() {\n"                                                  \
        "        static const auto descriptor = godot::C_Descriptor{\n"                            \
        "            //\"ComponentName\",\n"                                                       \
        "            godot::C_Field{&"                                                             \
                         #ECS_COMPONENT_NAME "::example, godot::Variant::STRING, \"example\"},\n"  \
        "        };\n"                                                                             \
        "\n"                                                                                       \
        "        return descriptor;\n"                                                             \
        "    }\n"                                                                                  \
        "\n"                                                                                       \
        "    static void emplace_or_replace(\n"                                                    \
        "        godot::Node &p_entity_node,\n"                                                    \
        "        " #GD_ECS_SINGLETON_TYPE "::RegistryType::entity_type &p_entity,\n"               \
        "        " #ECS_COMPONENT_NAME " &p_data\n"                                                \
        "    ) {\n"                                                                                \
        "        auto &reg = " #GD_ECS_SINGLETON_TYPE "::registry();\n"                            \
        "        reg.emplace_or_replace<" #ECS_COMPONENT_NAME ">(p_entity, p_data);\n"             \
        "    }\n"                                                                                  \
        "};\n\n\n"                                                                                 \
    );                                                                                             \
                                                                                                   \
                                                                                                   \
                                                                                                   \
public:                                                                                            \
    GD_ECS_EMPTY_SIGNAL_STRUCT(Signal, GD_ECS_COMPONENT_PARENT_TYPE::Signal)                       \
                                                                                                   \
                                                                                                   \
                                                                                                   \
public:                                                                                            \
    using ComponentType = ECS_COMPONENT_NAME;                                                      \
    using DescriptorType = std::remove_reference_t<decltype(ComponentType::descriptor())>;         \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    ComponentType data{};                                                                          \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    static void register_types() {                                                                 \
        GDREGISTER_RUNTIME_CLASS(GD_ECS_COMPONENT_NAME);                                           \
                                                                                                   \
        auto &descriptor = ECS_COMPONENT_NAME::descriptor();                                       \
        if (descriptor.name.is_empty()) {                                                          \
            GD_ECS_SINGLETON_TYPE::register_type<ECS_COMPONENT_NAME>(#ECS_COMPONENT_NAME);         \
        }                                                                                          \
        else {                                                                                     \
            GD_ECS_SINGLETON_TYPE::register_type<ECS_COMPONENT_NAME>(descriptor.name);             \
        }                                                                                          \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    template <std::size_t I, typename T>                                                           \
    void set(const T& p_value) {                                                                   \
        auto &descriptor = ECS_COMPONENT_NAME::descriptor();                                       \
        descriptor.set<I>(data, p_value);                                                          \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    template <std::size_t I, typename T>                                                           \
    T get() const {                                                                                \
        auto &descriptor = ECS_COMPONENT_NAME::descriptor();                                       \
        if (auto *value = descriptor.try_get<I>(data)) {                                           \
            return *value;                                                                         \
        }                                                                                          \
                                                                                                   \
        return T{};                                                                                \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    virtual void emplace_or_replace(                                                               \
        godot::Node &p_entity_node,                                                                \
        GD_ECS_SINGLETON_TYPE::RegistryType::entity_type &p_entity                                 \
    ) override {                                                                                   \
        if constexpr (SUPER_CALL_POLICY == C_SuperCallPolicy::Before) {                            \
            GD_ECS_COMPONENT_PARENT_TYPE::emplace_or_replace(p_entity_node, p_entity);             \
        }                                                                                          \
                                                                                                   \
        ECS_COMPONENT_NAME::emplace_or_replace(p_entity_node, p_entity, data);                     \
                                                                                                   \
        if constexpr (SUPER_CALL_POLICY == C_SuperCallPolicy::After) {                             \
            GD_ECS_COMPONENT_PARENT_TYPE::emplace_or_replace(p_entity_node, p_entity);             \
        }                                                                                          \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
protected:                                                                                         \
    static void _bind_methods() {                                                                  \
        bind_all(                                                                                  \
            std::make_index_sequence<std::tuple_size_v<typename DescriptorType::FieldTypeTuple>>{} \
        );                                                                                         \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
private:                                                                                           \
    template <std::size_t I>                                                                       \
    static void bind_field() {                                                                     \
        using FieldType =                                                                          \
            std::tuple_element_t<I, typename DescriptorType::FieldTypeTuple>::FieldType;           \
                                                                                                   \
        auto &descriptor = ECS_COMPONENT_NAME::descriptor();                                       \
        auto &field = std::get<I>(descriptor.fields);                                              \
                                                                                                   \
        godot::ClassDB::bind_method(                                                               \
            godot::D_METHOD(field.set_fn, "p_value"),                                              \
            &GD_ECS_COMPONENT_NAME::set<I, FieldType>                                              \
        );                                                                                         \
        godot::ClassDB::bind_method(                                                               \
            godot::D_METHOD(field.get_fn),                                                         \
            &GD_ECS_COMPONENT_NAME::get<I, FieldType>                                              \
        );                                                                                         \
                                                                                                   \
        ADD_PROPERTY(field.property_info, field.set_fn, field.get_fn);                             \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    template <std::size_t ...I>                                                                    \
    static void bind_all(std::index_sequence<I...>) {                                              \
        (bind_field<I>(), ...);                                                                    \
    }                                                                                              \
                                                                                                   \
};



//==================================================================================================
// GD_ECS_COMPONENT_WITH_PARENT
//==================================================================================================

 /**
 * using ECSType = godot::ECS;
 *
 * struct Empty { GD_ECS_EMPTY_COMPONENT_IMPL(Empty) };
 *
 * GD_ECS_COMPONENT_WITH_PARENT(ECSType, C_Empty, Empty, ECSType::ComponentType)
 *
 * //// Do not forget to expose the new component to Godot:
 * //ECSType::register_types();
 * //// ...
 * //C_Empty::register_types(); // This also calls ECS::register_type<Empty>(...);
 */
#define GD_ECS_COMPONENT_WITH_PARENT(                                                              \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    ECS_COMPONENT_NAME,                                                                            \
    GD_ECS_COMPONENT_PARENT_TYPE                                                                   \
)                                                                                                  \
GD_ECS_COMPONENT_WITH_PARENT_AND_POLICY(                                                           \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    ECS_COMPONENT_NAME,                                                                            \
    GD_ECS_COMPONENT_PARENT_TYPE,                                                                  \
    C_SuperCallPolicy::Never                                                                       \
)



//==================================================================================================
// GD_ECS_COMPONENT_WITH_POLICY
//==================================================================================================

 /**
 * using ECSType = godot::ECS;
 *
 * struct Empty { GD_ECS_EMPTY_COMPONENT_IMPL(Empty) };
 *
 * GD_ECS_COMPONENT_WITH_PARENT(ECSType, C_Empty, Empty, C_SuperCallPolicy::After)
 *
 * //// Do not forget to expose the new component to Godot:
 * //ECSType::register_types();
 * //// ...
 * //C_Empty::register_types(); // This also calls ECS::register_type<Empty>(...);
 */
#define GD_ECS_COMPONENT_WITH_POLICY(                                                              \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    ECS_COMPONENT_NAME,                                                                            \
    SUPER_CALL_POLICY                                                                              \
)                                                                                                  \
GD_ECS_COMPONENT_WITH_PARENT_AND_POLICY(                                                           \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    ECS_COMPONENT_NAME,                                                                            \
    GD_ECS_SINGLETON_TYPE::ComponentType,                                                          \
    SUPER_CALL_POLICY                                                                              \
)



//==================================================================================================
// GD_ECS_COMPONENT
//==================================================================================================

 /**
 * using ECSType = godot::ECS;
 *
 * struct Empty { GD_ECS_EMPTY_COMPONENT_IMPL(Empty) };
 *
 * GD_ECS_COMPONENT(ECSType, C_Empty, Empty)
 *
 * //// Do not forget to expose the new component to Godot:
 * //ECSType::register_types();
 * //// ...
 * //C_Empty::register_types(); // This also calls ECS::register_type<Empty>(...);
 */
#define GD_ECS_COMPONENT(GD_ECS_SINGLETON_TYPE, GD_ECS_COMPONENT_NAME, ECS_COMPONENT_NAME)         \
GD_ECS_COMPONENT_WITH_PARENT_AND_POLICY(                                                           \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    ECS_COMPONENT_NAME,                                                                            \
    GD_ECS_SINGLETON_TYPE::ComponentType,                                                          \
    C_SuperCallPolicy::Never                                                                       \
)
