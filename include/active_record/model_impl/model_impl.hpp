#pragma once
#include "../model.hpp"

namespace active_record {
    template<typename Derived>
    inline constexpr active_record::string model<Derived>::insert_column_names_to_string() {
        active_record::string table = active_record::string{ "\"" } + active_record::string{ Derived::table_name } + "\"(";
        active_record::string delimiter = "";
        const auto column_names = Derived::column_names();
        for (const auto& col_name : column_names) {
            table += delimiter + "\"" + active_record::string{ col_name } + "\"";
            delimiter = ",";
        }
        table += ")";
        return table;
    }

    template<typename Derived>
    inline constexpr auto model<Derived>::column_names() noexcept {
        return std::apply(
            []<Attribute... Attrs>([[maybe_unused]]Attrs...){ return std::array<const active_record::string_view, sizeof...(Attrs)>{(Attrs::column_name)...}; },
            Derived{}.attributes
        );
    }
}