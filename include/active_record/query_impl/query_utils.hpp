#pragma once
#include "../query.hpp"
#include "../model.hpp"

namespace active_record::detail {
    template<Attribute Last>
    constexpr active_record::string column_full_names_to_string(){
        constexpr auto full_name = Last::column_full_name();
        return active_record::string{ "\"" } + active_record::string{ full_name.first } + "\".\"" + active_record::string{ full_name.second } + "\"";
    }
    template<Attribute Head, Attribute... Tail>
    requires (sizeof...(Tail) > 0)
    constexpr active_record::string column_full_names_to_string(){
        return column_full_names_to_string<Head>() + "," + column_full_names_to_string<Tail...>();
    }

    template<Model Mod>
    constexpr active_record::string model_column_full_names_to_string(){
        auto column_names = Mod::column_names();
        active_record::string columns = "";
        active_record::string delimiter = "";
        for (auto& col_name : column_names) {
            columns += delimiter + "\"" + active_record::string{ Mod::table_name } + "\".\"" + active_record::string{ col_name } + "\"";
            delimiter = ",";
        }
        return columns;
    }
    
    template<std::size_t I, typename Query, Attribute Attr>
    void attributes_to_condition_string(Query& query, const Attr& attr){
        if (!query.query_condition.empty()) query.query_condition.push_back(" AND ");
        query.temporary_attrs.push_back(attr);
        std::get<I>(query.bind_attrs) = std::any_cast<Attr>(&query.temporary_attrs.back());
        query.query_condition.push_back(detail::column_full_names_to_string<Attr>() + " = ");
        query.query_condition.push_back(I);
    }
    template<std::size_t I, typename Query, Attribute Head, Attribute... Tail>
    void attributes_to_condition_string(Query& query, const Head& head, const Tail&... tail){
        attributes_to_condition_string<I>(query, head);
        attributes_to_condition_string<I + 1>(query, tail...);
    }

    template<std::size_t I, typename BindAttr>
    void set_bind_attrs_ptr(BindAttr& bind_attrs, const std::vector<std::any>& temp_attr){
        if constexpr(std::tuple_size_v<BindAttr> != 0) std::get<I>(bind_attrs) = std::any_cast<std::tuple_element<I, BindAttr>>(&temp_attr[I]);
        if constexpr(I + 1 < std::tuple_size_v<BindAttr>) set_bind_attrs_ptr<I + 1>(bind_attrs, temp_attr);
    }
}