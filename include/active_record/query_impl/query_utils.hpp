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
        auto column_names = Mod::column_names();
        active_record::string columns = "";
        active_record::string delimiter = "";
        for (auto& col_name : column_names) {
            columns += delimiter + "\"" + active_record::string{ Mod::table_name } + "\".\"" + active_record::string{ col_name } + "\"";
            delimiter = ",";
        }
        return columns;
    }

    template<std::size_t I, typename BindAttr>
    void set_bind_attrs_ptr(BindAttr& bind_attrs, const std::vector<std::any>& temp_attr){
        if constexpr(std::tuple_size_v<BindAttr> != 0) std::get<I>(bind_attrs) = std::any_cast<std::remove_pointer_t<std::tuple_element_t<I, BindAttr>>>(&temp_attr[I]);
        if constexpr(I + 1 < std::tuple_size_v<BindAttr>) set_bind_attrs_ptr<I + 1>(bind_attrs, temp_attr);
    }
}