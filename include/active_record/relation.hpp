#pragma once
#include "attribute.hpp"
#include "query.hpp"

namespace active_record {
    namespace relation {
        template<typename PrimaryKey>
        struct reference_to : public attribute<typename PrimaryKey::model_type, typename PrimaryKey::attribute_type, typename PrimaryKey::value_type> {
        private:
            std::optional<typename PrimaryKey::model_type> data;
        public:
            using attribute<typename PrimaryKey::model_type, typename PrimaryKey::attribute_type, typename PrimaryKey::value_type>::attribute;
            using primary_key_type = PrimaryKey;
            operator bool() const {
                return static_cast<bool>(data);
            }
            query_relation<typename PrimaryKey::model_type> load() {

            }
        };
    }
}