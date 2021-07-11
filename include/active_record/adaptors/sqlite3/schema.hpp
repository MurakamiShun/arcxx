#pragma once
#include "../../attributes/attributes.hpp"

namespace active_record::sqlite3 {
    namespace {
        template<Attribute T>
        struct is_reference {
            template<typename S>
            static decltype(std::declval<typename S::foreign_key_type>(), std::true_type{}) check(S);
            static std::false_type check(...);
            static constexpr bool value = decltype(check(std::declval<T>()))::value;
        };
        
        template<Attribute T>
        static active_record::string reference_definition(){
            if constexpr(is_reference<T>::value){
                return active_record::string{ ",FOREIGN KEY(" }
                    + active_record::string{ T::column_name } + ") REFERENCES "
                    + active_record::string{ T::foreign_key_type::model_type::table_name } + "("
                    + active_record::string{ T::foreign_key_type::column_name } + ")";
            }
            else return "";
        }
    }
    template<Attribute T>
    requires std::same_as<typename T::value_type, active_record::string>
    active_record::string column_definition() {
        return active_record::string{ T::column_name }
            + " TEXT"
            + (T::has_constraint(T::unique) ? " UNIQUE" : "")
            + (T::has_constraint(T::primary_key) ? " PRIMARY KEY" : "")
            + (T::has_constraint(T::not_null) ? " NOT NULL" : "")
            + (T::has_constraint(typename T::constraint_default_value_impl{}) ?
                active_record::string{ (T::has_constraint(T::not_null) ? " ON CONFLICT REPLACE" : "") } + " DEFAULT '"
                + active_record::sanitize(T::get_constraint(typename T::constraint_default_value_impl{})->template target<typename T::constraint_default_value_impl>()->default_value) + "'"
                : "")
            + (T::has_constraint(typename T::constraint_length_impl{}) ?
                active_record::string{ " CHECK(length(" } + active_record::string{ T::column_name }
                + ")<=" + std::to_string(T::get_constraint(typename T::constraint_length_impl{})->template target<typename T::constraint_length_impl>()->length) + ")"
                : "")
            + reference_definition<T>();
    }

    template<Attribute T>
    requires std::integral<typename T::value_type>
    active_record::string column_definition() {
        return active_record::string{ T::column_name }
            + active_record::string{ " INTEGER" }
            + (T::has_constraint(T::unique) ? " UNIQUE" : "")
            + (T::has_constraint(T::primary_key) ? " PRIMARY KEY" : "")
            + (T::has_constraint(T::not_null) ? " NOT NULL" : "")
            + (T::has_constraint(typename T::constraint_default_value_impl{}) ?
                active_record::string{ (T::has_constraint(T::not_null) ? " ON CONFLICT REPLACE" : "") } + " DEFAULT "
                + std::to_string(T::get_constraint(typename T::constraint_default_value_impl{})->template target<typename T::constraint_default_value_impl>()->default_value)
                : "")
            + reference_definition<T>();
    }
    
    template<Attribute T>
    requires std::floating_point<typename T::value_type>
    active_record::string column_definition() {
        return active_record::string{ T::column_name }
            + active_record::string{ " REAL" }
            + (T::has_constraint(T::unique) ? " UNIQUE" : "")
            + (T::has_constraint(T::primary_key) ? " PRIMARY KEY" : "")
            + (T::has_constraint(T::not_null) ? " NOT NULL" : "")
            + (T::has_constraint(typename T::constraint_default_value_impl{}) ?
                active_record::string{ (T::has_constraint(T::not_null) ? " ON CONFLICT REPLACE" : "") } + " DEFAULT "
                + std::to_string(T::get_constraint(typename T::constraint_default_value_impl{})->template target<typename T::constraint_default_value_impl>()->default_value)
                : "")
            + reference_definition<T>();
    }
}