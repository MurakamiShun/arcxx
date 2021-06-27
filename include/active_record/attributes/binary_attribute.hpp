#pragma once
#include "attribute_common_impl.hpp"
#include <vector>

namespace active_record {
    template<Attribute Attr>
    requires std::same_as<typename Attr::value_type, std::vector<std::byte>>
    [[nodiscard]] constexpr active_record::string to_string(const Attr& attr) {
        return static_cast<bool>(attr) ? "true" : "null";
    }
    template<Attribute Attr>
    requires std::same_as<typename Attr::value_type, std::vector<std::byte>>
    void from_string(Attr& attr, const active_record::string_view str){

    }

    template<typename Model, typename Attribute>
    struct attribute<Model, Attribute, std::vector<std::byte>> : public attribute_common<Model, Attribute, std::vector<std::byte>> {
        using attribute_common<Model, Attribute, std::vector<std::byte>>::attribute_common;
        
        [[nodiscard]] constexpr virtual active_record::string to_string() const override {
            return active_record::to_string(*dynamic_cast<const Attribute*>(this));
        }
        virtual void from_string(const active_record::string& str) override {
            active_record::from_string(*dynamic_cast<Attribute*>(this), str);
        }
    };

    namespace attributes {
        template<typename Model, typename Attribute>
        struct binary : public attribute<Model, Attribute, std::vector<std::byte>>{
            using attribute<Model, Attribute, std::vector<std::byte>>::attribute;
        };
    }
}