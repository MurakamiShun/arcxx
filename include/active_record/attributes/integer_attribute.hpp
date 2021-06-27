#pragma once
#include "attribute_common_impl.hpp"
#include <charconv>

namespace active_record {
    template<Attribute Attr>
    requires std::integral<typename Attr::value_type>
    [[nodiscard]] constexpr active_record::string to_string(const Attr& attr) {
        return static_cast<bool>(attr) ? std::to_string(attr.value()) : "null";
    }
    template<Attribute Attr>
    requires std::integral<typename Attr::value_type>
    void from_string(Attr& attr, const active_record::string_view str) {
        if(str != "null" && str != "NULL"){
            typename Attr::value_type tmp;
            std::from_chars(&*str.begin(), &*str.end(), tmp);
            attr = tmp;
        }
    }

    template<typename Model, typename Attribute, std::integral Integer>
    struct attribute<Model, Attribute, Integer> : public attribute_common<Model, Attribute, Integer> {
        using attribute_common<Model, Attribute, Integer>::attribute_common;
        
        inline static const attribute_common<Model, Attribute, Integer>::constraint auto_increment = [](const std::optional<Integer>& t) constexpr { return not_null(t) && unique(t); };
        [[nodiscard]] constexpr virtual active_record::string to_string() const override {
            return active_record::to_string(*dynamic_cast<const Attribute*>(this));
        }
        virtual void from_string(const active_record::string& str) override {
            active_record::from_string(*dynamic_cast<Attribute*>(this), str);
        }
    };

    namespace attributes {    
        template<typename Model, typename Attribute, std::integral Integer = int32_t>
        struct integer : public attribute<Model, Attribute, Integer>{
            using attribute<Model, Attribute, Integer>::attribute;
        };
    }
}