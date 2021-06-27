#pragma once
#include "utils.hpp"

namespace active_record{
    struct adaptor {
        template<typename T>
        active_record::string attribute_typename();
    };
    struct common_adaptor : public adaptor{};
}