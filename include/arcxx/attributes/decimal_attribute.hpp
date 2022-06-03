#pragma once
/*
 * ARCXX: https://github.com/akisute514/arcxx
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
#include "attribute_common.hpp"

namespace arcxx {
    template<typename Model, typename Attribute, std::floating_point FP>
    struct attribute<Model, Attribute, FP> : attribute_common<Model, Attribute, FP> {
        using attribute_common<Model, Attribute, FP>::attribute_common;

        template<std::convertible_to<FP> ArgType1, std::convertible_to<FP> ArgType2>
        [[nodiscard]] static auto between(const ArgType1 value1, const ArgType2 value2){
            query_condition<std::tuple<Attribute, Attribute>> ret;
            ret.bind_attrs = std::make_tuple(static_cast<Attribute>(value1), static_cast<Attribute>(value2));
            ret.condition.reserve(4);
            ret.condition.push_back(concat_strings(Attribute::column_full_name(), " BETWEEN "));
            ret.condition.push_back(0UL);
            ret.condition.push_back(" AND ");
            ret.condition.push_back(1UL);
            return ret;
        }

        struct sum : public attribute_aggregator<Model, Attribute, sum> {
            inline static decltype(auto) aggregation_func = "sum";
        };
        struct avg : public attribute_aggregator<Model, Attribute, avg> {
            inline static decltype(auto) aggregation_func = "avg";
        };
        struct max : public attribute_aggregator<Model, Attribute, max> {
            inline static decltype(auto) aggregation_func = "max";
        };
        struct min : public attribute_aggregator<Model, Attribute, min> {
            inline static decltype(auto) aggregation_func = "min";
        };
    };

    namespace attributes {
        template<typename Model, typename Attribute, std::floating_point FP = double>
        struct decimal : public attribute<Model, Attribute, FP>{
            using attribute<Model, Attribute, FP>::attribute;
        };

        template<typename Model, typename Attribute>
        struct f32_t : public attribute<Model, Attribute, float>{
            using attribute<Model, Attribute, float>::attribute;
        };
        template<typename Model, typename Attribute>
        struct f64_t : public attribute<Model, Attribute, double>{
            using attribute<Model, Attribute, double>::attribute;
        };
    }
}
