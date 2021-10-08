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
    struct query_condition;

    template<typename Result, typename... Args>
    requires ((Attribute<Args> || std::convertible_to<Args, active_record::string_view>) && ...)
    auto raw_query(Args&&... args);
}