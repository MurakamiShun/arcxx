#pragma once
#include "../attribute.hpp"
#include "../query.hpp"

namespace active_record {
    template<typename Model, typename Attribute>
    struct attribute<Model, Attribute, active_record::string> : public attribute_common<Model, Attribute, active_record::string> {
        using attribute_common<Model, Attribute, active_record::string>::attribute_common;
        
        template<std::convertible_to<active_record::string> StringType>
        constexpr attribute(const StringType& default_value) : attribute_common<Model, Attribute, active_record::string>(active_record::string{default_value}) {}

        struct constraint_length_impl {
            const size_t length;
            constexpr bool operator()(const std::optional<active_record::string>& t) {
                return static_cast<bool>(t) && t.value().length() <= length;
            }
        };
        static const attribute_common<Model, Attribute, active_record::string>::constraint length(const size_t len) noexcept {
            return constraint_length_impl{ len };
        };

        [[nodiscard]] constexpr virtual active_record::string to_string() const override {
            // require sanitize
            return static_cast<bool>(*this) ? active_record::string{"\'"} + active_record::sanitize(this->value()) + "\'" : "null";
        }
        virtual void from_string(const active_record::string& str) override {
            if(str != "null" && str != "NULL"){
                this->data = str;
            }
        }

        template<std::convertible_to<active_record::string> StringType>
        static constexpr query_condition like(const StringType& value){
            // require sanitize
            constexpr auto names = Attribute::column_full_name();
            return query_condition {
                active_record::string{ "\"" } + active_record::string{ names.first } + "\".\"" + active_record::string{ names.second }
                    + "\" LIKE \'" + active_record::sanitize(value) + "\'"
            };
        }
    };

    namespace attributes {
        template<typename Model, typename Attribute>
        struct string : public attribute<Model, Attribute, active_record::string>{
            using attribute<Model, Attribute, active_record::string>::attribute;
        };
    }
}