/**
 * This header provides macros that are intended to make it easier to define a component class
 * (GDCLASS).
 *
 * Define GD_ECS_DO_NOT_GENERATE_SIGNAL_CODE before including this file to completely disable all
 * generated signal code.
 *
 * Usage Example:
 *
 * #pragma once
 *
 * #include "godot_cpp_util/ecs/ecs.hpp"
 *
 * using ECSType = godot::ECS;
 * struct Marker {};
 * GD_ECS_EMPTY_COMPONENT(ECSType, C_Marker, Marker)
 *
 * //// Do not forget to expose the new component to Godot:
 * //ECSType::register_types();
 * //// ...
 * //C_Marker::register_types();
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



//==================================================================================================
// Helpers
//==================================================================================================

namespace godot {

struct C_Field {
    godot::PropertyInfo property_info{};
    godot::StringName set_fn{};
    godot::StringName get_fn{};

    static C_Field field(godot::Variant::Type p_type, godot::StringName p_name) {
        return {
            godot::PropertyInfo(p_type, p_name),
            godot::String{"set_"} + p_name,
            godot::String{"get_"} + p_name,
        };
    }
};



template <typename... Ts>
struct C_Descriptor {
    using TupleType = std::tuple<Ts...>;

    static constexpr std::size_t FieldCount = sizeof...(Ts);

    std::array<C_Field, FieldCount> fields{};
    TupleType defaults{};
};



template <typename T>
concept gd_ecs_has_component_descriptor =
requires(T t) {
    typename T::Descriptor;
    { T::descriptor() } -> std::same_as<const typename T::Descriptor&>;
    { T::from_tuple(std::declval<const typename T::Descriptor::TupleType&>()) } -> std::same_as<T>;
    { t.to_tuple() } -> std::same_as<typename T::Descriptor::TupleType>;
}
&& requires { typename std::tuple_size<typename T::Descriptor::TupleType>; };



template <typename TupleType, typename DescriptorType, std::size_t... I>
godot::Dictionary gd_ecs_tuple_to_dict_impl(
    const TupleType &p_tuple,
    const DescriptorType &p_descriptor,
    std::index_sequence<I...>
) {
    godot::Dictionary dictionary{};

    [[maybe_unused]] auto result = std::initializer_list<int>{
        (dictionary[p_descriptor.fields[I].property_info.name] =
            godot::Variant{std::get<I>(p_tuple)}, 0)...
    };

    return dictionary;
}



template <typename... Ts>
godot::Variant gd_ecs_tuple_to_variant(
    const std::tuple<Ts...> &p_tuple,
    const C_Descriptor<Ts...> &p_descriptor
) {
    if constexpr (sizeof...(Ts) == 0) {
        return godot::Variant{};
    }
    else if constexpr (sizeof...(Ts) == 1) {
        return godot::Variant{std::get<0>(p_tuple)};
    }
    else {
        return gd_ecs_tuple_to_dict_impl(p_tuple, p_descriptor, std::index_sequence_for<Ts...>{});
    }
}



template <typename T>
T gd_ecs_variant_cast(const godot::Variant &p_variant) {
    return p_variant.operator T();
}



template <typename TupleType, typename DescriptorType, std::size_t... I>
TupleType gd_ecs_dict_to_tuple_impl(
    const godot::Dictionary &p_dictionary,
    const DescriptorType &p_descriptor,
    std::index_sequence<I...>
) {
    return TupleType{
        gd_ecs_variant_cast<std::tuple_element_t<I, TupleType>>(
            p_dictionary.has(p_descriptor.fields[I].property_info.name)
                ? p_dictionary[p_descriptor.fields[I].property_info.name]
                : godot::Variant{}
        )...
    };
}



template <typename... Ts>
std::tuple<Ts...> gd_ecs_variant_to_tuple(
    const godot::Variant &p_variant,
    const C_Descriptor<Ts...> &p_descriptor
) {
    using TupleType = std::tuple<Ts...>;

    if constexpr (sizeof...(Ts) == 0) {
        return {};
    }
    else if constexpr (sizeof...(Ts) == 1) {
        return TupleType{p_variant.operator Ts()...};
    }
    else {
        const godot::Dictionary dictionary = p_variant;
        return gd_ecs_dict_to_tuple_impl<TupleType>(
            dictionary,
            p_descriptor,
            std::index_sequence_for<Ts...>{}
        );
    }
}



template <typename T, typename RegistryType, typename EntityType, typename TupleType>
requires std::is_empty_v<T>
void gd_ecs_emplace_or_replace_tuple(
    RegistryType &p_registry,
    const EntityType &p_entity,
    const TupleType&
) {
    p_registry.template emplace_or_replace<T>(p_entity);
}



template <typename T, typename RegistryType, typename EntityType, typename TupleType>
requires (!std::is_empty_v<T>)
         && gd_ecs_has_component_descriptor<T>
void gd_ecs_emplace_or_replace_tuple(
    RegistryType &p_registry,
    const EntityType &p_entity,
    const TupleType &p_data
) {
    p_registry.template emplace_or_replace<T>(p_entity, T::from_tuple(p_data));
}

} // namespace godot



//==================================================================================================
// GD_ECS_EMPTY_COMPONENT_DESCRIPTOR
//==================================================================================================

#define GD_ECS_EMPTY_COMPONENT_DESCRIPTOR(ECS_COMPONENT_NAME)                                      \
    using Descriptor = godot::C_Descriptor<>;                                                      \
                                                                                                   \
    static const Descriptor& descriptor() {                                                        \
        static Descriptor descriptor{};                                                            \
        return descriptor;                                                                         \
    }                                                                                              \
                                                                                                   \
    static ECS_COMPONENT_NAME from_tuple(const Descriptor::TupleType&) {                           \
        return {};                                                                                 \
    }                                                                                              \
                                                                                                   \
    Descriptor::TupleType to_tuple() const {                                                       \
        return {};                                                                                 \
    }



//==================================================================================================
// GD_ECS_COMPONENT_WITH_PARENT_EMPLACE_OR_REPLACE
//==================================================================================================

#define GD_ECS_COMPONENT_WITH_PARENT_EMPLACE_OR_REPLACE(                                           \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    GD_ECS_COMPONENT_PARENT_TYPE,                                                                  \
    ECS_COMPONENT_NAME,                                                                            \
    EMPLACE_OR_REPLACE_BODY                                                                        \
)                                                                                                  \
class GD_ECS_COMPONENT_NAME : public GD_ECS_COMPONENT_PARENT_TYPE {                                \
    GDCLASS(GD_ECS_COMPONENT_NAME, GD_ECS_COMPONENT_PARENT_TYPE)                                   \
                                                                                                   \
    static_assert(                                                                                 \
        godot::gd_ecs_has_component_descriptor<ECS_COMPONENT_NAME>,                                \
        "Concept violation summary:\n"                                                             \
        #ECS_COMPONENT_NAME " is not a valid gd_ecs_has_component_descriptor component type.\n"    \
        "\n"                                                                                       \
        "Expected interface:\n"                                                                    \
        "\n"                                                                                       \
        "struct " #ECS_COMPONENT_NAME " {\n"                                                       \
        "    using Descriptor = std::tuple<...>;\n"                                                \
        "    static const Descriptor& descriptor();\n"                                             \
        "    static " #ECS_COMPONENT_NAME " from_tuple(const Descriptor::TupleType& p_data);\n"    \
        "    Descriptor::TupleType to_tuple() const;\n"                                            \
        "};"                                                                                       \
    );                                                                                             \
                                                                                                   \
                                                                                                   \
                                                                                                   \
public:                                                                                            \
    using ComponentType = ECS_COMPONENT_NAME;                                                      \
    using DescriptorType = ComponentType::Descriptor;                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    DescriptorType::TupleType data{ComponentType::descriptor().defaults};                          \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    static void register_types() {                                                                 \
        GDREGISTER_RUNTIME_CLASS(GD_ECS_COMPONENT_NAME);                                           \
        GD_ECS_SINGLETON_TYPE::register_type<ECS_COMPONENT_NAME>(#ECS_COMPONENT_NAME);             \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    template <std::size_t I, typename T>                                                           \
    void set(const T& p_value) {                                                                   \
        std::get<I>(data) = p_value;                                                               \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    template <std::size_t I, typename T>                                                           \
    T get() const {                                                                                \
        return std::get<I>(data);                                                                  \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
    virtual void emplace_or_replace(                                                               \
        [[maybe_unused]] godot::Node &p_entity_node,                                               \
        [[maybe_unused]] GD_ECS_SINGLETON_TYPE::RegistryType::entity_type &p_entity                \
    ) override {                                                                                   \
        EMPLACE_OR_REPLACE_BODY                                                                    \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
protected:                                                                                         \
    static void _bind_methods() {                                                                  \
        bind_all(std::make_index_sequence<std::tuple_size_v<DescriptorType::TupleType>>{});        \
    }                                                                                              \
                                                                                                   \
                                                                                                   \
                                                                                                   \
private:                                                                                           \
    template <std::size_t I>                                                                       \
    static void bind_field() {                                                                     \
        using FieldType = std::tuple_element_t<I, DescriptorType::TupleType>;                      \
                                                                                                   \
        auto &descriptor = ECS_COMPONENT_NAME::descriptor();                                       \
        auto &field = descriptor.fields[I];                                                        \
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
    template <std::size_t... I>                                                                    \
    static void bind_all(std::index_sequence<I...>) {                                              \
        (bind_field<I>(), ...);                                                                    \
    }                                                                                              \
                                                                                                   \
};



//==================================================================================================
// GD_ECS_COMPONENT_EMPLACE_OR_REPLACE
//==================================================================================================

#define GD_ECS_COMPONENT_EMPLACE_OR_REPLACE(                                                       \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    ECS_COMPONENT_NAME,                                                                            \
    EMPLACE_OR_REPLACE_BODY                                                                        \
)                                                                                                  \
GD_ECS_COMPONENT_WITH_PARENT_EMPLACE_OR_REPLACE(                                                   \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    GD_ECS_SINGLETON_TYPE::ComponentType,                                                          \
    ECS_COMPONENT_NAME,                                                                            \
    EMPLACE_OR_REPLACE_BODY                                                                        \
)                                                                                                  \



//==================================================================================================
// GD_ECS_COMPONENT_WITH_PARENT
//==================================================================================================

#define GD_ECS_COMPONENT_WITH_PARENT(                                                              \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    GD_ECS_COMPONENT_PARENT_TYPE,                                                                  \
    ECS_COMPONENT_NAME                                                                             \
)                                                                                                  \
GD_ECS_COMPONENT_WITH_PARENT_EMPLACE_OR_REPLACE(                                                   \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    GD_ECS_COMPONENT_PARENT_TYPE,                                                                  \
    ECS_COMPONENT_NAME,                                                                            \
    {                                                                                              \
        auto &reg = GD_ECS_SINGLETON_TYPE::registry();                                             \
        godot::gd_ecs_emplace_or_replace_tuple<ECS_COMPONENT_NAME>(reg, p_entity, data);           \
    }                                                                                              \
)                                                                                                  \



//==================================================================================================
// GD_ECS_COMPONENT
//==================================================================================================

#define GD_ECS_COMPONENT(GD_ECS_SINGLETON_TYPE, GD_ECS_COMPONENT_NAME, ECS_COMPONENT_NAME)         \
GD_ECS_COMPONENT_WITH_PARENT(                                                                      \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    GD_ECS_SINGLETON_TYPE::ComponentType,                                                          \
    ECS_COMPONENT_NAME                                                                             \
)
