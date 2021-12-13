#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT Lisence.
 */
#include "attribute_common.hpp"

namespace active_record {
    template<typename Model, typename Attribute>
    struct attribute<Model, Attribute, bool> : public attribute_common<Model, Attribute, bool> {
        using attribute_common<Model, Attribute, bool>::attribute_common;
    };

    namespace attributes {
        template<typename Model, typename Attribute>
        struct boolean : public attribute<Model, Attribute, bool>{
            using attribute<Model, Attribute, bool>::attribute;
        };
    }
}