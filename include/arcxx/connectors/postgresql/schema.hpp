#pragma once
/*
 * ARCXX: https://github.com/akisute514/arcxx
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
#include "../../attributes/attributes.hpp"

namespace arcxx::PostgreSQL {
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
            T::column_name,
            (T::has_constraint(T::length(0)) ?
                concat_strings(" VARCHAR(", std::to_string(T::get_constraint(T::length(0))->length), ")")
                : " TEXT"
            ),
            T::has_constraint(T::unique) ? " UNIQUE" : "",
            T::has_constraint(T::primary_key) ? " PRIMARY KEY" : "",
            (T::has_constraint(T::default_value("")) ?
                concat_strings(" DEFAULT '", T::get_constraint(T::default_value(""))->default_value, "'")
                : ""
            ),
            T::has_constraint(T::not_null) ? " NOT NULL" : "",
            detail::reference_definition<T>()
        );
    }

    template<is_attribute T>
    requires std::integral<typename T::value_type>
    [[nodiscard]] inline arcxx::string column_definition() {
        return concat_strings(
            T::column_name,
            []{
                if      constexpr (sizeof(typename T::value_type) == 8) return " BIGINT";
                else if constexpr (sizeof(typename T::value_type) <= 2) return " SMALLINT";
                else return " INTEGER";
            }(),
            T::has_constraint(T::unique) ? " UNIQUE" : "",
            T::has_constraint(T::primary_key) ? " PRIMARY KEY" : "",
            (T::has_constraint(T::default_value(0)) ?
                concat_strings(" DEFAULT ", std::to_string(T::get_constraint(T::default_value(0))->default_value))
                : ""
            ),
            T::has_constraint(T::not_null) ? " NOT NULL" : "",
            detail::reference_definition<T>()
        );
    }

    template<is_attribute T>
    requires std::floating_point<typename T::value_type>
    [[nodiscard]] inline arcxx::string column_definition() {
        return concat_strings(
            T::column_name,
            []{
                if constexpr (sizeof(typename T::value_type) == 8) return " DOUBLE PRECISION";
                else return " REAL";
            }(),
            T::has_constraint(T::unique) ? " UNIQUE" : "",
            T::has_constraint(T::primary_key) ? " PRIMARY KEY" : "",
            (T::has_constraint(T::default_value(0.0)) ?
                concat_strings(" DEFAULT ", std::to_string(T::get_constraint(T::default_value(0.0))->default_value))
                : ""
            ),
            T::has_constraint(T::not_null) ? " NOT NULL" : "",
            detail::reference_definition<T>()
        );
    }

    template<is_attribute T>
    requires regarded_as_clock<typename T::value_type>
    [[nodiscard]] inline arcxx::string column_definition() {
        return concat_strings(
            T::column_name,
            std::is_same_v<typename T::value_type::duration, std::chrono::days> ? " TIMESTAMP" : " DATE",
            T::has_constraint(T::unique) ? " UNIQUE" : "",
            T::has_constraint(T::primary_key) ? " PRIMARY KEY" : "",
            T::has_constraint(T::not_null) ? " NOT NULL" : "",
            /*
            (T::has_constraint(T::current_time) ?
                concat_strings(" DEFAULT ", std::is_same_v<T::value_type::duration, std::chrono::days>) ? "current_date" : "current_timestamp")
                : ""
            ),*/
            detail::reference_definition<T>()
        );
    }
}