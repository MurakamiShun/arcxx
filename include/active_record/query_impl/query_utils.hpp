#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
#include "../model.hpp"
namespace active_record::detail {
    template<is_model Mod>
    inline active_record::string insert_column_names_to_string() {
        active_record::string table;
        const auto column_names = Mod::column_names();
        table.reserve([&column_names]() constexpr {
            return std::transform_reduce(
                column_names.begin(), column_names.end(), static_cast<std::size_t>(0),
                [](auto acc, const auto len){ return acc += len; },
                [](const auto& str){ return str.length(); }
            ) + column_names.size() * 3 + 4 + static_cast<active_record::string_view>(Mod::table_name).length();
        }());
        table += concat_strings("\"", Mod::table_name, "\"(");
        active_record::string_view delimiter = "";
        for (const auto& col_name : column_names) {
            table += delimiter;
            table += "\"";
            table += col_name;
            table += "\"";
            delimiter = ",";
        }
        table += ")";
        return table;
    }

    template<typename Last>
    [[nodiscard]] inline constexpr auto column_full_names_to_string(){
        return Last::column_full_name();
    }
    template<typename Head, typename... Tail>
    requires (sizeof...(Tail) > 0)
    [[nodiscard]] inline constexpr auto column_full_names_to_string(){
        return concat_strings(column_full_names_to_string<Head>().c_str(), ",", column_full_names_to_string<Tail...>().c_str());
    }

    template<is_model Mod>
    [[nodiscard]] inline active_record::string model_column_full_names_to_string(){
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