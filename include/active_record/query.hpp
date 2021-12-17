#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT Lisence.
 */
#include "attribute.hpp"

namespace active_record {
    enum class query_operation {
        unspecified,
        select,
        insert,
        destroy,
        update,
        condition
    };

    enum class order {
        asc,
        desc
    };

    template<typename T, specialized_from<std::tuple> BindAttrs>
    struct query_relation;

    template<specialized_from<std::tuple> BindAttrs>
    struct query_condition;

    template<typename Result, typename... Args>
    requires ((Attribute<Args> || std::convertible_to<Args, active_record::string_view>) && ...)
    auto raw_query(Args&&... args);
}