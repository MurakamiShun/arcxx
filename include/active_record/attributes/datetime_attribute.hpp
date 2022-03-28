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
    concept regarded_as_clock = requires{
        { T::now() } -> std::same_as<typename T::time_point>;
        T::to_sys();
        T::from_sys();
    };

    template<typename Model, typename Attribute, regarded_as_clock DateTime>
    struct attribute<Model, Attribute, DateTime> : public attribute_common<Model, Attribute, DateTime> {
        using attribute_common<Model, Attribute, DateTime>::attribute_common;

        inline static const typename attribute_common<Model, Attribute, DateTime>::constraint current_timestamp = [](const std::optional<DateTime>&) constexpr { return true; };

        struct max : public attribute_aggregator<Model, Attribute, max> {
            inline static decltype(auto) aggregation_func = "max";
        };
        struct min : public attribute_aggregator<Model, Attribute, min> {
            inline static decltype(auto) aggregation_func = "min";
        };
    };

    namespace attributes {
        template<typename Model, typename Attribute>
        using utc_time = attribute<Model, Attribute, std::chrono::system_clock>;
    }
}