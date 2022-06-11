#pragma once
/*
 * ARCXX: https://github.com/akisute514/arcxx
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
#include "../../connector.hpp"
#include "../../attributes/attributes.hpp"

namespace arcxx {
    class postgresql_connector;

    inline namespace postgresql_string_convertors{
        // boolean
        template<std::same_as<postgresql_connector> Connector, is_attribute Attr>
        requires std::same_as<typename Attr::value_type, bool>
        [[nodiscard]] inline arcxx::string to_string(const Attr& attr) {
            return static_cast<bool>(attr) ? (attr.value() ? "1" : "0") : "null";
        }
        template<std::same_as<postgresql_connector> Connector, is_attribute Attr>
        requires std::same_as<typename Attr::value_type, bool>
        inline void from_string(Attr& attr, const arcxx::string_view str){
            if(str != "null"){
                attr = ((str == "0") ? false : true);
            }
        }

        // integer
        template<std::same_as<postgresql_connector> Connector, is_attribute Attr>
        requires std::integral<typename Attr::value_type>
        inline void from_string(Attr& attr, const arcxx::string_view str) {
            if(str != "null" && str != "NULL"){
                typename Attr::value_type tmp = static_cast<typename Attr::value_type>(0);
                std::from_chars(std::to_address(str.begin()), std::to_address(str.end()), tmp);
                attr = tmp;
            }
        }

        // decimal
        template<std::same_as<postgresql_connector> Connector, is_attribute Attr>
        requires std::floating_point<typename Attr::value_type>
        inline void from_string(Attr& attr, const arcxx::string_view str){
            if(str != "null" && str != "NULL"){
                typename Attr::value_type tmp = static_cast<typename Attr::value_type>(0);
                std::from_chars(std::to_address(str.begin()), std::to_address(str.end()), tmp);
                attr = tmp;
            }
        }

        // string
        template<std::same_as<postgresql_connector> Connector, is_attribute Attr>
        requires std::same_as<typename Attr::value_type, arcxx::string>
        [[nodiscard]] inline arcxx::string to_string(const Attr& attr) {
            return static_cast<bool>(attr) ? attr.value() : "null";
        }
        template<std::same_as<postgresql_connector> Connector, is_attribute Attr>
        requires std::same_as<typename Attr::value_type, arcxx::string>
        inline void from_string(Attr& attr, const arcxx::string_view str) {
            attr = arcxx::string{ str };
        }

        // datetime
        template<std::same_as<postgresql_connector> Connector, is_attribute Attr>
        requires regarded_as_clock<typename Attr::value_type>
        [[nodiscard]] inline arcxx::string to_string(const Attr& attr, arcxx::string&& buff = {}) {
            // YYYY-MM-DD hh:mm:ss (UTC or GMT)
            using clock = Attr::value_type::clock;
            using duration = typename Attr::value_type::duration;
            namespace chrono = std::chrono;

            if(attr){
                #ifdef _MSC_VER
                using duration = typename Attr::value_type::duration;
                if constexpr(std::is_same_v<duration, chrono::days>){
                    std::format_to(std::back_inserter(buff),"{:%F}", attr.value());
                }
                else {
                    std::format_to(std::back_inserter(buff), "{:%F %T}", chrono::floor<chrono::seconds>(attr.value()));
                }
                #else
                const std::time_t t = clock::to_time_t(attr.value());
                std::tm* gmt = std::gmtime(&t);
                if(gmt == nullptr) throw std::runtime_error("std::gmtime causes error");
                char str[32]{0};
                const char* const fmt_str = [](){
                    if constexpr(std::is_same_v<duration, chrono::days>) return "%F";
                    else return "%F %T";
                }();
                if(std::strftime(str, 32, fmt_str, gmt) == 0) throw std::runtime_error("std::strftime causes error");
                buff += str;
                #endif
            }
            else{
                buff += "null";
            }
            return std::move(buff);
        }
        template<std::same_as<postgresql_connector> Connector, is_attribute Attr>
        requires regarded_as_clock<typename Attr::value_type>
        inline void from_string(Attr& attr, const arcxx::string_view str){
            using clock = Attr::value_type::clock;
            using duration = typename Attr::value_type::duration;
            namespace chrono = std::chrono;

            if(str == "null") attr = std::nullopt;
            else{
                #ifdef _MSC_VER
                chrono::time_point<clock, duration> result;
                std::basic_istringstream<typename arcxx::string::value_type> iss{ arcxx::string{str} };
                if constexpr((std::is_same_v<duration, chrono::days>)){
                    iss >> std::chrono::parse("%F", result);
                }
                else {
                    iss >> std::chrono::parse("%F %T", result);
                }
                if (!iss) throw std::runtime_error("unavailable clock format");
                #else
                std::tm gmt;
                const char* const fmt_str = [](){
                    if constexpr(std::is_same_v<duration, chrono::days>) return "%F";
                    else return "%F %T";
                }();
                if(strptime(&str.front(), fmt_str, &gmt) == &str.front()) throw std::runtime_error("unavailable clock format");
                const time_t t = timegm(&gmt);
                if(t == time_t(-1)) throw std::runtime_error("unavailable clock format");
                attr = time_point_cast<duration>(clock::from_time_t(t));
                #endif
            }
        }

        // binary
        template<std::same_as<postgresql_connector> Connector, is_attribute Attr>
        requires std::same_as<typename Attr::value_type, std::vector<std::byte>>
        [[nodiscard]] inline arcxx::string to_string(const Attr& attr) {
            arcxx::string hex = "x'";
            char buf[4];
            for(const auto b : attr.value()){
                hex += arcxx::string_view{buf, std::to_chars(buf, &buf[3], b, 16) + 1};
            }
            return static_cast<bool>(attr) ? hex + "'" : "null";
        }
        template<std::same_as<postgresql_connector> Connector, is_attribute Attr>
        requires std::same_as<typename Attr::value_type, std::vector<std::byte>>
        inline void from_string(Attr& attr, const arcxx::string_view str){
            attr = std::vector<std::byte>{};
            attr.value().reserve(str.size());
            std::copy(str.begin(), str.end(), attr.value().begin());
        }
    }
}