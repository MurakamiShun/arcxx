#pragma once
#include "attribute_common_impl.hpp"

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
            attr = ((str == "false" || str == "0") ? false : true);
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