#pragma once
#include "attribute_common_impl.hpp"

namespace active_record {
    template<Attribute Attr>
    requires std::same_as<typename Attr::value_type, active_record::string>
    [[nodiscard]] constexpr active_record::string to_string(const Attr& attr) {
        // require sanitize
        return static_cast<bool>(attr) ? active_record::string{"\'"} + active_record::sanitize(attr.value()) + "\'" : "null";
    }
    template<Attribute Attr>
    requires std::same_as<typename Attr::value_type, active_record::string>
    void from_string(Attr& attr, const active_record::string_view str) {
        if(str != "null" && str != "NULL"){
            attr = active_record::string{ str };
        }
    }

    template<typename Model, typename Attribute>
    struct attribute<Model, Attribute, active_record::string> : public attribute_common<Model, Attribute, active_record::string> {
        using attribute_common<Model, Attribute, active_record::string>::attribute_common;
        
        template<std::convertible_to<active_record::string> StringType>
        constexpr attribute(const StringType& default_value) : attribute_common<Model, Attribute, active_record::string>(active_record::string{ default_value }) {}

        struct constraint_length_impl {
            const size_t length;
            constexpr bool operator()(const std::optional<active_record::string>& t) {
                return static_cast<bool>(t) && t.value().length() <= length;
            }
        };
        static const attribute_common<Model, Attribute, active_record::string>::constraint length(const size_t len) noexcept {
            return constraint_length_impl{ len };
        };

        template<std::convertible_to<active_record::string> StringType>
        static constexpr query_condition like(const StringType& value){
            // require sanitize
            constexpr auto names = Attribute::column_full_name();
            return query_condition {
                active_record::string{ "\"" } + active_record::string{ names.first } + "\".\"" + active_record::string{ names.second }
                    + "\" LIKE \'" + active_record::sanitize(value) + "\'"
            };
        }
        [[nodiscard]] constexpr virtual active_record::string to_string() const override {
            return active_record::to_string(*dynamic_cast<const Attribute*>(this));
        }
        virtual void from_string(const active_record::string& str) override {
            active_record::from_string(*dynamic_cast<Attribute*>(this), str);
        }
    };

    namespace attributes {
        template<typename Model, typename Attribute>
        struct string : public attribute<Model, Attribute, active_record::string>{
            using attribute<Model, Attribute, active_record::string>::attribute;
        };
    }
}