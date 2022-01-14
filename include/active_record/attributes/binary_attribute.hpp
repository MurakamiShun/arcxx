#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
#include "attribute_common.hpp"

namespace active_record {
    template<typename Model, typename Attribute>
    struct attribute<Model, Attribute, std::vector<std::byte>> : public attribute_common<Model, Attribute, std::vector<std::byte>> {
        using attribute_common<Model, Attribute, std::vector<std::byte>>::attribute_common;
    };

    namespace attributes {
        template<typename Model, typename Attribute>
        struct binary : public attribute<Model, Attribute, std::vector<std::byte>>{
            using attribute<Model, Attribute, std::vector<std::byte>>::attribute;
        };
    }
}