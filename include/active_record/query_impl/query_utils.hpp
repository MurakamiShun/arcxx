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
#include "../query.hpp"
#include "../model.hpp"

namespace active_record::detail {
    template<typename Last>
    [[nodiscard]] constexpr auto column_full_names_to_string(){
        return Last::column_full_name();
    }
    template<typename Head, typename... Tail>
    requires (sizeof...(Tail) > 0)
    [[nodiscard]] constexpr auto column_full_names_to_string(){
        return concat_strings(column_full_names_to_string<Head>().c_str(), ",", column_full_names_to_string<Tail...>().c_str());
    }

    template<Model Mod>
    [[nodiscard]] active_record::string model_column_full_names_to_string(){
        const auto column_names = Mod::column_names();
        active_record::string buff;
        const std::size_t buff_size = std::transform_reduce(
            column_names.begin(), column_names.end(), static_cast<std::size_t>(0),
            [](auto acc, const auto len) constexpr { return acc += len; },
            [](const auto& str) constexpr { return str.length() + static_cast<active_record::string_view>(Mod::table_name).length() + 6; }
        );
        buff.reserve(buff_size);
        active_record::string_view delimiter = "";
        for (const auto& col_name : column_names) {
            buff += delimiter;
            buff += "\"";
            buff += Mod::table_name;
            buff += "\".\"";
            buff += col_name;
            buff += "\"";
            delimiter = ",";
        }
        return buff;
    }
}