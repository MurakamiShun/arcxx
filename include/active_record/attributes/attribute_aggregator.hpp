#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT Lisence.
 */
#include "../attribute.hpp"

namespace active_record {
    template<typename Model, typename Attribute, typename Aggregator>
    struct attribute_aggregator {
        using model_type = Model;
        using attribute_type = Attribute;
        using aggregator_type = Aggregator;
        using value_type = typename Attribute::value_type;

        static constexpr auto column_full_name() {
            return concat_strings(Aggregator::aggregation_func, "(", Attribute::column_full_name(), ")");
        }
    };
}