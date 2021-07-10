#pragma once
#include "query_impl.hpp"

namespace active_record {
     namespace {

        template<Attribute Last>
        constexpr active_record::string column_full_names_to_string([[maybe_unused]]const Last&){
            constexpr auto full_name = Last::column_full_name();
            return active_record::string{ "\"" } + active_record::string{ full_name.first } + "\".\"" + active_record::string{ full_name.second } + "\"";
        }
        template<Attribute Head, Attribute... Tail>
        constexpr active_record::string column_full_names_to_string([[maybe_unused]] const Head&, [[maybe_unused]] const Tail&...){
            return column_full_names_to_string(std::declval<Head>()) + "," + column_full_names_to_string(std::declval<Tail>()...);
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

        template<Attribute Attr>
        constexpr active_record::string attributes_to_condition_string(const Attr& attr){
            return column_full_names_to_string(attr) + " = " + attr.to_string();
        }
        
        template<std::same_as<query_condition> Condition>
        constexpr active_record::string attributes_to_condition_string(const Condition& cond){
            return cond.str;
        }
        template<Attribute Head, Attribute... Tail>
        constexpr active_record::string attributes_to_condition_string(const Head& head, const Tail&... tail){
            return attributes_to_condition_string(head) + " AND " + attributes_to_condition_string(tail...);
        }
    }

    template<typename Derived>
    inline query_relation<std::vector<Derived>, std::tuple<void_attribute*>> model<Derived>::all() {
        query_relation<std::vector<Derived>, std::tuple<void_attribute*>> ret;
        
        ret.operation = query_operation::select;
        ret.query_op_arg.push_back(model_column_full_names_to_string<Derived>());
        ret.query_table.push_back(active_record::string{ "\"" } + active_record::string{ Derived::table_name } + "\"");

        return ret;    
    }

    template<typename Derived>
    template<Attribute... Attrs>
    inline query_relation<std::vector<std::tuple<Attrs...>>, std::tuple<void_attribute*>> model<Derived>::select(const Attrs... attrs) {
        query_relation<std::vector<std::tuple<Attrs...>>, std::tuple<void_attribute*>> ret;
        
        ret.operation = query_operation::select;
        ret.query_op_arg.push_back(column_full_names_to_string(attrs...));
        ret.query_table.push_back(active_record::string{ "\"" } + active_record::string{ Derived::table_name } + "\"");
        return ret;
    }

    template<typename Derived>
    template<Attribute Attr>
    inline query_relation<std::vector<Attr>, std::tuple<void_attribute*>> model<Derived>::pluck(const Attr attr) {
        query_relation<std::vector<Attr>, std::tuple<void_attribute*>> ret;
        
        ret.operation = query_operation::select;
        ret.query_op_arg.push_back(column_full_names_to_string(attr));
        ret.query_table.push_back(active_record::string{ "\"" } + active_record::string{ Derived::table_name } + "\"");
        return ret;
    }

    template<typename Derived>
    template<Attribute... Attrs>
    inline query_relation<std::vector<Derived>, std::tuple<Attrs*...>> model<Derived>::where(const Attrs... attrs) {
        query_relation<std::vector<Derived>, std::tuple<Attrs*...>> ret;
        
        ret.operation = query_operation::condition;
        ret.query_op_arg.push_back(model_column_full_names_to_string<Derived>());
        ret.query_table.push_back(active_record::string{ "\"" } + active_record::string{ Derived::table_name } + "\""),
        ret.query_condition.push_back(attributes_to_condition_string(attrs...));
        
        return ret;
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

        return active_record::string{"CREATE TABLE"} + (create_if_not_exist ? "IF NOT EXISTS " : "")
            + active_record::string{ Derived::table_name }
            + "(" + col_defs + ");";
    }
}