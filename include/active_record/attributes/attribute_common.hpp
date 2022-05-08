#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
#include "../attribute.hpp"
#include "../query_impl/query_condition.hpp"
#include "attribute_aggregator.hpp"

namespace active_record {
    template<typename Model, typename Attribute, typename Type>
    struct attribute_common : public std::optional<Type>{
        using model_type = Model;
        using attribute_type = Attribute;

        using std::optional<Type>::optional;
        constexpr attribute_common() = default;
        constexpr attribute_common(std::convertible_to<std::optional<Type>> auto&&);

        static constexpr bool has_column_name() noexcept;
        static constexpr bool has_constraints() noexcept;
        [[nodiscard]] static constexpr auto column_full_name();

        struct not_null_impl;
        struct unique_impl;
        struct primary_key_impl;
        struct default_value_impl;
        static constexpr auto not_null    = std::tuple<not_null_impl>{};
        static constexpr auto unique      = std::tuple<unique_impl>{};
        static constexpr auto primary_key = std::tuple<primary_key_impl>{};
        static constexpr auto default_value = [](const std::convertible_to<Type> auto& def_val) constexpr {
            return std::make_tuple(default_value_impl{ {}, def_val });
        };

        template<typename Constraint>
        [[nodiscard]] static constexpr bool has_constraint(Constraint) noexcept;
        template<typename Constraint>
        [[nodiscard]] static constexpr const std::tuple_element_t<0, Constraint>* get_constraint(Constraint) noexcept;

        [[nodiscard]] constexpr bool is_valid() const;

        template<std::convertible_to<Attribute>... Attrs>
        static auto in(const Attrs&... values);

        constexpr static struct compare_type {
            auto operator==(const std::convertible_to<std::optional<Type>> auto&) const;
            auto operator!=(const std::convertible_to<std::optional<Type>> auto&) const;
            auto operator<(const std::convertible_to<std::optional<Type>> auto&) const;
            auto operator<=(const std::convertible_to<std::optional<Type>> auto&) const;
            auto operator>(const std::convertible_to<std::optional<Type>> auto&) const;
            auto operator>=(const std::convertible_to<std::optional<Type>> auto&) const;
        } cmp{};

        struct count : public attribute_aggregator<Model, Attribute, count>{
            static constexpr auto aggregation_func = "count";
        };
    };

    template<is_attribute Attribute1, is_attribute Attribute2>
    requires requires(const typename Attribute1::value_type a, const typename Attribute2::value_type b){ { a == b }-> std::same_as<bool>; }
    inline constexpr bool operator==(const Attribute1& a, const Attribute2& b){
        return static_cast<const std::optional<typename Attribute1::value_type>&>(a) == static_cast<const std::optional<typename Attribute2::value_type>&>(b);
    }
}

#include "attribute_common_impl.ipp"