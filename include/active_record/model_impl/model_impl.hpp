#pragma once
#include "../model.hpp"

namespace active_record {
    template<typename Derived>
    inline constexpr active_record::string model<Derived>::insert_column_names_to_string() {
        active_record::string table;
        const auto column_names = Derived::column_names();
        table.reserve([&column_names]() constexpr {
            return std::transform_reduce(
                column_names.begin(), column_names.end(), static_cast<std::size_t>(0),
                [](auto acc, const auto len){ return acc += len; },
                [](const auto& str){ return str.length(); }
            ) + column_names.size() * 3 + 4 + static_cast<active_record::string_view>(Derived::table_name).length();
        }());
        table += concat_strings("\"", Derived::table_name, "\"(");
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

    template<typename Derived>
    inline constexpr auto model<Derived>::column_names() noexcept {
        return std::apply(
            []<Attribute... Attrs>([[maybe_unused]]Attrs...) constexpr { return std::array<const active_record::string_view, sizeof...(Attrs)>{(Attrs::column_name)...}; },
            Derived{}.get_attributes_tuple()
        );
    }
}