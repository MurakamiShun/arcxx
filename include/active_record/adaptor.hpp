#pragma once
#include "utils.hpp"

namespace active_record{
    class adaptor {
    private:
    public:
        template<typename T>
        active_record::string attribute_typename();
    };
}