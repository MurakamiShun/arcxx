#pragma once
/*
 * ARCXX: https://github.com/akisute514/arcxx
 * Copyright (c) 2021 akisute514
 *
 * Released under the MIT License.
 */
#include "../attribute.hpp"

namespace arcxx {
    struct common_connector : public connector {
        static constexpr bool bindable = false;
        static arcxx::string bind_variable_str(const std::size_t, arcxx::string&& buff = {}) {
            return std::move(buff += "'common_connector can not bind parameters. Library has some problems.'");
        }
    };

    // integer
    template<std::same_as<common_connector> Connector, is_attribute Attr>
    requires std::integral<typename Attr::value_type>
    [[nodiscard]] inline arcxx::string to_string(const Attr& attr, arcxx::string&& buff = {}) {
        if(attr){
            std::array<arcxx::string::value_type, std::numeric_limits<typename Attr::value_type>::digits10 + 2> str_buff{0};
            std::to_chars(&(*str_buff.begin()), &(*str_buff.end()), attr.value());
            buff += str_buff.data();
        }
        else{
            buff += "null";
        }
        return std::move(buff);
    }
    template<std::same_as<common_connector> Connector, is_attribute Attr>
    requires std::integral<typename Attr::value_type>
    inline void from_string(Attr& attr, const arcxx::string_view str) {
        if(str != "null" && str != "NULL"){
            typename Attr::value_type tmp = static_cast<typename Attr::value_type>(0);
            std::from_chars(&(*str.begin()), &(*str.end()), tmp);
            attr = tmp;
        }
    }

    // string
    template<std::same_as<common_connector> Connector, is_attribute Attr>
    requires std::same_as<typename Attr::value_type, arcxx::string>
    [[nodiscard]] inline arcxx::string to_string(const Attr& attr, arcxx::string&& buff = {}) {
        // require sanitize
        if(attr){
            buff = concat_strings(std::move(buff), "\'", arcxx::sanitize(attr.value()), "\'");
        }
        else{
            buff += "null";
        }
        return std::move(buff);
    }
    template<std::same_as<common_connector> Connector, is_attribute Attr>
    requires std::same_as<typename Attr::value_type, arcxx::string>
    inline void from_string(Attr& attr, const arcxx::string_view str) {
        if(str != "null" && str != "NULL"){
            attr = arcxx::string{ str };
        }
    }

    // decimal
    template<std::same_as<common_connector> Connector, is_attribute Attr>
    requires std::floating_point<typename Attr::value_type>
    [[nodiscard]] inline arcxx::string to_string(const Attr& attr, arcxx::string&& buff = {}) {
        if(attr){
            std::array<arcxx::string::value_type, 32> str_buff{0};
            std::to_chars(&(*str_buff.begin()), &(*str_buff.end()), attr.value());
            buff += str_buff.data();
        }
        else{
            buff += "null";
        }
        return std::move(buff);
    }
    template<std::same_as<common_connector> Connector, is_attribute Attr>
    requires std::floating_point<typename Attr::value_type>
    inline void from_string(Attr& attr, const arcxx::string_view str){
        if(str != "null" && str != "NULL"){
            typename Attr::value_type tmp = static_cast<typename Attr::value_type>(0);
            std::from_chars(&(*str.begin()), &(*str.end()), tmp);
            attr = tmp;
        }
    }

    // datetime
    template<std::same_as<common_connector> Connector, is_attribute Attr>
    requires regarded_as_clock<typename Attr::value_type>
    [[nodiscard]] inline arcxx::string to_string(const Attr& attr, arcxx::string&& buff) {
        // YYYY-MM-DD hh:mm:ss
        using clock = Attr::value_type::clock;
        using duration = typename Attr::value_type::duration;
        namespace chrono = std::chrono;

        if(attr){
            #ifdef _MSC_VER
            using duration = typename Attr::value_type::duration;
            if constexpr(std::is_same_v<duration, chrono::days>){
                std::format_to(std::back_inserter(buff),"\'{:%F}\'", attr.value());
            }
            else {
                std::format_to(std::back_inserter(buff), "\'{:%F %T}\'", chrono::floor<chrono::seconds>(attr.value()));
            }
            #else
            const std::time_t t = clock::to_time_t(attr.value());
            std::tm* gmt = std::gmtime(&t);
            if(gmt == nullptr) throw std::runtime_error("std::gmtime causes error");
            char str[32]{0};
            const char* const fmt_str = [](){
                if constexpr(std::is_same_v<duration, chrono::days>) return "\'%F\'";
                else return "\'%F %T\'";
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
    template<std::same_as<common_connector> Connector, is_attribute Attr>
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
            attr = clock::from_time_t(t);
            #endif
        }
    }

    // boolean
    template<std::same_as<common_connector> Connector, is_attribute Attr>
    requires std::same_as<typename Attr::value_type, bool>
    [[nodiscard]] inline arcxx::string to_string(const Attr& attr, arcxx::string&& buff = {}) {
        if(attr){
            buff += (attr.value() ? "true" : "false");
        }
        else{
            buff += "null";
        }
        return std::move(buff);
    }
    template<std::same_as<common_connector> Connector, is_attribute Attr>
    requires std::same_as<typename Attr::value_type, bool>
    inline void from_string(Attr& attr, const arcxx::string_view str){
        if(str != "null" && str != "NULL"){
            attr = ((str == "false" || str == "0" || str == "False") ? false : true);
        }
    }

    // binary
    template<std::same_as<common_connector> Connector, is_attribute Attr>
    requires std::same_as<typename Attr::value_type, std::vector<std::byte>>
    [[nodiscard]] inline arcxx::string to_string(const Attr& attr) {
        return static_cast<bool>(attr) ? "" : "null";
    }
    template<std::same_as<common_connector> Connector, is_attribute Attr>
    requires std::same_as<typename Attr::value_type, std::vector<std::byte>>
    inline void from_string(Attr& attr, const arcxx::string_view str){
    }
}