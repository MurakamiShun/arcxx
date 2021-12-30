#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT Lisence.
 */
#include "../../adaptor.hpp"
#include "../../attributes/attributes.hpp"

namespace active_record {
    class postgresql_adaptor;

    inline namespace postgresql_string_convertors{
        // boolean
        template<std::same_as<postgresql_adaptor> Adaptor, is_attribute Attr>
        requires std::same_as<typename Attr::value_type, bool>
        [[nodiscard]] inline active_record::string to_string(const Attr& attr) {
            return static_cast<bool>(attr) ? (attr.value() ? "1" : "0") : "null";
        }
        template<std::same_as<postgresql_adaptor> Adaptor, is_attribute Attr>
        requires std::same_as<typename Attr::value_type, bool>
        inline void from_string(Attr& attr, const active_record::string_view str){
            if(str != "null"){
                attr = ((str == "0") ? false : true);
            }
        }

        // integer
        template<std::same_as<postgresql_adaptor> Adaptor, is_attribute Attr>
        requires std::integral<typename Attr::value_type>
        inline void from_string(Attr& attr, const active_record::string_view str) {
            if(str != "null" && str != "NULL"){
                typename Attr::value_type tmp = static_cast<typename Attr::value_type>(0);
                std::from_chars(&*str.begin(), &*str.end(), tmp);
                attr = tmp;
            }
        }

        // decimal
        template<std::same_as<postgresql_adaptor> Adaptor, is_attribute Attr>
        requires std::floating_point<typename Attr::value_type>
        inline void from_string(Attr& attr, const active_record::string_view str){
            if(str != "null" && str != "NULL"){
                typename Attr::value_type tmp = static_cast<typename Attr::value_type>(0);
                std::from_chars(&*str.begin(), &*str.end(), tmp);
                attr = tmp;
            }
        }

        // string
        template<std::same_as<postgresql_adaptor> Adaptor, is_attribute Attr>
        requires std::same_as<typename Attr::value_type, active_record::string>
        [[nodiscard]] inline active_record::string to_string(const Attr& attr) {
            return static_cast<bool>(attr) ? attr.value() : "null";
        }
        template<std::same_as<postgresql_adaptor> Adaptor, is_attribute Attr>
        requires std::same_as<typename Attr::value_type, active_record::string>
        inline void from_string(Attr& attr, const active_record::string_view str) {
            attr = active_record::string{ str };
        }

        // datetime
        template<std::same_as<postgresql_adaptor> Adaptor, is_attribute Attr>
        requires std::same_as<typename Attr::value_type, active_record::datetime>
        [[nodiscard]] inline active_record::string to_string(const Attr& attr) {
            // ISO 8601 yyyyMMddTHHmmss (sqlite supports only utc)
            //return static_cast<bool>(attr) ? std::format("%FT%T", attr.value()) : "null";
            return "";
        }
        template<std::same_as<postgresql_adaptor> Adaptor, is_attribute Attr>
        requires std::same_as<typename Attr::value_type, active_record::datetime>
        inline void from_string(Attr& attr, const active_record::string_view str){
            active_record::datetime dt;
            //std::chrono::parse("%fT%T", dt, str);
            attr = dt;
        }

        // binary
        template<std::same_as<postgresql_adaptor> Adaptor, is_attribute Attr>
        requires std::same_as<typename Attr::value_type, std::vector<std::byte>>
        [[nodiscard]] inline active_record::string to_string(const Attr& attr) {
            active_record::string hex = "x'";
            char buf[4];
            for(const auto b : attr.value()){
                hex += active_record::string_view{buf, std::to_chars(buf, &buf[3], b, 16) + 1};
            }
            return static_cast<bool>(attr) ? hex + "'" : "null";
        }
        template<std::same_as<postgresql_adaptor> Adaptor, is_attribute Attr>
        requires std::same_as<typename Attr::value_type, std::vector<std::byte>>
        inline void from_string(Attr& attr, const active_record::string_view str){
            attr = std::vector<std::byte>{};
            attr.value().reserve(str.size());
            std::copy(str.begin(), str.end(), attr.value().begin());
        }
    }
}