#pragma once
/*
 * ARCXX: https://github.com/akisute514/arcxx
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
#include "attribute_common.hpp"

namespace arcxx {
    template<typename Model, typename Attribute, std::integral Integer>
    struct attribute<Model, Attribute, Integer> : public attribute_common<Model, Attribute, Integer> {
        struct avg_attribute : public attribute_common<Model, avg_attribute, double>{
            inline static const auto column_name = Attribute::column_name;
            using attribute_common<Model, avg_attribute, double>::attribute_common;
        };
        using attribute_common<Model, Attribute, Integer>::attribute_common;

        static constexpr struct auto_imcrement_impl : constraint<Integer>{
            constexpr bool operator()(const std::optional<Integer>&) noexcept { return false; }
        } auto_increment{};

        template<std::convertible_to<Integer> ArgType1, std::convertible_to<Integer> ArgType2>
        [[nodiscard]] static auto between(const ArgType1 value1, const ArgType2 value2){
            query_condition<std::tuple<Attribute, Attribute>> ret;
            ret.bind_attrs =std::make_tuple(static_cast<Attribute>(value1), static_cast<Attribute>(value2));
            ret.condition.reserve(4);
            ret.condition.push_back(concat_strings(Attribute::column_full_name(), " BETWEEN "));
            ret.condition.push_back(static_cast<std::size_t>(0));
            ret.condition.push_back(" AND ");
            ret.condition.push_back(static_cast<std::size_t>(1));
            return ret;
        }

        struct sum : public attribute_aggregator<Model, Attribute, sum> {
            inline static decltype(auto) aggregation_func = "sum";
        };
        struct avg : public attribute_aggregator<Model, avg_attribute, avg> {
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
        template<typename Model, typename Attribute, std::integral Integer = int32_t>
        struct integer : public attribute<Model, Attribute, Integer>{
            using attribute<Model, Attribute, Integer>::attribute;
        };

        // int8
        template<typename Model, typename Attribute>
        struct i8_t : public attribute<Model, Attribute, int8_t>{
            using attribute<Model, Attribute, int8_t>::attribute;
        };
        template<typename Model, typename Attribute>
        struct u8_t : public attribute<Model, Attribute, uint8_t>{
            using attribute<Model, Attribute, int8_t>::attribute;
        };

        // int16
        template<typename Model, typename Attribute>
        struct i16_t : public attribute<Model, Attribute, int16_t>{
            using attribute<Model, Attribute, int8_t>::attribute;
        };
        template<typename Model, typename Attribute>
        struct u16_t : public attribute<Model, Attribute, uint16_t>{
            using attribute<Model, Attribute, int8_t>::attribute;
        };

        // int32
        template<typename Model, typename Attribute>
        struct i32_t : public attribute<Model, Attribute, int32_t>{
            using attribute<Model, Attribute, int8_t>::attribute;
        };
        template<typename Model, typename Attribute>
        struct u32_t : public attribute<Model, Attribute, uint32_t>{
            using attribute<Model, Attribute, int8_t>::attribute;
        };

        // int64
        template<typename Model, typename Attribute>
        struct i64_t : public attribute<Model, Attribute, int64_t>{
            using attribute<Model, Attribute, int8_t>::attribute;
        };
        template<typename Model, typename Attribute>
        struct u64_t : public attribute<Model, Attribute, uint64_t>{
            using attribute<Model, Attribute, int8_t>::attribute;
        };

    }
}