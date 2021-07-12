#pragma once
#include "utils.hpp"
#include <variant>
#include <any>
#include <vector>

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
    struct query_condition {
        using str_or_bind = std::variant<active_record::string, std::size_t>;
        std::vector<str_or_bind> condition;
        BindAttrs bind_attrs;
        std::vector<std::any> temporary_attrs;

        static constexpr std::size_t bind_attrs_count() {
            return std::tuple_size_v<BindAttrs>;
        }
    };
}