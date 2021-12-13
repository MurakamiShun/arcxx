#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT Lisence.
 */
#include "attribute_common.hpp"

namespace active_record {
    template<std::same_as<common_adaptor> Adaptor, Attribute Attr>
    requires std::same_as<typename Attr::value_type, active_record::datetime>
    [[nodiscard]] active_record::string to_string(const Attr&) {
        // ISO 8601 yyyyMMddTHHmmss+09:00
        //return static_cast<bool>(attr) ? std::format("%FT%T%z", attr.value()) : "null";
        return active_record::string{ "" };
    }
    template<std::same_as<common_adaptor> Adaptor, Attribute Attr>
    requires std::same_as<typename Attr::value_type, active_record::datetime>
    void from_string(Attr& attr, const active_record::string_view){
        active_record::datetime dt;
        //std::chrono::parse("%fT%T%z", dt, str);
        attr = dt;
    }

    template<typename Model, typename Attribute>
    struct attribute<Model, Attribute, active_record::datetime> : public attribute_common<Model, Attribute, active_record::datetime> {
        using attribute_common<Model, Attribute, active_record::datetime>::attribute_common;

        inline static const attribute_common<Model, Attribute, active_record::datetime>::constraint current_timestamp = [](const std::optional<active_record::datetime>& t) constexpr { return true; };

        template<std::derived_from<adaptor> Adaptor = common_adaptor>
        [[nodiscard]] active_record::string to_string() const {
            return active_record::to_string<Adaptor>(*this);
        }
        template<std::derived_from<adaptor> Adaptor = common_adaptor>
        void from_string(const active_record::string_view str) {
            active_record::from_string<Adaptor>(*this, str);
        }

        struct max : public attribute_aggregator<Model, Attribute, max> {
            inline static decltype(auto) aggregation_func = "max";
        };
        struct min : public attribute_aggregator<Model, Attribute, min> {
            inline static decltype(auto) aggregation_func = "min";
        };
    };

    namespace attributes {
        template<typename Model, typename Attribute>
        struct datetime : public attribute<Model, Attribute, active_record::datetime> {
            using attribute<Model, Attribute, active_record::datetime>::attribute;
        };
    }
}