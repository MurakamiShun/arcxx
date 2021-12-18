#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT Lisence.
 */
#include "../attribute.hpp"
#include "../query_impl/query_condition.hpp"
#include "attribute_aggregator.hpp"

namespace active_record {
    template<typename Model, typename Attribute, typename Type>
    class attribute_common {
    private:
        struct compare;
        std::optional<Type> data;
    public:
        using constraint = std::function<bool(const std::optional<Type>&)>;
        using model_type = Model;
        using attribute_type = Attribute;
        using value_type = Type;

        /* This code is not work on MSVC
        static constexpr bool has_column_name = requires {Attribute::column_name;};
        static constexpr bool has_constraints = requires {Attribute::constraints;};
        */

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

        constexpr attribute_common();
        template<std::convertible_to<std::optional<Type>> T>
        constexpr attribute_common(const T& default_value);
        template<std::convertible_to<std::optional<Type>> T>
        constexpr attribute_common(T&& default_value);

        [[nodiscard]] constexpr bool is_valid() const;
        explicit constexpr operator bool() const noexcept;
        [[nodiscard]] const Type& value() const&;
        [[nodiscard]] Type& value()&;
        [[nodiscard]] Type&& value()&&;

        [[nodiscard]] constexpr operator const std::optional<Type>&() const& noexcept;
        [[nodiscard]] constexpr operator std::optional<Type>&()& noexcept;
        [[nodiscard]] constexpr operator std::optional<Type>()&& noexcept;

        template<std::convertible_to<std::optional<Type>> T>
        [[nodiscard]] constexpr bool operator==(const T&) const;
        template<std::convertible_to<std::optional<Type>> T>
        [[nodiscard]] constexpr bool operator!=(const T&) const;

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
}

#include "attribute_common_impl.ipp"