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
#include "attribute_common.hpp"

namespace active_record::attributes {
    template<typename Table, typename Attribute, typename ForeignKey>
    struct foreign_key : public attribute<Table, Attribute, typename ForeignKey::value_type> {
        using attribute<Table, Attribute, typename ForeignKey::value_type>::attribute;
        using foreign_key_type = ForeignKey;
    };
}