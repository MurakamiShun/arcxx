#pragma once
#include <concepts>
#include <string>
#include <string_view>
#include <type_traits>
#include <chrono>
#include <tuple>
#include <functional>
#include <variant>
#include <vector>
#include <array>
#include <utility>
#include <optional>
#include <charconv>
#include <unordered_map>
#include <numeric>
#include <stdexcept>

#ifdef _MSC_VER
#include <format>
#endif

#include "third_party/tuptup.hpp"
#include "third_party/tl/expected.hpp"
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */

namespace active_record{
    using string = std::string;
    using string_view = std::basic_string_view<typename active_record::string::value_type>;

    template <class T, class E>
    using expected = tl::expected<T, E>;
    template<typename E>
    decltype(auto) make_unexpected(E&& e){
        return tl::make_unexpected(std::forward<E>(e));
    }

    namespace detail{
        template<typename T, template<typename...>typename U>
        struct specialized_from_impl {
            static constexpr bool value = false;
        };
        template<template<typename...>typename T, template<typename...>typename U, typename... Args>
        requires std::same_as<T<Args...>, U<Args...>>
        struct specialized_from_impl<T<Args...>, U>{
            static constexpr bool value = true;
        };
    }

    // Whether T is template specialization from U
    template<typename T, template<typename...>typename U>
    concept specialized_from = detail::specialized_from_impl<std::remove_cvref_t<T>, U>::value;

    template<specialized_from<std::variant> Variant, class... Lambdas>
    constexpr decltype(auto) visit_by_lambda(Variant&& var, Lambdas&&... lambdas){
        struct : public Lambdas...{
            using Lambdas::operator()...;
        } visitor{std::forward<Lambdas>(lambdas)...};
        return std::visit(visitor, std::forward<Variant>(var));
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
    [[nodiscard]] active_record::string concat_strings(Strings&&... strings) {
        const std::array<active_record::string_view, sizeof...(Strings)> str_views = { static_cast<active_record::string_view>(strings)... };
        active_record::string buff;
        buff.reserve(std::transform_reduce(
            str_views.begin(), str_views.end(), static_cast<std::size_t>(0),
            [](auto acc, const auto len) noexcept { return acc += len; },
            [](const auto& str) noexcept { return str.length(); }
        ));
        for(const auto& str : str_views) buff += str;
        return buff;
    }
    
    template<typename CharT, std::size_t N>
    struct basic_string_literal {
        using traits_type = std::char_traits<CharT>;
        const CharT data[N]; // including null charactor
        static constexpr std::size_t size() noexcept { return N; } 
        consteval basic_string_literal() = delete;
        template<std::size_t... I>
        consteval basic_string_literal(const CharT* const arg, [[maybe_unused]]std::index_sequence<I...>) noexcept : data{ arg[I]... } {}
        consteval basic_string_literal(const CharT (&arg)[N]) noexcept : basic_string_literal(arg, std::make_index_sequence<N>{}) {}
        
        constexpr operator std::basic_string_view<CharT>() const {
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
            CharT tmp[N+M-1];
            for(std::size_t i = 0; i < N-1; ++i) tmp[i] = data[i];
            for(std::size_t i = 0; i < M; ++i) tmp[i+N-1] = arg.data[i];
            return basic_string_literal<CharT, N+M-1>(tmp);
        }
    };


    template<std::size_t N>
    using string_literal = basic_string_literal<typename active_record::string::value_type, N>;

    template<std::size_t N>
    using built_in_string_literal = const typename active_record::string::value_type (&)[N];

    template<std::size_t... Ns>
    [[nodiscard]] consteval auto concat_strings(built_in_string_literal<Ns>... strings) noexcept {
        return (... + string_literal<Ns>{ strings });
    }
}