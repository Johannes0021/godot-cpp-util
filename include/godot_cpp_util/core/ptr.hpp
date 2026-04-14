/**
 * Ptr is a lightweight pointer wrapper that provides safe access utilities and functional-style
 * transformations over raw pointers.
 *
 * It is designed for code where nullptr checks are frequent and composition of pointer operations
 * is preferred over manual branching.
 *
 * This is not a smart pointer in the RAII sense. It does not manage memory. It is a thin
 * abstraction over raw pointers with added ergonomic helpers.
 */



#pragma once



#include <concepts>
#include <cstdlib>
#include <iostream>

#include "godot_cpp/core/error_macros.hpp"



/**
 * Ptr is a non-owning pointer wrapper that adds safe access and functional-style composition
 * helpers.
 *
 * It behaves similarly to a nullable reference to T, with explicit handling of empty state
 * (nullptr).
 *
 * Ptr does not manage lifetime. It assumes external ownership and guarantees nothing about the
 * validity of the underlying pointer.
 */
template <typename T>
class Ptr final {

public:
    using ElementType = T;



private:
    T* m_ptr = nullptr;



public:
    Ptr() = default;



    Ptr(T *p_ptr)
        : m_ptr(p_ptr)
    {}



    // Sets the pointer.
    void set(T *p_ptr) {
        m_ptr = p_ptr;
    }



    // Gets the raw pointer.
    T* ptr() const {
        return m_ptr;
    }


    [[nodiscard]]
    explicit operator bool() const {
        return m_ptr != nullptr;
    }



    T* operator->() const {
        return &unwrap();
    }



    T& operator*() const {
        return unwrap();
    }



    /**
     * Returns the contained value.
     *
     * Crashes if the value is a nullptr with a custom panic message provided by p_msg.
     */
    T& expect(const char *p_msg) const {
        if (!m_ptr) {
            CRASH_NOW_MSG(p_msg);

            // Fallback crash, just to make this sure.
            std::cerr << p_msg << "\n";
            std::abort();
        }

        return *m_ptr;
    }



    /**
     * Returns the contained value.
     *
     * Crashes if the value is a nullptr.
     */
    T& unwrap() const {
        return expect("Attempted to dereference nullptr!");
    }



    // Returns the contained value if it is not a nullptr, otherwise returns p_other.
    T& unwrap_or(T& p_other) const {
        if (m_ptr) {
            return *m_ptr;
        }

        return p_other;
    }



    // Returns the contained value if it is not a nullptr, otherwise computes it by p_fn.
    template <typename F>
    requires std::same_as<std::invoke_result_t<F>, T&>
    T& unwrap_or_else(F&& p_fn) const {
        if (m_ptr) {
            return *m_ptr;
        }

        return p_fn();
    }



    // Takes the value, leaving a nullptr in its place.
    Ptr<T> take() {
        Ptr<T> other{m_ptr};
        m_ptr = nullptr;

        return other;
    }



    /**
     * Takes the value if and only if the predicate evaluates to true.
     *
     * If this Ptr is empty (m_ptr == nullptr), the predicate is not executed and an empty Ptr is
     * returned.
     */
    template <typename F>
    requires std::same_as<std::invoke_result_t<F, const T&>, bool>
    Ptr<T> take_if(F&& p_fn) {
        if (m_ptr && p_fn(*m_ptr)) {
            return take();
        }

        return Ptr<T>{nullptr};
    }



    /**
     * Calls a function with a reference to the contained value if != nullptr.
     *
     * Returns the original Ptr<T>.
     */
    template <typename F>
    requires std::invocable<F, T&>
    Ptr<T> on_some(F&& p_fn) const {
        if (m_ptr) {
            p_fn(*m_ptr);
        }

        return Ptr<T>{m_ptr};
    }



    /**
     * Calls a function if == nullptr.
     *
     * Returns the original Ptr<T>.
     */
    template <typename F>
    requires std::invocable<F>
    Ptr<T> on_none(F&& p_fn) const {
        if (!m_ptr) {
            p_fn();
        }

        return Ptr<T>{m_ptr};
    }



    /**
     * Calls p_on_some with a reference to the contained value if != nullptr.
     *
     * Calls p_on_none if == nullptr.
     *
     * Returns the original Ptr<T>.
     */
    template <typename F1, typename F2>
    requires std::invocable<F1, T&> && std::invocable<F2>
    Ptr<T> match_inspect(F1&& p_on_some, F2&& p_on_none) const {
        if (m_ptr) {
            p_on_some(*m_ptr);
        }
        else {
            p_on_none();
        }

        return Ptr<T>{m_ptr};
    }



    // Maps Ptr<T> to Ptr<U> by applying a function to a contained value (if != nullptr) or returns
    // Ptr<U>{nullptr} (if nullptr).
    template <typename F>
    requires std::is_pointer_v<std::invoke_result_t<F, T&>>
    auto map(F&& p_fn) -> Ptr<std::remove_pointer_t<std::invoke_result_t<F, T&>>> const {
        using U = std::remove_pointer_t<std::invoke_result_t<F, T&>>;

        if (m_ptr) {
            return Ptr<U>{p_fn(*m_ptr)};
        }

        return Ptr<U>{nullptr};
    }



    // Returns Ptr<U>{nullptr} if the value is a nullptr, otherwise returns p_ptr.
    template <typename U>
    Ptr<U> and_then(Ptr<U> p_ptr) const {
        if (m_ptr) {
            return p_ptr;
        }

        return Ptr<U>{nullptr};
    }



    // Returns Ptr<U>{nullptr} if the value is a nullptr, otherwise calls p_fn with the wrapped
    // value and returns the result.
    template <typename F>
    auto and_then(F&& p_fn) const {
        using Raw = std::invoke_result_t<F, T&>;
        using U = typename Raw::ElementType;

        if (m_ptr) {
            return p_fn(*m_ptr);
        }

        return Ptr<U>{nullptr};
    }



    // Returns the Ptr if it contains a value, otherwise returns p_ptr.
    Ptr<T> or_else(Ptr<T> p_ptr) const {
        if (m_ptr) {
            return Ptr<T>{m_ptr};
        }

        return p_ptr;
    }



    // Returns the Ptr if it contains a value, otherwise calls p_fn and returns the result.
    template <typename F>
    requires std::same_as<std::invoke_result_t<F>, Ptr<T>>
    Ptr<T> or_else(F&& p_fn) const {
        if (m_ptr) {
            return Ptr<T>{m_ptr};
        }

        return p_fn();
    }



    // Converts from Ptr<Ptr<T>> to Ptr<T>.
    template <typename U = T>
    requires requires { typename U::ElementType; }
    Ptr<typename U::ElementType> flatten() const {
        if (m_ptr) {
            return Ptr<typename U::ElementType>{*m_ptr};
        }

        return Ptr<typename U::ElementType>{nullptr};
    }

};



template <typename... Ts>
auto ptr_tuple(std::tuple<Ts...> p_t) {
    return std::apply(
        [](auto... xs) {
            return std::tuple{Ptr{xs}...};
        },
        p_t
    );
}
