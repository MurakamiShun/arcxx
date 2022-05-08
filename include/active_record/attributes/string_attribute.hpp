#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
#include "attribute_common.hpp"

namespace active_record {
    template<typename Model, typename Attribute>
    struct attribute<Model, Attribute, active_record::string> : public attribute_common<Model, Attribute, active_record::string> {
        using attribute_common<Model, Attribute, active_record::string>::attribute_common;

        template<std::convertible_to<active_record::string> StringType>
        requires (!std::convertible_to<StringType, std::optional<active_record::string>>)
        constexpr attribute(const StringType& default_value) : attribute_common<Model, Attribute, active_record::string>(active_record::string{ default_value }) {}
    
        static constexpr auto length = [](const std::size_t len) constexpr noexcept {
            struct constraint_length_impl : constraint<active_record::string>{
                const std::size_t length;
                constexpr bool operator()(const std::optional<active_record::string>& t){ return t ? (length <= t.value().length()) : true; }
            } result{ {}, len };
            return std::make_tuple(result);
        };

        struct string_default_value_impl : constraint<active_record::string>{
            const active_record::string_view default_value; // basic_string is not constexpr literal
            constexpr bool operator()(const std::optional<active_record::string>&) noexcept { return true; }
        };
        static constexpr auto default_value = [](const std::convertible_to<active_record::string> auto& def_val) constexpr {
            return std::make_tuple(string_default_value_impl{ {}, static_cast<active_record::string_view>(def_val) });
        };

        template<std::convertible_to<active_record::string> StringType>
        [[nodiscard]] static constexpr query_condition<std::tuple<Attribute>> like(const StringType& value){
            query_condition<std::tuple<Attribute>> ret;
            ret.bind_attrs = std::make_tuple<Attribute>(active_record::string{ value });
            ret.condition.push_back(concat_strings(Attribute::column_full_name(), " LIKE "));
            ret.condition.push_back(static_cast<std::size_t>(0));
            return ret;
        }
    };

    namespace attributes {
        template<typename Model, typename Attribute>
        struct string : public attribute<Model, Attribute, active_record::string>{
            using attribute<Model, Attribute, active_record::string>::attribute;
        };
    }
}