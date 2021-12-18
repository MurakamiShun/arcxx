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

        static constexpr bool has_column_name() noexcept { return requires {Attribute::column_name;}; }
        static constexpr bool has_constraints() noexcept { return requires {Attribute::constraints;}; }
        [[nodiscard]] static constexpr auto column_full_name();

        // constexpr std::type_info::operator== is C++23
        inline static const constraint not_null = [](const std::optional<Type>& t) constexpr { return static_cast<bool>(t); };
        inline static const constraint unique = [](const std::optional<Type>&) constexpr { return true; };
        inline static const constraint primary_key = [](const std::optional<Type>& t) { return not_null(t) && unique(t); };
        struct constraint_default_value_impl{
            const Type default_value;
            constexpr bool operator()(const std::optional<Type>&){ return true; }
        };
        [[nodiscard]] static const constraint default_value(const Type& def_val);

        [[nodiscard]] static bool has_constraint(const constraint& c) noexcept;
        template<typename Constraint>
        [[nodiscard]]static const Constraint* get_constraint();


        constexpr attribute_common() {}
        template<std::convertible_to<std::optional<Type>> T>
        constexpr attribute_common(const T& default_value) : data(static_cast<std::optional<Type>>(default_value)) {}
        template<std::convertible_to<std::optional<Type>> T>
        constexpr attribute_common(T&& default_value) : data(static_cast<std::optional<Type>>(std::move(default_value))) {}

        [[nodiscard]] constexpr bool is_valid() const;
        explicit constexpr operator bool() const noexcept { return static_cast<bool>(data); }
        [[nodiscard]] const Type& value() const& { return data.value(); }
        [[nodiscard]] Type& value()& { return data.value(); }
        [[nodiscard]] Type&& value()&& { return std::move(data.value()); }

        [[nodiscard]] constexpr operator const std::optional<Type>&() const& noexcept { return data; }
        [[nodiscard]] constexpr operator std::optional<Type>&()& noexcept { return data; }
        [[nodiscard]] constexpr operator std::optional<Type>()&& noexcept { return std::move(data); }

        template<std::convertible_to<std::optional<Type>> T>
        [[nodiscard]] constexpr bool operator==(const T& cmp) const {
            return data == static_cast<std::optional<Type>>(cmp);
        }

        template<std::convertible_to<Attribute>... Attrs>
        static auto in(const Attrs&... values);

        constexpr static const struct {
        private:
            template<std::size_t N>
            static auto make_condition(Attribute&& attr, built_in_string_literal<N> op){
                query_condition<std::tuple<Attribute>> ret;
                ret.condition.push_back(concat_strings(Attribute::column_full_name(), op));
                ret.condition.push_back(0UL);
                ret.bind_attrs = std::make_tuple(std::move(attr));
                return ret;
            }
        public:
            auto operator==(const std::convertible_to<std::optional<Type>> auto& attr) const {
                return make_condition(Attribute{ static_cast<std::optional<Type>>(attr) }, " = ");
            }
            auto operator!=(const std::convertible_to<std::optional<Type>> auto& attr) const {
                return make_condition(Attribute{ static_cast<std::optional<Type>>(attr) }, " != ");
            }
            auto operator<(const std::convertible_to<std::optional<Type>> auto& attr) const {
                return make_condition(Attribute{ static_cast<std::optional<Type>>(attr) }, " < ");
            }
            auto operator<=(const std::convertible_to<std::optional<Type>> auto& attr) const {
                return make_condition(Attribute{ static_cast<std::optional<Type>>(attr) }, " <= ");
            }
            auto operator>(const std::convertible_to<std::optional<Type>> auto& attr) const {
                return make_condition(Attribute{ static_cast<std::optional<Type>>(attr) }, " > ");
            }
            auto operator>=(const std::convertible_to<std::optional<Type>> auto& attr) const {
                return make_condition(Attribute{ static_cast<std::optional<Type>>(attr) }, " >= ");
            }
        } cmp{};

        template<specialized_from<std::tuple> BindAttrs>
        [[nodiscard]] auto operator&&(query_condition<BindAttrs>&& cond) const {
            return (cmp == data) && cond;
        }
        template<specialized_from<std::tuple> BindAttrs>
        [[nodiscard]] auto operator||(query_condition<BindAttrs>&& cond) const {
            return (cmp == data) || cond;
        }
        template<typename Attr>
        requires std::derived_from<Attr, attribute_common<typename Attr::model_type, typename Attr::attribute_type, typename Attr::value_type>>
        [[nodiscard]] auto operator&&(const Attr& attr) const {
            return (cmp == data) && (Attr::cmp == attr);
        }
        template<typename Attr>
        requires std::derived_from<Attr, attribute_common<typename Attr::model_type, typename Attr::attribute_type, typename Attr::value_type>>
        [[nodiscard]] auto operator||(const Attr& attr) const {
            return (cmp == data) || (Attr::cmp == attr);
        }

        struct count : public attribute_aggregator<Model, Attribute, count>{
            static constexpr auto aggregation_func = "count";
        };
    };
}

#include "attribute_common_impl.ipp"