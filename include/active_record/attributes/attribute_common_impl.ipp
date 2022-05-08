#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
namespace active_record{
    template<typename Model, typename Attribute, typename Type>
    struct attribute_common<Model, Attribute, Type>::not_null_impl : constraint<Type>{
        constexpr bool operator()(const std::optional<Type>& t) noexcept { return static_cast<bool>(t); }
    };
    template<typename Model, typename Attribute, typename Type>
    struct attribute_common<Model, Attribute, Type>::unique_impl : constraint<Type>{
        constexpr bool operator()(const std::optional<Type>&) noexcept { return true; }
    };
    template<typename Model, typename Attribute, typename Type>
    struct attribute_common<Model, Attribute, Type>::primary_key_impl : constraint<Type>{
        constexpr bool operator()(const std::optional<Type>& t) noexcept { return not_null(t) && unique(t); }
    };

    template<typename Model, typename Attribute, typename Type>
    struct attribute_common<Model, Attribute, Type>::default_value_impl : constraint<Type>{
        const Type default_value;
        constexpr bool operator()(const std::optional<Type>&) noexcept { return true; }
    };

    namespace detail{
        template<typename T>
        concept is_constraint = specialized_from<T, std::tuple>
            && std::invocable<std::tuple_element_t<0, T>, const std::optional<typename std::tuple_element_t<0, T>::value_type>>
            && std::derived_from<std::tuple_element_t<0, T>, constraint<typename std::tuple_element_t<0, T>::value_type>>;
    }
    template<detail::is_constraint A, detail::is_constraint B>
    requires std::same_as<typename std::tuple_element_t<0, A>::value_type, typename std::tuple_element_t<0, B>::value_type>
    constexpr auto operator&(const A a, const B b){
        return std::tuple_cat(a, b);
    }

    template<typename Model, typename Attribute, typename Type>
    inline constexpr attribute_common<Model, Attribute, Type>::attribute_common(std::convertible_to<std::optional<Type>> auto&& val)
        : std::optional<Type>(std::forward<decltype(val)>(val)) {
    }
    
    template<typename Model, typename Attribute, typename Type>
    inline constexpr bool attribute_common<Model, Attribute, Type>::has_column_name() noexcept {
        return requires {Attribute::column_name;};
    }
    template<typename Model, typename Attribute, typename Type>
    inline constexpr bool attribute_common<Model, Attribute, Type>::has_constraints() noexcept {
        return requires {Attribute::constraints;};
    }
    template<typename Model, typename Attribute, typename Type>
    inline constexpr auto attribute_common<Model, Attribute, Type>::column_full_name() {
        return concat_strings("\"", Model::table_name, "\".\"", Attribute::column_name, "\"");
    }

    template<typename Model, typename Attribute, typename Type>
    template<typename Constraint>
    inline constexpr bool attribute_common<Model, Attribute, Type>::has_constraint([[maybe_unused]]Constraint) noexcept {
        if constexpr (has_constraints()){
            return tuptup::contains_in_tuple<std::tuple_element_t<0, Constraint>, decltype(Attribute::constraints)>::value;
        }
        return false;
    }

    template<typename Model, typename Attribute, typename Type>
    template<typename Constraint>
    inline constexpr const std::tuple_element_t<0, Constraint>* attribute_common<Model, Attribute, Type>::get_constraint(Constraint) noexcept {
        if constexpr(has_constraints()) {
            if constexpr(tuptup::contains_in_tuple<std::tuple_element_t<0, Constraint>, decltype(Attribute::constraints)>::value){
                return &std::get<std::tuple_element_t<0, Constraint>>(Attribute::constraints);
            }
        }
        return nullptr;
    }

    namespace detail{
        template<typename Attribute, std::size_t N>
        inline auto make_condition(Attribute&& attr, built_in_string_literal<N> op){
            query_condition<std::tuple<Attribute>> ret;
            ret.condition.push_back(concat_strings(Attribute::column_full_name(), op));
            ret.condition.push_back(static_cast<std::size_t>(0));
            ret.bind_attrs = std::make_tuple(std::move(attr));
            return ret;
        }

        template<typename Attribute, std::size_t I, typename Condition, std::convertible_to<Attribute> Last>
        inline void copy_and_set_attrs_to_condition(Condition& ret, const Last& last) {
            std::get<I>(ret.bind_attrs) = Attribute{ last };
            ret.condition.push_back(I);
        }

        template<typename Attribute, std::size_t I, typename Condition, std::convertible_to<Attribute> Head, std::convertible_to<Attribute>... Tails>
        inline void copy_and_set_attrs_to_condition(Condition& ret, const Head& head, const Tails&... tails) {
            copy_and_set_attrs_to_condition<Attribute, I+1>(ret, head);
            ret.condition.push_back(",");
            copy_and_set_attrs_to_condition<Attribute, I+1>(ret, tails...);
        }
    }

    template<typename Model, typename Attribute, typename Type>
    template<std::convertible_to<Attribute>... Attrs>
    inline auto attribute_common<Model, Attribute, Type>::in(const Attrs&... values) {
        query_condition<std::tuple<decltype(values, std::declval<Attribute>())...>> ret;
        ret.condition.push_back(concat_strings(
            "\"", Model::table_name, "\".\"",
            Attribute::column_name, "\" in ("
        ));
        detail::copy_and_set_attrs_to_condition<Attribute, 0>(ret, values...);
        ret.condition.push_back(")");
        return ret;
    }
    template<typename Model, typename Attribute, typename Type>
    inline auto attribute_common<Model, Attribute, Type>::compare_type::operator==(const std::convertible_to<std::optional<Type>> auto& attr) const {
        return detail::make_condition(Attribute{ static_cast<std::optional<Type>>(attr) }, " = ");
    }
    template<typename Model, typename Attribute, typename Type>
    inline auto attribute_common<Model, Attribute, Type>::compare_type::operator!=(const std::convertible_to<std::optional<Type>> auto& attr) const {
        return detail::make_condition(Attribute{ static_cast<std::optional<Type>>(attr) }, " != ");
    }
    template<typename Model, typename Attribute, typename Type>
    inline auto attribute_common<Model, Attribute, Type>::compare_type::operator<(const std::convertible_to<std::optional<Type>> auto& attr) const {
        return detail::make_condition(Attribute{ static_cast<std::optional<Type>>(attr) }, " < ");
    }
    template<typename Model, typename Attribute, typename Type>
    inline auto attribute_common<Model, Attribute, Type>::compare_type::operator<=(const std::convertible_to<std::optional<Type>> auto& attr) const {
        return detail::make_condition(Attribute{ static_cast<std::optional<Type>>(attr) }, " <= ");
    }
    template<typename Model, typename Attribute, typename Type>
    inline auto attribute_common<Model, Attribute, Type>::compare_type::operator>(const std::convertible_to<std::optional<Type>> auto& attr) const {
        return detail::make_condition(Attribute{ static_cast<std::optional<Type>>(attr) }, " > ");
    }
    template<typename Model, typename Attribute, typename Type>
    inline auto attribute_common<Model, Attribute, Type>::compare_type::operator>=(const std::convertible_to<std::optional<Type>> auto& attr) const {
        return detail::make_condition(Attribute{ static_cast<std::optional<Type>>(attr) }, " >= ");
    }
}