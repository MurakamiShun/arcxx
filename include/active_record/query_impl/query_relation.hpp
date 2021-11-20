#pragma once
/*
 * Copyright 2021 akisute514
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "../model.hpp"
#include "../query.hpp"
#include "../attribute.hpp"
#include "../adaptor.hpp"
#include "query_condition.hpp"
#include "query_utils.hpp"

namespace active_record {
    template<specialized_from<std::tuple> BindAttrs>
    struct query_relation_common {
    public:
        using str_or_bind = std::variant<active_record::string, std::size_t>;
    private:
        template<std::derived_from<adaptor> Adaptor>
        struct sob_to_string_impl;
    public:
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
    struct query_relation<bool, BindAttrs> : public query_relation_common<BindAttrs> {
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
#include "query_relation_impl/result_scalar.ipp"
#include "query_relation_impl/result_vector.ipp"
#include "query_relation_impl/result_map.ipp"