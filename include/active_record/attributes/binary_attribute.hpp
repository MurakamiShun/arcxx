#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT Lisence.
 */
#include "attribute_common.hpp"

namespace active_record {
    template<std::same_as<common_adaptor> Adaptor, Attribute Attr>
    requires std::same_as<typename Attr::value_type, std::vector<std::byte>>
    [[nodiscard]] active_record::string to_string(const Attr& attr) {
        return static_cast<bool>(attr) ? "" : "null";
    }
    template<std::same_as<common_adaptor> Adaptor, Attribute Attr>
    requires std::same_as<typename Attr::value_type, std::vector<std::byte>>
    void from_string(Attr&, const active_record::string_view){
    }

    template<typename Model, typename Attribute>
    struct attribute<Model, Attribute, std::vector<std::byte>> : public attribute_common<Model, Attribute, std::vector<std::byte>> {
        using attribute_common<Model, Attribute, std::vector<std::byte>>::attribute_common;

        template<std::derived_from<adaptor> Adaptor = common_adaptor>
        [[nodiscard]] active_record::string to_string() const {
            return active_record::to_string<Adaptor>(*this);
        }
        template<std::derived_from<adaptor> Adaptor = common_adaptor>
        void from_string(const active_record::string_view str) {
            active_record::from_string<Adaptor>(*this, str);
        }
    };

    namespace attributes {
        template<typename Model, typename Attribute>
        struct binary : public attribute<Model, Attribute, std::vector<std::byte>>{
            using attribute<Model, Attribute, std::vector<std::byte>>::attribute;
        };
    }
}