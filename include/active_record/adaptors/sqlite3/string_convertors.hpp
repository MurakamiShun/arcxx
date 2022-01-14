#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
#include "../../attributes/attributes.hpp"

namespace active_record {
    class sqlite3_adaptor;

    inline namespace sqlite3_string_convertors{
        // boolean
        template<std::same_as<sqlite3_adaptor> Adaptor, is_attribute Attr>
        requires std::same_as<typename Attr::value_type, bool>
        [[nodiscard]] inline active_record::string to_string(const Attr& attr) {
            return static_cast<bool>(attr) ? (attr.value() ? "1" : "0") : "null";
        }
        template<std::same_as<sqlite3_adaptor> Adaptor, is_attribute Attr>
        requires std::same_as<typename Attr::value_type, bool>
        inline void from_string(Attr& attr, const active_record::string_view str){
            if(str != "null"){
                attr = ((str == "0") ? false : true);
            }
        }

        // datetime
        template<std::same_as<sqlite3_adaptor> Adaptor, is_attribute Attr>
        requires std::same_as<typename Attr::value_type, active_record::datetime>
        [[nodiscard]] inline active_record::string to_string(const Attr&) {
            // yyyy-MM-dd HH:mm:ss (sqlite supports only utc)
            //return std::format("{:%F} {:%T}", attr.value());
            return "";
        }
        template<std::same_as<sqlite3_adaptor> Adaptor, is_attribute Attr>
        requires std::same_as<typename Attr::value_type, active_record::datetime>
        inline void from_string(Attr& attr, const active_record::string_view){
            active_record::datetime dt;
            //std::chrono::parse("{:%F} {:%T}", dt, str);
            attr = dt;
        }

        // binary
        template<std::same_as<sqlite3_adaptor> Adaptor, is_attribute Attr>
        requires std::same_as<typename Attr::value_type, std::vector<std::byte>>
        [[nodiscard]] inline active_record::string to_string(const Attr& attr) {
            active_record::string hex = "x'";
            char buf[4];
            for(const auto b : attr.value()){
                hex += active_record::string_view{buf, std::to_chars(buf, &buf[3], b, 16) + 1};
            }
            return static_cast<bool>(attr) ? hex + "'" : "null";
        }
        template<std::same_as<sqlite3_adaptor> Adaptor, is_attribute Attr>
        requires std::same_as<typename Attr::value_type, std::vector<std::byte>>
        inline void from_string(Attr& attr, const active_record::string_view str){
            attr = std::vector<std::byte>{};
            attr.value().reserve(str.size());
            std::copy(str.begin(), str.end(), attr.value().begin());
        }
    }
}