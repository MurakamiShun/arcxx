#pragma once

namespace active_record {
    namespace detail {
        template<typename Model>
        struct relation {

        };
    }
    namespace relation {
        template<typename Model, typename Relation, typename KeyType = int32_t>
        struct belongs_to : public attribute<Model, Relation, KeyType>{

        };
    }
}