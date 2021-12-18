#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT Lisence.
 */
#include "../../attributes/attributes.hpp"

namespace active_record::sqlite3 {
    namespace detail {
        template<Attribute T>
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

    template<Attribute T>
    requires std::same_as<typename T::value_type, active_record::string>
    [[nodiscard]] inline active_record::string column_definition() {
        return concat_strings(
            T::column_name, " TEXT",
            T::has_constraint(T::unique) ? " UNIQUE" : "",
            T::has_constraint(T::primary_key) ? " PRIMARY KEY" : "",
            T::has_constraint(T::not_null) ? " NOT NULL" : "",
            (T::has_constraint(typename T::constraint_default_value_impl{}) ?
                concat_strings(
                    T::has_constraint(T::not_null) ? " ON CONFLICT REPLACE" : "",
                    " DEFAULT '",
                    T::template get_constraint<typename T::constraint_default_value_impl>()->default_value,
                    "'"
                ) : ""),
            (T::has_constraint(typename T::constraint_length_impl{}) ?
                concat_strings(
                    " CHECK(length(", T::column_name, ")<=",
                    std::to_string(T::template get_constraint<typename T::constraint_length_impl>()->length),
                    ")"
                ) : ""),
            detail::reference_definition<T>()
        );
    }

    template<Attribute T>
    requires std::integral<typename T::value_type>
    [[nodiscard]] inline active_record::string column_definition() {
        return concat_strings(
            T::column_name, " INTEGER",
            T::has_constraint(T::unique) ? " UNIQUE" : "",
            T::has_constraint(T::primary_key) ? " PRIMARY KEY" : "",
            T::has_constraint(T::not_null) ? " NOT NULL" : "",
            (T::has_constraint(typename T::constraint_default_value_impl{}) ?
                concat_strings(
                    T::has_constraint(T::not_null) ? " ON CONFLICT REPLACE" : "",
                    " DEFAULT ",
                    std::to_string(T::template get_constraint<typename T::constraint_default_value_impl>()->default_value)
                ) : ""),
            detail::reference_definition<T>()
        );
    }

    template<Attribute T>
    requires std::floating_point<typename T::value_type>
    [[nodiscard]] inline active_record::string column_definition() {
        return concat_strings(
            T::column_name, " REAL",
            T::has_constraint(T::unique) ? " UNIQUE" : "",
            T::has_constraint(T::primary_key) ? " PRIMARY KEY" : "",
            T::has_constraint(T::not_null) ? " NOT NULL" : "",
            (T::has_constraint(typename T::constraint_default_value_impl{}) ?
                concat_strings(
                    T::has_constraint(T::not_null) ? " ON CONFLICT REPLACE" : "",
                    " DEFAULT ",
                    std::to_string(T::template get_constraint<typename T::constraint_default_value_impl>()->default_value)
                ) : ""),
            detail::reference_definition<T>()
        );
    }

    template<Attribute T>
    requires std::same_as<typename T::value_type, active_record::datetime>
    [[nodiscard]] inline active_record::string column_definition() {
        return concat_strings(
            T::column_name, " DATETIME CHECK(", T::column_name, " LIKE '____-__-__ __:__:__')",
            T::has_constraint(T::unique) ? " UNIQUE" : "",
            T::has_constraint(T::primary_key) ? " PRIMARY KEY" : "",
            T::has_constraint(T::not_null) ? " NOT NULL" : "",
            (T::has_constraint(typename T::constraint_default_value_impl{}) ?
                concat_strings(
                    T::has_constraint(T::not_null) ? " ON CONFLICT REPLACE" : "",
                    " DEFAULT ",
                    std::to_string(T::template get_constraint<typename T::constraint_default_value_impl>()->default_value)
                ) : ""),
            detail::reference_definition<T>()
        );
    }
}