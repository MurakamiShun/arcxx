#pragma once
#include "../attribute.hpp"
#include "../relation.hpp"

namespace active_record::relation {
    template<typename Table, typename Attribute, typename ForeignKey>
    struct reference_to : public attribute<Table, Attribute, typename ForeignKey::value_type> {
        using attribute<Table, Attribute, typename ForeignKey::value_type>::attribute;
        using foreign_key_type = ForeignKey;

        reference_to(const ForeignKey::model_type& model) {
            const auto& foreign_key = std::get<ForeignKey&>(model.attributes);
            if(foreign_key) *this = foreign_key.value();
            else *this = std::nullopt;
        }
    };
}