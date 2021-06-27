#pragma once
#include "attribute_common_impl.hpp"

namespace active_record {
    template<Attribute Attr>
    requires std::same_as<typename Attr::value_type, active_record::datetime>
    [[nodiscard]] constexpr active_record::string to_string(const Attr& attr) {
        // ISO 8601 yyyyMMddTHHmmss+09:00
        //return static_cast<bool>(attr) ? std::format("%FT%T%z", attr.value()) : "null";
        return active_record::string{ "" };
    }
    template<Attribute Attr>
    requires std::same_as<typename Attr::value_type, active_record::datetime>
    void from_string(Attr& attr, const active_record::string_view str){
        
    }

    template<typename Model, typename Attribute>
    struct attribute<Model, Attribute, active_record::datetime> : public attribute_common<Model, Attribute, active_record::datetime> {
        using attribute_common<Model, Attribute, active_record::datetime>::attribute_common;
        
        [[nodiscard]] constexpr virtual active_record::string to_string() const override {
            return active_record::to_string(*dynamic_cast<const Attribute*>(this));
        }
        virtual void from_string(const active_record::string& str) override {
            active_record::from_string(*dynamic_cast<Attribute*>(this), str);
        }
    };

    namespace attributes {
        template<typename Model, typename Attribute>
        struct datetime : public attribute<Model, Attribute, active_record::datetime> {
            using attribute<Model, Attribute, active_record::datetime>::attribute;
        };
    }
}