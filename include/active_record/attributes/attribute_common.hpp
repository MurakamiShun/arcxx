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
        using constraint = std::function<bool(const std::optional<Type>&)>;
        using model_type = Model;
        using attribute_type = Attribute;

        using std::optional<Type>::optional;
        constexpr attribute_common() = default;
        constexpr attribute_common(std::convertible_to<std::optional<Type>> auto&&);

        static constexpr bool has_column_name() noexcept;
        static constexpr bool has_constraints() noexcept;
        [[nodiscard]] static constexpr auto column_full_name();


        // constexpr std::type_info::operator== is C++23
        inline static const constraint not_null = [](const std::optional<Type>& t) constexpr { return static_cast<bool>(t); };
        inline static const constraint unique = [](const std::optional<Type>&) constexpr { return true; };
        inline static const constraint primary_key = [](const std::optional<Type>& t) { return not_null(t) && unique(t); };
        struct constraint_default_value_impl;
        [[nodiscard]] static const constraint default_value(const Type& def_val);

        [[nodiscard]] static bool has_constraint(const constraint&) noexcept;
        template<typename Constraint>
        [[nodiscard]] static const Constraint* get_constraint();

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
    requires requires(const typename Attribute1::value_type a, const Attribute2::value_type b){ { a == b }-> std::same_as<bool>; }
    inline constexpr bool operator==(const Attribute1& a, const Attribute2& b){
        return static_cast<const std::optional<typename Attribute1::value_type>&>(a) == static_cast<const std::optional<typename Attribute2::value_type>&>(b);
    }
}

#include "attribute_common_impl.ipp"