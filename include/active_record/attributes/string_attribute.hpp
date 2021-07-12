#pragma once
#include "attribute_common_impl.hpp"

namespace active_record {
    template<std::same_as<common_adaptor> Adaptor, Attribute Attr>
    requires std::same_as<typename Attr::value_type, active_record::string>
    [[nodiscard]] constexpr active_record::string to_string(const Attr& attr) {
        // require sanitize
        return static_cast<bool>(attr) ? active_record::string{"\'"} + active_record::sanitize(attr.value()) + "\'" : "null";
    }
    template<std::same_as<common_adaptor> Adaptor, Attribute Attr>
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
            const std::size_t length;
            constexpr bool operator()(const std::optional<active_record::string>& t) {
                return static_cast<bool>(t) && t.value().length() <= length;
            }
        };
        static const attribute_common<Model, Attribute, active_record::string>::constraint length(const std::size_t len) noexcept {
            return constraint_length_impl{ len };
        };

        template<std::convertible_to<active_record::string> StringType>
        static constexpr query_condition<std::tuple<const Attribute*>> like(const StringType& value){
            query_condition<std::tuple<const Attribute*>> ret;
            ret.temporary_attrs.push_back(Attribute{ value });
            std::get<0>(ret.bind_attrs) = std::any_cast<Attribute>(&(ret.temporary_attrs.back()));
            ret.condition.push_back(
                active_record::string{ "\"" } + Model::table_name + "\".\""
                + Attribute::column_name + "\" LIKE "
            );
            ret.condition.push_back(0);
            return ret;
        }

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
        struct string : public attribute<Model, Attribute, active_record::string>{
            using attribute<Model, Attribute, active_record::string>::attribute;
        };
    }
}