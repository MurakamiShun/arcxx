#pragma once
#include "attribute.hpp"
#include "query.hpp"

namespace active_record {
    namespace relation {
        template<typename ForeignKey, typename Table, typename Attribute>
        struct reference_to : public attribute<Table, Attribute, typename ForeignKey::value_type> {
        private:
            std::optional<typename ForeignKey::model_type> data;
        public:
            using attribute<Table, Attribute, typename ForeignKey::value_type>::attribute;
            using foreign_key_type = ForeignKey;
            const typename ForeignKey::model_type* operator->() const {
                return data.operator->();
            }
            typename ForeignKey::model_type* operator->() {
                return data.operator->();
            }
        };
    }
}