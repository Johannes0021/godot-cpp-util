/**
 * Requires C++20 standard.
 *
 * Provides a shared Godot StringName that is lazily initialized on first access and can be used as
 * a static variable.
 */



#pragma once



#include "godot_cpp/variant/string_name.hpp"



namespace godot {



//==================================================================================================
// StaticStringNameData struct
//==================================================================================================

/**
 * Compile time container for a string literal.
 * Used as a non type template parameter.
 */
template<std::size_t N>
struct StaticStringNameData final {

    char c_str[N]{};



    /// Copies the string literal.
    constexpr StaticStringNameData(const char(&p_name)[N]) {
        for(std::size_t i = 0; i < N; ++i) {
            c_str[i] = p_name[i];
        }
    }

};



//==================================================================================================
// StaticStringName struct
//==================================================================================================

/**
 * Provides a shared Godot StringName that is lazily initialized on first access and can be used as
 * a static variable.
 */
template<StaticStringNameData Name>
struct StaticStringName final {

    /// Returns the shared StringName instance.
    static const StringName& get() {
        static const StringName sname{Name.c_str, true};

        return sname;
    }



    /// Implicit conversion to const StringName&.
    operator const StringName&() const {
        return get();
    }

};



} // namespace godot
