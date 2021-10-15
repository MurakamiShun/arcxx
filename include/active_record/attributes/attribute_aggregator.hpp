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
#include "../attribute.hpp"

namespace active_record {
    template<typename Model, typename Attribute, typename Aggregator>
    struct attribute_aggregator {
        using model_type = Model;
        using attribute_type = Attribute;
        using aggregator_type = Aggregator;
        using value_type = typename Attribute::value_type;

        static constexpr auto column_full_name() {
            return concat_strings(Aggregator::aggregation_func, "(", Attribute::column_full_name(), ")");
        }
    };
}