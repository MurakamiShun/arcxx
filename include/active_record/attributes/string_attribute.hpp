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
#include "attribute_common.hpp"

namespace active_record {
    template<std::same_as<common_adaptor> Adaptor, Attribute Attr>
    requires std::same_as<typename Attr::value_type, active_record::string>
    [[nodiscard]] constexpr active_record::string to_string(const Attr& attr) {
        // require sanitize
        return static_cast<bool>(attr) ? concat_strings("\'", active_record::sanitize(attr.value()), "\'") : "null";
    }
    template<std::same_as<common_adaptor> Adaptor, Attribute Attr>
    requires std::same_as<typename Attr::value_type, active_record::string>
    void from_string(Attr& attr, const active_record::string_view str) {
        if(str != "null" && str != "NULL"){
            attr = active_record::string{ str };
        }
    }

    template<typename Model, typename Attribute>
    struct attribute<Model, Attribute, active_record::string> : public attribute_common<Model, Attribute, active_record::string> {
        using attribute_common<Model, Attribute, active_record::string>::attribute_common;

        template<std::convertible_to<active_record::string> StringType>
        constexpr attribute(const StringType& default_value) : attribute_common<Model, Attribute, active_record::string>(active_record::string{ default_value }) {}

        struct constraint_length_impl {
            const std::size_t length;
            constexpr bool operator()(const std::optional<active_record::string>& t) {
                return static_cast<bool>(t) && t.value().length() <= length;
            }
        };
        [[nodiscard]] static const typename attribute_common<Model, Attribute, active_record::string>::constraint length(const std::size_t len) noexcept {
            return constraint_length_impl{ len };
        };

        template<std::convertible_to<active_record::string> StringType>
        [[nodiscard]] static constexpr query_condition<std::tuple<Attribute>> like(const StringType& value){
            query_condition<std::tuple<Attribute>> ret;
            ret.bind_attrs = std::make_tuple<Attribute>(active_record::string{ value });
            ret.condition.push_back(concat_strings(Attribute::column_full_name(), " LIKE "));
            ret.condition.push_back(static_cast<std::size_t>(0));
            return ret;
        }

        template<std::derived_from<adaptor> Adaptor = common_adaptor>
        [[nodiscard]] constexpr active_record::string to_string() const {
            return active_record::to_string<Adaptor>(*this);
        }
        template<std::derived_from<adaptor> Adaptor = common_adaptor>
        void from_string(const active_record::string_view str) {
            active_record::from_string<Adaptor>(*this, str);
        }
    };

    namespace attributes {
        template<typename Model, typename Attribute>
        struct string : public attribute<Model, Attribute, active_record::string>{
            using attribute<Model, Attribute, active_record::string>::attribute;
        };
    }
}