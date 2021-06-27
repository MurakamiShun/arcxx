#pragma once
#include "attribute_common_impl.hpp"
#include <charconv>

namespace active_record {
    template<Attribute Attr>
    requires std::floating_point<typename Attr::value_type>
    [[nodiscard]] constexpr active_record::string to_string(const Attr& attr) {
        return static_cast<bool>(attr) ? std::to_string(attr.value()) : "null";
    }
    template<Attribute Attr>
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
        
        [[nodiscard]] constexpr virtual active_record::string to_string() const override {
            return active_record::to_string(*dynamic_cast<const Attribute*>(this));
        }
        virtual void from_string(const active_record::string& str) override {
            active_record::from_string(*dynamic_cast<Attribute*>(this), str);
        }
    };

    namespace attributes {
        template<typename Model, typename Attribute, std::floating_point FP = float>
        struct decimal : public attribute<Model, Attribute, FP>{
            using attribute<Model, Attribute, FP>::attribute;
        };
    }
}
