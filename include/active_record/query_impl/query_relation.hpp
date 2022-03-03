#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
#include "../model.hpp"
#include "query_utils.hpp"
#include "query_condition.hpp"
#include "../adaptors/common_adaptor.hpp"

namespace active_record {
    template<specialized_from<std::tuple> BindAttrs>
    struct query_relation_common {
    private:
        template<std::derived_from<adaptor> Adaptor>
        struct sob_to_string_impl;
    public:
        using str_or_bind = std::variant<active_record::string, std::size_t>;

        const query_operation operation;
        std::vector<str_or_bind> op_args;
        std::vector<str_or_bind> tables;
        std::vector<str_or_bind> conditions;
        std::vector<str_or_bind> options; // order, limit

        BindAttrs bind_attrs;

        [[nodiscard]] static consteval std::size_t bind_attrs_count() noexcept {
            return std::tuple_size_v<BindAttrs>;
        }

        query_relation_common(const query_operation op) :
            operation(op) {
        }

        template<std::derived_from<adaptor> Adaptor = common_adaptor>
        [[nodiscard]] const active_record::string to_sql() const;
    };
}
#include "query_relation_common_impl.ipp"

namespace active_record{
    template<specialized_from<std::tuple> BindAttrs>
    struct query_relation<void, BindAttrs> : public query_relation_common<BindAttrs> {
        using query_relation_common<BindAttrs>::query_relation_common;
        template<std::derived_from<adaptor> Adaptor>
        auto exec(Adaptor& adapt) const {
            return adapt.exec(*this);
        }
    };
}
/*
 * Clang and MSVC has this bug.
 * https://bugs.llvm.org/show_bug.cgi?id=48020
 */
#include "query_relation_impl.ipp"
#include "model_query_impl.ipp"