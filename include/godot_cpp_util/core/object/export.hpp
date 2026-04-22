/**
 * GD_EXPORT is a helper macro that simplifies Godot property export bindings.
 *
 * Its main purpose is to reduce repetitive boilerplate when exposing C++ class members to Godot.
 * Instead of manually writing PropertyInfo declarations, getter and setter bindings, and
 * registration code for each property, GD_EXPORT centralizes all export definitions in a single
 * structured block.
 *
 * Usage Example:
 *
 * #pragma once
 *
 * #include "godot_cpp_util/core/object/export.hpp"
 *
 * using namespace godot;
 *
 * class Data : public godot::Object {
 *     GDCLASS(Data, godot::Object)
 *
 * private:
 *     int id{21};
 *     float length{21.21f};
 *     godot::String name{"SomeName"};
 *     godot::Dictionary meta{};
 *
 * public:
 *     // These explicit getter and setter functions may look like extra boilerplate.
 *     // In most cases they are not required and exist here mainly to demonstrate how
 *     // '.with_set(...)' and '.with_get(...)' can override the default member access behavior.
 *     // Normally GD_EXPORT uses direct member access automatically when no overrides are provided.
 *
 *     // Explicit setter for the 'id' field.
 *     void set_id(int value) {
 *         print_line("set value: ", value);
 *         id = value;
 *     }
 *
 *     // Explicit getter for the 'id' field.
 *     int get_id() const {
 *         print_line("get value: ", id);
 *         return id;
 *     }
 *
 *     // Setter for 'name' using const reference semantics.
 *     void set_name(const String& ref) {
 *         print_line("set ref: ", ref);
 *         name = ref;
 *     }
 *
 *     // Getter for 'name' returning a const reference.
 *     const String& get_name() const {
 *         print_line("get ref: ", name);
 *         return name;
 *     }
 *
 * protected:
 *     // GD_EXPORT declares all exported fields for this class.
 *     //
 *     // IMPORTANT:
 *     // GD_EXPORT must appear after all exported members and all setter and getter functions have
 *     // been declared. The compiler must see everything referenced by the export declarations
 *     // before this macro is instantiated.
 *     //
 *     // Each field can optionally override its setter or getter using '.with_set(...)' and
 *     // '.with_get(...)'.
 *     //
 *     // If a setter or getter is not overridden, the default behavior is used:
 *     // - The setter assigns directly to the member.
 *     // - The getter returns the member value.
 *     //
 *     // When overriding, the user is responsible for performing the assignment or returning the
 *     // value explicitly.
 *     GD_EXPORT(Data,
 *         // Field with explicit PropertyInfo and setter and getter.
 *         ExportByValue{&Data::id, PropertyInfo(Variant::Type::INT, "id"), "set_id", "get_id"}
 *             .with_set(&Data::set_id)
 *             .with_get(&Data::get_id),
 *
 *         // Field using a simplified constructor with Variant type.
 *         ExportByValue{&Data::length, Variant::Type::FLOAT, "length", "set_length", "get_length"},
 *
 *         // Field with:
 *         // - PropertyInfo
 *         // - "set_name"
 *         // - "get_name"
 *         ExportByRef{&Data::name, PropertyInfo(Variant::Type::STRING, "name")}
 *             .with_set(&Data::set_name)
 *             .with_get(&Data::get_name),
 *
 *         // Field with:
 *         // - PropertyInfo(Variant::Type::DICTIONARY, "meta")
 *         // - "set_meta"
 *         // - "get_meta"
 *         ExportByRef{&Data::meta, Variant::Type::DICTIONARY, "meta"}
 *     )
 *
 *     // Binds all exported fields declared in GD_EXPORT.
 *     // No manual property binding is required.
 *     static void _bind_methods() {
 *         bind_all_export_fields();
 *     }
 *
 * };
 *
 * // Type registration
 * inline void register_types() {
 *     GDREGISTER_RUNTIME_CLASS(Data);
 * }
 */



#pragma once



#include <concepts>
#include <cstddef>
#include <iostream>
#include <tuple>
#include <utility>

#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/property_info.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/string_name.hpp"
#include "godot_cpp/variant/variant.hpp"



namespace godot {



//==================================================================================================
// ExportField
//==================================================================================================

struct ExportByValueMarker {};
struct ExportByRefMarker {};

template<typename T, typename ValuePolicy>
struct ExportType;

template<typename T>
struct ExportType<T, ExportByValueMarker> {
    using Type = T;
};

template<typename T>
struct ExportType<T, ExportByRefMarker> {
    using Type = const T&;
};



template<typename StructT, typename FieldT, typename ValuePolicy>
struct ExportField final {
    using StructType = StructT;
    using FieldType = FieldT;

    using ExposedType = typename ExportType<FieldType, ValuePolicy>::Type;

    using MemberPtr = FieldType StructType::*;
    using SetFn = void (StructType::*)(ExposedType);
    using GetFn = ExposedType (StructType::*)() const;



    MemberPtr member = nullptr;
    godot::PropertyInfo property_info{};
    godot::StringName set_fn{};
    godot::StringName get_fn{};
    SetFn set_fn_impl = nullptr;
    GetFn get_fn_impl = nullptr;



    ExportField(
        MemberPtr p_member,
        const godot::PropertyInfo &p_property_info,
        const godot::StringName &p_set_fn,
        const godot::StringName &p_get_fn
    )
        : member(p_member)
        , property_info(p_property_info)
        , set_fn(p_set_fn)
        , get_fn(p_get_fn)
    {}



    ExportField(
        MemberPtr p_member,
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



    ExportField(MemberPtr p_member, const godot::PropertyInfo &p_property_info)
        : member(p_member)
        , property_info(p_property_info)
        , set_fn(godot::String{"set_"} + p_property_info.name)
        , get_fn(godot::String{"get_"} + p_property_info.name)
    {}



    ExportField(
        MemberPtr p_member,
        godot::Variant::Type p_type,
        const godot::StringName &p_name
    )
        : member(p_member)
        , property_info(godot::PropertyInfo(p_type, p_name))
        , set_fn(godot::String{"set_"} + p_name)
        , get_fn(godot::String{"get_"} + p_name)
    {}



    ExportField& with_set(SetFn p_set_fn) {
        set_fn_impl = p_set_fn;
        return *this;
    }



    ExportField& with_get(GetFn p_get_fn) {
        get_fn_impl = p_get_fn;
        return *this;
    }

};



template<typename StructType, typename FieldType>
using ExportByValue = ExportField<StructType, FieldType, ExportByValueMarker>;



template<typename StructType, typename FieldType>
using ExportByRef = ExportField<StructType, FieldType, ExportByRefMarker>;



//==================================================================================================
// ExportDescriptor
//==================================================================================================

template <typename StructT, typename ...Fields>
struct ExportDescriptor final {
    using StructType = StructT;
    using FieldTypeTuple = std::tuple<Fields...>;



    static_assert(
        (std::is_same_v<typename Fields::StructType, StructType> && ...),
        "All ExportField types must use the same StructType as ExportDescriptor."
    );



    godot::StringName name{};
    FieldTypeTuple fields{};



    ExportDescriptor(const godot::StringName &p_name, const Fields &...p_fields)
        : name(p_name)
        , fields(p_fields...)
    {}



    template <std::size_t I>
    void set(
        StructType &p_instance,
        std::tuple_element<I, FieldTypeTuple>::type::ExposedType p_value
    ) const {
        auto &field = std::get<I>(fields);

        if (field.set_fn_impl) {
            (p_instance.*(field.set_fn_impl))(p_value);
            return;
        }
        else if (field.member) {
            p_instance.*(field.member) = p_value;
            return;
        }

        ERR_PRINT(godot::vformat(
            "ExportDescriptor.set(): Invalid ExportField configuration%s. "
            "Property '%s' cannot be written because it has neither a setter nor a writable member "
            "pointer. Value provided: %s",
            name.is_empty() ? "" : godot::vformat(" (%s)", name),
            field.property_info.name,
            p_value
        ));
    }



    template <std::size_t I>
    std::tuple_element<I, FieldTypeTuple>::type::ExposedType get(const StructType &p_instance) const
    {
        auto &field = std::get<I>(fields);

        if (field.get_fn_impl) {
            return (p_instance.*(field.get_fn_impl))();
        }
        else if (field.member) {
            return p_instance.*(field.member);
        }

        CRASH_NOW_MSG(godot::vformat(
            "ExportDescriptor.get(): Failed to read property '%s'%s. "
            "No getter function and no member pointer are available. "
            "This ExportField configuration is invalid.",
            field.property_info.name,
            name.is_empty() ? "" : godot::vformat(" (%s)", name)
        ));

        // Fallback crash, just to make this sure.
        std::cerr
            << "Fatal error in ExportDescriptor.get(). "
            << "Reached unreachable fallback path while reading property. "
            << "ExportField state is invalid."
            << std::endl;
        std::abort();
    }



    void set_variant(StructType &p_instance, const godot::Variant &p_variant) const {
        if constexpr (std::tuple_size_v<FieldTypeTuple> == 1) {
            using FieldType = std::tuple_element_t<0, FieldTypeTuple>::FieldType;
            FieldType value = p_variant;
            set<0>(p_instance, value);
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



    godot::Variant to_variant(const StructType &p_instance) const {
        if constexpr (std::tuple_size_v<FieldTypeTuple> == 1) {
            return godot::Variant{get<0>(p_instance)};
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
    template <std::size_t I>
    void set_field_from_dictionary_if_present(
        StructType &p_instance,
        const godot::Dictionary &p_dictionary
    ) const {
        auto &field = std::get<I>(fields);

        if (p_dictionary.has(field.property_info.name)) {
            using FieldType = std::tuple_element_t<I, FieldTypeTuple>::FieldType;
            FieldType value = p_dictionary[field.property_info.name];
            set<I>(p_instance, value);
        }
    }



    template <std::size_t ...Is>
    void set_from_dictionary(
        StructType &p_instance,
        const godot::Dictionary &p_dictionary,
        std::index_sequence<Is...>
    ) const {
        (set_field_from_dictionary_if_present<Is>(p_instance, p_dictionary), ...);
    }



    template <std::size_t I>
    void try_insert_field_to_dictionary(
        const StructType &p_instance,
        godot::Dictionary &p_dictionary
    ) const {
        auto &field = std::get<I>(fields);
        p_dictionary[field.property_info.name] = godot::Variant{get<I>(p_instance)};
    }



    template <std::size_t ...Is>
    godot::Dictionary try_to_dictionary(
        const StructType &p_instance,
        std::index_sequence<Is...>
    ) const {
        godot::Dictionary dictionary;

        (try_insert_field_to_dictionary<Is>(p_instance, dictionary), ...);

        return dictionary;
    }

};



template<typename FirstField, typename... RestFields>
ExportDescriptor(const godot::StringName&, const FirstField&, const RestFields&...)
    -> ExportDescriptor<typename FirstField::StructType, FirstField, RestFields...>;



template <typename StructType>
struct ExportDescriptorOfType final {

    template <typename... Fields>
    static auto make(const godot::StringName &p_name, const Fields &...p_fields) {
        return ExportDescriptor<StructType, Fields...>{p_name, p_fields...};
    }

};



} // namespace godot



//==================================================================================================
// GD_EXPORT
//==================================================================================================

#define GD_EXPORT(CLASS_TYPE, ...)                                                                 \
using ExportDescriptorType = std::remove_reference_t<decltype(                                     \
    godot::ExportDescriptorOfType<CLASS_TYPE>::make(                                               \
        CLASS_TYPE::get_class_static()                                                             \
        __VA_OPT__(,) __VA_ARGS__                                                                  \
    )                                                                                              \
)>;                                                                                                \
                                                                                                   \
                                                                                                   \
                                                                                                   \
static const ExportDescriptorType& export_descriptor() {                                           \
    static const ExportDescriptorType descriptor{                                                  \
        CLASS_TYPE::get_class_static()                                                             \
        __VA_OPT__(,) __VA_ARGS__                                                                  \
    };                                                                                             \
                                                                                                   \
    return descriptor;                                                                             \
}                                                                                                  \
                                                                                                   \
                                                                                                   \
                                                                                                   \
template <std::size_t I, typename T>                                                               \
void set_export_field(T p_value) {                                                                 \
    auto &descriptor = CLASS_TYPE::export_descriptor();                                            \
    descriptor.set<I>(*this, p_value);                                                             \
}                                                                                                  \
                                                                                                   \
                                                                                                   \
                                                                                                   \
template <std::size_t I, typename T>                                                               \
T get_export_field() const {                                                                       \
    auto &descriptor = CLASS_TYPE::export_descriptor();                                            \
    return descriptor.get<I>(*this);                                                               \
}                                                                                                  \
                                                                                                   \
                                                                                                   \
                                                                                                   \
protected:                                                                                         \
template <std::size_t I>                                                                           \
static void bind_export_field() {                                                                  \
    using ExposedType =                                                                            \
        std::tuple_element_t<I, typename ExportDescriptorType::FieldTypeTuple>::ExposedType;       \
                                                                                                   \
    auto &descriptor = CLASS_TYPE::export_descriptor();                                            \
    auto &field = std::get<I>(descriptor.fields);                                                  \
                                                                                                   \
    godot::ClassDB::bind_method(                                                                   \
        godot::D_METHOD(field.set_fn, "p_value"),                                                  \
        &CLASS_TYPE::set_export_field<I, ExposedType>                                              \
    );                                                                                             \
    godot::ClassDB::bind_method(                                                                   \
        godot::D_METHOD(field.get_fn),                                                             \
        &CLASS_TYPE::get_export_field<I, ExposedType>                                              \
    );                                                                                             \
                                                                                                   \
    ADD_PROPERTY(field.property_info, field.set_fn, field.get_fn);                                 \
}                                                                                                  \
                                                                                                   \
                                                                                                   \
                                                                                                   \
template <std::size_t ...I>                                                                        \
static void bind_all_export_fields_is(std::index_sequence<I...>) {                                 \
    (bind_export_field<I>(), ...);                                                                 \
}                                                                                                  \
                                                                                                   \
                                                                                                   \
                                                                                                   \
static void bind_all_export_fields() {                                                             \
    bind_all_export_fields_is(                                                                     \
        std::make_index_sequence<                                                                  \
            std::tuple_size_v<typename ExportDescriptorType::FieldTypeTuple>                       \
        >{}                                                                                        \
    );                                                                                             \
}                                                                                                  \
                                                                                                   \
                                                                                                   \
                                                                                                   \
private:
