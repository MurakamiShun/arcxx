#pragma once
#include "attribute_common_impl.hpp"

namespace active_record::attributes {
    template<typename Table, typename Attribute, typename ForeignKey>
    struct foreign_key : public attribute<Table, Attribute, typename ForeignKey::value_type> {
        using attribute<Table, Attribute, typename ForeignKey::value_type>::attribute;
        using foreign_key_type = ForeignKey;
    };
}