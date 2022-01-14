#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
#include "../../attributes/attributes.hpp"

namespace active_record::PostgreSQL {
    namespace detail {
        template<is_attribute T>
        [[nodiscard]] inline active_record::string reference_definition(){
            if constexpr(requires { typename T::foreign_key_type; }){
                    return concat_strings(
                        " REFERENCES ",
                        T::foreign_key_type::model_type::table_name,
                        "(", T::foreign_key_type::column_name, ")"
                    );
            }
            else return "";
        }
    }

    template<is_attribute T>
    requires std::same_as<typename T::value_type, active_record::string>
    [[nodiscard]] inline active_record::string column_definition() {
        return concat_strings(
            T::column_name,
            (T::has_constraint(typename T::constraint_length_impl{}) ?
                concat_strings(
                    " VARCHAR(",
                    std::to_string(T::template get_constraint<typename T::constraint_length_impl>()->length),
                    ")"
                ) : " TEXT"),
            T::has_constraint(T::unique) ? " UNIQUE" : "",
            T::has_constraint(T::primary_key) ? " PRIMARY KEY" : "",
            (T::has_constraint(typename T::constraint_default_value_impl{}) ?
                concat_strings(
                    " DEFAULT '",
                    T::template get_constraint<typename T::constraint_default_value_impl>()->default_value,
                    "'"
                ) : ""),
            T::has_constraint(T::not_null) ? " NOT NULL" : "",
            detail::reference_definition<T>()
        );
    }

    template<is_attribute T>
    requires std::integral<typename T::value_type>
    [[nodiscard]] inline active_record::string column_definition() {
        return concat_strings(
            T::column_name,
            []{
                if      constexpr (sizeof(typename T::value_type) == 8) return " BIGINT";
                else if constexpr (sizeof(typename T::value_type) <= 2) return " SMALLINT";
                else return " INTEGER";
            }(),
            T::has_constraint(T::unique) ? " UNIQUE" : "",
            T::has_constraint(T::primary_key) ? " PRIMARY KEY" : "",
            (T::has_constraint(typename T::constraint_default_value_impl{}) ?
                concat_strings(
                    " DEFAULT ",
                    std::to_string(T::template get_constraint<typename T::constraint_default_value_impl>()->default_value)
                ) : ""),
            T::has_constraint(T::not_null) ? " NOT NULL" : "",
            detail::reference_definition<T>()
        );
    }

    template<is_attribute T>
    requires std::floating_point<typename T::value_type>
    [[nodiscard]] inline active_record::string column_definition() {
        return concat_strings(
            T::column_name,
            []{
                if constexpr (sizeof(typename T::value_type) == 8) return " DOUBLE PRECISION";
                else return " REAL";
            }(),
            T::has_constraint(T::unique) ? " UNIQUE" : "",
            T::has_constraint(T::primary_key) ? " PRIMARY KEY" : "",
            (T::has_constraint(typename T::constraint_default_value_impl{}) ?
                concat_strings(
                    " DEFAULT ",
                    std::to_string(T::template get_constraint<typename T::constraint_default_value_impl>()->default_value)
                ) : ""),
            T::has_constraint(T::not_null) ? " NOT NULL" : "",
            detail::reference_definition<T>()
        );
    }
}