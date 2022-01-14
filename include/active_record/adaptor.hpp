#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
#include "utils.hpp"

namespace active_record{
    struct adaptor {
        //static constexpr bool bindable = false;
        //static active_record::string bind_variable_str(const std::size_t idx);
    };

    enum class transaction{
        rollback,
        commit
    };

    struct common_adaptor;
}