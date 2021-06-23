#pragma once
#include "../attribute.hpp"
#include "../query.hpp"
#include <vector>

namespace active_record {
    template<typename Model, typename Attribute>
    struct attribute<Model, Attribute, std::vector<std::byte>> : public attribute_common<Model, Attribute, std::vector<std::byte>> {
        using attribute_common<Model, Attribute, std::vector<std::byte>>::attribute_common;

        [[nodiscard]] constexpr virtual active_record::string to_string() const override {
            return static_cast<bool>(*this) ? "true" : "null";
        }
    };

    namespace attributes {
        template<typename Model, typename Attribute>
        struct binary : public attribute<Model, Attribute, std::vector<std::byte>>{
            using attribute<Model, Attribute, std::vector<std::byte>>::attribute;
        };
    }
}