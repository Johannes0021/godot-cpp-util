# Code Example



This example assumes that the C++ ECS library EnTT is accessible to the project.
See (https://github.com/skypjack/entt).

If the default configuration is not suitable, EnTT is not used, or more than one ECS singleton
must coexist, see "ecs_impl.hpp" and provide a custom configuration by defining GD_ECS_IMPL.

For more information on how to use the ECS, refer to the other files in this directory.



```cpp
#pragma once



#include "godot_cpp_util/ecs/ecs.hpp"



using namespace godot;



//==================================================================================================
// Entity: E_Node
//==================================================================================================

// Defines an ECS entity named E_Node that inherits from Godot's Node.
GD_ECS_ENTITY(ECS, E_Node, Node)



//==================================================================================================
// Component: Data
//==================================================================================================

struct Data {
    int id{21};
    godot::String name{"SomeName"};
    float length{21.21f};
    godot::Dictionary meta{};

    // Default constructor is required by the ECS.
    Data() = default;

    GD_ECS_COMPONENT_IMPL(ECS, Data, "Data",
        godot::C_Field{&Data::id,     godot::Variant::Type::INT,        "id"},
        godot::C_Field{&Data::name,   godot::Variant::Type::STRING,     "name"},
        godot::C_Field{&Data::length, godot::Variant::Type::FLOAT,      "length"},
        godot::C_Field{&Data::meta,   godot::Variant::Type::DICTIONARY, "meta"}
    )
};

/**
 * Defines a Resource wrapper for the Data component.
 * This allows the component to be created, stored, and edited as a Godot Resource.
 * Entities such as E_Node can add and manage this component themselves.
 *
 * There are also additional component resource wrapper macros that offer more control over the
 * parent component or how the component is inserted.
 */
GD_ECS_RES_COMPONENT(ECS, C_Data, Data)
//GD_ECS_RES_COMPONENT_WITH_PARENT_AND_POLICY(
//    ECS, C_Data, Data, C_Component, C_SuperCallPolicy::Before
//)
//GD_ECS_RES_COMPONENT_WITH_PARENT(ECS, C_Data, Data, C_Component)
//GD_ECS_RES_COMPONENT_WITH_POLICY(ECS, C_Data, Data, C_SuperCallPolicy::After)



//==================================================================================================
// Component: DataExtended
//==================================================================================================

// A component that stores basic example data.
struct DataExtended {
    int id{21};
    String name{"SomeName"};
    float length{21.21f};
    Dictionary meta{};

    // Default constructor is required by the ECS.
    DataExtended() = default;

    /**
     * Required for:
     * - Translating C++ ECS components to editor-compatible variants.
     * - Resource wrapper components.
     *
     * This function is functionally equivalent to:
     * GD_ECS_COMPONENT_DESCRIPTOR_IMPL("DataExtended",
     *     godot::C_Field{&DataExtended::id,     godot::Variant::Type::INT,        "id"},
     *     godot::C_Field{&DataExtended::name,   godot::Variant::Type::STRING,     "name"},
     *     godot::C_Field{&DataExtended::length, godot::Variant::Type::FLOAT,      "length"},
     *     godot::C_Field{&DataExtended::meta,   godot::Variant::Type::DICTIONARY, "meta"}
     * )
     */
    static const auto& descriptor() {
        static const C_Descriptor descriptor{
            // ComponentName
            "DataExtended",

            // Field with explicit PropertyInfo and setter and getter.
            C_Field{&DataExtended::id, PropertyInfo(Variant::Type::INT, "id"), "set_id", "get_id"},

            // Field using a simplified constructor with Variant type.
            C_Field{&DataExtended::name, Variant::Type::STRING, "name", "set_name", "get_name"},

            // Field with:
            // - PropertyInfo
            // - "set_length"
            // - "get_length"
            C_Field{&DataExtended::length, PropertyInfo(Variant::Type::FLOAT, "length")},

            // Field with:
            // - PropertyInfo(Variant::Type::DICTIONARY, "meta")
            // - "set_meta"
            // - "get_meta"
            C_Field{&DataExtended::meta, Variant::Type::DICTIONARY, "meta"}
        };

        return descriptor;
    }

    /**
     * Required for:
     * - Resource wrapper components.
     * - Default components of entities.
     *
     * This function is functionally equivalent to:
     * GD_ECS_COMPONENT_EMPLACE_OR_REPLACE_IMPL(ECS, DataExtended)
     */
    static void emplace_or_replace(
        Node &/* p_entity_node */,
        entt::entity &p_entity,
        DataExtended &p_data
    ) {
        auto &reg = ECS::registry();
        reg.emplace_or_replace<DataExtended>(p_entity, p_data);
    }
};

GD_ECS_RES_COMPONENT(ECS, C_DataExtended, DataExtended)



//==================================================================================================
// Component: Empty
//==================================================================================================

/**
 * A component with no data.
 * Useful as a marker or tag component.
 */
struct Empty {
    // Default constructor is required by the ECS.
    Empty() = default;

    // Generates an empty component descriptor and a default emplace_or_replace implementation.
    GD_ECS_EMPTY_COMPONENT_IMPL(ECS, Empty, "Empty")

    // The macros above use these sub-macros:
    //GD_ECS_EMPTY_COMPONENT_DESCRIPTOR_IMPL(ECS_COMPONENT_NAME, C_DESCRIPTOR_NAME)
    //GD_ECS_COMPONENT_EMPLACE_OR_REPLACE_IMPL(GD_ECS_SINGLETON_TYPE, ECS_COMPONENT_NAME)
};

// Resource wrapper for the Empty component.
GD_ECS_RES_COMPONENT(ECS, C_Empty, Empty)



//==================================================================================================
// Entity: E_NodeWithDefaults
//==================================================================================================

/**
 * Defines an ECS entity named E_NodeWithDefaults that inherits from Godot's Node and inserts
 * default components.
 */
GD_ECS_ENTITY(ECS, E_NodeWithDefaults, Node, Data, Empty)



//==================================================================================================
// Type registration
//==================================================================================================

inline void register_types() {
    // Register core ECS types.
    ECS::register_types();

    // Register entities.
    E_Node::register_types();
    E_NodeWithDefaults::register_types();

    // Register components and component resources.
    C_Data::register_types();
    C_DataExtended::register_types();
    C_Empty::register_types();

    // If a component does not need to be exposed as a Resource, it can be registered directly
    // instead.
    //ECS::register_type<Data>();
    // This is internally handled by C_Data::register_types().

    // The advantage of exposing a component as a Godot Resource is that it can be edited in a type
    // safe way inside the editor. All fields are known at compile time and validated by the engine.
    //
    // If a component is only registered directly with the ECS and not wrapped as a Resource, it can
    // only be accessed through its string based component name and generic Variant data.
    // This approach is more flexible, but it loses type safety and editor support.
    //
    // Components that are not intended for editor usage can still benefit from being registered via
    // ECS::register_type<Data>().
    //
    // Even though they are not exposed as Resources, this registration allows them to appear in
    // debug builds inside the remote inspector view for entities. This makes it possible to inspect
    // and modify their values at runtime, which is especially useful for debugging and development
    // workflows.
    //
    // This provides a lightweight alternative to full Resource exposure while still keeping runtime
    // visibility and editability in debug environments.
    //
    // Using '+', '-', and '+-' or '-+' commands, multiple components can be added or removed at
    // once. This functionality is specifically intended for the remote runtime view in the editor,
    // allowing entities to be modified live while the game is running (debug build only).
    // Note: In Godot 4.6.x, Dictionary add and remove operations are currently broken in the remote
    // view (see issue: https://github.com/godotengine/godot/issues/116748).

    auto &reg = ECS::registry();
    auto entity = reg.create();
    reg.emplace<Data>(entity);

    Variant data_as_variant = ECS::get(entity, "Data");
    // Change data_as_variant...
    ECS::emplace_or_replace(entity, "Data", data_as_variant);
    if (ECS::has(entity, "Data")) {
        // ...
    }
    ECS::remove(entity, "Data");

    E_Node *e_node = memnew(E_Node);
    entity = e_node->get_entity();

    // The following functions are also accessible from GDScript.
    e_node->set_res_component(Ref{memnew(C_Data)});
    e_node->set_res_components({Ref{memnew(C_Data)}, Ref{memnew(C_Empty)}});
    e_node->set_component("Empty", Variant{});
    e_node->remove_component("Empty");
    if (e_node->has_component("Empty")) {
        // ...
    }
    data_as_variant = e_node->get_component("Data");

    // components_diff
    Dictionary diff{};

    // Default behavior.
    // When a Dictionary is used as a value, it represents one or more components,
    // where the key is the component name and the value is the component data.
    // These entries are applied using emplace_or_replace unless stated otherwise.

    // The '+' command expects one of the following value types:
    // String, StringName, PackedStringArray, Array, or Dictionary.
    // Using a Dictionary allows emplace_or_replace of multiple components at once.
    // key(component name) -> value(component data).
    diff["+"] = "Empty"; // emplace_or_replace Empty{}

    // The '-' command expects:
    // String, StringName, PackedStringArray, or Array.
    diff["-"] = "Data"; // remove Data

    // The '+-' or '-+' commands expect a Dictionary value.
    // They allow combining add and remove operations in a single diff.
    // This is particularly useful in the remote runtime view, where adding a single key pair
    // can add or remove multiple components at once.
    // Note: In Godot 4.6.x, Dictionary add and remove operations are currently broken in the remote
    // view (see issue: https://github.com/godotengine/godot/issues/116748).

    e_node->components_diff(diff);

    memdelete(e_node);
}
```
