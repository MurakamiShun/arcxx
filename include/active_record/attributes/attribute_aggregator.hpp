#pragma once
#include "../attribute.hpp"

template<typename Model, typename Attribute, typename Aggregator>
struct attribute_aggregator {
    using model_type = Model;
    using attribute_type = Attribute;
    using aggregator_type = Aggregator;
    using value_type = Attribute::value_type;


    static constexpr active_record::string column_full_name() {
        return Aggregator::aggregation_func + "(" + Attribute::column_full_name() + ")";
    }
};