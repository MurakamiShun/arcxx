#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT Lisence.
 */
namespace active_record{
    template<typename Model, typename Attribute, typename Type>
    constexpr auto attribute_common<Model, Attribute, Type>::column_full_name() {
        return concat_strings("\"", Model::table_name, "\".\"", Attribute::column_name, "\"");
    }

    template<typename Model, typename Attribute, typename Type>
    const typename attribute_common<Model, Attribute, Type>::constraint attribute_common<Model, Attribute, Type>::default_value(const Type& def_val) {
        return constraint_default_value_impl{ def_val };
    }

    template<typename Model, typename Attribute, typename Type>
    bool attribute_common<Model, Attribute, Type>::has_constraint(const constraint& c) noexcept {
        if constexpr (!has_constraints()) return false;
        else{
            for(const auto& con : Attribute::constraints){
                if(con.target_type() == c.target_type()) return true;
            }
            return false;
        }
    }
    
    template<typename Model, typename Attribute, typename Type>
    const std::optional<std::reference_wrapper<const typename attribute_common<Model, Attribute, Type>::constraint>> attribute_common<Model, Attribute, Type>::get_constraint(const constraint& c) {
        if constexpr (!has_constraints()) return std::nullopt;
        else{
            for(const auto& con : Attribute::constraints){
                if(con.target_type() == c.target_type()) return std::cref(con);
            }
            return std::nullopt;
        }
    }

    template<typename Model, typename Attribute, typename Type>
    constexpr bool attribute_common<Model, Attribute, Type>::is_valid() const {
        if constexpr (has_constraints()) {
            for (const auto& val : Attribute::constraints) {
                if (!val(data)) return false;
            }
        }
        return true;
    }

    namespace detail {
    template<typename Attribute, std::size_t I, typename Condition, std::convertible_to<Attribute> Last>
    void copy_and_set_attrs_to_condition(Condition& ret, const Last& last) {
        std::get<I>(ret.bind_attrs) = Attribute{ last };
        ret.condition.push_back(I);
    }

    template<typename Attribute, std::size_t I, typename Condition, std::convertible_to<Attribute> Head, std::convertible_to<Attribute>... Tails>
    void copy_and_set_attrs_to_condition(Condition& ret, const Head& head, const Tails&... tails) {
        copy_and_set_attrs_to_condition<Attribute, I+1>(ret, head);
        ret.condition.push_back(",");
        copy_and_set_attrs_to_condition<Attribute, I+1>(ret, tails...);
    }

    }

    template<typename Model, typename Attribute, typename Type>
    template<std::convertible_to<Attribute>... Attrs>
    auto attribute_common<Model, Attribute, Type>::in(const Attrs&... values) {
        query_condition<std::tuple<decltype(values, std::declval<Attribute>())...>> ret;
        ret.condition.push_back(concat_strings(
            "\"", Model::table_name, "\".\"",
            Attribute::column_name, "\" in ("
        ));
        detail::copy_and_set_attrs_to_condition<Attribute, 0>(ret, values...);
        ret.condition.push_back(")");
        return ret;
    }
}