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
 * #include <godot_cpp/classes/animation_player.hpp>
 * #include <godot_cpp/classes/node2d.hpp>
 *
 * #include "godot_cpp_util/include/typed_object_id.hpp"
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
 *         return m_anim_player.try_get();
 *     }
 *
 *     virtual void _ready() override {
 *         if (AnimationPlayer *anim_player = m_anim_player.try_get()) {
 *             anim_player->play("run");
 *         }
 *         else {
 *             ERR_PRINT("No valid AnimationPlayer node has been set.");
 *         }
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



#include <cstdlib>
#include <iostream>

#include <godot_cpp/core/object.hpp>



using namespace godot;



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
class TypedObjectID {
    static_assert(std::is_base_of_v<Object, T>, "T must inherit from Object.");



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
     * of the wrong type, returns nullptr.
     *
     * @return Pointer to the object of type T or nullptr if unavailable.
     */
    T* try_get() const {
        return Object::cast_to<T>(ObjectDB::get_instance(m_id));
    }



    /**
     * @brief Retrieves the referenced object or crashes if invalid.
     *
     * If the object does not exist or the cast fails, the program crashes with the provided
     * message. This is useful for situations where a null reference is considered a critical error.
     *
     * @param p_msg Optional message to display on crash.
     * @return Reference to the object of type T.
     */
    T& get_or_crash(const char *p_msg = "Attempted to access null object!") const {
        T *obj = try_get();
        if (!obj) {
            CRASH_NOW_MSG(p_msg);

            // Fallback crash, just to make this sure.
            std::cerr << p_msg << "\n";
            std::abort();
        }

        return *obj;
    }

};
