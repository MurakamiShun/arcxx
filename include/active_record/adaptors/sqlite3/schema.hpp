#pragma once
#include "../../attributes/attributes.hpp"

namespace active_record::sqlite3 {
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
                    + active_record::string{ T::foreign_key_type::model_type::table_name } + "("
                    + active_record::string{ T::foreign_key_type::column_name } + ")";
            }
            else return "";
        }
    }
    template<Attribute T>
    requires std::same_as<typename T::value_type, active_record::string>
    [[nodiscard]] active_record::string column_definition() {
        return active_record::string{ T::column_name }
            + " TEXT"
            + (T::has_constraint(T::unique) ? " UNIQUE" : "")
            + (T::has_constraint(T::primary_key) ? " PRIMARY KEY" : "")
            + (T::has_constraint(T::not_null) ? " NOT NULL" : "")
            + (T::has_constraint(typename T::constraint_default_value_impl{}) ?
                active_record::string{ (T::has_constraint(T::not_null) ? " ON CONFLICT REPLACE" : "") } + " DEFAULT '"
                + active_record::sanitize(T::get_constraint(typename T::constraint_default_value_impl{})->get().template target<typename T::constraint_default_value_impl>()->default_value) + "'"
                : "")
            + (T::has_constraint(typename T::constraint_length_impl{}) ?
                active_record::string{ " CHECK(length(" } + active_record::string{ T::column_name }
                + ")<=" + std::to_string(T::get_constraint(typename T::constraint_length_impl{})->get().template target<typename T::constraint_length_impl>()->length) + ")"
                : "")
            + detail::reference_definition<T>();
    }

    template<Attribute T>
    requires std::integral<typename T::value_type>
    [[nodiscard]] active_record::string column_definition() {
        return active_record::string{ T::column_name }
            + active_record::string{ " INTEGER" }
            + (T::has_constraint(T::unique) ? " UNIQUE" : "")
            + (T::has_constraint(T::primary_key) ? " PRIMARY KEY" : "")
            + (T::has_constraint(T::not_null) ? " NOT NULL" : "")
            + (T::has_constraint(typename T::constraint_default_value_impl{}) ?
                active_record::string{ (T::has_constraint(T::not_null) ? " ON CONFLICT REPLACE" : "") } + " DEFAULT "
                + std::to_string(T::get_constraint(typename T::constraint_default_value_impl{})->get().template target<typename T::constraint_default_value_impl>()->default_value)
                : "")
            + detail::reference_definition<T>();
    }
    
    template<Attribute T>
    requires std::floating_point<typename T::value_type>
    [[nodiscard]] active_record::string column_definition() {
        return active_record::string{ T::column_name }
            + active_record::string{ " REAL" }
            + (T::has_constraint(T::unique) ? " UNIQUE" : "")
            + (T::has_constraint(T::primary_key) ? " PRIMARY KEY" : "")
            + (T::has_constraint(T::not_null) ? " NOT NULL" : "")
            + (T::has_constraint(typename T::constraint_default_value_impl{}) ?
                active_record::string{ (T::has_constraint(T::not_null) ? " ON CONFLICT REPLACE" : "") } + " DEFAULT "
                + std::to_string(T::get_constraint(typename T::constraint_default_value_impl{})->get().template target<typename T::constraint_default_value_impl>()->default_value)
                : "")
            + detail::reference_definition<T>();
    }

    template<Attribute T>
    requires std::same_as<typename T::value_type, active_record::datetime>
    [[nodiscard]] active_record::string column_definition() {
        return active_record::string{ T::column_name }
            + active_record::string{ " TEXT CHECK(" } + active_record::string{ T::column_name } + " LIKE '____-__-__ __:__:__')"
            + (T::has_constraint(T::unique) ? " UNIQUE" : "")
            + (T::has_constraint(T::primary_key) ? " PRIMARY KEY" : "")
            + (T::has_constraint(T::not_null) ? " NOT NULL" : "")
            + (T::has_constraint(typename T::constraint_default_value_impl{}) ?
                active_record::string{ (T::has_constraint(T::not_null) ? " ON CONFLICT REPLACE" : "") } + " DEFAULT "
                + std::to_string(T::get_constraint(typename T::constraint_default_value_impl{})->get().template target<typename T::constraint_default_value_impl>()->default_value)
                : "")
            + detail::reference_definition<T>();
    }
}