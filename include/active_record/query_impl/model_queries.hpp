#pragma once
#include "query_relation.hpp"

namespace active_record {
     namespace detail {
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
    }

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

    namespace detail {
        template<std::size_t I, typename Query, Attribute Attr>
        void attributes_to_condition_string(Query& query, const Attr& attr){
            query.temporary_attrs.push_back(attr);
            std::get<I>(query.bind_attrs) = std::any_cast<Attr>(&query.temporary_attrs.back());
            query.query_condition.push_back(detail::column_full_names_to_string<Attr>() + " = ");
            query.query_condition.push_back(I);
        }
        template<std::size_t I, typename Query, Attribute Head, Attribute... Tail>
        void attributes_to_condition_string(Query& query, const Head& head, const Tail&... tail){
            attributes_to_condition_string<I>(query, head);
            query.query_condition.push_back(" AND ");
            attributes_to_condition_string<I + 1>(query, tail...);
        }
    }

    template<typename Derived>
    template<Attribute... Attrs>
    inline query_relation<std::vector<Derived>, std::tuple<const Attrs*...>> model<Derived>::where(const Attrs... attrs) {
        query_relation<std::vector<Derived>, std::tuple<const Attrs*...>> ret;
        
        ret.operation = query_operation::condition;
        ret.query_op_arg.push_back(detail::model_column_full_names_to_string<Derived>());
        ret.query_table.push_back(active_record::string{ "\"" } + active_record::string{ Derived::table_name } + "\""),
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