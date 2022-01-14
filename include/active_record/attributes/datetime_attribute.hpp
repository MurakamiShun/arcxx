#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
#include "attribute_common.hpp"

namespace active_record {
    template<typename Model, typename Attribute>
    struct attribute<Model, Attribute, active_record::datetime> : public attribute_common<Model, Attribute, active_record::datetime> {
        using attribute_common<Model, Attribute, active_record::datetime>::attribute_common;

        inline static const attribute_common<Model, Attribute, active_record::datetime>::constraint current_timestamp = [](const std::optional<active_record::datetime>& t) constexpr { return true; };

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