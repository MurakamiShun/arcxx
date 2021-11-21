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
#include "../model.hpp"

namespace active_record {
    template<typename Derived>
    inline active_record::string model<Derived>::insert_column_names_to_string() {
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
            Derived{}.attributes_as_tuple()
        );
    }

    template<typename Derived>
    inline auto model<Derived>::attributes_as_tuple() noexcept {
        using namespace tuptup::type_placeholders;
        auto attributes_tuple = tuptup::struct_binder<Derived>{}(*reinterpret_cast<Derived*>(this));
        return tuptup::tuple_filter<is_attribute<defer<std::remove_reference<_1>>>>(attributes_tuple);
    }

    template<typename Derived>
    inline auto model<Derived>::attributes_as_tuple() const noexcept {
        using namespace tuptup::type_placeholders;
        const auto attributes_tuple = tuptup::struct_binder<Derived>{}(*reinterpret_cast<const Derived*>(this));
        return tuptup::tuple_filter<is_attribute<defer<std::remove_reference<_1>>>>(attributes_tuple);
    }
}