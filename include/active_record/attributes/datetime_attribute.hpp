#pragma once
#include "../attribute.hpp"
#include "../query.hpp"

namespace active_record {
    template<typename Model, typename Attribute>
    struct attribute<Model, Attribute, active_record::datetime> : public attribute_common<Model, Attribute, active_record::datetime> {
        using attribute_common<Model, Attribute, active_record::datetime>::attribute_common;
        
        [[nodiscard]] constexpr virtual active_record::string to_string() const override {
            // ISO 8601 yyyyMMddTHHmmss+09:00
            //return static_cast<bool>(*this) ? std::format("%FT%T%z", this->value()) : "null";
            return active_record::string{ "" };
        }
    };

    namespace attributes {
        template<typename Model, typename Attribute>
        struct datetime : public attribute<Model, Attribute, active_record::datetime> {
            using attribute<Model, Attribute, active_record::datetime>::attribute;
        };
    }
}