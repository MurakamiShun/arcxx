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

namespace active_record{
    template<typename Model, typename Attribute, typename Type>
    constexpr auto attribute_common<Model, Attribute, Type>::column_full_name() {
        return concat_strings("\"", Model::table_name, "\".\"", Attribute::column_name, "\"");
    }

    template<typename Model, typename Attribute, typename Type>
    const attribute_common<Model, Attribute, Type>::constraint attribute_common<Model, Attribute, Type>::default_value(const Type& def_val) {
        return constraint_default_value_impl{ def_val };
    }

    template<typename Model, typename Attribute, typename Type>
    bool attribute_common<Model, Attribute, Type>::has_constraint(const constraint& c) noexcept {
        if constexpr (!has_constraints) return false;
        else{
            for(const auto& con : Attribute::constraints){
                if(con.target_type() == c.target_type()) return true;
            }
            return false;
        }
    }
    
    template<typename Model, typename Attribute, typename Type>
    const std::optional<std::reference_wrapper<const typename attribute_common<Model, Attribute, Type>::constraint>> attribute_common<Model, Attribute, Type>::get_constraint(const constraint& c) {
        if constexpr (!has_constraints) return std::nullopt;
        else{
            for(const auto& con : Attribute::constraints){
                if(con.target_type() == c.target_type()) return std::cref(con);
            }
            return std::nullopt;
        }
    }

    template<typename Model, typename Attribute, typename Type>
    constexpr bool attribute_common<Model, Attribute, Type>::is_valid() const {
        if constexpr (has_constraints) {
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