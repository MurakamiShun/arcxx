#pragma once
#include "attribute_common_impl.hpp"
#include <charconv>

namespace active_record {
    template<std::same_as<common_adaptor> Adaptor, Attribute Attr>
    requires std::floating_point<typename Attr::value_type>
    [[nodiscard]] constexpr active_record::string to_string(const Attr& attr) {
        return static_cast<bool>(attr) ? std::to_string(attr.value()) : "null";
    }
    template<std::same_as<common_adaptor> Adaptor, Attribute Attr>
    requires std::floating_point<typename Attr::value_type>
    void from_string(Attr& attr, const active_record::string_view str){
        if(str != "null" && str != "NULL"){
            typename Attr::value_type tmp;
            std::from_chars(&*str.begin(), &*str.end(), tmp);
            attr = tmp;
        }
    }
    
    template<typename Model, typename Attribute, std::floating_point FP>
    struct attribute<Model, Attribute, FP> : attribute_common<Model, Attribute, FP> {
        using attribute_common<Model, Attribute, FP>::attribute_common;

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
        template<typename Model, typename Attribute, std::floating_point FP = float>
        struct decimal : public attribute<Model, Attribute, FP>{
            using attribute<Model, Attribute, FP>::attribute;
        };
    }
}
