/**
 * This header provides macros that are intended to make it easier to define a component class
 * (GDCLASS).
 *
 * Usage Example:
 *
 * #pragma once
 *
 * #include "godot_cpp_util/ecs/ecs.hpp"
 *
 * struct Marker {};
 *
 * GD_ECS_EMPTY_COMPONENT(GDMarker, Marker)
 *
 * // Do not forget to expose the new component to Godot:
 * // ECS::register_types();
 * // // ...
 * // GDREGISTER_CLASS(GDMarker);
 */



#ifndef GODOT_CPP_UTIL_ECS_MACROS_IMPL_HPP
#define GODOT_CPP_UTIL_ECS_MACROS_IMPL_HPP



namespace GD_ECS_NAMESPACE {



//==================================================================================================
// GD_ECS_EMPTY_COMPONENT_WITH_PARENT_EMPLACE_OR_REPLACE
//==================================================================================================

/**
 * struct Marker {};
 *
 * GD_ECS_EMPTY_COMPONENT_WITH_PARENT_EMPLACE_OR_REPLACE(
 *     GDMarker, GD_ECS_NAMESPACE::GD_ECS_COMPONENT_NAME,
 *     // virtual void emplace_or_replace(GD_ECS_NAMESPACE::GD_ECS_ENTITY_NAME &p_entity) override
 *     {
 *         auto &ecs = GD_ECS_NAMESPACE::GD_ECS_SINGLETON_NAME::get_instance();
 *         auto &reg = ecs.get_registry();
 *         reg.emplace_or_replace<Marker>(p_entity.get_entity());
 *     }
 * )
 *
 * --- or ---
 *
 * GD_ECS_EMPTY_COMPONENT_WITH_PARENT_EMPLACE_OR_REPLACE(
 *     GDMarker, GD_ECS_NAMESPACE::GD_ECS_COMPONENT_NAME,
 *     // virtual void emplace_or_replace(GD_ECS_NAMESPACE::GD_ECS_ENTITY_NAME &p_entity) override
 *     {
 *         auto &ecs = GD_ECS_NAMESPACE::GD_ECS_SINGLETON_NAME::get_instance();
 *         auto &reg = ecs.get_registry();
 *         reg.emplace_or_replace<godot::Ref<GDMarker>>(p_entity.get_entity(), this);
 *     }
 * )
 */
#define GD_ECS_EMPTY_COMPONENT_WITH_PARENT_EMPLACE_OR_REPLACE(                                     \
    COMPONENT_NAME,                                                                                \
    PARENT_TYPE,                                                                                   \
    EMPLACE_OR_REPLACE_BODY                                                                        \
)                                                                                                  \
class COMPONENT_NAME : public PARENT_TYPE {                                                        \
    GDCLASS(COMPONENT_NAME, PARENT_TYPE)                                                           \
                                                                                                   \
public:                                                                                            \
    virtual void emplace_or_replace(GD_ECS_NAMESPACE::GD_ECS_ENTITY_NAME &p_entity) override {     \
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
 * struct Marker {};
 *
 * GD_ECS_EMPTY_COMPONENT_EMPLACE_OR_REPLACE(
 *     GDMarker,
 *     // virtual void emplace_or_replace(GD_ECS_NAMESPACE::GD_ECS_ENTITY_NAME &p_entity) override
 *     {
 *         auto &ecs = GD_ECS_NAMESPACE::GD_ECS_SINGLETON_NAME::get_instance();
 *         auto &reg = ecs.get_registry();
 *         reg.emplace_or_replace<Marker>(p_entity.get_entity());
 *     }
 * )
 *
 * --- or ---
 *
 * GD_ECS_EMPTY_COMPONENT_EMPLACE_OR_REPLACE(
 *     GDMarker,
 *     // virtual void emplace_or_replace(GD_ECS_NAMESPACE::GD_ECS_ENTITY_NAME &p_entity) override
 *     {
 *         auto &ecs = GD_ECS_NAMESPACE::GD_ECS_SINGLETON_NAME::get_instance();
 *         auto &reg = ecs.get_registry();
 *         reg.emplace_or_replace<godot::Ref<GDMarker>>(p_entity.get_entity(), this);
 *     }
 * )
 */
#define GD_ECS_EMPTY_COMPONENT_EMPLACE_OR_REPLACE(COMPONENT_NAME, EMPLACE_OR_REPLACE_BODY)         \
GD_ECS_EMPTY_COMPONENT_WITH_PARENT_EMPLACE_OR_REPLACE(                                             \
    COMPONENT_NAME,                                                                                \
    GD_ECS_NAMESPACE::GD_ECS_COMPONENT_NAME,                                                       \
    EMPLACE_OR_REPLACE_BODY                                                                        \
)



//==================================================================================================
// GD_ECS_EMPTY_COMPONENT_WITH_PARENT
//==================================================================================================

/**
 * struct Marker {};
 *
 * GD_ECS_EMPTY_COMPONENT_WITH_PARENT(
 *     GDMarker, GD_ECS_NAMESPACE::GD_ECS_COMPONENT_NAME,
 *     Marker
 * )
 *
 * --- or ---
 *
 * GD_ECS_EMPTY_COMPONENT_WITH_PARENT(
 *     GDMarker, GD_ECS_NAMESPACE::GD_ECS_COMPONENT_NAME,
 *     godot::Ref<GDMarker>
 * )
 */
#define GD_ECS_EMPTY_COMPONENT_WITH_PARENT(COMPONENT_NAME, PARENT_TYPE, ECS_COMPONENT_NAME)        \
GD_ECS_EMPTY_COMPONENT_WITH_PARENT_EMPLACE_OR_REPLACE(                                             \
    COMPONENT_NAME,                                                                                \
    GD_ECS_NAMESPACE::GD_ECS_COMPONENT_NAME,                                                       \
    {                                                                                              \
        auto &ecs = GD_ECS_NAMESPACE::GD_ECS_SINGLETON_NAME::get_instance();                       \
        auto &reg = ecs.get_registry();                                                            \
        reg.emplace_or_replace<ECS_COMPONENT_NAME>(p_entity.get_entity());                         \
    }                                                                                              \
)



//==================================================================================================
// GD_ECS_EMPTY_COMPONENT
//==================================================================================================

/**
 * struct Marker {};
 *
 * GD_ECS_EMPTY_COMPONENT(GDMarker, Marker)
 *
 * --- or ---
 *
 * GD_ECS_EMPTY_COMPONENT(GDMarker, godot::Ref<GDMarker>)
 */
#define GD_ECS_EMPTY_COMPONENT(COMPONENT_NAME, ECS_COMPONENT_NAME)                                 \
GD_ECS_EMPTY_COMPONENT_WITH_PARENT(                                                                \
    COMPONENT_NAME,                                                                                \
    GD_ECS_NAMESPACE::GD_ECS_COMPONENT_NAME,                                                       \
    ECS_COMPONENT_NAME                                                                             \
)



//==================================================================================================
// GD_ECS_COMPONENT_VARIANT_EXPORT
//==================================================================================================

/**
 * struct Point {
 *     godot::Vector2 pos{};
 * };
 *
 * class GDPoint : public GD_ECS_NAMESPACE::GD_ECS_COMPONENT_NAME {
 *     GDCLASS(GDPoint, GD_ECS_NAMESPACE::GD_ECS_COMPONENT_NAME)
 *
 * private:
 *     godot::Vector2 m_pos{};
 *
 * public:
 *     virtual void emplace_or_replace(GD_ECS_NAMESPACE::GD_ECS_ENTITY_NAME &p_entity) override {
 *         auto &ecs = GD_ECS_NAMESPACE::GD_ECS_SINGLETON_NAME::get_instance();
 *         auto &reg = ecs.get_registry();
 *         reg.emplace_or_replace<Point>(p_entity.get_entity(), m_pos);
 *     }
 *
 *     void set_pos(const godot::Vector2 pos) { m_pos = pos; }
 *     godot::Vector2 get_pos() const { return m_pos; }
 *
 * protected:
 *     static void _bind_methods() {
 *         GD_ECS_COMPONENT_VARIANT_EXPORT(
 *             GDPoint, godot::Variant::Type::VECTOR2, pos, set_pos, get_pos
 *         );
 *     }
 *
 * };
 *
 * --- or ---
 *
 * class GDPoint : public GD_ECS_NAMESPACE::GD_ECS_COMPONENT_NAME {
 *     GDCLASS(GDPoint, GD_ECS_NAMESPACE::GD_ECS_COMPONENT_NAME)
 *
 * private:
 *     Vector2 m_pos{};
 *
 * public:
 *     virtual void emplace_or_replace(GD_ECS_NAMESPACE::GD_ECS_ENTITY_NAME &p_entity) override {
 *         auto &ecs = GD_ECS_NAMESPACE::GD_ECS_SINGLETON_NAME::get_instance();
 *         auto &reg = ecs.get_registry();
 *         reg.emplace_or_replace<godot::Ref<GDPoint>>(p_entity.get_entity(), this);
 *     }
 *
 *     void set_pos(const Vector2 pos) { m_pos = pos; }
 *     Vector2 get_pos() const { return m_pos; }
 *
 * protected:
 *     static void _bind_methods() {
 *         GD_ECS_COMPONENT_VARIANT_EXPORT(GDPoint, Variant::Type::VECTOR2, pos, set_pos, get_pos);
 *     }
 *
 * };
 */
#define GD_ECS_COMPONENT_VARIANT_EXPORT(                                                           \
    COMPONENT_NAME,                                                                                \
    VARIANT_TYPE_ENUM,                                                                             \
    VALUE_NAME,                                                                                    \
    SET_FN,                                                                                        \
    GET_FN                                                                                         \
)                                                                                                  \
godot::ClassDB::bind_method(godot::D_METHOD(#SET_FN, "p_" #VALUE_NAME), &COMPONENT_NAME::SET_FN);  \
        godot::ClassDB::bind_method(godot::D_METHOD(#GET_FN), &COMPONENT_NAME::GET_FN);            \
        if constexpr (VARIANT_TYPE_ENUM == godot::Variant::Type::NIL) {                            \
            ADD_PROPERTY(                                                                          \
                godot::PropertyInfo(                                                               \
                    VARIANT_TYPE_ENUM,                                                             \
                    #VALUE_NAME,                                                                   \
                    godot::PROPERTY_HINT_NONE,                                                     \
                    "",                                                                            \
                    godot::PROPERTY_USAGE_DEFAULT | godot::PROPERTY_USAGE_NIL_IS_VARIANT           \
                ),                                                                                 \
                #SET_FN,                                                                           \
                #GET_FN                                                                            \
            );                                                                                     \
        }                                                                                          \
        else {                                                                                     \
            ADD_PROPERTY(                                                                          \
                godot::PropertyInfo(VARIANT_TYPE_ENUM, #VALUE_NAME),                               \
                #SET_FN,                                                                           \
                #GET_FN                                                                            \
            );                                                                                     \
        }



//==================================================================================================
// Helpers
//==================================================================================================

#define _HELPER_GD_ECS_COMPONENT_FIELD_DECLARE(VALUE_TYPE, VARIANT_TYPE_ENUM, VALUE_NAME, ...)     \
    VALUE_TYPE VALUE_NAME{__VA_ARGS__};



#define _HELPER_GD_ECS_COMPONENT_TO__VA_ARGS__(VALUE_TYPE, VARIANT_TYPE_ENUM, VALUE_NAME, ...)     \
    __VA_ARGS__



#define _HELPER_GD_ECS_COMPONENT_FIELD_SET_GET(VALUE_TYPE, VARIANT_TYPE_ENUM, VALUE_NAME, ...)     \
    void set_##VALUE_NAME(const VALUE_TYPE &p_##VALUE_NAME) {                                      \
        VALUE_NAME = p_##VALUE_NAME;                                                               \
    }                                                                                              \
                                                                                                   \
    VALUE_TYPE get_##VALUE_NAME() const {                                                          \
        return VALUE_NAME;                                                                         \
    }



#define _HELPER_GD_ECS_COMPONENT_FIELD_BIND(VALUE_TYPE, VARIANT_TYPE_ENUM, VALUE_NAME, ...)        \
GD_ECS_COMPONENT_VARIANT_EXPORT(                                                                   \
    _HELPER_GD_ECS_COMPONENT_FIELD_BIND_ComponentName,                                             \
    VARIANT_TYPE_ENUM,                                                                             \
    VALUE_NAME,                                                                                    \
    set_##VALUE_NAME,                                                                              \
    get_##VALUE_NAME                                                                               \
)



//==================================================================================================
// GD_ECS_COMPONENT_WITH_PARENT_EMPLACE_OR_REPLACE
//==================================================================================================

/**
 * struct Single {
 *     real_t x{};
 * };
 *
 * GD_ECS_COMPONENT_WITH_PARENT_EMPLACE_OR_REPLACE(
 *     GDSingle, GD_ECS_NAMESPACE::GD_ECS_COMPONENT_NAME,
 *     real_t, Variant::Type::FLOAT, x,
 *     // virtual void emplace_or_replace(GD_ECS_NAMESPACE::GD_ECS_ENTITY_NAME &p_entity) override
 *     {
 *         auto &ecs = GD_ECS_NAMESPACE::GD_ECS_SINGLETON_NAME::get_instance();
 *         auto &reg = ecs.get_registry();
 *         reg.emplace_or_replace<Single>(p_entity.get_entity(), x);
 *     }
 * )
 *
 * --- or ---
 *
 * GD_ECS_COMPONENT_WITH_PARENT_EMPLACE_OR_REPLACE(
 *     GDSingle, GD_ECS_NAMESPACE::GD_ECS_COMPONENT_NAME,
 *     real_t, Variant::Type::FLOAT, x,
 *     // virtual void emplace_or_replace(GD_ECS_NAMESPACE::GD_ECS_ENTITY_NAME &p_entity) override
 *     {
 *         auto &ecs = GD_ECS_NAMESPACE::GD_ECS_SINGLETON_NAME::get_instance();
 *         auto &reg = ecs.get_registry();
 *         reg.emplace_or_replace<godot::Ref<GDSingle>>(p_entity.get_entity(), this);
 *     }
 * )
 */
#define GD_ECS_COMPONENT_WITH_PARENT_EMPLACE_OR_REPLACE(                                           \
    COMPONENT_NAME,                                                                                \
    PARENT_TYPE,                                                                                   \
    VALUE_TYPE,                                                                                    \
    VARIANT_TYPE_ENUM,                                                                             \
    VALUE_NAME,                                                                                    \
    EMPLACE_OR_REPLACE_BODY                                                                        \
)                                                                                                  \
class COMPONENT_NAME : public PARENT_TYPE {                                                        \
    GDCLASS(COMPONENT_NAME, PARENT_TYPE)                                                           \
                                                                                                   \
public:                                                                                            \
    VALUE_TYPE VALUE_NAME{};                                                                       \
                                                                                                   \
public:                                                                                            \
    virtual void emplace_or_replace(GD_ECS_NAMESPACE::GD_ECS_ENTITY_NAME &p_entity) override {     \
        EMPLACE_OR_REPLACE_BODY                                                                    \
    }                                                                                              \
                                                                                                   \
    _HELPER_GD_ECS_COMPONENT_FIELD_SET_GET(VALUE_TYPE, VARIANT_TYPE_ENUM, VALUE_NAME)              \
                                                                                                   \
protected:                                                                                         \
    static void _bind_methods() {                                                                  \
        using _HELPER_GD_ECS_COMPONENT_FIELD_BIND_ComponentName = COMPONENT_NAME;                  \
        _HELPER_GD_ECS_COMPONENT_FIELD_BIND(VALUE_TYPE, VARIANT_TYPE_ENUM, VALUE_NAME)             \
    }                                                                                              \
                                                                                                   \
};



//==================================================================================================
// GD_ECS_COMPONENT_EMPLACE_OR_REPLACE
//==================================================================================================

/**
 * struct Single {
 *     real_t x{};
 * };
 *
 * GD_ECS_COMPONENT_EMPLACE_OR_REPLACE(
 *     GDSingle,
 *     real_t, Variant::Type::FLOAT, x,
 *     // virtual void emplace_or_replace(GD_ECS_NAMESPACE::GD_ECS_ENTITY_NAME &p_entity) override
 *     {
 *         auto &ecs = GD_ECS_NAMESPACE::GD_ECS_SINGLETON_NAME::get_instance();
 *         auto &reg = ecs.get_registry();
 *         reg.emplace_or_replace<Single>(p_entity.get_entity(), x);
 *     }
 * )
 *
 * --- or ---
 *
 * GD_ECS_COMPONENT_EMPLACE_OR_REPLACE(
 *     GDSingle,
 *     real_t, Variant::Type::FLOAT, x,
 *     // virtual void emplace_or_replace(GD_ECS_NAMESPACE::GD_ECS_ENTITY_NAME &p_entity) override
 *     {
 *         auto &ecs = GD_ECS_NAMESPACE::GD_ECS_SINGLETON_NAME::get_instance();
 *         auto &reg = ecs.get_registry();
 *         reg.emplace_or_replace<godot::Ref<GDSingle>>(p_entity.get_entity(), this);
 *     }
 * )
 */
#define GD_ECS_COMPONENT_EMPLACE_OR_REPLACE(                                                       \
    COMPONENT_NAME,                                                                                \
    VALUE_TYPE,                                                                                    \
    VARIANT_TYPE_ENUM,                                                                             \
    VALUE_NAME,                                                                                    \
    EMPLACE_OR_REPLACE_BODY                                                                        \
)                                                                                                  \
GD_ECS_COMPONENT_WITH_PARENT_EMPLACE_OR_REPLACE(                                                   \
    COMPONENT_NAME,                                                                                \
    GD_ECS_NAMESPACE::GD_ECS_COMPONENT_NAME,                                                       \
    VALUE_TYPE,                                                                                    \
    VARIANT_TYPE_ENUM,                                                                             \
    VALUE_NAME,                                                                                    \
    EMPLACE_OR_REPLACE_BODY                                                                        \
)



//==================================================================================================
// GD_ECS_COMPONENT_MULTI_WITH_PARENT_EMPLACE_OR_REPLACE
//==================================================================================================

/**
 * struct Multi {
 *     StringName name{};
 *     Vector2 point{};
 *     Variant v{};
 * };
 *
 * #define GDMULTI_FIELDS(X)                                                                       \
 * X(StringName, Variant::Type::STRING_NAME, name)                                                 \
 * X(Vector2,    Variant::Type::VECTOR2,     point)                                                \
 * X(Variant,    Variant::Type::NIL,         v,     "Hello, world!")
 *
 * GD_ECS_COMPONENT_MULTI_WITH_PARENT_EMPLACE_OR_REPLACE(
 *     GDMulti, GD_ECS_NAMESPACE::GD_ECS_COMPONENT_NAME,
 *     GDMULTI_FIELDS,
 *     // virtual void emplace_or_replace(GD_ECS_NAMESPACE::GD_ECS_ENTITY_NAME &p_entity) override
 *     {
 *         auto &ecs = GD_ECS_NAMESPACE::GD_ECS_SINGLETON_NAME::get_instance();
 *         auto &reg = ecs.get_registry();
 *         reg.emplace_or_replace<Multi>(p_entity.get_entity(), name, point, v);
 *     }
 * )
 *
 * --- or ---
 *
 * #define GDMULTI_FIELDS(X)                                                                       \
 * X(StringName, Variant::Type::STRING_NAME, name)                                                 \
 * X(Vector2,    Variant::Type::VECTOR2,     point)                                                \
 * X(Variant,    Variant::Type::NIL,         v,     "Hello, world!")
 *
 * GD_ECS_COMPONENT_MULTI_WITH_PARENT_EMPLACE_OR_REPLACE(
 *     GDMulti, GD_ECS_NAMESPACE::GD_ECS_COMPONENT_NAME,
 *     GDMULTI_FIELDS,
 *     // virtual void emplace_or_replace(GD_ECS_NAMESPACE::GD_ECS_ENTITY_NAME &p_entity) override
 *     {
 *         auto &ecs = GD_ECS_NAMESPACE::GD_ECS_SINGLETON_NAME::get_instance();
 *         auto &reg = ecs.get_registry();
 *         reg.emplace_or_replace<godot::Ref<GDMulti>>(p_entity.get_entity(), this);
 *     }
 * )
 */
#define GD_ECS_COMPONENT_MULTI_WITH_PARENT_EMPLACE_OR_REPLACE(                                     \
    COMPONENT_NAME,                                                                                \
    PARENT_TYPE,                                                                                   \
    FIELD_LIST,                                                                                    \
    EMPLACE_OR_REPLACE_BODY                                                                        \
)                                                                                                  \
class COMPONENT_NAME : public PARENT_TYPE {                                                        \
    GDCLASS(COMPONENT_NAME, PARENT_TYPE)                                                           \
                                                                                                   \
public:                                                                                            \
    FIELD_LIST(_HELPER_GD_ECS_COMPONENT_FIELD_DECLARE)                                             \
                                                                                                   \
public:                                                                                            \
    virtual void emplace_or_replace(GD_ECS_NAMESPACE::GD_ECS_ENTITY_NAME &p_entity) override {     \
        EMPLACE_OR_REPLACE_BODY                                                                    \
    }                                                                                              \
                                                                                                   \
    FIELD_LIST(_HELPER_GD_ECS_COMPONENT_FIELD_SET_GET)                                             \
                                                                                                   \
protected:                                                                                         \
    static void _bind_methods() {                                                                  \
        using _HELPER_GD_ECS_COMPONENT_FIELD_BIND_ComponentName = COMPONENT_NAME;                  \
        FIELD_LIST(_HELPER_GD_ECS_COMPONENT_FIELD_BIND)                                            \
    }                                                                                              \
};



//==================================================================================================
// GD_ECS_COMPONENT_MULTI_EMPLACE_OR_REPLACE
//==================================================================================================

/**
 * struct Multi {
 *     StringName name{};
 *     Vector2 point{};
 *     Variant v{};
 * };
 *
 * #define GDMULTI_FIELDS(X)                                                                       \
 * X(StringName, Variant::Type::STRING_NAME, name)                                                 \
 * X(Vector2,    Variant::Type::VECTOR2,     point)                                                \
 * X(Variant,    Variant::Type::NIL,         v,     "Hello, world!")
 *
 * GD_ECS_COMPONENT_MULTI_EMPLACE_OR_REPLACE(
 *     GDMulti,
 *     GDMULTI_FIELDS,
 *     // virtual void emplace_or_replace(GD_ECS_NAMESPACE::GD_ECS_ENTITY_NAME &p_entity) override
 *     {
 *         auto &ecs = GD_ECS_NAMESPACE::GD_ECS_SINGLETON_NAME::get_instance();
 *         auto &reg = ecs.get_registry();
 *         reg.emplace_or_replace<Multi>(p_entity.get_entity(), name, point, v);
 *     }
 * )
 *
 * --- or ---
 *
 * #define GDMULTI_FIELDS(X)                                                                       \
 * X(StringName, Variant::Type::STRING_NAME, name)                                                 \
 * X(Vector2,    Variant::Type::VECTOR2,     point)                                                \
 * X(Variant,    Variant::Type::NIL,         v,     "Hello, world!")
 *
 * GD_ECS_COMPONENT_MULTI_EMPLACE_OR_REPLACE(
 *     GDMulti,
 *     GDMULTI_FIELDS,
 *     // virtual void emplace_or_replace(GD_ECS_NAMESPACE::GD_ECS_ENTITY_NAME &p_entity) override
 *     {
 *         auto &ecs = GD_ECS_NAMESPACE::GD_ECS_SINGLETON_NAME::get_instance();
 *         auto &reg = ecs.get_registry();
 *         reg.emplace_or_replace<godot::Ref<GDMulti>>(p_entity.get_entity(), this);
 *     }
 * )
 */
#define GD_ECS_COMPONENT_MULTI_EMPLACE_OR_REPLACE(                                                 \
    COMPONENT_NAME,                                                                                \
    FIELD_LIST,                                                                                    \
    EMPLACE_OR_REPLACE_BODY                                                                        \
)                                                                                                  \
GD_ECS_COMPONENT_MULTI_WITH_PARENT_EMPLACE_OR_REPLACE(                                             \
    COMPONENT_NAME,                                                                                \
    GD_ECS_NAMESPACE::GD_ECS_COMPONENT_NAME,                                                       \
    FIELD_LIST,                                                                                    \
    EMPLACE_OR_REPLACE_BODY                                                                        \
)



//==================================================================================================
// GD_ECS_COMPONENT_WITH_PARENT
//==================================================================================================

/**
 * struct Single {
 *     real_t x{};
 * };
 *
 * GD_ECS_COMPONENT_WITH_PARENT(
 *     GDSingle, GD_ECS_NAMESPACE::GD_ECS_COMPONENT_NAME,
 *     real_t, Variant::Type::FLOAT, x,
 *     Single
 * )
 *
 * --- or ---
 *
 * GD_ECS_COMPONENT_WITH_PARENT(
 *     GDSingle, GD_ECS_NAMESPACE::GD_ECS_COMPONENT_NAME,
 *     real_t, Variant::Type::FLOAT, x,
 *     godot::Ref<GDSingle>
 * )
 */
#define GD_ECS_COMPONENT_WITH_PARENT(                                                              \
    COMPONENT_NAME,                                                                                \
    PARENT_TYPE,                                                                                   \
    VALUE_TYPE,                                                                                    \
    VARIANT_TYPE_ENUM,                                                                             \
    VALUE_NAME,                                                                                    \
    ECS_COMPONENT_NAME                                                                             \
)                                                                                                  \
GD_ECS_COMPONENT_WITH_PARENT_EMPLACE_OR_REPLACE(                                                   \
    COMPONENT_NAME,                                                                                \
    PARENT_TYPE,                                                                                   \
    VALUE_TYPE,                                                                                    \
    VARIANT_TYPE_ENUM,                                                                             \
    VALUE_NAME,                                                                                    \
    {                                                                                              \
        auto &ecs = GD_ECS_NAMESPACE::GD_ECS_SINGLETON_NAME::get_instance();                       \
        auto &reg = ecs.get_registry();                                                            \
        reg.emplace_or_replace<ECS_COMPONENT_NAME>(                                                \
            p_entity.get_entity(),                                                                 \
            ECS_COMPONENT_NAME{VALUE_NAME}                                                         \
        );                                                                                         \
    }                                                                                              \
)



//==================================================================================================
// GD_ECS_COMPONENT
//==================================================================================================

/**
 * struct Single {
 *     real_t x{};
 * };
 *
 * GD_ECS_COMPONENT(GDSingle, real_t, Variant::Type::FLOAT, x, Single)
 *
 * --- or ---
 *
 * GD_ECS_COMPONENT(GDSingle, real_t, Variant::Type::FLOAT, x, godot::Ref<GDSingle>)
 */
#define GD_ECS_COMPONENT(                                                                          \
    COMPONENT_NAME,                                                                                \
    VALUE_TYPE,                                                                                    \
    VARIANT_TYPE_ENUM,                                                                             \
    VALUE_NAME,                                                                                    \
    ECS_COMPONENT_NAME                                                                             \
)                                                                                                  \
GD_ECS_COMPONENT_WITH_PARENT(                                                                      \
    COMPONENT_NAME,                                                                                \
    GD_ECS_NAMESPACE::GD_ECS_COMPONENT_NAME,                                                       \
    VALUE_TYPE,                                                                                    \
    VARIANT_TYPE_ENUM,                                                                             \
    VALUE_NAME,                                                                                    \
    ECS_COMPONENT_NAME                                                                             \
)



//==================================================================================================
// GD_ECS_COMPONENT_MULTI_WITH_PARENT
//==================================================================================================

/**
 * struct Multi {
 *     StringName name{};
 *     Vector2 point{};
 *     Variant v{};
 * };
 *
 * #define GDMULTI_FIELDS(X)                                                                       \
 * X(StringName, Variant::Type::STRING_NAME, name)                                                 \
 * X(Vector2,    Variant::Type::VECTOR2,     point)                                                \
 * X(Variant,    Variant::Type::NIL,         v,     "Hello, world!")
 *
 * GD_ECS_COMPONENT_MULTI_WITH_PARENT(
 *     GDMulti, GD_ECS_NAMESPACE::GD_ECS_COMPONENT_NAME,
 *     GDMULTI_FIELDS,
 *     Multi
 * )
 *
 * --- or ---
 *
 * #define GDMULTI_FIELDS(X)                                                                       \
 * X(StringName, Variant::Type::STRING_NAME, name)                                                 \
 * X(Vector2,    Variant::Type::VECTOR2,     point)                                                \
 * X(Variant,    Variant::Type::NIL,         v,     "Hello, world!")
 *
 * GD_ECS_COMPONENT_MULTI_WITH_PARENT(
 *     GDMulti, GD_ECS_NAMESPACE::GD_ECS_COMPONENT_NAME,
 *     GDMULTI_FIELDS,
 *     godot::Ref<GDMulti>
 * )
 */
#define GD_ECS_COMPONENT_MULTI_WITH_PARENT(                                                        \
    COMPONENT_NAME,                                                                                \
    PARENT_TYPE,                                                                                   \
    FIELD_LIST,                                                                                    \
    ECS_COMPONENT_NAME                                                                             \
)                                                                                                  \
GD_ECS_COMPONENT_MULTI_WITH_PARENT_EMPLACE_OR_REPLACE(                                             \
    COMPONENT_NAME,                                                                                \
    PARENT_TYPE,                                                                                   \
    FIELD_LIST,                                                                                    \
    {                                                                                              \
        auto &ecs = GD_ECS_NAMESPACE::GD_ECS_SINGLETON_NAME::get_instance();                       \
        auto &reg = ecs.get_registry();                                                            \
        reg.emplace_or_replace<ECS_COMPONENT_NAME>(                                                \
            p_entity.get_entity(),                                                                 \
            ECS_COMPONENT_NAME{FIELD_LIST(_HELPER_GD_ECS_COMPONENT_TO__VA_ARGS__)}                 \
        );                                                                                         \
    }                                                                                              \
)



//==================================================================================================
// GD_ECS_COMPONENT_MULTI
//==================================================================================================

/**
 * struct Multi {
 *     StringName name{};
 *     Vector2 point{};
 *     Variant v{};
 * };
 *
 * #define GDMULTI_FIELDS(X)                                                                       \
 * X(StringName, Variant::Type::STRING_NAME, name)                                                 \
 * X(Vector2,    Variant::Type::VECTOR2,     point)                                                \
 * X(Variant,    Variant::Type::NIL,         v,     "Hello, world!")
 *
 * GD_ECS_COMPONENT_MULTI(GDMulti, GDMULTI_FIELDS, Multi)
 *
 * --- or ---
 *
 * #define GDMULTI_FIELDS(X)                                                                       \
 * X(StringName, Variant::Type::STRING_NAME, name)                                                 \
 * X(Vector2,    Variant::Type::VECTOR2,     point)                                                \
 * X(Variant,    Variant::Type::NIL,         v,     "Hello, world!")
 *
 * GD_ECS_COMPONENT_MULTI(GDMulti, GDMULTI_FIELDS, godot::Ref<GDMulti>)
 */
#define GD_ECS_COMPONENT_MULTI(COMPONENT_NAME, FIELD_LIST, ECS_COMPONENT_NAME)                     \
GD_ECS_COMPONENT_MULTI_WITH_PARENT(                                                                \
    COMPONENT_NAME,                                                                                \
    GD_ECS_NAMESPACE::GD_ECS_COMPONENT_NAME,                                                       \
    FIELD_LIST,                                                                                    \
    ECS_COMPONENT_NAME                                                                             \
)



} // namespace GD_ECS_NAMESPACE



#endif // GODOT_CPP_UTIL_ECS_MACROS_IMPL_HPP
