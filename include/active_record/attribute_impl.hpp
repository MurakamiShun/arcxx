#pragma once
#include "attribute.hpp"
#include "query.hpp"
//#include <format>
#include <vector>

namespace active_record {
    template<typename Model, typename Attribute>
    struct attribute<Model, Attribute, active_record::string> : public attribute_common<Model, Attribute, active_record::string> {
        using attribute_common<Model, Attribute, active_record::string>::attribute_common;
        
        template<std::convertible_to<active_record::string> StringType>
        constexpr attribute(const StringType& default_value) : attribute_common<Model, Attribute, active_record::string>(active_record::string{default_value}) {}

        [[nodiscard]] constexpr virtual active_record::string to_string() const override {
            // require sanitize
            return static_cast<bool>(*this) ? active_record::string{"\'"} + active_record::sanitize(this->value()) + "\'" : "null";
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

    template<typename Model, typename Attribute, std::integral Integer>
    struct attribute<Model, Attribute, Integer> : public attribute_common<Model, Attribute, Integer> {
        using attribute_common<Model, Attribute, Integer>::attribute_common;
        
        [[nodiscard]] constexpr virtual active_record::string to_string() const override {
            return static_cast<bool>(*this) ? std::to_string(this->value()) : "null";
        }
    };

    template<typename Model, typename Attribute, std::floating_point FP>
    struct attribute<Model, Attribute, FP> : attribute_common<Model, Attribute, FP> {
        using attribute_common<Model, Attribute, FP>::attribute_common;
        
        [[nodiscard]] constexpr virtual active_record::string to_string() const override {
            return static_cast<bool>(*this) ? std::to_string(this->value()) : "null";
        }
    };

    template<typename Model, typename Attribute>
    struct attribute<Model, Attribute, bool> : public attribute_common<Model, Attribute, bool> {
        using attribute_common<Model, Attribute, bool>::attribute_common;
        
        [[nodiscard]] constexpr virtual active_record::string to_string() const override {
            return static_cast<bool>(*this) ? (this->value() ? "true" : "false") : "null";
        }
    };


    template<typename Model, typename Attribute>
    struct attribute<Model, Attribute, active_record::datetime> : public attribute_common<Model, Attribute, active_record::datetime> {
        using attribute_common<Model, Attribute, active_record::datetime>::attribute_common;
        
        [[nodiscard]] constexpr virtual active_record::string to_string() const override {
            // ISO 8601 yyyyMMddTHHmmss+09:00
            //return static_cast<bool>(*this) ? std::format("%FT%T%z", this->value()) : "null";
            return active_record::string{ "" };
        }
    };

    template<typename Model, typename Attribute>
    struct attribute<Model, Attribute, std::vector<std::byte>> : public attribute_common<Model, Attribute, std::vector<std::byte>> {
        using attribute_common<Model, Attribute, std::vector<std::byte>>::attribute_common;

        [[nodiscard]] constexpr virtual active_record::string to_string() const override {
            return static_cast<bool>(*this) ? "true" : "null";
        }
    };

    namespace attributes {
        template<typename Model, typename Attribute>
        struct string : public attribute<Model, Attribute, active_record::string>{
            using attribute<Model, Attribute, active_record::string>::attribute;
        };
        
        template<typename Model, typename Attribute, std::integral Integer = int32_t>
        struct integer : public attribute<Model, Attribute, Integer>{
            using attribute<Model, Attribute, Integer>::attribute;
        };

        template<typename Model, typename Attribute, std::floating_point FP = float>
        struct decimal : public attribute<Model, Attribute, FP>{
            using attribute<Model, Attribute, FP>::attribute;
        };
        
        template<typename Model, typename Attribute>
        struct datetime : public attribute<Model, Attribute, active_record::datetime>{
            using attribute<Model, Attribute, active_record::datetime>::attribute;
        };
        
        template<typename Model, typename Attribute>
        struct boolean : public attribute<Model, Attribute, bool>{
            using attribute<Model, Attribute, bool>::attribute;
        };
        
        template<typename Model, typename Attribute>
        struct binary : public attribute<Model, Attribute, std::vector<std::byte>>{
            using attribute<Model, Attribute, std::vector<std::byte>>::attribute;
        };
    }
}