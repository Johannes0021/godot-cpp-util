# Code Example



```cpp
#pragma once



#include "godot_cpp_util/ecs/ecs.hpp"



using namespace godot;



//==================================================================================================
// Entity definition
//==================================================================================================

// Defines an ECS entity named E_Node that inherits from Godot's Node.
GD_ECS_ENTITY(ECS, E_Node, Node)



//==================================================================================================
// Component: Data
//==================================================================================================

// A component that stores basic example data.
struct Data {
    int id{21};
    String name{"SomeName"};
    float length{21.21f};
    Dictionary meta{};

    // Default constructor is required by the ECS.
    Data() = default;

    // Descriptor used by the ECS to expose this component to the Godot editor.
    // It defines how fields are interpreted, serialized, and edited.
    static const auto& descriptor() {
        static const auto descriptor = C_Descriptor{
            // "ComponentName", // Defaults to an empty string if not explicitly specified.
            // When registered via a resource, this name is used directly. If it is empty, the name
            // is taken from GD_ECS_COMPONENT, for example GD_ECS_COMPONENT(ECS, C_Data, Data)
            // assigns the name "Data".

            // Field with explicit PropertyInfo and setter and getter.
            C_Field{&Data::id, PropertyInfo(Variant::Type::INT, "id"), "set_id", "get_id"},

            // Field using a simplified constructor with Variant type.
            C_Field{&Data::name, Variant::Type::STRING, "name", "set_name", "get_name"},

            // Field with:
            // - PropertyInfo
            // - "set_length"
            // - "get_length"
            C_Field{&Data::length, PropertyInfo(Variant::Type::FLOAT, "length")},

            // Field with:
            // - PropertyInfo(Variant::Type::DICTIONARY, "meta")
            // - "set_meta"
            // - "get_meta"
            C_Field{&Data::meta, Variant::Type::DICTIONARY, "meta"}
        };

        return descriptor;
    }
};

/**
 * Defines a Resource wrapper for the Data component.
 * This allows the component to be created, stored, and edited as a Godot Resource.
 * Entities such as E_Node can add and manage this component themselves.
 */
GD_ECS_COMPONENT(ECS, C_Data, Data)



//==================================================================================================
// Component: Empty
//==================================================================================================

// A component with no data.
// Useful as a marker or tag component.
struct Empty {
    // Default constructor is required by the ECS.
    Empty() = default;

    // Generates an empty component descriptor.
    // The component name defaults to an empty string unless explicitly specified.
    // When registered via a resource, this name is used directly. If it is empty, the name is taken
    // from GD_ECS_COMPONENT, for example GD_ECS_COMPONENT(ECS, C_Empty, Empty) assigns the name
    // "Empty".
    GD_ECS_EMPTY_COMPONENT_DESCRIPTOR(Empty)

    // Alternative with a custom component name.
    //GD_ECS_EMPTY_COMPONENT_DESCRIPTOR(Empty, "ComponentName")
};

/**
 * Resource wrapper for the Empty component.
 * Allows attaching the component to entities via the editor or code.
 */
GD_ECS_COMPONENT(ECS, C_Empty, Empty)


//==================================================================================================
// Type registration
//==================================================================================================

inline void register_types() {
    // Register core ECS types.
    ECS::register_types();

    // Register entities.
    E_Node::register_types();

    // Register component resources.
    C_Data::register_types();
    C_Empty::register_types();

    // If a component does not need to be exposed as a Resource,
    // it can be registered directly instead.
    //ECS::register_type<Data>("ComponentName");
    // This is internally handled by C_Data::register_types().

    // The advantage of exposing a component as a Godot Resource is that it can be edited in a type
    // safe way inside the editor. All fields are known at compile time and validated by the engine.
    //
    // If a component is only registered directly with the ECS and not wrapped as a Resource, it can
    // only be accessed through its string based component name and generic Variant data.
    // This approach is more flexible, but it loses type safety and editor support.
    //
    // Components that are not intended for editor usage can still benefit from being registered via
    // ECS::register_type<Data>("ComponentName").
    //
    // Even though they are not exposed as Resources, this registration allows them to appear in
    // debug builds inside the remote inspector view for entities. This makes it possible to inspect
    // and modify their values at runtime, which is especially useful for debugging and development
    // workflows.
    //
    // This provides a lightweight alternative to full Resource exposure while still keeping runtime
    // visibility and editability in debug environments.
    //
    // Using '+', '-', and '+-' commands, multiple components can be added or removed at once.
    // This functionality is specifically intended for the remote runtime view in the editor,
    // allowing entities to be modified live while the game is running (debug build only).
    // Note: In Godot 4.6.x, Dictionary add and remove operations are currently broken in the remote
    // view (see issue: https://github.com/godotengine/godot/issues/116748).
}
```
