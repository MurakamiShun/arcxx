#pragma once
#include "../query.hpp"
#include "../model.hpp"

namespace active_record::detail {
    template<typename Last>
    [[nodiscard]] constexpr active_record::string column_full_names_to_string(){
        return Last::column_full_name();
    }
    template<typename Head, typename... Tail>
    requires (sizeof...(Tail) > 0)
    [[nodiscard]] constexpr active_record::string column_full_names_to_string(){
        return column_full_names_to_string<Head>() + "," + column_full_names_to_string<Tail...>();
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

    template<std::size_t I, typename BindAttr>
    void set_bind_attrs_ptr(BindAttr& bind_attrs, const std::vector<std::any>& temp_attr){
        if constexpr(std::tuple_size_v<BindAttr> != 0) std::get<I>(bind_attrs) = std::any_cast<std::remove_pointer_t<std::tuple_element_t<I, BindAttr>>>(&temp_attr[I]);
        if constexpr(I + 1 < std::tuple_size_v<BindAttr>) set_bind_attrs_ptr<I + 1>(bind_attrs, temp_attr);
    }
}