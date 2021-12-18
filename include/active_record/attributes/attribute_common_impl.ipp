#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT Lisence.
 */
namespace active_record{
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
    struct attribute_common<Model, Attribute, Type>::constraint_default_value_impl{
        const Type default_value;
        constexpr bool operator()(const std::optional<Type>&){ return true; }
    };

    template<typename Model, typename Attribute, typename Type>
    inline constexpr attribute_common<Model, Attribute, Type>::attribute_common() {
    }
    template<typename Model, typename Attribute, typename Type>
    template<std::convertible_to<std::optional<Type>> T>
    inline constexpr attribute_common<Model, Attribute, Type>::attribute_common(const T& default_value)
        : data(static_cast<std::optional<Type>>(default_value)) {
    }
    template<typename Model, typename Attribute, typename Type>
    template<std::convertible_to<std::optional<Type>> T>
    inline constexpr attribute_common<Model, Attribute, Type>::attribute_common(T&& default_value)
        : data(static_cast<std::optional<Type>>(std::move(default_value))) {
    }

    template<typename Model, typename Attribute, typename Type>
    inline constexpr attribute_common<Model, Attribute, Type>::operator bool() const noexcept {
        return static_cast<bool>(data);
    }
    template<typename Model, typename Attribute, typename Type>
    inline const Type& attribute_common<Model, Attribute, Type>::value() const& {
        return data.value();
    }
    template<typename Model, typename Attribute, typename Type>
    inline Type& attribute_common<Model, Attribute, Type>::value()& {
        return data.value();
    }
    template<typename Model, typename Attribute, typename Type>
    inline Type&& attribute_common<Model, Attribute, Type>::value()&& {
        return std::move(data.value());
    }
    template<typename Model, typename Attribute, typename Type>
    inline constexpr attribute_common<Model, Attribute, Type>::operator const std::optional<Type>&() const& noexcept {
        return data;
    }
    template<typename Model, typename Attribute, typename Type>
    inline constexpr attribute_common<Model, Attribute, Type>::operator std::optional<Type>&()& noexcept {
        return data;
    }
    template<typename Model, typename Attribute, typename Type>
    inline constexpr attribute_common<Model, Attribute, Type>::operator std::optional<Type>()&& noexcept {
        return std::move(data);
    }

    template<typename Model, typename Attribute, typename Type>
    template<std::convertible_to<std::optional<Type>> T>
    inline constexpr bool attribute_common<Model, Attribute, Type>::operator==(const T& cmp) const {
        return data == static_cast<std::optional<Type>>(cmp);
    }
    template<typename Model, typename Attribute, typename Type>
    template<std::convertible_to<std::optional<Type>> T>
    inline constexpr bool attribute_common<Model, Attribute, Type>::operator!=(const T& cmp) const {
        return data != static_cast<std::optional<Type>>(cmp);
    }

    template<typename Model, typename Attribute, typename Type>
    inline const typename attribute_common<Model, Attribute, Type>::constraint attribute_common<Model, Attribute, Type>::default_value(const Type& def_val) {
        return constraint_default_value_impl{ def_val };
    }

    template<typename Model, typename Attribute, typename Type>
    inline bool attribute_common<Model, Attribute, Type>::has_constraint(const constraint& c) noexcept {
        if constexpr (has_constraints()){
            for(const auto& con : Attribute::constraints){
                if(con.target_type() == c.target_type()) return true;
            }
        }
        return false;
    }
    
    template<typename Model, typename Attribute, typename Type>
    template<typename Constraint>
    inline const Constraint* attribute_common<Model, Attribute, Type>::get_constraint() {
        if constexpr(has_constraints()) {
            for(const auto& con : Attribute::constraints){
                if(con.target_type() == typeid(Constraint)){
                    return con.template target<Constraint>();
                }
            }
        }
        return nullptr;
    }

    template<typename Model, typename Attribute, typename Type>
    inline constexpr bool attribute_common<Model, Attribute, Type>::is_valid() const {
        if constexpr (has_constraints()) {
            for (const auto& val : Attribute::constraints) {
                if (!val(data)) return false;
            }
        }
        return true;
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