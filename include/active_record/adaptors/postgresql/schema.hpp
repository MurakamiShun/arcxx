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
#include "../../attributes/attributes.hpp"

namespace active_record::PostgreSQL {
    namespace detail {
        template<Attribute T>
        struct is_reference {
            template<typename S>
            static decltype(std::declval<typename S::foreign_key_type>(), std::true_type{}) check(S);
            static std::false_type check(...);
            static constexpr bool value = decltype(check(std::declval<T>()))::value;
        };

        template<Attribute T>
        [[nodiscard]] static active_record::string reference_definition(){
            if constexpr(is_reference<T>::value){
                    return active_record::string{ " REFERENCES " }
                    + active_record::string{ T::foreign_key_type::model_type::table_name }
                    + "(" + active_record::string{ T::foreign_key_type::column_name } + ")";
            }
            else return "";
        }
    }

    template<Attribute T>
    requires std::same_as<typename T::value_type, active_record::string>
    [[nodiscard]] active_record::string column_definition() {
        return active_record::string{ T::column_name }
            + (T::has_constraint(typename T::constraint_length_impl{}) ?
                " VARCHAR(" + std::to_string(T::get_constraint(typename T::constraint_length_impl{})->get().template target<typename T::constraint_length_impl>()->length) + ")"
                : " TEXT")
            + (T::has_constraint(T::unique) ? " UNIQUE" : "")
            + (T::has_constraint(T::primary_key) ? " PRIMARY KEY" : "")
            + (T::has_constraint(typename T::constraint_default_value_impl{}) ?
                " DEFAULT '" + active_record::sanitize(T::get_constraint(typename T::constraint_default_value_impl{})->get().template target<typename T::constraint_default_value_impl>()->default_value) + "'"
                : "")
            + (T::has_constraint(T::not_null) ? " NOT NULL" : "")
            + detail::reference_definition<T>();
    }

    template<Attribute T>
    requires std::integral<typename T::value_type>
    [[nodiscard]] active_record::string column_definition() {
        return active_record::string{ T::column_name }
            + []() -> active_record::string {
                if      constexpr (sizeof(typename T::value_type) == 8) return " BIGINT";
                else if constexpr (sizeof(typename T::value_type) <= 2) return " SMALLINT";
                else return " INTEGER";
            }()
            + (T::has_constraint(T::unique) ? " UNIQUE" : "")
            + (T::has_constraint(T::primary_key) ? " PRIMARY KEY" : "")
            + (T::has_constraint(typename T::constraint_default_value_impl{}) ?
                " DEFAULT " + std::to_string(T::get_constraint(typename T::constraint_default_value_impl{})->get().template target<typename T::constraint_default_value_impl>()->default_value)
                : "")
            + (T::has_constraint(T::not_null) ? " NOT NULL" : "")
            + detail::reference_definition<T>();
    }

    template<Attribute T>
    requires std::floating_point<typename T::value_type>
    [[nodiscard]] active_record::string column_definition() {
        return active_record::string{ T::column_name }
            + []() -> active_record::string {
                if constexpr (sizeof(typename T::value_type) == 8) return " DOUBLE PRECISION";
                else return " REAL";
            }()
            + (T::has_constraint(T::unique) ? " UNIQUE" : "")
            + (T::has_constraint(T::primary_key) ? " PRIMARY KEY" : "")
            + (T::has_constraint(typename T::constraint_default_value_impl{}) ?
                " DEFAULT " + std::to_string(T::get_constraint(typename T::constraint_default_value_impl{})->get().template target<typename T::constraint_default_value_impl>()->default_value)
                : "")
            + (T::has_constraint(T::not_null) ? " NOT NULL" : "")
            + detail::reference_definition<T>();
    }
}