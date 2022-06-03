#pragma once
/*
 * ARCXX: https://github.com/akisute514/arcxx
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
#include "attribute.hpp"

namespace arcxx {
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
    requires ((is_attribute<Args> || std::convertible_to<Args, arcxx::string_view>) && ...)
    auto raw_query(Args&&... args);
}