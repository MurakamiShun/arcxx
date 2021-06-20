#pragma once
#include "attribute.hpp"
#include "query.hpp"

namespace active_record {
    namespace relation {
        template<typename PrimaryKey, typename Table, typename Attribute>
        struct reference_to : public attribute<Table, Attribute, typename PrimaryKey::value_type> {
        private:
            std::optional<typename PrimaryKey::model_type> data;
        public:
            using attribute<Table, Attribute, typename PrimaryKey::value_type>::attribute;
            using primary_key_type = PrimaryKey;
            operator bool() const {
                return static_cast<bool>(data);
            }
            query_relation<typename PrimaryKey::model_type> load() {

            }
        };
    }
}