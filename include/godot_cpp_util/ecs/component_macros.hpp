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
 * GD_ECS_EMPTY_COMPONENT(ECS, GDMarker, Marker)
 *
 * // Do not forget to expose the new component to Godot:
 * // ECS::register_types();
 * // // ...
 * // GDREGISTER_RUNTIME_CLASS(GDMarker);
 */



#pragma once



namespace GD_ECS_NAMESPACE {



//==================================================================================================
// GD_ECS_EMPTY_COMPONENT_WITH_PARENT_EMPLACE_OR_REPLACE
//==================================================================================================

/**
 * struct Marker {};
 *
 * GD_ECS_EMPTY_COMPONENT_WITH_PARENT_EMPLACE_OR_REPLACE(
 *     ECS,
 *     GDMarker, ECS::ComponentType,
 *     // virtual void emplace_or_replace(GD_ECS_SINGLETON_TYPE::EntityType &p_entity) override
 *     {
 *         auto &ecs = ECS::get_instance();
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
 * struct Marker {};
 *
 * GD_ECS_EMPTY_COMPONENT_EMPLACE_OR_REPLACE(
 *     ECS,
 *     GDMarker,
 *     // virtual void emplace_or_replace(GD_ECS_SINGLETON_TYPE::EntityType &p_entity) override
 *     {
 *         auto &ecs = ECS::get_instance();
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
 * struct Marker {};
 *
 * GD_ECS_EMPTY_COMPONENT_WITH_PARENT(
 *     ECS,
 *     GDMarker, ECS::ComponentType,
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
 * struct Marker {};
 *
 * GD_ECS_EMPTY_COMPONENT(
 *     ECS,
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
// GD_ECS_COMPONENT_VARIANT_EXPORT
//==================================================================================================

/**
 * struct Point {
 *     godot::Vector2 pos{};
 * };
 *
 * class GDPoint : public ECS::ComponentType {
 *     GDCLASS(GDPoint, ECS::ComponentType)
 *
 * private:
 *     godot::Vector2 m_pos{};
 *
 * public:
 *     virtual void emplace_or_replace(GD_ECS_SINGLETON_TYPE::EntityType &p_entity) override {
 *         auto &ecs = ECS::get_instance();
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
 *             GDPoint, godot::Variant::Type::VECTOR2, pos, set_pos, get_pos
 *         );
 *     }
 *
 * };
 */
#define GD_ECS_COMPONENT_VARIANT_EXPORT(                                                           \
    GD_ECS_COMPONENT_NAME,                                                                         \
    GD_VARIANT_TYPE_ENUM,                                                                          \
    VALUE_NAME,                                                                                    \
    SET_FN,                                                                                        \
    GET_FN                                                                                         \
)                                                                                                  \
godot::ClassDB::bind_method(                                                                       \
    godot::D_METHOD(#SET_FN, "p_" #VALUE_NAME),                                                    \
    &GD_ECS_COMPONENT_NAME::SET_FN                                                                 \
);                                                                                                 \
godot::ClassDB::bind_method(godot::D_METHOD(#GET_FN), &GD_ECS_COMPONENT_NAME::GET_FN);             \
if constexpr (GD_VARIANT_TYPE_ENUM == godot::Variant::Type::NIL) {                                 \
    ADD_PROPERTY(                                                                                  \
        godot::PropertyInfo(                                                                       \
            GD_VARIANT_TYPE_ENUM,                                                                  \
            #VALUE_NAME,                                                                           \
            godot::PROPERTY_HINT_NONE,                                                             \
            "",                                                                                    \
            godot::PROPERTY_USAGE_DEFAULT | godot::PROPERTY_USAGE_NIL_IS_VARIANT                   \
        ),                                                                                         \
        #SET_FN,                                                                                   \
        #GET_FN                                                                                    \
    );                                                                                             \
}                                                                                                  \
else {                                                                                             \
    ADD_PROPERTY(godot::PropertyInfo(GD_VARIANT_TYPE_ENUM, #VALUE_NAME), #SET_FN, #GET_FN);        \
}



//==================================================================================================
// Helpers
//==================================================================================================

#define _HELPER_GD_ECS_COMPONENT_FIELD_DECLARE(GD_VARIANT_TYPE_ENUM, VALUE_TYPE, VALUE_NAME, ...)  \
VALUE_TYPE VALUE_NAME{__VA_ARGS__};



#define _HELPER_GD_ECS_COMPONENT_EXTRACT_VALUE_NAME_COMMA(                                         \
    GD_VARIANT_TYPE_ENUM,                                                                          \
    VALUE_TYPE,                                                                                    \
    VALUE_NAME,                                                                                    \
    ...                                                                                            \
)                                                                                                  \
VALUE_NAME,



#define _HELPER_GD_ECS_COMPONENT_FIELD_SET_GET(GD_VARIANT_TYPE_ENUM, VALUE_TYPE, VALUE_NAME, ...)  \
void set_##VALUE_NAME(const VALUE_TYPE &p_##VALUE_NAME) {                                          \
    VALUE_NAME = p_##VALUE_NAME;                                                                   \
}                                                                                                  \
                                                                                                   \
VALUE_TYPE get_##VALUE_NAME() const {                                                              \
    return VALUE_NAME;                                                                             \
}



#define _HELPER_GD_ECS_COMPONENT_FIELD_BIND(GD_VARIANT_TYPE_ENUM, VALUE_TYPE, VALUE_NAME, ...)     \
GD_ECS_COMPONENT_VARIANT_EXPORT(                                                                   \
    _HELPER_GD_ECS_COMPONENT_FIELD_BIND_GDComponentName,                                           \
    GD_VARIANT_TYPE_ENUM,                                                                          \
    VALUE_NAME,                                                                                    \
    set_##VALUE_NAME,                                                                              \
    get_##VALUE_NAME                                                                               \
)



//==================================================================================================
// GD_ECS_COMPONENT_WITH_PARENT_EMPLACE_OR_REPLACE
//==================================================================================================

/**
 * struct Single {
 *     bool x{};
 * };
 *
 * GD_ECS_COMPONENT_WITH_PARENT_EMPLACE_OR_REPLACE(
 *     ECS,
 *     GDSingle, ECS::ComponentType,
 *     // virtual void emplace_or_replace(GD_ECS_SINGLETON_TYPE::EntityType &p_entity) override
 *     {
 *         auto &ecs = ECS::get_instance();
 *         auto &reg = ecs.get_registry();
 *         reg.emplace_or_replace<Single>(p_entity.get_entity(), x);
 *         // Alternatively, the Godot component itself can be stored in the registry, for example
 *         // wrapped in godot::Ref.
 *         //reg.emplace_or_replace<godot::Ref<GDSingle>>(p_entity.get_entity(), this);
 *     },
 *     Variant::Type::BOOL, bool, x //, true // Optional default value.
 * )
 */
#define GD_ECS_COMPONENT_WITH_PARENT_EMPLACE_OR_REPLACE(                                           \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    GD_ECS_COMPONENT_PARENT_TYPE,                                                                  \
    EMPLACE_OR_REPLACE_BODY,                                                                       \
    GD_VARIANT_TYPE_ENUM,                                                                          \
    VALUE_TYPE,                                                                                    \
    VALUE_NAME,                                                                                    \
    ...                                                                                            \
)                                                                                                  \
class GD_ECS_COMPONENT_NAME : public GD_ECS_COMPONENT_PARENT_TYPE {                                \
    GDCLASS(GD_ECS_COMPONENT_NAME, GD_ECS_COMPONENT_PARENT_TYPE)                                   \
                                                                                                   \
public:                                                                                            \
    VALUE_TYPE VALUE_NAME{__VA_ARGS__};                                                            \
                                                                                                   \
public:                                                                                            \
    virtual void emplace_or_replace(GD_ECS_SINGLETON_TYPE::EntityType &p_entity) override {        \
        EMPLACE_OR_REPLACE_BODY                                                                    \
    }                                                                                              \
                                                                                                   \
    _HELPER_GD_ECS_COMPONENT_FIELD_SET_GET(VALUE_TYPE, GD_VARIANT_TYPE_ENUM, VALUE_NAME)           \
                                                                                                   \
protected:                                                                                         \
    static void _bind_methods() {                                                                  \
        using _HELPER_GD_ECS_COMPONENT_FIELD_BIND_GDComponentName = GD_ECS_COMPONENT_NAME;         \
        _HELPER_GD_ECS_COMPONENT_FIELD_BIND(VALUE_TYPE, GD_VARIANT_TYPE_ENUM, VALUE_NAME)          \
    }                                                                                              \
                                                                                                   \
};



//==================================================================================================
// GD_ECS_COMPONENT_EMPLACE_OR_REPLACE
//==================================================================================================

/**
 * struct Single {
 *     bool x{};
 * };
 *
 * GD_ECS_COMPONENT_EMPLACE_OR_REPLACE(
 *     ECS,
 *     GDSingle,
 *     // virtual void emplace_or_replace(GD_ECS_SINGLETON_TYPE::EntityType &p_entity) override
 *     {
 *         auto &ecs = ECS::get_instance();
 *         auto &reg = ecs.get_registry();
 *         reg.emplace_or_replace<Single>(p_entity.get_entity(), x);
 *         // Alternatively, the Godot component itself can be stored in the registry, for example
 *         // wrapped in godot::Ref.
 *         //reg.emplace_or_replace<godot::Ref<GDSingle>>(p_entity.get_entity(), this);
 *     },
 *     Variant::Type::BOOL, bool, x //, true // Optional default value.
 * )
 */
#define GD_ECS_COMPONENT_EMPLACE_OR_REPLACE(                                                       \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    EMPLACE_OR_REPLACE_BODY,                                                                       \
    GD_VARIANT_TYPE_ENUM,                                                                          \
    VALUE_TYPE,                                                                                    \
    VALUE_NAME,                                                                                    \
    ...                                                                                            \
)                                                                                                  \
GD_ECS_COMPONENT_WITH_PARENT_EMPLACE_OR_REPLACE(                                                   \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    GD_ECS_SINGLETON_TYPE::ComponentType,                                                          \
    EMPLACE_OR_REPLACE_BODY,                                                                       \
    GD_VARIANT_TYPE_ENUM,                                                                          \
    VALUE_TYPE,                                                                                    \
    VALUE_NAME,                                                                                    \
    __VA_ARGS__                                                                                    \
)



//==================================================================================================
// GD_ECS_COMPONENT_MULTI_WITH_PARENT_EMPLACE_OR_REPLACE
//==================================================================================================

/**
 * struct Multi {
 *     StringName name{};
 *     Vector2 point{};
 *     bool x{};
 *     Variant v{};
 * };
 *
 * #define GDMULTI_FIELDS(X)                                                                       \
 * X(Variant::Type::STRING_NAME, StringName, name)                                                 \
 * X(Variant::Type::VECTOR2,     Vector2,    point)                                                \
 * X(Variant::Type::BOOL,        bool,       x,     true)                                          \
 * X(Variant::Type::NIL,         Variant,    v,     "Hello, world!") // Optional default values.
 *
 * GD_ECS_COMPONENT_MULTI_WITH_PARENT_EMPLACE_OR_REPLACE(
 *     ECS,
 *     GDMulti, ECS::ComponentType,
 *     // virtual void emplace_or_replace(GD_ECS_SINGLETON_TYPE::EntityType &p_entity) override
 *     {
 *         auto &ecs = ECS::get_instance();
 *         auto &reg = ecs.get_registry();
 *         reg.emplace_or_replace<Multi>(p_entity.get_entity(), name, point, v);
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
 * struct Multi {
 *     StringName name{};
 *     Vector2 point{};
 *     bool x{};
 *     Variant v{};
 * };
 *
 * #define GDMULTI_FIELDS(X)                                                                       \
 * X(Variant::Type::STRING_NAME, StringName, name)                                                 \
 * X(Variant::Type::VECTOR2,     Vector2,    point)                                                \
 * X(Variant::Type::BOOL,        bool,       x,     true)                                          \
 * X(Variant::Type::NIL,         Variant,    v,     "Hello, world!") // Optional default values.
 *
 * GD_ECS_COMPONENT_MULTI_EMPLACE_OR_REPLACE(
 *     ECS,
 *     GDMulti,
 *     // virtual void emplace_or_replace(GD_ECS_SINGLETON_TYPE::EntityType &p_entity) override
 *     {
 *         auto &ecs = ECS::get_instance();
 *         auto &reg = ecs.get_registry();
 *         reg.emplace_or_replace<Multi>(p_entity.get_entity(), name, point, v);
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
 * struct Single {
 *     bool x{};
 * };
 *
 * GD_ECS_COMPONENT_WITH_PARENT(
 *     ECS,
 *     GDSingle, ECS::ComponentType,
 *     Single,
 *     // Alternatively, the Godot component itself can be stored in the registry, for example
 *     // wrapped in godot::Ref.
 *     //godot::Ref<GDSingle>,
 *     // virtual void emplace_or_replace(GD_ECS_SINGLETON_TYPE::EntityType &p_entity) override
 *     Variant::Type::BOOL, bool, x //, true // Optional default value.
 * )
 */
#define GD_ECS_COMPONENT_WITH_PARENT(                                                              \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    GD_ECS_COMPONENT_PARENT_TYPE,                                                                  \
    ECS_COMPONENT_NAME,                                                                            \
    GD_VARIANT_TYPE_ENUM,                                                                          \
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
    GD_VARIANT_TYPE_ENUM,                                                                          \
    VALUE_TYPE,                                                                                    \
    VALUE_NAME,                                                                                    \
    __VA_ARGS__                                                                                    \
)



//==================================================================================================
// GD_ECS_COMPONENT
//==================================================================================================

/**
 * struct Single {
 *     bool x{};
 * };
 *
 * GD_ECS_COMPONENT(
 *     ECS,
 *     GDSingle,
 *     Single,
 *     // Alternatively, the Godot component itself can be stored in the registry, for example
 *     // wrapped in godot::Ref.
 *     //godot::Ref<GDSingle>,
 *     // virtual void emplace_or_replace(GD_ECS_SINGLETON_TYPE::EntityType &p_entity) override
 *     Variant::Type::BOOL, bool, x //, true // Optional default value.
 * )
 */
#define GD_ECS_COMPONENT(                                                                          \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    ECS_COMPONENT_NAME,                                                                            \
    GD_VARIANT_TYPE_ENUM,                                                                          \
    VALUE_TYPE,                                                                                    \
    VALUE_NAME,                                                                                    \
    ...                                                                                            \
)                                                                                                  \
GD_ECS_COMPONENT_WITH_PARENT(                                                                      \
    GD_ECS_SINGLETON_TYPE,                                                                         \
    GD_ECS_COMPONENT_NAME,                                                                         \
    GD_ECS_SINGLETON_TYPE::ComponentType,                                                          \
    ECS_COMPONENT_NAME,                                                                            \
    GD_VARIANT_TYPE_ENUM,                                                                          \
    VALUE_TYPE,                                                                                    \
    VALUE_NAME,                                                                                    \
    __VA_ARGS__                                                                                    \
)



//==================================================================================================
// GD_ECS_COMPONENT_MULTI_WITH_PARENT
//==================================================================================================

/**
 * struct Multi {
 *     StringName name{};
 *     Vector2 point{};
 *     bool x{};
 *     Variant v{};
 * };
 *
 * #define GDMULTI_FIELDS(X)                                                                       \
 * X(Variant::Type::STRING_NAME, StringName, name)                                                 \
 * X(Variant::Type::VECTOR2,     Vector2,    point)                                                \
 * X(Variant::Type::BOOL,        bool,       x,     true)                                          \
 * X(Variant::Type::NIL,         Variant,    v,     "Hello, world!") // Optional default values.
 *
 * GD_ECS_COMPONENT_MULTI_WITH_PARENT(
 *     ECS,
 *     GDMulti, ECS::ComponentType,
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
 * struct Multi {
 *     StringName name{};
 *     Vector2 point{};
 *     bool x{};
 *     Variant v{};
 * };
 *
 * #define GDMULTI_FIELDS(X)                                                                       \
 * X(Variant::Type::STRING_NAME, StringName, name)                                                 \
 * X(Variant::Type::VECTOR2,     Vector2,    point)                                                \
 * X(Variant::Type::BOOL,        bool,       x,     true)                                          \
 * X(Variant::Type::NIL,         Variant,    v,     "Hello, world!") // Optional default values.
 *
 * GD_ECS_COMPONENT_MULTI(
 *     ECS,
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



} // namespace GD_ECS_NAMESPACE
