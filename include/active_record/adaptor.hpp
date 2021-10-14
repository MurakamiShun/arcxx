#pragma once
/*
 * Copyright 2021 akisute514
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "utils.hpp"

namespace active_record{
    struct adaptor {
        //static constexpr bool bindable = false;
        //static active_record::string bind_variable_str(const std::size_t idx);
    };

    struct common_adaptor : public adaptor {
        static constexpr bool bindable = false;
        static active_record::string bind_variable_str(const std::size_t idx) {
            return active_record::string{ "'common_adaptor can not bind parameters. Library has some problems.'" } + std::to_string(idx);
        }
    };

    enum class transaction{
        rollback,
        commit
    };
}