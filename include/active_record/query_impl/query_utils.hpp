#pragma once
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
    [[nodiscard]] constexpr active_record::string model_column_full_names_to_string(){
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