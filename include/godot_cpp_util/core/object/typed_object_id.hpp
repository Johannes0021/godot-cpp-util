/**
 * TypedObjectID is a convenience wrapper for a Godot ObjectID.
 *
 * This template class stores a Godot ObjectID and provides helper functions to retrieve the object
 * as type T. It does not enforce that the ObjectID actually points to an object of type T, so
 * retrieving the object may return nullptr if the cast fails or the object no longer exists.
 *
 * Usage Example:
 *
 * #pragma once
 *
 * #include "godot_cpp/classes/animation_player.hpp"
 * #include "godot_cpp/classes/node2d.hpp"
 *
 * #include "godot_cpp_util/core/object/typed_object_id.hpp"
 *
 * using namespace godot;
 *
 * class CppMyObject : public Node {
 *     GDCLASS(CppMyObject, Node)
 *
 * public:
 *     TypedObjectID<AnimationPlayer> m_anim_player{};
 *
 * public:
 *     void set_animation_player(const Variant &p_animation_player) {
 *         m_anim_player.set(p_animation_player);
 *     }
 *
 *     Variant get_animation_player() const {
 *         return m_anim_player.try_get().ptr();
 *     }
 *
 *     virtual void _ready() override {
 *         m_anim_player.try_get().match_inspect(
 *             [](auto &anim_player) { anim_player.play("run"); },
 *             []() { ERR_PRINT("No valid AnimationPlayer node has been set."); }
 *         );
 *     }
 *
 * protected:
 *     static void _bind_methods() {
 *         ClassDB::bind_method(
 *             D_METHOD("set_animation_player", "p_animation_player"),
 *             &CppMyObject::set_animation_player
 *         );
 *         ClassDB::bind_method(
 *             D_METHOD("get_animation_player"),
 *             &CppMyObject::get_animation_player
 *         );
 *
 *         ADD_PROPERTY(
 *             PropertyInfo(
 *                 Variant::OBJECT,
 *                 "animation_player",
 *                 PROPERTY_HINT_NODE_TYPE,
 *                 AnimationPlayer::get_class_static()
 *             ),
 *             "set_animation_player",
 *             "get_animation_player"
 *         );
 *     }
 *
 * };
 */



#pragma once



#include "godot_cpp/core/object.hpp"

#include "godot_cpp_util/core/ptr.hpp"



namespace godot {



/**
 * @brief TypedObjectID is a convenience wrapper for a Godot ObjectID.
 *
 * This template class stores a Godot ObjectID and provides helper functions to retrieve the object
 * as type T. It does not enforce that the ObjectID actually points to an object of type T, so
 * retrieving the object may return nullptr if the cast fails or the object no longer exists.
 *
 * @tparam T The Godot object type used for casting. Must inherit from godot::Object.
 */
template <typename T>
requires std::derived_from<T, Object>
class TypedObjectID final {

private:
    ObjectID m_id{};



public:
    // Initializes with an empty ObjectID.
    TypedObjectID() = default;



    /**
     * @brief Constructor from an ObjectID.
     * @param p_id The ObjectID to wrap.
     */
    TypedObjectID(ObjectID p_id) {
        set(p_id);
    }



    /**
     * @brief Constructor from a Variant.
     * @param p_variant Variant to extract the ObjectID from.
     */
    TypedObjectID(const Variant &p_variant) {
        set(p_variant);
    }



    /**
     * @brief Sets the ObjectID.
     * @param p_id The new ObjectID to store.
     */
    void set(ObjectID p_id) {
        m_id = p_id;
    }



    // Resets the stored ObjectID to an empty value.
    void reset() {
        m_id = ObjectID{};
    }



    // Returns the underlying ObjectID.
    ObjectID get_id() const {
        return m_id;
    }



    /**
     * @brief Attempts to retrieve the referenced object.
     *
     * This function casts the object from ObjectDB to type T. If the object no longer exists or is
     * of the wrong type, returns Ptr<T>{nullptr}.
     *
     * @return Ptr<T> to the object or Ptr<T>{nullptr} if unavailable.
     */
    Ptr<T> try_get() const {
        return Ptr{Object::cast_to<T>(ObjectDB::get_instance(m_id))};
    }

};



} // namespace godot
