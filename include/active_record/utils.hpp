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
#include <bit>

#include "third_party/tuptup.hpp"
/*
 * Copyright 2021 akisute514
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

namespace active_record{
    using string = std::string;
    using string_view = std::basic_string_view<typename active_record::string::value_type>;
    // utc_clock is not implements in gcc.
    //using datetime = std::chrono::utc_clock;
    using datetime = std::chrono::system_clock;

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