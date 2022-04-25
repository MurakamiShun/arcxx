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

        // datetime(seconds)
        template<std::same_as<sqlite3_adaptor> Adaptor, is_attribute Attr>
        requires regarded_as_clock<typename Attr::value_type>
        [[nodiscard]] inline active_record::string to_string(const Attr& attr) {
            // yyyy-MM-dd HH:mm:ss (sqlite supports only utc)
            /*
            namespace chrono = std::chrono;
            if(attr){
                using duration = typename Attr::value_type::duration;
                const auto utc_time = chrono::clock_cast<chrono::utc_clock>(attr.value());
                if constexpr(std::is_same_v<duration, chrono::seconds>){
                    return std::format("{:%F %T}", chrono::floor<chrono::seconds>(utc_time));
                }
                else if constexpr(std::is_same_v<duration, chrono::days>){
                    return std::format("{:%F}", chrono::floor<chrono::days>(utc_time));
                }
            }
            */
            return "null";
        }
        template<std::same_as<sqlite3_adaptor> Adaptor, is_attribute Attr>
        requires regarded_as_clock<typename Attr::value_type>
        inline void from_string(Attr& attr, const active_record::string_view str){
            /*
            if(str != "null"){
                namespace chrono = std::chrono;
                if constexpr(std::is_same_v<duration, chrono::seconds>){
                    chrono::time_point<chrono::utc_clock, chrono::seconds> result;
                    std::basic_istringstream<typename active_record::string::value_type> iss{ active_record::string{str} };
                    ss >> std::chrono::parse("%F %T", result);
                    if (ss) chrono::clock_cast<typename Attr::value_type>(result);
                    else return "unavailable clock format";
                }
                else if constexpr(std::is_same_v<duration, chrono::days>){
                    chrono::time_point<chrono::utc_clock, chrono::seconds> result;
                    std::basic_istringstream<typename active_record::string::value_type> iss{ active_record::string{str} };
                    ss >> std::chrono::parse("%F", result);
                    if (ss) chrono::clock_cast<typename Attr::value_type>(result);
                    else return "unavailable clock format";
                }
            }
            return std::nullopt;
            */
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