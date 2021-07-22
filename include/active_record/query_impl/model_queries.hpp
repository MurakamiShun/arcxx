#pragma once
#include "query_relation_impl.hpp"

namespace active_record {
    template<typename Derived>
    inline query_relation<std::vector<Derived>, std::tuple<>> model<Derived>::all() {
        query_relation<std::vector<Derived>, std::tuple<>> ret;
        
        ret.operation = query_operation::select;
        ret.query_op_arg.push_back(detail::model_column_full_names_to_string<Derived>());
        ret.query_table.push_back(active_record::string{ "\"" } + active_record::string{ Derived::table_name } + "\"");

        return ret;    
    }

    template<typename Derived>
    template<Attribute... Attrs>
    inline query_relation<std::vector<std::tuple<Attrs...>>, std::tuple<>> model<Derived>::select() {
        query_relation<std::vector<std::tuple<Attrs...>>, std::tuple<>> ret;
        
        ret.operation = query_operation::select;
        ret.query_op_arg.push_back(detail::column_full_names_to_string<Attrs...>());
        ret.query_table.push_back(active_record::string{ "\"" } + active_record::string{ Derived::table_name } + "\"");
        return ret;
    }
    template<typename Derived>
    template<Attribute... Attrs>
    inline query_relation<std::vector<std::tuple<Attrs...>>, std::tuple<>> model<Derived>::select([[maybe_unused]]const Attrs... attrs) {
        return model<Derived>::select<Attrs...>();
    }

    template<typename Derived>
    template<Attribute Attr>
    inline query_relation<std::vector<Attr>, std::tuple<>> model<Derived>::pluck() {
        query_relation<std::vector<Attr>, std::tuple<>> ret;
        
        ret.operation = query_operation::select;
        ret.query_op_arg.push_back(detail::column_full_names_to_string<Attr>());
        ret.query_table.push_back(active_record::string{ "\"" } + active_record::string{ Derived::table_name } + "\"");
        return ret;
    }
    template<typename Derived>
    template<Attribute Attr>
    inline query_relation<std::vector<Attr>, std::tuple<>> model<Derived>::pluck([[maybe_unused]]const Attr attr) {
        return model<Derived>::pluck<Attr>();
    }

    template<typename Derived>
    template<Attribute... Attrs>
    inline query_relation<std::vector<Derived>, std::tuple<const Attrs*...>> model<Derived>::where(const Attrs... attrs) {
        query_relation<std::vector<Derived>, std::tuple<const Attrs*...>> ret;
        
        ret.operation = query_operation::condition;
        ret.query_op_arg.push_back(detail::model_column_full_names_to_string<Derived>());
        ret.query_table.push_back(active_record::string{ "\"" } + active_record::string{ Derived::table_name } + "\"");
        detail::attributes_to_condition_string<0>(ret, attrs...);
        return ret;
    }

    template<typename Derived>
    inline query_relation<std::vector<Derived>, std::tuple<>> model<Derived>::limit(const std::size_t lim) {
        query_relation<std::vector<Derived>, std::tuple<>> ret;
        
        ret.operation = query_operation::select;
        ret.query_op_arg.push_back(detail::model_column_full_names_to_string<Derived>());
        ret.query_table.push_back(active_record::string{ "\"" } + active_record::string{ Derived::table_name } + "\"");
        ret.query_options.push_back(active_record::string{ "LIMIT " } + std::to_string(lim));

        return ret;
    }

    template<typename Derived>
    template<Attribute Attr>
    inline query_relation<std::vector<Derived>, std::tuple<>> model<Derived>::order_by(const active_record::order order) {
        query_relation<std::vector<Derived>, std::tuple<>> ret;
        
        ret.operation = query_operation::select;
        ret.query_op_arg.push_back(detail::model_column_full_names_to_string<Derived>());
        ret.query_table.push_back(active_record::string{ "\"" } + active_record::string{ Derived::table_name } + "\"");

        
        ret.query_options.push_back(
            active_record::string{ "ORDER BY " }
            + detail::column_full_names_to_string<Attr>()
            + (order == active_record::order::asc ? " ASC" : " DESC" )
        );

        return ret;   
    }

    template<typename Derived>
    template<typename Relation>
    inline query_relation<std::vector<Derived>, std::tuple<>> model<Derived>::join() {
        query_relation<std::vector<Derived>, std::tuple<>> ret;
        
        ret.operation = query_operation::select;
        ret.query_op_arg.push_back(detail::model_column_full_names_to_string<Derived>());
        ret.query_table.push_back(
            active_record::string{ "\"" } + active_record::string{ Derived::table_name }
            + "\" JOIN \"" + active_record::string{ Relation::foreign_key_type::model_type::table_name }
            + "\" ON " + detail::column_full_names_to_string<typename Relation::foreign_key_type>()
            + " = " + detail::column_full_names_to_string<Relation>()
        );
        
        return ret;  
    }
    template<typename Derived>
    template<typename Relation>
    inline query_relation<std::vector<Derived>, std::tuple<>> model<Derived>::join([[maybe_unused]]const Relation) {
        return model<Derived>::join<Relation>();
    }

    template<typename Derived>
    template<std::derived_from<adaptor> Adaptor>
    inline active_record::string model<Derived>::schema::to_sql(bool create_if_not_exist) {
        const auto column_definitions = std::apply(
            []<typename... Attrs>(const Attrs&...){ return std::array<const active_record::string, sizeof...(Attrs)>{(Adaptor::template column_definition<Attrs>())...}; },
            Derived{}.attributes
        );
        active_record::string col_defs = "";
        active_record::string delimiter = "";
        for(const auto& col_def : column_definitions){
            col_defs += delimiter + col_def;
            delimiter = ",";
        }

        return active_record::string{"CREATE TABLE "} + (create_if_not_exist ? "IF NOT EXISTS " : "")
            + active_record::string{ Derived::table_name }
            + "(" + col_defs + ");";
    }
}