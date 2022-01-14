#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
#include "attribute_common.hpp"

namespace active_record::attributes {
    template<typename Table, typename Attribute, typename ForeignKey>
    struct foreign_key : public attribute<Table, Attribute, typename ForeignKey::value_type> {
        using attribute<Table, Attribute, typename ForeignKey::value_type>::attribute;
        using foreign_key_type = ForeignKey;
    };
}