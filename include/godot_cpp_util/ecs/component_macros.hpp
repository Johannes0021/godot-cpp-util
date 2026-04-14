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



//==================================================================================================
// GD_ECS_COMPONENT_VARIANT_EXPORT
//==================================================================================================

/**
 * using ECSType = godot::ECS;
 *
 * struct Point {
 *     godot::Vector2 pos{};
 * };
 *
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
 *     virtual void emplace_or_replace(
 *         [[maybe_unused]] godot::Node &p_entity_node,
 *         ECSType::RegistryType::entity_type &p_entity
 *     ) override {
 *         auto &ecs = ECSType::get_instance();
 *         auto &reg = ecs.get_registry();
 *         reg.emplace_or_replace<Point>(p_entity, m_pos);
 *     }
 *
 *     void set_pos(const godot::Vector2 pos) { m_pos = pos; }
 *     godot::Vector2 get_pos() const { return m_pos; }
 *
 * protected:
 *     static void _bind_methods() {
 *         GD_ECS_COMPONENT_VARIANT_EXPORT(
 *             C_Point,
 *             godot::PropertyInfo(godot::Variant::Type::VECTOR2, "pos"),
 *             pos,
 *             set_pos,
 *             get_pos
 *         );
 *     }
 *
 * };
 *
 * //// Do not forget to expose the new component to Godot:
 * //ECSType::register_types();
 * //// ...
 * //C_Point::register_types();
 */
#define GD_ECS_COMPONENT_VARIANT_EXPORT(                                                           \
    GD_ECS_COMPONENT_NAME,                                                                         \
    GD_PROPERTY_INFO,                                                                              \
    VALUE_NAME,                                                                                    \
    SET_FN,                                                                                        \
    GET_FN                                                                                         \
)                                                                                                  \
godot::ClassDB::bind_method(                                                                       \
    godot::D_METHOD(#SET_FN, "p_" #VALUE_NAME),                                                    \
    &GD_ECS_COMPONENT_NAME::SET_FN                                                                 \
);                                                                                                 \
godot::ClassDB::bind_method(godot::D_METHOD(#GET_FN), &GD_ECS_COMPONENT_NAME::GET_FN);             \
ADD_PROPERTY(GD_PROPERTY_INFO, #SET_FN, #GET_FN);



//==================================================================================================
// Helpers
//==================================================================================================

#define _HELPER_GD_ECS_COMPONENT_FIELD_DECLARE(GD_PROPERTY_INFO, VALUE_TYPE, VALUE_NAME, ...)      \
VALUE_TYPE VALUE_NAME{__VA_ARGS__};



#define _HELPER_GD_ECS_COMPONENT_EXTRACT_VALUE_NAME_COMMA(                                         \
    GD_PROPERTY_INFO,                                                                              \
    VALUE_TYPE,                                                                                    \
    VALUE_NAME,                                                                                    \
    ...                                                                                            \
)                                                                                                  \
VALUE_NAME,



#define _HELPER_GD_ECS_COMPONENT_FIELD_SET_GET(GD_PROPERTY_INFO, VALUE_TYPE, VALUE_NAME, ...)      \
void set_##VALUE_NAME(const VALUE_TYPE &p_##VALUE_NAME) {                                          \
    VALUE_NAME = p_##VALUE_NAME;                                                                   \
}                                                                                                  \
                                                                                                   \
VALUE_TYPE get_##VALUE_NAME() const {                                                              \
    return VALUE_NAME;                                                                             \
}



#define _HELPER_GD_ECS_COMPONENT_FIELD_BIND(GD_PROPERTY_INFO, VALUE_TYPE, VALUE_NAME, ...)         \
GD_ECS_COMPONENT_VARIANT_EXPORT(                                                                   \
    _HELPER_GD_ECS_COMPONENT_FIELD_BIND_GDComponentName,                                           \
    GD_PROPERTY_INFO,                                                                              \
    VALUE_NAME,                                                                                    \
    set_##VALUE_NAME,                                                                              \
    get_##VALUE_NAME                                                                               \
)



//==================================================================================================
// GD_ECS_EMPTY_COMPONENT_WITH_PARENT_EMPLACE_OR_REPLACE
//==================================================================================================

/**
 * The parent class must expose a public 'Signal' struct when signal generation is not disabled.
 *
 * using ECSType = godot::ECS;
 *
 * struct Marker {};
 *
 * GD_ECS_EMPTY_COMPONENT_WITH_PARENT_EMPLACE_OR_REPLACE(
 *     ECSType,
 *     C_Marker, ECSType::ComponentType,
 *     // void emplace_or_replace(Node &p_entity_node, EntityType &p_entity)
 *     {
 *         auto &ecs = ECSType::get_instance();
 *         auto &reg = ecs.get_registry();
 *         reg.emplace_or_replace<Marker>(p_entity);
 *         // Alternatively, the Godot component itself can be stored in the registry, for example
 *         // wrapped in godot::Ref.
 *         //reg.emplace_or_replace<godot::Ref<C_Marker>>(p_entity, this);
 *     }
 * )
 *
 * //// Do not forget to expose the new component to Godot:
 * //ECSType::register_types();
 * //// ...
 * //C_Marker::register_types();
 */
#define GD_ECS_EMPTY_COMPONENT_WITH_PARENT_EMPLACE_OR_REPLACE(                                     \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    GD_ECS_COMPONENT_PARENT_TYPE,                                                                  \
    EMPLACE_OR_REPLACE_BODY                                                                        \
)                                                                                                  \
class GD_ECS_COMPONENT_NAME : public GD_ECS_COMPONENT_PARENT_TYPE {                                \
    GDCLASS(GD_ECS_COMPONENT_NAME, GD_ECS_COMPONENT_PARENT_TYPE)                                   \
                                                                                                   \
public:                                                                                            \
    GD_ECS_EMPTY_SIGNAL_STRUCT(Signal, GD_ECS_COMPONENT_PARENT_TYPE::Signal)                       \
                                                                                                   \
public:                                                                                            \
    static void register_types() {                                                                 \
        GDREGISTER_RUNTIME_CLASS(GD_ECS_COMPONENT_NAME);                                           \
    }                                                                                              \
                                                                                                   \
    virtual void emplace_or_replace(                                                               \
        [[maybe_unused]] godot::Node &p_entity_node,                                               \
        [[maybe_unused]] GD_ECS_SINGLETON_TYPE::RegistryType::entity_type &p_entity                \
    ) override {                                                                                   \
        EMPLACE_OR_REPLACE_BODY                                                                    \
    }                                                                                              \
                                                                                                   \
protected:                                                                                         \
    static void _bind_methods() {}                                                                 \
                                                                                                   \
};



//==================================================================================================
// GD_ECS_EMPTY_COMPONENT_EMPLACE_OR_REPLACE
//==================================================================================================

/**
 * using ECSType = godot::ECS;
 *
 * struct Marker {};
 *
 * GD_ECS_EMPTY_COMPONENT_EMPLACE_OR_REPLACE(
 *     ECSType,
 *     C_Marker,
 *     // void emplace_or_replace(Node &p_entity_node, EntityType &p_entity)
 *     {
 *         auto &ecs = ECSType::get_instance();
 *         auto &reg = ecs.get_registry();
 *         reg.emplace_or_replace<Marker>(p_entity);
 *         // Alternatively, the Godot component itself can be stored in the registry, for example
 *         // wrapped in godot::Ref.
 *         //reg.emplace_or_replace<godot::Ref<C_Marker>>(p_entity, this);
 *     }
 * )
 *
 * //// Do not forget to expose the new component to Godot:
 * //ECSType::register_types();
 * //// ...
 * //C_Marker::register_types();
 */
#define GD_ECS_EMPTY_COMPONENT_EMPLACE_OR_REPLACE(                                                 \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    EMPLACE_OR_REPLACE_BODY                                                                        \
)                                                                                                  \
GD_ECS_EMPTY_COMPONENT_WITH_PARENT_EMPLACE_OR_REPLACE(                                             \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    GD_ECS_SINGLETON_TYPE::ComponentType,                                                          \
    EMPLACE_OR_REPLACE_BODY                                                                        \
)



//==================================================================================================
// GD_ECS_EMPTY_COMPONENT_WITH_PARENT
//==================================================================================================

/**
 * The parent class must expose a public 'Signal' struct when signal generation is not disabled.
 *
 * using ECSType = godot::ECS;
 *
 * struct Marker {};
 *
 * GD_ECS_EMPTY_COMPONENT_WITH_PARENT(
 *     ECSType,
 *     C_Marker, ECSType::ComponentType,
 *     Marker // Do not use godot::Ref<C_Marker> here.
 * )
 *
 * //// Do not forget to expose the new component to Godot:
 * //ECSType::register_types();
 * //// ...
 * //C_Marker::register_types();
 */
#define GD_ECS_EMPTY_COMPONENT_WITH_PARENT(                                                        \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    GD_ECS_COMPONENT_PARENT_TYPE,                                                                  \
    ECS_COMPONENT_NAME                                                                             \
)                                                                                                  \
GD_ECS_EMPTY_COMPONENT_WITH_PARENT_EMPLACE_OR_REPLACE(                                             \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    GD_ECS_COMPONENT_PARENT_TYPE,                                                                  \
    {                                                                                              \
        auto &ecs = GD_ECS_SINGLETON_TYPE::get_instance();                                         \
        auto &reg = ecs.get_registry();                                                            \
        reg.emplace_or_replace<ECS_COMPONENT_NAME>(p_entity);                                      \
    }                                                                                              \
)



//==================================================================================================
// GD_ECS_EMPTY_COMPONENT
//==================================================================================================

/**
 * using ECSType = godot::ECS;
 *
 * struct Marker {};
 *
 * GD_ECS_EMPTY_COMPONENT(
 *     ECSType,
 *     C_Marker,
 *     Marker // Do not use godot::Ref<C_Marker> here.
 * )
 *
 * //// Do not forget to expose the new component to Godot:
 * //ECSType::register_types();
 * //// ...
 * //C_Marker::register_types();
 */
#define GD_ECS_EMPTY_COMPONENT(GD_ECS_SINGLETON_TYPE, GD_ECS_COMPONENT_NAME, ECS_COMPONENT_NAME)   \
GD_ECS_EMPTY_COMPONENT_WITH_PARENT(                                                                \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    GD_ECS_SINGLETON_TYPE::ComponentType,                                                          \
    ECS_COMPONENT_NAME                                                                             \
)



//==================================================================================================
// GD_ECS_COMPONENT_WITH_PARENT_EMPLACE_OR_REPLACE
//==================================================================================================

/**
 * The parent class must expose a public 'Signal' struct when signal generation is not disabled.
 *
 * using ECSType = godot::ECS;
 *
 * struct Single {
 *     bool x{};
 * };
 *
 * GD_ECS_COMPONENT_WITH_PARENT_EMPLACE_OR_REPLACE(
 *     ECSType,
 *     C_Single, ECSType::ComponentType,
 *     // void emplace_or_replace(Node &p_entity_node, EntityType &p_entity)
 *     {
 *         auto &ecs = ECSType::get_instance();
 *         auto &reg = ecs.get_registry();
 *         reg.emplace_or_replace<Single>(p_entity, x);
 *         // Alternatively, the Godot component itself can be stored in the registry, for example
 *         // wrapped in godot::Ref.
 *         //reg.emplace_or_replace<godot::Ref<C_Single>>(p_entity, this);
 *     },
 *     godot::PropertyInfo(godot::Variant::Type::BOOL, "x"), bool, x //, true // Optional value.
 * )
 *
 * //// Do not forget to expose the new component to Godot:
 * //ECSType::register_types();
 * //// ...
 * //C_Single::register_types();
 */
#define GD_ECS_COMPONENT_WITH_PARENT_EMPLACE_OR_REPLACE(                                           \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    GD_ECS_COMPONENT_PARENT_TYPE,                                                                  \
    EMPLACE_OR_REPLACE_BODY,                                                                       \
    GD_PROPERTY_INFO,                                                                              \
    VALUE_TYPE,                                                                                    \
    VALUE_NAME,                                                                                    \
    ...                                                                                            \
)                                                                                                  \
class GD_ECS_COMPONENT_NAME : public GD_ECS_COMPONENT_PARENT_TYPE {                                \
    GDCLASS(GD_ECS_COMPONENT_NAME, GD_ECS_COMPONENT_PARENT_TYPE)                                   \
                                                                                                   \
public:                                                                                            \
    GD_ECS_EMPTY_SIGNAL_STRUCT(Signal, GD_ECS_COMPONENT_PARENT_TYPE::Signal)                       \
                                                                                                   \
public:                                                                                            \
    VALUE_TYPE VALUE_NAME{__VA_ARGS__};                                                            \
                                                                                                   \
public:                                                                                            \
    static void register_types() {                                                                 \
        GDREGISTER_RUNTIME_CLASS(GD_ECS_COMPONENT_NAME);                                           \
    }                                                                                              \
                                                                                                   \
    virtual void emplace_or_replace(                                                               \
        [[maybe_unused]] godot::Node &p_entity_node,                                               \
        [[maybe_unused]] GD_ECS_SINGLETON_TYPE::RegistryType::entity_type &p_entity                \
    ) override {                                                                                   \
        EMPLACE_OR_REPLACE_BODY                                                                    \
    }                                                                                              \
                                                                                                   \
    _HELPER_GD_ECS_COMPONENT_FIELD_SET_GET(GD_PROPERTY_INFO, VALUE_TYPE, VALUE_NAME, __VA_ARGS__)  \
                                                                                                   \
protected:                                                                                         \
    static void _bind_methods() {                                                                  \
        using _HELPER_GD_ECS_COMPONENT_FIELD_BIND_GDComponentName = GD_ECS_COMPONENT_NAME;         \
        _HELPER_GD_ECS_COMPONENT_FIELD_BIND(GD_PROPERTY_INFO, VALUE_TYPE, VALUE_NAME, __VA_ARGS__) \
    }                                                                                              \
                                                                                                   \
};



//==================================================================================================
// GD_ECS_COMPONENT_EMPLACE_OR_REPLACE
//==================================================================================================

/**
 * using ECSType = godot::ECS;
 *
 * struct Single {
 *     bool x{};
 * };
 *
 * GD_ECS_COMPONENT_EMPLACE_OR_REPLACE(
 *     ECSType,
 *     C_Single,
 *     // void emplace_or_replace(Node &p_entity_node, EntityType &p_entity)
 *     {
 *         auto &ecs = ECSType::get_instance();
 *         auto &reg = ecs.get_registry();
 *         reg.emplace_or_replace<Single>(p_entity, x);
 *         // Alternatively, the Godot component itself can be stored in the registry, for example
 *         // wrapped in godot::Ref.
 *         //reg.emplace_or_replace<godot::Ref<C_Single>>(p_entity, this);
 *     },
 *     godot::PropertyInfo(godot::Variant::Type::BOOL, "x"), bool, x //, true // Optional value.
 * )
 *
 * //// Do not forget to expose the new component to Godot:
 * //ECSType::register_types();
 * //// ...
 * //C_Single::register_types();
 */
#define GD_ECS_COMPONENT_EMPLACE_OR_REPLACE(                                                       \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    EMPLACE_OR_REPLACE_BODY,                                                                       \
    GD_PROPERTY_INFO,                                                                              \
    VALUE_TYPE,                                                                                    \
    VALUE_NAME,                                                                                    \
    ...                                                                                            \
)                                                                                                  \
GD_ECS_COMPONENT_WITH_PARENT_EMPLACE_OR_REPLACE(                                                   \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    GD_ECS_SINGLETON_TYPE::ComponentType,                                                          \
    EMPLACE_OR_REPLACE_BODY,                                                                       \
    GD_PROPERTY_INFO,                                                                              \
    VALUE_TYPE,                                                                                    \
    VALUE_NAME,                                                                                    \
    __VA_ARGS__                                                                                    \
)



//==================================================================================================
// GD_ECS_COMPONENT_MULTI_WITH_PARENT_EMPLACE_OR_REPLACE
//==================================================================================================

/**
 * The parent class must expose a public 'Signal' struct when signal generation is not disabled.
 *
 * using ECSType = godot::ECS;
 *
 * struct Multi {
 *     godot::StringName name{};
 *     godot::Vector2 point{};
 *     bool x{};
 * };
 *
 * #define C_Multi_FIELDS(X)                                                                       \
 * X(godot::PropertyInfo(godot::Variant::Type::STRING_NAME, "name"), godot::StringName, name)      \
 * X(godot::PropertyInfo(godot::Variant::Type::VECTOR2, "point"), godot::Vector2, point)           \
 * X(godot::PropertyInfo(godot::Variant::Type::BOOL, "x"), bool, x, true) // Optional value.
 *
 * GD_ECS_COMPONENT_MULTI_WITH_PARENT_EMPLACE_OR_REPLACE(
 *     ECSType,
 *     C_Multi, ECSType::ComponentType,
 *     // void emplace_or_replace(Node &p_entity_node, EntityType &p_entity)
 *     {
 *         auto &ecs = ECSType::get_instance();
 *         auto &reg = ecs.get_registry();
 *         reg.emplace_or_replace<Multi>(p_entity, name, point);
 *         // Alternatively, the Godot component itself can be stored in the registry, for example
 *         // wrapped in godot::Ref.
 *         //reg.emplace_or_replace<godot::Ref<C_Multi>>(p_entity, this);
 *     },
 *     C_Multi_FIELDS
 * )
 *
 * //// Do not forget to expose the new component to Godot:
 * //ECSType::register_types();
 * //// ...
 * //C_Multi::register_types();
 */
#define GD_ECS_COMPONENT_MULTI_WITH_PARENT_EMPLACE_OR_REPLACE(                                     \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    GD_ECS_COMPONENT_PARENT_TYPE,                                                                  \
    EMPLACE_OR_REPLACE_BODY,                                                                       \
    FIELD_LIST                                                                                     \
)                                                                                                  \
class GD_ECS_COMPONENT_NAME : public GD_ECS_COMPONENT_PARENT_TYPE {                                \
    GDCLASS(GD_ECS_COMPONENT_NAME, GD_ECS_COMPONENT_PARENT_TYPE)                                   \
                                                                                                   \
public:                                                                                            \
    GD_ECS_EMPTY_SIGNAL_STRUCT(Signal, GD_ECS_COMPONENT_PARENT_TYPE::Signal)                       \
                                                                                                   \
public:                                                                                            \
    FIELD_LIST(_HELPER_GD_ECS_COMPONENT_FIELD_DECLARE)                                             \
                                                                                                   \
public:                                                                                            \
    static void register_types() {                                                                 \
        GDREGISTER_RUNTIME_CLASS(GD_ECS_COMPONENT_NAME);                                           \
    }                                                                                              \
                                                                                                   \
    virtual void emplace_or_replace(                                                               \
        [[maybe_unused]] godot::Node &p_entity_node,                                               \
        [[maybe_unused]] GD_ECS_SINGLETON_TYPE::RegistryType::entity_type &p_entity                \
    ) override {                                                                                   \
        EMPLACE_OR_REPLACE_BODY                                                                    \
    }                                                                                              \
                                                                                                   \
    FIELD_LIST(_HELPER_GD_ECS_COMPONENT_FIELD_SET_GET)                                             \
                                                                                                   \
protected:                                                                                         \
    static void _bind_methods() {                                                                  \
        using _HELPER_GD_ECS_COMPONENT_FIELD_BIND_GDComponentName = GD_ECS_COMPONENT_NAME;         \
        FIELD_LIST(_HELPER_GD_ECS_COMPONENT_FIELD_BIND)                                            \
    }                                                                                              \
};



//==================================================================================================
// GD_ECS_COMPONENT_MULTI_EMPLACE_OR_REPLACE
//==================================================================================================

/**
 * using ECSType = godot::ECS;
 *
 * struct Multi {
 *     godot::StringName name{};
 *     godot::Vector2 point{};
 *     bool x{};
 * };
 *
 * #define C_Multi_FIELDS(X)                                                                       \
 * X(godot::PropertyInfo(godot::Variant::Type::STRING_NAME, "name"), godot::StringName, name)      \
 * X(godot::PropertyInfo(godot::Variant::Type::VECTOR2, "point"), godot::Vector2, point)           \
 * X(godot::PropertyInfo(godot::Variant::Type::BOOL, "x"), bool, x, true) // Optional value.
 *
 * GD_ECS_COMPONENT_MULTI_EMPLACE_OR_REPLACE(
 *     ECSType,
 *     C_Multi,
 *     // void emplace_or_replace(Node &p_entity_node, EntityType &p_entity)
 *     {
 *         auto &ecs = ECSType::get_instance();
 *         auto &reg = ecs.get_registry();
 *         reg.emplace_or_replace<Multi>(p_entity, name, point);
 *         // Alternatively, the Godot component itself can be stored in the registry, for example
 *         // wrapped in godot::Ref.
 *         //reg.emplace_or_replace<godot::Ref<C_Multi>>(p_entity, this);
 *     },
 *     C_Multi_FIELDS
 * )
 *
 * //// Do not forget to expose the new component to Godot:
 * //ECSType::register_types();
 * //// ...
 * //C_Multi::register_types();
 */
#define GD_ECS_COMPONENT_MULTI_EMPLACE_OR_REPLACE(                                                 \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    EMPLACE_OR_REPLACE_BODY,                                                                       \
    FIELD_LIST                                                                                     \
)                                                                                                  \
GD_ECS_COMPONENT_MULTI_WITH_PARENT_EMPLACE_OR_REPLACE(                                             \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    GD_ECS_SINGLETON_TYPE::ComponentType,                                                          \
    EMPLACE_OR_REPLACE_BODY,                                                                       \
    FIELD_LIST                                                                                     \
)



//==================================================================================================
// GD_ECS_COMPONENT_WITH_PARENT
//==================================================================================================

/**
 * The parent class must expose a public 'Signal' struct when signal generation is not disabled.
 *
 * using ECSType = godot::ECS;
 *
 * struct Single {
 *     bool x{};
 * };
 *
 * GD_ECS_COMPONENT_WITH_PARENT(
 *     ECSType,
 *     C_Single, ECSType::ComponentType,
 *     Single, // Do not use godot::Ref<C_Single> here.
 *     godot::PropertyInfo(godot::Variant::Type::BOOL, "x"), bool, x //, true // Optional value.
 * )
 *
 * //// Do not forget to expose the new component to Godot:
 * //ECSType::register_types();
 * //// ...
 * //C_Single::register_types();
 */
#define GD_ECS_COMPONENT_WITH_PARENT(                                                              \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    GD_ECS_COMPONENT_PARENT_TYPE,                                                                  \
    ECS_COMPONENT_NAME,                                                                            \
    GD_PROPERTY_INFO,                                                                              \
    VALUE_TYPE,                                                                                    \
    VALUE_NAME,                                                                                    \
    ...                                                                                            \
)                                                                                                  \
GD_ECS_COMPONENT_WITH_PARENT_EMPLACE_OR_REPLACE(                                                   \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    GD_ECS_COMPONENT_PARENT_TYPE,                                                                  \
    {                                                                                              \
        auto &ecs = GD_ECS_SINGLETON_TYPE::get_instance();                                         \
        auto &reg = ecs.get_registry();                                                            \
        reg.emplace_or_replace<ECS_COMPONENT_NAME>(p_entity, ECS_COMPONENT_NAME{VALUE_NAME});      \
    },                                                                                             \
    GD_PROPERTY_INFO,                                                                              \
    VALUE_TYPE,                                                                                    \
    VALUE_NAME,                                                                                    \
    __VA_ARGS__                                                                                    \
)



//==================================================================================================
// GD_ECS_COMPONENT
//==================================================================================================

/**
 * using ECSType = godot::ECS;
 *
 * struct Single {
 *     bool x{};
 * };
 *
 * GD_ECS_COMPONENT(
 *     ECSType,
 *     C_Single,
 *     Single, // Do not use godot::Ref<C_Single> here.
 *     godot::PropertyInfo(godot::Variant::Type::BOOL, "x"), bool, x //, true // Optional value.
 * )
 *
 * //// Do not forget to expose the new component to Godot:
 * //ECSType::register_types();
 * //// ...
 * //C_Single::register_types();
 */
#define GD_ECS_COMPONENT(                                                                          \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    ECS_COMPONENT_NAME,                                                                            \
    GD_PROPERTY_INFO,                                                                              \
    VALUE_TYPE,                                                                                    \
    VALUE_NAME,                                                                                    \
    ...                                                                                            \
)                                                                                                  \
GD_ECS_COMPONENT_WITH_PARENT(                                                                      \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    GD_ECS_SINGLETON_TYPE::ComponentType,                                                          \
    ECS_COMPONENT_NAME,                                                                            \
    GD_PROPERTY_INFO,                                                                              \
    VALUE_TYPE,                                                                                    \
    VALUE_NAME,                                                                                    \
    __VA_ARGS__                                                                                    \
)



//==================================================================================================
// GD_ECS_COMPONENT_MULTI_WITH_PARENT
//==================================================================================================

/**
 * The parent class must expose a public 'Signal' struct when signal generation is not disabled.
 *
 * using ECSType = godot::ECS;
 *
 * struct Multi {
 *     godot::StringName name{};
 *     godot::Vector2 point{};
 *     bool x{};
 * };
 *
 * #define C_Multi_FIELDS(X)                                                                       \
 * X(godot::PropertyInfo(godot::Variant::Type::STRING_NAME, "name"), godot::StringName, name)      \
 * X(godot::PropertyInfo(godot::Variant::Type::VECTOR2, "point"), godot::Vector2, point)           \
 * X(godot::PropertyInfo(godot::Variant::Type::BOOL, "x"), bool, x, true) // Optional value.
 *
 * GD_ECS_COMPONENT_MULTI_WITH_PARENT(
 *     ECSType,
 *     C_Multi, ECSType::ComponentType,
 *     Multi, // Do not use godot::Ref<C_Multi> here.
 *     C_Multi_FIELDS
 * )
 *
 * //// Do not forget to expose the new component to Godot:
 * //ECSType::register_types();
 * //// ...
 * //C_Multi::register_types();
 */
#define GD_ECS_COMPONENT_MULTI_WITH_PARENT(                                                        \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    GD_ECS_COMPONENT_PARENT_TYPE,                                                                  \
    ECS_COMPONENT_NAME,                                                                            \
    FIELD_LIST                                                                                     \
)                                                                                                  \
GD_ECS_COMPONENT_MULTI_WITH_PARENT_EMPLACE_OR_REPLACE(                                             \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    GD_ECS_COMPONENT_PARENT_TYPE,                                                                  \
    {                                                                                              \
        auto &ecs = GD_ECS_SINGLETON_TYPE::get_instance();                                         \
        auto &reg = ecs.get_registry();                                                            \
        reg.emplace_or_replace<ECS_COMPONENT_NAME>(                                                \
            p_entity,                                                                              \
            ECS_COMPONENT_NAME{FIELD_LIST(_HELPER_GD_ECS_COMPONENT_EXTRACT_VALUE_NAME_COMMA)}      \
        );                                                                                         \
    },                                                                                             \
    FIELD_LIST                                                                                     \
)



//==================================================================================================
// GD_ECS_COMPONENT_MULTI
//==================================================================================================

/**
 * using ECSType = godot::ECS;
 *
 * struct Multi {
 *     godot::StringName name{};
 *     godot::Vector2 point{};
 *     bool x{};
 * };
 *
 * #define C_Multi_FIELDS(X)                                                                       \
 * X(godot::PropertyInfo(godot::Variant::Type::STRING_NAME, "name"), godot::StringName, name)      \
 * X(godot::PropertyInfo(godot::Variant::Type::VECTOR2, "point"), godot::Vector2, point)           \
 * X(godot::PropertyInfo(godot::Variant::Type::BOOL, "x"), bool, x, true) // Optional value.
 *
 * GD_ECS_COMPONENT_MULTI(
 *     ECSType,
 *     C_Multi,
 *     Multi, // Do not use godot::Ref<C_Multi> here.
 *     C_Multi_FIELDS
 * )
 *
 * //// Do not forget to expose the new component to Godot:
 * //ECSType::register_types();
 * //// ...
 * //C_Multi::register_types();
 */
#define GD_ECS_COMPONENT_MULTI(                                                                    \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    ECS_COMPONENT_NAME,                                                                            \
    FIELD_LIST                                                                                     \
)                                                                                                  \
GD_ECS_COMPONENT_MULTI_WITH_PARENT(                                                                \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    GD_ECS_SINGLETON_TYPE::ComponentType,                                                          \
    ECS_COMPONENT_NAME,                                                                            \
    FIELD_LIST                                                                                     \
)
