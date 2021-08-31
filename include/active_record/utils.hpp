#pragma once
#include <concepts>
#include <string>
#include <string_view>
#include <type_traits>
#include <chrono>
#include <tuple>
#include <functional>
#include <variant>
#include <any>
#include <vector>
#include <array>
#include <utility>
#include <optional>
#include <charconv>
#include <unordered_map>
#include <numeric>
#include <bit>

#include <iostream>

namespace active_record{
    using string = std::string;
    using string_view = std::basic_string_view<typename active_record::string::value_type>;
    // utc_clock is not implements any compiler.
    //using datetime = std::chrono::utc_clock;
    using datetime = std::chrono::system_clock;

    template <class ContainerType>
    concept Container = requires(ContainerType a, const ContainerType b)
    {
        requires std::regular<ContainerType>;
        requires std::swappable<ContainerType>;
        requires std::destructible<typename ContainerType::value_type>;
        requires std::same_as<typename ContainerType::reference, typename ContainerType::value_type &>;
        requires std::same_as<typename ContainerType::const_reference, const typename ContainerType::value_type &>;
        requires std::forward_iterator<typename ContainerType::iterator>;
        requires std::forward_iterator<typename ContainerType::const_iterator>;
        requires std::signed_integral<typename ContainerType::difference_type>;
        requires std::same_as<typename ContainerType::difference_type, typename std::iterator_traits<typename ContainerType::iterator>::difference_type>;
        requires std::same_as<typename ContainerType::difference_type, typename std::iterator_traits<typename ContainerType::const_iterator>::difference_type>;
        { a.begin() } -> std::same_as<typename ContainerType::iterator>;
        { a.end() } -> std::same_as<typename ContainerType::iterator>;
        { b.begin() } -> std::same_as<typename ContainerType::const_iterator>;
        { b.end() } -> std::same_as<typename ContainerType::const_iterator>;
        { a.cbegin() } -> std::same_as<typename ContainerType::const_iterator>;
        { a.cend() } -> std::same_as<typename ContainerType::const_iterator>;
        { a.size() } -> std::same_as<typename ContainerType::size_type>;
        { a.max_size() } -> std::same_as<typename ContainerType::size_type>;
        { a.empty() } -> std::same_as<bool>;
    };

    template<class TupleType>
    concept Tuple = requires {
        std::tuple_size<TupleType>::value;
        // { std::bool_constant<std::is_standard_layout_v<TupleType>>{} } -> std::same_as<std::bool_constant<false>>;
    };

    template<typename T>
    concept same_as_vector = std::same_as<T, std::vector<typename T::value_type>>;

    template<typename T>
    concept same_as_unordered_map = std::same_as<T, std::unordered_map<typename T::key_type, typename T::mapped_type>>;

    namespace detail{
        /*
         * Here is why not use lambda
         * https://gcc.gnu.org/bugzilla/show_bug.cgi?id=100594
         */
        template<template<typename>typename W, template<typename...>typename TupleType, typename... Elm>
        auto apply_to_elements_impl(TupleType<Elm...>) ->TupleType<W<Elm>...>;
    }
    template<typename T, template<typename>typename W>
    using apply_to_elements_t = decltype(detail::apply_to_elements_impl<W>(std::declval<T>()));

    template<class... Tuples>
    using tuple_cat_t = decltype(std::tuple_cat(std::declval<Tuples>()...));

    template<class Tuple, std::size_t... I>
    [[nodiscard]] constexpr auto tuple_slice(Tuple& t, std::index_sequence<I...>&&) noexcept {
        return std::tie(std::get<I>(t)...);
    }
    template<class Tuple, std::size_t... I>
    [[nodiscard]] constexpr auto tuple_slice(Tuple&& t, std::index_sequence<I...>&&) {
        return std::make_tuple(std::get<I>(t)...);
    }

    template<std::size_t from, std::size_t to>
    [[nodiscard]] constexpr auto make_index_sequence_between() noexcept {
        return []<std::size_t... I>(std::index_sequence<I...>&&){
            return std::index_sequence<(I + from)...>{};
        }(std::make_index_sequence<to-from>());
    }
    
    namespace detail {
        // non const
        template<std::size_t I>
        [[nodiscard]] constexpr auto indexed_apply_aux(std::tuple<>&&) noexcept {
            return std::make_tuple();
        }
        template<std::size_t I>
        [[nodiscard]] constexpr auto indexed_apply_aux(std::tuple<>&) noexcept {
            return std::make_tuple();
        }
        template<std::size_t I, class Head, class... Tail>
        [[nodiscard]] constexpr auto indexed_apply_aux(std::tuple<Head&, Tail&...>&& t) noexcept {
            return std::tuple_cat(
                std::make_tuple(std::make_pair(I, std::ref(std::get<0>(t)))),
                indexed_apply_aux<I+1>(tuple_slice(
                    t,
                    make_index_sequence_between<1, sizeof...(Tail)+1>()
                ))
            );
        }
        template<std::size_t I, class Head, class... Tail>
        [[nodiscard]] constexpr auto indexed_apply_aux(std::tuple<Head, Tail...>& t) noexcept {
            return std::tuple_cat(
                std::make_tuple(std::make_pair(I, std::ref(std::get<0>(t)))),
                indexed_apply_aux<I+1>(tuple_slice(
                    t,
                    make_index_sequence_between<1, sizeof...(Tail)+1>()
                ))
            );
        }
        // const overload
        template<std::size_t I>
        [[nodiscard]] constexpr auto indexed_apply_aux(const std::tuple<>&) noexcept {
            return std::make_tuple();
        }
        template<std::size_t I, class Head, class... Tail>
        [[nodiscard]] constexpr auto indexed_apply_aux(const std::tuple<const Head&, const Tail&...>&& t) noexcept {
            return std::tuple_cat(
                std::make_tuple(std::make_pair(I, std::cref(std::get<0>(t)))),
                indexed_apply_aux<I+1>(tuple_slice(
                    t,
                    make_index_sequence_between<1, sizeof...(Tail)+1>()
                ))
            );
        }
        template<std::size_t I, class Head, class... Tail>
        [[nodiscard]] constexpr auto indexed_apply_aux(const std::tuple<Head, Tail...>& t) noexcept {
            return std::tuple_cat(
                std::make_tuple(std::make_pair(I, std::cref(std::get<0>(t)))),
                indexed_apply_aux<I+1>(tuple_slice(
                    t,
                    make_index_sequence_between<1, sizeof...(Tail)+1>()
                ))
            );
        }
    }

    template<class F, class Tuple>
    constexpr decltype(auto) indexed_apply(F&& f, Tuple&& t){
        return std::apply(f, detail::indexed_apply_aux<0>(t));
    }

    [[nodiscard]] inline active_record::string sanitize(const active_record::string& src) {
        active_record::string result;
        result.reserve(src.length());
        for(const auto& c : src) {
            switch(c) {
                case '\'':
                    result += "\'\'";
                    break;
                case '\\':
                    result += "\\\\";
                    break;
                default:
                    result += c;
                    break;
            }
        };
        return result;
    }

    template<typename... Strings>
    requires requires (Strings... strs){
        (std::convertible_to<Strings, active_record::string_view> && ...);
    }
    [[nodiscard]] constexpr active_record::string concat_strings(const Strings&... strings) {
        if (std::is_constant_evaluated()) std::cout << "constant evaluated!!!" << std::endl;
        const std::array<active_record::string_view, sizeof...(Strings)> str_views = { static_cast<active_record::string_view>(strings)... };
        active_record::string buff;
        buff.reserve(std::transform_reduce(
            str_views.begin(), str_views.end(), static_cast<std::size_t>(0),
            [](auto acc, const auto len){ return acc += len; },
            [](const auto& str){ return str.length(); }
        ));
        for(const auto& str : str_views) buff += str;
        return buff;
    }
    
    template<typename CharT, std::size_t N>
    struct basic_string_literal {
        using traits_type = std::char_traits<CharT>;
        CharT data[N] = {0}; // including null charactor
        static constexpr std::size_t size() noexcept { return N; } 
        consteval basic_string_literal() noexcept = default;
        consteval basic_string_literal(const CharT (&arg)[N]) noexcept { traits_type::copy(data, arg, N); }
        constexpr operator std::basic_string_view<CharT>() const noexcept {
            return std::basic_string_view<CharT>{ data, N-1 };
        }
        operator std::basic_string<CharT>() const {
            return std::basic_string<CharT>{ data, N-1 };
        }
        constexpr const CharT (&c_str() const noexcept)[N]{
            return data;
        }
        template<std::size_t M>
        consteval basic_string_literal<CharT, N+M-1> operator+(const basic_string_literal<CharT, M>& arg) const noexcept {
            basic_string_literal<CharT, N+M-1> tmp;
            traits_type::copy(tmp.data,         data,     N - 1); // remove null charactor
            traits_type::copy(tmp.data + N - 1, arg.data, M);
            return tmp;
        }
    };

    template<std::size_t N>
    using string_literal = basic_string_literal<typename active_record::string::value_type, N>;

    template<std::size_t N>
    using built_in_string_literal = const typename active_record::string::value_type (&)[N];

    template<std::size_t... Ns>
    [[nodiscard]] consteval auto concat_strings(built_in_string_literal<Ns>... strings) {
        return (... + string_literal{ strings });
    }
}