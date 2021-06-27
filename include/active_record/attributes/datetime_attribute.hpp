#pragma once
#include "attribute_common_impl.hpp"

namespace active_record {
    template<std::same_as<common_adaptor> Adaptor, Attribute Attr>
    requires std::same_as<typename Attr::value_type, active_record::datetime>
    [[nodiscard]] constexpr active_record::string to_string(const Attr& attr) {
        // ISO 8601 yyyyMMddTHHmmss+09:00
        //return static_cast<bool>(attr) ? std::format("%FT%T%z", attr.value()) : "null";
        return active_record::string{ "" };
    }
    template<std::same_as<common_adaptor> Adaptor, Attribute Attr>
    requires std::same_as<typename Attr::value_type, active_record::datetime>
    void from_string(Attr& attr, const active_record::string_view str){
        active_record::datetime dt;
        //std::chrono::parse("%fT%T%z", dt, str);
        attr = dt;
    }

    template<typename Model, typename Attribute>
    struct attribute<Model, Attribute, active_record::datetime> : public attribute_common<Model, Attribute, active_record::datetime> {
        using attribute_common<Model, Attribute, active_record::datetime>::attribute_common;

        template<std::derived_from<adaptor> Adaptor = common_adaptor>
        [[nodiscard]] constexpr active_record::string to_string() const {
            return active_record::to_string<Adaptor>(*dynamic_cast<const Attribute*>(this));
        }
        template<std::derived_from<adaptor> Adaptor = common_adaptor>
        void from_string(const active_record::string_view str) {
            active_record::from_string<Adaptor>(*dynamic_cast<Attribute*>(this), str);
        }
    };

    namespace attributes {
        template<typename Model, typename Attribute>
        struct datetime : public attribute<Model, Attribute, active_record::datetime> {
            using attribute<Model, Attribute, active_record::datetime>::attribute;
        };
    }
}