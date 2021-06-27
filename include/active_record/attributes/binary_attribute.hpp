#pragma once
#include "attribute_common_impl.hpp"
#include <vector>

namespace active_record {
    template<std::same_as<common_adaptor> Adaptor, Attribute Attr>
    requires std::same_as<typename Attr::value_type, std::vector<std::byte>>
    [[nodiscard]] constexpr active_record::string to_string(const Attr& attr) {
        return static_cast<bool>(attr) ? "" : "null";
    }
    template<std::same_as<common_adaptor> Adaptor, Attribute Attr>
    requires std::same_as<typename Attr::value_type, std::vector<std::byte>>
    void from_string(Attr& attr, const active_record::string_view str){
    }

    template<typename Model, typename Attribute>
    struct attribute<Model, Attribute, std::vector<std::byte>> : public attribute_common<Model, Attribute, std::vector<std::byte>> {
        using attribute_common<Model, Attribute, std::vector<std::byte>>::attribute_common;

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
        struct binary : public attribute<Model, Attribute, std::vector<std::byte>>{
            using attribute<Model, Attribute, std::vector<std::byte>>::attribute;
        };
    }
}