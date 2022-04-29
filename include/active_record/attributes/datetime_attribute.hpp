#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
#include "attribute_common.hpp"

namespace active_record {
    template<typename T>
    concept regarded_as_clock = specialized_from<T, std::chrono::time_point>;

    using system_datetime = std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>;
    using system_date = std::chrono::time_point<std::chrono::system_clock, std::chrono::days>;

    template<typename Model, typename Attribute, regarded_as_clock DateTime>
    struct attribute<Model, Attribute, DateTime> : public attribute_common<Model, Attribute, DateTime> {
        using attribute_common<Model, Attribute, DateTime>::attribute_common;

        template<typename T>
        requires requires { {std::chrono::duration_cast<typename DateTime::duration>(std::declval<T>())}->std::same_as<DateTime>; }
        attribute(T&& t) : 
            attribute_common<Model, Attribute, DateTime>(std::chrono::duration_cast<typename DateTime::duration>(std::forward<T>(t))){
        }

        inline static const typename attribute_common<Model, Attribute, DateTime>::constraint current_timestamp = [](const std::optional<DateTime>&) constexpr { return true; };

        struct max : public attribute_aggregator<Model, Attribute, max> {
            inline static decltype(auto) aggregation_func = "max";
        };
        struct min : public attribute_aggregator<Model, Attribute, min> {
            inline static decltype(auto) aggregation_func = "min";
        };

        template<std::convertible_to<DateTime> ArgType1, std::convertible_to<DateTime> ArgType2>
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
    };

    namespace attributes {
        template<typename Model, typename Attribute>
        struct datetime : attribute<Model, Attribute, system_datetime>{
            using attribute<Model, Attribute, system_datetime>::attribute;
        };

        template<typename Model, typename Attribute>
        struct date : attribute<Model, Attribute, system_date>{
            using attribute<Model, Attribute, system_date>::attribute;
        };
    }
}