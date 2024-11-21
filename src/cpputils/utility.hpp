#pragma once

#include <type_traits>
#include <concepts>
#include <utility>
#include <ostream>

#include <cpputils/type_traits.hpp>

namespace cpputils {

//! Stores either an instance or a reference to T, depending on the constructor argument
template<typename T>
class value_or_reference {
public:
    static constexpr bool holds_reference = std::is_lvalue_reference_v<T>;
    using stored_t = std::conditional_t<holds_reference, T, std::remove_cvref_t<T>>;

    template<std::convertible_to<stored_t> _T>
    constexpr explicit value_or_reference(_T&& value) noexcept
    : _value{std::forward<_T>(value)}
    {}

    constexpr T&& get() && noexcept { return std::move(_value); }
    constexpr auto& get() & noexcept { return _value; }
    constexpr const auto& get() const & noexcept { return _value; }

private:
    stored_t _value;
};

template<typename T>
value_or_reference(T&&) -> value_or_reference<T>;


#ifndef DOXYGEN
namespace detail {

    template<std::size_t I, typename T>
    struct indexed_element {
        using index = index_constant<I>;

        template<typename _T> requires(std::is_same_v<std::remove_cvref_t<T>, std::remove_cvref_t<_T>>)
        constexpr index index_of() const noexcept { return {}; }
        constexpr index index_of(const T&) const noexcept { return {}; }

        constexpr T get(const index&) const noexcept requires(std::is_default_constructible_v<T>) {
            return T{};
        }
    };

    template<typename... Ts>
    struct indexed;
    template<std::size_t... I, typename... Ts>
    struct indexed<std::index_sequence<I...>, Ts...> : indexed_element<I, Ts>... {
        using indexed_element<I, Ts>::index_of...;
        using indexed_element<I, Ts>::get...;
    };

}  // namespace detail
#endif  // DOXYGEN

//! A list of unique types, where each type is assigned a unique index
template<typename... Ts> requires(are_unique_v<Ts...>)
struct indexed : detail::indexed<std::make_index_sequence<sizeof...(Ts)>, Ts...> {};


#ifndef DOXYGEN
namespace detail {

    template<std::size_t I, typename T>
    struct indexed_tuple_element {
        using index = index_constant<I>;

        constexpr indexed_tuple_element(T t) noexcept : _storage{std::forward<T>(t)} {}

        template<typename _T> requires(std::is_same_v<std::remove_cvref_t<T>, std::remove_cvref_t<_T>>)
        constexpr index index_of() const noexcept { return {}; }
        constexpr index index_of(const T&) const noexcept { return {}; }

        constexpr const auto& get(const index&) const noexcept { return _storage.get(); }
        constexpr auto& get(const index&) noexcept { return _storage.get(); }

    private:
        value_or_reference<T> _storage;
    };

    template<typename... Ts>
    struct indexed_tuple;
    template<std::size_t... I, typename... Ts>
    struct indexed_tuple<std::index_sequence<I...>, Ts...> : indexed_tuple_element<I, Ts>... {
        constexpr indexed_tuple(Ts... ts) noexcept : indexed_tuple_element<I, Ts>(std::forward<Ts>(ts))... {}
        using indexed_tuple_element<I, Ts>::index_of...;
        using indexed_tuple_element<I, Ts>::get...;
    };

}  // namespace detail
#endif  // DOXYGEN

//! Stores a set unique types by reference or value and provides access to them via unique indices
template<typename... Ts> requires(are_unique_v<Ts...>)
struct indexed_tuple : detail::indexed_tuple<std::make_index_sequence<sizeof...(Ts)>, Ts...> {
 private:
    using base = detail::indexed_tuple<std::make_index_sequence<sizeof...(Ts)>, Ts...>;

 public:
    constexpr indexed_tuple(Ts... ts) noexcept : base(std::forward<Ts>(ts)...) {}
};

template<typename... Ts>
indexed_tuple(Ts&&...) -> indexed_tuple<Ts...>;

#ifndef DOXYGEN
namespace detail {

    template<std::size_t i, auto v>
    struct value_i {
        static constexpr auto at(index_constant<i>) noexcept {
            return v;
        }
    };

    template<typename I, auto...>
    struct values;
    template<std::size_t... i, auto... v> requires(sizeof...(i) == sizeof...(v))
    struct values<std::index_sequence<i...>, v...> : value_i<i, v>... {
        using value_i<i, v>::at...;
    };

    template<auto... v>
    struct value_list_helper {};

    template<std::size_t i, std::size_t n, auto... values>
    struct drop_n;
    template<std::size_t i, std::size_t n, auto v0, auto... v> requires(i < n)
    struct drop_n<i, n, v0, v...> : drop_n<i+1, n, v...> {};
    template<std::size_t i, std::size_t n, auto... v> requires(i == n)
    struct drop_n<i, n, v...> : std::type_identity<value_list_helper<v...>> {};

}  // namespace detail
#endif  // DOXYGEN

//! Class to represent a list of compile-time values.
template<auto... v>
struct values : detail::values<std::make_index_sequence<sizeof...(v)>, v...> {
    static constexpr std::size_t size = sizeof...(v);

    //! Return a new list with the values from this list, dropping the first n values
    template<std::size_t n> requires(n <= sizeof...(v))
    static constexpr auto drop() noexcept {
        return [] <auto... _v> (const detail::value_list_helper<_v...>&) constexpr {
            return values<_v...>{};
        }(typename detail::drop_n<0, n, v...>::type{});
    }

    //! Return a new list with the values from this list, dropping the last n values
    template<std::size_t n> requires(n <= sizeof...(v))
    static constexpr auto crop() noexcept {
        return [] <std::size_t... i> (const std::index_sequence<i...>&) constexpr {
            return values<at(index_constant<i>{})...>{};
        }(std::make_index_sequence<sizeof...(v) - n>{});
    }

    //! Return a new list with the first n values from this list
    template<std::size_t n> requires(n <= sizeof...(v))
    static constexpr auto take() noexcept {
        return [] <std::size_t... i> (const std::index_sequence<i...>&) constexpr {
            return values<at(index_constant<i>{})...>{};
        }(std::make_index_sequence<n>{});
    }

    //! Return the first value in the list
    static constexpr auto first() noexcept {
        return at(index_constant<0>{});
    }

    //! Return the last value in the list
    static constexpr auto last() noexcept {
        return at(index_constant<size-1>{});
    }

    //! Return the value at the given index in the list
    template<std::size_t i> requires(i < size)
    static constexpr auto at(index_constant<i> idx) noexcept {
        using base = detail::values<std::make_index_sequence<size>, v...>;
        return base::at(idx);
    }

    //! Perform a reduction operation on this list
    template<typename op, typename T>
    static constexpr auto reduce_with(op&& action, T&& initial) noexcept {
        return _reduce_with(std::forward<op>(action), std::forward<T>(initial), v...);
    }

    //! Concatenate this list with another one
    template<auto... _v>
    constexpr auto operator+(const values<_v...>&) const {
        return values<v..., _v...>{};
    }

    //! Test this list for equality with another one
    template<auto... _v>
    constexpr bool operator==(const values<_v...>&) const {
        if constexpr (sizeof...(_v) == size)
            return std::conjunction_v<is_equal<v, _v>...>;
        return false;
    }

    //! Write this list to the given output stream
    friend std::ostream& operator<<(std::ostream& s, const values&) {
        if constexpr (size > 0)
            _write_to<v...>(s);
        return s;
    }

 private:
    template<auto v0, auto... vs>
    static void _write_to(std::ostream& s) {
        s << std::to_string(v0);
        (..., (s << ", " << std::to_string(vs)));
    }

    template<typename op, typename T>
    static constexpr auto _reduce_with(op&&, T&& initial) noexcept {
        return std::forward<T>(initial);
    }

    template<typename op, typename T, typename V0, typename... V>
    static constexpr auto _reduce_with(op&& action, T&& initial, V0&& v0, V&&... values) noexcept {
        auto next = action(std::forward<T>(initial), std::forward<V0>(v0));
        if constexpr (sizeof...(V) == 0) {
            return next;
        } else {
            return _reduce_with(std::forward<op>(action), std::move(next), std::forward<V>(values)...);
        }
    }
};

}  // namespace cpputils
