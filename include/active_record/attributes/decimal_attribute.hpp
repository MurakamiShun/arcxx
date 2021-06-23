#pragma once
#include "../attribute.hpp"
#include "../query.hpp"

namespace active_record {
    template<typename Model, typename Attribute, std::floating_point FP>
    struct attribute<Model, Attribute, FP> : attribute_common<Model, Attribute, FP> {
        using attribute_common<Model, Attribute, FP>::attribute_common;
        
        [[nodiscard]] constexpr virtual active_record::string to_string() const override {
            return static_cast<bool>(*this) ? std::to_string(this->value()) : "null";
        }
    };

    namespace attributes {
        template<typename Model, typename Attribute, std::floating_point FP = float>
        struct decimal : public attribute<Model, Attribute, FP>{
            using attribute<Model, Attribute, FP>::attribute;
        };
    }
}
