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
    requires std::same_as<typename Attr::value_type, bool>
    [[nodiscard]] constexpr active_record::string to_string(const Attr& attr) {
        return static_cast<bool>(attr) ? (attr.value() ? "true" : "false") : "null";
    }
    template<std::same_as<common_adaptor> Adaptor, Attribute Attr>
    requires std::same_as<typename Attr::value_type, bool>
    void from_string(Attr& attr, const active_record::string_view str){
        if(str != "null" && str != "NULL"){
            attr = ((str == "false" || str == "0" || str == "False") ? false : true);
        }
    }

    template<typename Model, typename Attribute>
    struct attribute<Model, Attribute, bool> : public attribute_common<Model, Attribute, bool> {
        using attribute_common<Model, Attribute, bool>::attribute_common;

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
        struct boolean : public attribute<Model, Attribute, bool>{
            using attribute<Model, Attribute, bool>::attribute;
        };
    }
}