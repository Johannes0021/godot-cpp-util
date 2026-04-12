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
 * GD_ECS_EMPTY_COMPONENT(ECSType, GDMarker, Marker)
 *
 * // Do not forget to expose the new component to Godot:
 * // ECSType::register_types();
 * // // ...
 * // GDREGISTER_RUNTIME_CLASS(GDMarker);
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
 * class GDPoint : public ECSType::ComponentType {
 *     GDCLASS(GDPoint, ECSType::ComponentType)
 *
 * public:
 *     // Define the Signal struct when signal generation is not disabled.
 *     GD_ECS_EMPTY_SIGNAL_STRUCT(Signal, ECSType::ComponentType::Signal)
 *
 * private:
 *     godot::Vector2 m_pos{};
 *
 * public:
 *     virtual void emplace_or_replace(ECSType::EntityType &p_entity) override {
 *         auto &ecs = ECSType::get_instance();
 *         auto &reg = ecs.get_registry();
 *         reg.emplace_or_replace<Point>(p_entity.get_entity(), m_pos);
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
 *         GD_ECS_COMPONENT_VARIANT_EXPORT(
 *             GDPoint,
 *             godot::PropertyInfo(godot::Variant::Type::VECTOR2, "pos"),
 *             pos,
 *             set_pos,
 *             get_pos
 *         );
 *     }
 *
 * };
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
 *     GDMarker, ECSType::ComponentType,
 *     // virtual void emplace_or_replace(GD_ECS_SINGLETON_TYPE::EntityType &p_entity) override
 *     {
 *         auto &ecs = ECSType::get_instance();
 *         auto &reg = ecs.get_registry();
 *         reg.emplace_or_replace<Marker>(p_entity.get_entity());
 *         // Alternatively, the Godot component itself can be stored in the registry, for example
 *         // wrapped in godot::Ref.
 *         //reg.emplace_or_replace<godot::Ref<GDMarker>>(p_entity.get_entity(), this);
 *     }
 * )
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
    virtual void emplace_or_replace(GD_ECS_SINGLETON_TYPE::EntityType &p_entity) override {        \
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
 *     GDMarker,
 *     // virtual void emplace_or_replace(GD_ECS_SINGLETON_TYPE::EntityType &p_entity) override
 *     {
 *         auto &ecs = ECSType::get_instance();
 *         auto &reg = ecs.get_registry();
 *         reg.emplace_or_replace<Marker>(p_entity.get_entity());
 *         // Alternatively, the Godot component itself can be stored in the registry, for example
 *         // wrapped in godot::Ref.
 *         //reg.emplace_or_replace<godot::Ref<GDMarker>>(p_entity.get_entity(), this);
 *     }
 * )
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
 * using ECSType = godot::ECS;
 *
 * struct Marker {};
 *
 * GD_ECS_EMPTY_COMPONENT_WITH_PARENT(
 *     ECSType,
 *     GDMarker, ECSType::ComponentType,
 *     Marker
 *     // Alternatively, the Godot component itself can be stored in the registry, for example
 *     // wrapped in godot::Ref.
 *     //godot::Ref<GDMarker>
 * )
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
        reg.emplace_or_replace<ECS_COMPONENT_NAME>(p_entity.get_entity());                         \
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
 *     GDMarker,
 *     Marker
 *     // Alternatively, the Godot component itself can be stored in the registry, for example
 *     // wrapped in godot::Ref.
 *     //godot::Ref<GDMarker>
 * )
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
 *     GDSingle, ECSType::ComponentType,
 *     // virtual void emplace_or_replace(GD_ECS_SINGLETON_TYPE::EntityType &p_entity) override
 *     {
 *         auto &ecs = ECSType::get_instance();
 *         auto &reg = ecs.get_registry();
 *         reg.emplace_or_replace<Single>(p_entity.get_entity(), x);
 *         // Alternatively, the Godot component itself can be stored in the registry, for example
 *         // wrapped in godot::Ref.
 *         //reg.emplace_or_replace<godot::Ref<GDSingle>>(p_entity.get_entity(), this);
 *     },
 *     godot::PropertyInfo(godot::Variant::Type::BOOL, "x"), bool, x //, true // Optional value.
 * )
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
    virtual void emplace_or_replace(GD_ECS_SINGLETON_TYPE::EntityType &p_entity) override {        \
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
 *     GDSingle,
 *     // virtual void emplace_or_replace(GD_ECS_SINGLETON_TYPE::EntityType &p_entity) override
 *     {
 *         auto &ecs = ECSType::get_instance();
 *         auto &reg = ecs.get_registry();
 *         reg.emplace_or_replace<Single>(p_entity.get_entity(), x);
 *         // Alternatively, the Godot component itself can be stored in the registry, for example
 *         // wrapped in godot::Ref.
 *         //reg.emplace_or_replace<godot::Ref<GDSingle>>(p_entity.get_entity(), this);
 *     },
 *     godot::PropertyInfo(godot::Variant::Type::BOOL, "x"), bool, x //, true // Optional value.
 * )
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
 * #define GDMULTI_FIELDS(X)                                                                       \
 * X(godot::PropertyInfo(godot::Variant::Type::STRING_NAME, "name"), godot::StringName, name)      \
 * X(godot::PropertyInfo(godot::Variant::Type::VECTOR2, "point"), godot::Vector2, point)           \
 * X(godot::PropertyInfo(godot::Variant::Type::BOOL, "x"), bool, x, true) // Optional value.
 *
 * GD_ECS_COMPONENT_MULTI_WITH_PARENT_EMPLACE_OR_REPLACE(
 *     ECSType,
 *     GDMulti, ECSType::ComponentType,
 *     // virtual void emplace_or_replace(GD_ECS_SINGLETON_TYPE::EntityType &p_entity) override
 *     {
 *         auto &ecs = ECSType::get_instance();
 *         auto &reg = ecs.get_registry();
 *         reg.emplace_or_replace<Multi>(p_entity.get_entity(), name, point);
 *         // Alternatively, the Godot component itself can be stored in the registry, for example
 *         // wrapped in godot::Ref.
 *         //reg.emplace_or_replace<godot::Ref<GDMulti>>(p_entity.get_entity(), this);
 *     },
 *     GDMULTI_FIELDS
 * )
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
    virtual void emplace_or_replace(GD_ECS_SINGLETON_TYPE::EntityType &p_entity) override {        \
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
 * #define GDMULTI_FIELDS(X)                                                                       \
 * X(godot::PropertyInfo(godot::Variant::Type::STRING_NAME, "name"), godot::StringName, name)      \
 * X(godot::PropertyInfo(godot::Variant::Type::VECTOR2, "point"), godot::Vector2, point)           \
 * X(godot::PropertyInfo(godot::Variant::Type::BOOL, "x"), bool, x, true) // Optional value.
 *
 * GD_ECS_COMPONENT_MULTI_EMPLACE_OR_REPLACE(
 *     ECSType,
 *     GDMulti,
 *     // virtual void emplace_or_replace(GD_ECS_SINGLETON_TYPE::EntityType &p_entity) override
 *     {
 *         auto &ecs = ECSType::get_instance();
 *         auto &reg = ecs.get_registry();
 *         reg.emplace_or_replace<Multi>(p_entity.get_entity(), name, point);
 *         // Alternatively, the Godot component itself can be stored in the registry, for example
 *         // wrapped in godot::Ref.
 *         //reg.emplace_or_replace<godot::Ref<GDMulti>>(p_entity.get_entity(), this);
 *     },
 *     GDMULTI_FIELDS
 * )
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
 * using ECSType = godot::ECS;
 *
 * struct Single {
 *     bool x{};
 * };
 *
 * GD_ECS_COMPONENT_WITH_PARENT(
 *     ECSType,
 *     GDSingle, ECSType::ComponentType,
 *     Single,
 *     // Alternatively, the Godot component itself can be stored in the registry, for example
 *     // wrapped in godot::Ref.
 *     //godot::Ref<GDSingle>,
 *     // virtual void emplace_or_replace(GD_ECS_SINGLETON_TYPE::EntityType &p_entity) override
 *     godot::PropertyInfo(godot::Variant::Type::BOOL, "x"), bool, x //, true // Optional value.
 * )
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
        reg.emplace_or_replace<ECS_COMPONENT_NAME>(                                                \
            p_entity.get_entity(),                                                                 \
            ECS_COMPONENT_NAME{VALUE_NAME}                                                         \
        );                                                                                         \
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
 *     GDSingle,
 *     Single,
 *     // Alternatively, the Godot component itself can be stored in the registry, for example
 *     // wrapped in godot::Ref.
 *     //godot::Ref<GDSingle>,
 *     // virtual void emplace_or_replace(GD_ECS_SINGLETON_TYPE::EntityType &p_entity) override
 *     godot::PropertyInfo(godot::Variant::Type::BOOL, "x"), bool, x //, true // Optional value.
 * )
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
 * using ECSType = godot::ECS;
 *
 * struct Multi {
 *     godot::StringName name{};
 *     godot::Vector2 point{};
 *     bool x{};
 * };
 *
 * #define GDMULTI_FIELDS(X)                                                                       \
 * X(godot::PropertyInfo(godot::Variant::Type::STRING_NAME, "name"), godot::StringName, name)      \
 * X(godot::PropertyInfo(godot::Variant::Type::VECTOR2, "point"), godot::Vector2, point)           \
 * X(godot::PropertyInfo(godot::Variant::Type::BOOL, "x"), bool, x, true) // Optional value.
 *
 * GD_ECS_COMPONENT_MULTI_WITH_PARENT(
 *     ECSType,
 *     GDMulti, ECSType::ComponentType,
 *     Multi,
 *     // Alternatively, the Godot component itself can be stored in the registry, for example
 *     // wrapped in godot::Ref.
 *     //godot::Ref<GDMulti>,
 *     GDMULTI_FIELDS
 * )
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
            p_entity.get_entity(),                                                                 \
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
 * #define GDMULTI_FIELDS(X)                                                                       \
 * X(godot::PropertyInfo(godot::Variant::Type::STRING_NAME, "name"), godot::StringName, name)      \
 * X(godot::PropertyInfo(godot::Variant::Type::VECTOR2, "point"), godot::Vector2, point)           \
 * X(godot::PropertyInfo(godot::Variant::Type::BOOL, "x"), bool, x, true) // Optional value.
 *
 * GD_ECS_COMPONENT_MULTI(
 *     ECSType,
 *     GDMulti,
 *     Multi,
 *     // Alternatively, the Godot component itself can be stored in the registry, for example
 *     // wrapped in godot::Ref.
 *     //godot::Ref<GDMulti>,
 *     GDMULTI_FIELDS
 * )
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
