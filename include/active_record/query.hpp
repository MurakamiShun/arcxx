#pragma once
#include "utils.hpp"

namespace active_record {
    enum class query_operation {
        unspecified,
        select,
        insert,
        destroy,
        update,
        condition,
        create_table
    };

    enum class order {
        asc,
        desc
    };
    template<typename T>
    struct query_relation;

    struct query_condition {
        active_record::string str;
    };

    template<typename T>
    query_relation<T> raw_query(const active_record::string_view);
}