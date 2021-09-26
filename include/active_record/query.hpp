#pragma once
#include "utils.hpp"
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

    template<typename T, Tuple BindAttrs>
    struct query_relation;

    template<Tuple BindAttrs>
    struct query_expression;

    template<Tuple BindAttrs>
    struct query_condition;

    template<typename Result, typename... Args>
    requires ((Attribute<Args> || std::convertible_to<Args, active_record::string_view>) && ...)
    auto raw_query(Args&&... args);
}