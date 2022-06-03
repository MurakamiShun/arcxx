#pragma once
/*
 * ARCXX: https://github.com/akisute514/arcxx
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
#include "../../attributes/attributes.hpp"

namespace arcxx::sqlite3 {
    namespace detail {
        template<is_attribute T>
        [[nodiscard]] inline arcxx::string reference_definition(){
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
    requires std::same_as<typename T::value_type, arcxx::string>
    [[nodiscard]] inline arcxx::string column_definition() {
        return concat_strings(
            T::column_name, " TEXT",
            T::has_constraint(T::unique) ? " UNIQUE" : "",
            T::has_constraint(T::primary_key) ? " PRIMARY KEY" : "",
            T::has_constraint(T::not_null) ? " NOT NULL" : "",
            (T::has_constraint(T::default_value("")) ?
                concat_strings(
                    T::has_constraint(T::not_null) ? " ON CONFLICT REPLACE" : "",
                    " DEFAULT '",
                    T::get_constraint(T::default_value(""))->default_value,
                    "'"
                ) : ""),
            (T::has_constraint(T::length(0)) ?
                concat_strings(
                    " CHECK(length(", T::column_name, ")<=",
                    std::to_string(T::get_constraint(T::length(0))->length),
                    ")"
                ) : ""),
            detail::reference_definition<T>()
        );
    }

    template<is_attribute T>
    requires std::integral<typename T::value_type>
    [[nodiscard]] inline arcxx::string column_definition() {
        return concat_strings(
            T::column_name, " INTEGER",
            T::has_constraint(T::unique) ? " UNIQUE" : "",
            T::has_constraint(T::primary_key) ? " PRIMARY KEY" : "",
            T::has_constraint(T::not_null) ? " NOT NULL" : "",
            (T::has_constraint(T::default_value(0)) ?
                concat_strings(
                    T::has_constraint(T::not_null) ? " ON CONFLICT REPLACE" : "",
                    " DEFAULT ",
                    std::to_string(T::get_constraint(T::default_value(0))->default_value)
                ) : ""),
            detail::reference_definition<T>()
        );
    }

    template<is_attribute T>
    requires std::floating_point<typename T::value_type>
    [[nodiscard]] inline arcxx::string column_definition() {
        return concat_strings(
            T::column_name, " REAL",
            T::has_constraint(T::unique) ? " UNIQUE" : "",
            T::has_constraint(T::primary_key) ? " PRIMARY KEY" : "",
            T::has_constraint(T::not_null) ? " NOT NULL" : "",
            (T::has_constraint(T::default_value(0.0)) ?
                concat_strings(
                    T::has_constraint(T::not_null) ? " ON CONFLICT REPLACE" : "",
                    " DEFAULT ",
                    std::to_string(T::get_constraint(T::default_value(0.0))->default_value)
                ) : ""),
            detail::reference_definition<T>()
        );
    }

    template<is_attribute T>
    requires regarded_as_clock<typename T::value_type>
    [[nodiscard]] inline arcxx::string column_definition() {
        return concat_strings(
            T::column_name, " DATETIME CHECK(", T::column_name,
            std::is_same_v<typename T::value_type::duration, std::chrono::days> ? " LIKE '____-__-__')": " LIKE '____-__-__ __:__:__')",
            T::has_constraint(T::unique) ? " UNIQUE" : "",
            T::has_constraint(T::primary_key) ? " PRIMARY KEY" : "",
            T::has_constraint(T::not_null) ? " NOT NULL" : "",
            /*
            (T::has_constraint(T::current_time) ?
                concat_strings(T::has_constraint(T::not_null) ? " ON CONFLICT REPLACE" : "",
                    " DEFAULT ",
                    (std::is_same_v<T::value_type::duration, std::chrono::days>) ? "CURRENT_DATE" : "CURRENT_TIMESTAMP"
                ) : ""),*/
            detail::reference_definition<T>()
        );
    }
}