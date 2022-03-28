#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
#include "../attribute.hpp"

namespace active_record {
    struct common_adaptor : public adaptor {
        static constexpr bool bindable = false;
        static active_record::string bind_variable_str(const std::size_t idx) {
            return active_record::string{ "'common_adaptor can not bind parameters. Library has some problems.'" } + std::to_string(idx);
        }
    };

    // integer
    template<std::same_as<common_adaptor> Adaptor, is_attribute Attr>
    requires std::integral<typename Attr::value_type>
    [[nodiscard]] inline active_record::string to_string(const Attr& attr) {
        return static_cast<bool>(attr) ? std::to_string(attr.value()) : "null";
    }
    template<std::same_as<common_adaptor> Adaptor, is_attribute Attr>
    requires std::integral<typename Attr::value_type>
    inline void from_string(Attr& attr, const active_record::string_view str) {
        if(str != "null" && str != "NULL"){
            typename Attr::value_type tmp = static_cast<typename Attr::value_type>(0);
            std::from_chars(&*str.begin(), &*str.end(), tmp);
            attr = tmp;
        }
    }

    // string
    template<std::same_as<common_adaptor> Adaptor, is_attribute Attr>
    requires std::same_as<typename Attr::value_type, active_record::string>
    [[nodiscard]] inline active_record::string to_string(const Attr& attr) {
        // require sanitize
        return static_cast<bool>(attr) ? concat_strings("\'", active_record::sanitize(attr.value()), "\'") : "null";
    }
    template<std::same_as<common_adaptor> Adaptor, is_attribute Attr>
    requires std::same_as<typename Attr::value_type, active_record::string>
    inline void from_string(Attr& attr, const active_record::string_view str) {
        if(str != "null" && str != "NULL"){
            attr = active_record::string{ str };
        }
    }

    // decimal
    template<std::same_as<common_adaptor> Adaptor, is_attribute Attr>
    requires std::floating_point<typename Attr::value_type>
    [[nodiscard]] inline active_record::string to_string(const Attr& attr) {
        return static_cast<bool>(attr) ? std::to_string(attr.value()) : "null";
    }
    template<std::same_as<common_adaptor> Adaptor, is_attribute Attr>
    requires std::floating_point<typename Attr::value_type>
    inline void from_string(Attr& attr, const active_record::string_view str){
        if(str != "null" && str != "NULL"){
            typename Attr::value_type tmp = static_cast<typename Attr::value_type>(0);
            std::from_chars(&*str.begin(), &*str.end(), tmp);
            attr = tmp;
        }
    }

    // datetime
    template<std::same_as<common_adaptor> Adaptor, is_attribute Attr>
    requires regarded_as_clock<typename Attr::value_type>
    [[nodiscard]] inline active_record::string to_string(const Attr& attr) {
        // ISO 8601 yyyyMMddTHHmmss+09:00
        //return static_cast<bool>(attr) ? std::format("{%F}T{%T}{%z}", attr.value()) : "null";
        return active_record::string{ "" };
    }
    template<std::same_as<common_adaptor> Adaptor, is_attribute Attr>
    requires regarded_as_clock<typename Attr::value_type>
    inline void from_string(Attr& attr, const active_record::string_view str){
        //std::chrono::parse("{%f}T{%T}{%z}", attr.value(), str);
        
    }

    // boolean
    template<std::same_as<common_adaptor> Adaptor, is_attribute Attr>
    requires std::same_as<typename Attr::value_type, bool>
    [[nodiscard]] inline active_record::string to_string(const Attr& attr) {
        return static_cast<bool>(attr) ? (attr.value() ? "true" : "false") : "null";
    }
    template<std::same_as<common_adaptor> Adaptor, is_attribute Attr>
    requires std::same_as<typename Attr::value_type, bool>
    inline void from_string(Attr& attr, const active_record::string_view str){
        if(str != "null" && str != "NULL"){
            attr = ((str == "false" || str == "0" || str == "False") ? false : true);
        }
    }

    // binary
    template<std::same_as<common_adaptor> Adaptor, is_attribute Attr>
    requires std::same_as<typename Attr::value_type, std::vector<std::byte>>
    [[nodiscard]] inline active_record::string to_string(const Attr& attr) {
        return static_cast<bool>(attr) ? "" : "null";
    }
    template<std::same_as<common_adaptor> Adaptor, is_attribute Attr>
    requires std::same_as<typename Attr::value_type, std::vector<std::byte>>
    inline void from_string(Attr& attr, const active_record::string_view str){
    }
}