#pragma once
#include "../attribute.hpp"
#include "../query.hpp"

namespace active_record {
    template<typename Model, typename Attribute, std::integral Integer>
    struct attribute<Model, Attribute, Integer> : public attribute_common<Model, Attribute, Integer> {
        using attribute_common<Model, Attribute, Integer>::attribute_common;
        inline static const attribute_common<Model, Attribute, Integer>::constraint auto_increment = [](const std::optional<Integer>& t) constexpr { return not_null(t) && unique(t); };
        
        [[nodiscard]] constexpr virtual active_record::string to_string() const override {
            return static_cast<bool>(*this) ? std::to_string(this->value()) : "null";
        }
    };

    namespace attributes {    
        template<typename Model, typename Attribute, std::integral Integer = int32_t>
        struct integer : public attribute<Model, Attribute, Integer>{
            using attribute<Model, Attribute, Integer>::attribute;
        };
    }
}