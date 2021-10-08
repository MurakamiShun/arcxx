#pragma once
/*
 * Copyright 2021 akisute514
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "../../adaptor.hpp"
#include "../../attributes/attributes.hpp"

namespace active_record {
    class sqlite3_adaptor;

    inline namespace sqlite3_string_convertors{
        // boolean
        template<std::same_as<sqlite3_adaptor> Adaptor, Attribute Attr>
        requires std::same_as<typename Attr::value_type, bool>
        [[nodiscard]] constexpr active_record::string to_string(const Attr& attr) {
            return static_cast<bool>(attr) ? (attr.value() ? "1" : "0") : "null";
        }
        template<std::same_as<sqlite3_adaptor> Adaptor, Attribute Attr>
        requires std::same_as<typename Attr::value_type, bool>
        void from_string(Attr& attr, const active_record::string_view str){
            if(str != "null"){
                attr = ((str == "0") ? false : true);
            }
        }

        // integer
        template<std::same_as<sqlite3_adaptor> Adaptor, Attribute Attr>
        requires std::integral<typename Attr::value_type>
        [[nodiscard]] constexpr active_record::string to_string(const Attr& attr) {
            return to_string<common_adaptor>(attr);
        }
        template<std::same_as<sqlite3_adaptor> Adaptor, Attribute Attr>
        requires std::integral<typename Attr::value_type>
        void from_string(Attr& attr, const active_record::string_view str) {
            from_string<common_adaptor>(attr, str);
        }

        // decimal
        template<std::same_as<sqlite3_adaptor> Adaptor, Attribute Attr>
        requires std::floating_point<typename Attr::value_type>
        [[nodiscard]] constexpr active_record::string to_string(const Attr& attr) {
            return to_string<common_adaptor>(attr);
        }
        template<std::same_as<sqlite3_adaptor> Adaptor, Attribute Attr>
        requires std::floating_point<typename Attr::value_type>
        void from_string(Attr& attr, const active_record::string_view str){
            from_string<common_adaptor>(attr, str);
        }

        // string
        template<std::same_as<sqlite3_adaptor> Adaptor, Attribute Attr>
        requires std::same_as<typename Attr::value_type, active_record::string>
        [[nodiscard]] constexpr active_record::string to_string(const Attr& attr) {
            return to_string<common_adaptor>(attr);
        }
        template<std::same_as<sqlite3_adaptor> Adaptor, Attribute Attr>
        requires std::same_as<typename Attr::value_type, active_record::string>
        void from_string(Attr& attr, const active_record::string_view str) {
            from_string<common_adaptor>(attr, str);
        }

        // datetime
        template<std::same_as<sqlite3_adaptor> Adaptor, Attribute Attr>
        requires std::same_as<typename Attr::value_type, active_record::datetime>
        [[nodiscard]] constexpr active_record::string to_string(const Attr& attr) {
            // ISO 8601 yyyyMMddTHHmmss (sqlite supports only utc)
            //return static_cast<bool>(attr) ? std::format("%FT%T", attr.value()) : "null";
            return "";
        }
        template<std::same_as<sqlite3_adaptor> Adaptor, Attribute Attr>
        requires std::same_as<typename Attr::value_type, active_record::datetime>
        void from_string(Attr& attr, const active_record::string_view str){
            active_record::datetime dt;
            //std::chrono::parse("%fT%T", dt, str);
            attr = dt;
        }
        template<std::same_as<sqlite3_adaptor> Adaptor, Attribute Attr>
        requires std::same_as<typename Attr::value_type, std::vector<std::byte>>
        [[nodiscard]] constexpr active_record::string to_string(const Attr& attr) {
            active_record::string hex = "x'";
            char buf[4];
            for(const auto b : attr.value()){
                hex += active_record::string_view{buf, std::to_chars(buf, &buf[3], b, 16) + 1};
            }
            return static_cast<bool>(attr) ? hex + "'" : "null";
        }
        template<std::same_as<sqlite3_adaptor> Adaptor, Attribute Attr>
        requires std::same_as<typename Attr::value_type, std::vector<std::byte>>
        void from_string(Attr& attr, const active_record::string_view str){
            attr = std::vector<std::byte>{};
            attr.value().reserve(str.size());
            std::copy(str.begin(), str.end(), attr.value().begin());
        }
    }
}