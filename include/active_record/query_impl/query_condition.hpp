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
#include "../query.hpp"
#include "../attribute.hpp"
#include "query_utils.hpp"

namespace active_record {
    template<Tuple BindAttrs>
    struct query_condition {
    private:
        enum class conjunction{
            AND,
            OR
        };

        template<Tuple SrcBindAttrs>
        [[nodiscard]] query_condition<tuptup::tuple_cat_t<BindAttrs, SrcBindAttrs>> concat_conditions(query_condition<SrcBindAttrs>&&, const conjunction);
    public:
        using str_or_bind = std::variant<active_record::string, std::size_t>;
        std::vector<str_or_bind> condition;
        BindAttrs bind_attrs;

        [[nodiscard]] static consteval std::size_t bind_attrs_count() noexcept {
            return std::tuple_size_v<BindAttrs>;
        }

        template<Tuple DestBindAttrs>
        auto operator&&(query_condition<DestBindAttrs>&& cond) && {
            return concat_conditions(std::move(cond), conjunction::AND);
        }
        template<Tuple DestBindAttrs>
        auto operator||(query_condition<DestBindAttrs>&& cond) && {
            return concat_conditions(std::move(cond), conjunction::OR);
        }

        template<Attribute Attr>
        auto operator&&(const Attr& cond) && {
            return this->operator&&(Attr::cmp == cond);
        }
        template<Attribute Attr>
        auto operator||(const Attr& cond) && {
            return this->operator||(Attr::cmp == cond);
        }
    };
}

#include "query_condition_impl.ipp"