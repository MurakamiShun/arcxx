#pragma once
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
#include "../attribute.hpp"
#include "../query_impl/query_condition.hpp"
#include "attribute_aggregator.hpp"

namespace active_record {
    template<typename Model, typename Attribute, typename Type>
    class attribute_common {
    private:
        std::optional<Type> data;

        struct has_column_name_impl {
            template<typename S>
            static decltype(S::column_name, std::true_type{}) check(S);
            static std::false_type check(...);
            static constexpr bool value = decltype(check(std::declval<Attribute>()))::value;
        };

        struct has_constraints_impl {
            template<typename S>
            static decltype(S::constraints, std::true_type{}) check(S);
            static std::false_type check(...);
            static constexpr bool value = decltype(check(std::declval<Attribute>()))::value;
        };

        template<std::size_t I, typename Condition, std::convertible_to<Attribute> Last>
        static void copy_and_set_attrs_to_condition(Condition& ret, const Last& last);

        template<std::size_t I, typename Condition, std::convertible_to<Attribute> Head, std::convertible_to<Attribute>... Tails>
        static void copy_and_set_attrs_to_condition(Condition& ret, const Head& head, const Tails&... tails);
    public:
        using constraint = std::function<bool(const std::optional<Type>&)>;
        using model_type = Model;
        using attribute_type = Attribute;
        using value_type = Type;

        static constexpr bool has_column_name = has_column_name_impl::value;
        static constexpr bool has_constraints = has_constraints_impl::value;
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
        static const std::optional<std::reference_wrapper<const constraint>> get_constraint(const constraint& c);

        inline static const bool is_primary_key =  [](){
            return has_constraint(primary_key);
        }();

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

        template<Tuple BindAttrs>
        [[nodiscard]] auto operator&&(query_condition<BindAttrs>&& cond) const {
            return (cmp == data) && cond;
        }
        template<Tuple BindAttrs>
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