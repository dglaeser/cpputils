#pragma once

#include <type_traits>
#include <concepts>
#include <utility>

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

}  // namespace cpputils
