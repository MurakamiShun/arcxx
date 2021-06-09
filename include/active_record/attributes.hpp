#include "attribute.hpp"
//#include <format>

namespace active_record {
    namespace attributes {
        template<typename Model, typename Attribute>
        struct string : public attribute<Model, Attribute, active_record::string> {
            using attribute<Model, Attribute, active_record::string>::attribute;

            constexpr string(const active_record::string_view default_value) : attribute<Model, Attribute, active_record::string>(active_record::string(default_value)) {}
            constexpr string(active_record::string::const_pointer default_value) : attribute<Model, Attribute, active_record::string>(active_record::string(default_value)) {}

            [[nodiscard]] constexpr virtual active_record::string to_string() const override {
                return static_cast<bool>(*this) ? this->value() : "null";
            }
        };

        template<typename Model, typename Attribute>
        struct integer : public attribute<Model, Attribute, int32_t> {
            using attribute<Model, Attribute, int32_t>::attribute;
            
            [[nodiscard]] constexpr virtual active_record::string to_string() const override {
                return static_cast<bool>(*this) ? std::to_string(this->value()) : "null";
            }
        };

        template<typename Model, typename Attribute>
        struct decimal : public attribute<Model, Attribute, float> {
            using attribute<Model, Attribute, float>::attribute;
            
            [[nodiscard]] constexpr virtual active_record::string to_string() const override {
                return static_cast<bool>(*this) ? std::to_string(this->value()) : "null";
            }
        };

        template<typename Model, typename Attribute>
        struct boolean : public attribute<Model, Attribute, bool> {
            using attribute<Model, Attribute, bool>::attribute;
            
            [[nodiscard]] constexpr virtual active_record::string to_string() const override {
                return static_cast<bool>(*this) ? (this->value() ? "true" : "false") : "null";
            }
        };

        template<typename Model, typename Attribute>
        struct datetime : public attribute<Model, Attribute, active_record::datetime> {
            using attribute<Model, Attribute, active_record::datetime>::attribute;
            
            [[nodiscard]] constexpr virtual active_record::string to_string() const override {
                // ISO 8601 yyyyMMddTHHmmss+09:00
                //return static_cast<bool>(*this) ? std::format("%FT%T%z", this->value()) : "null";
                return active_record::string{ "" };
            }
        };

        template<typename Model, typename Attribute>
        struct binary : public attribute<Model, Attribute, std::vector<std::byte>> {
            using attribute<Model, Attribute, std::vector<std::byte>>::attribute;

            [[nodiscard]] constexpr virtual active_record::string to_string() const override {
                return static_cast<bool>(*this) ? "true" : "null";
            }
        };
    }
}