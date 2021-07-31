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
    template<Attribute Attr>
    inline query_relation<bool, std::tuple<const Attr*>> model<Derived>::destroy(const Attr&& attr){
        return destory(attr.to_equ_condition());
    }

    template<typename Derived>
    template<Tuple SrcBindAttrs>
    inline query_relation<bool, SrcBindAttrs> model<Derived>::destroy(query_condition<SrcBindAttrs>&& cond) {
        query_relation<bool, SrcBindAttrs> ret;

        ret.operation = query_operation::destroy;
        ret.query_table.push_back(active_record::string{ "\"" } + active_record::string{ Derived::table_name } + "\"");
        ret.query_condition = std::move(cond.condition);
        ret.temporary_attrs = std::move(cond.temporary_attrs);
        detail::set_bind_attrs_ptr<0>(ret.bind_attrs, ret.temporary_attrs);
        return ret;
    }

    template<typename Derived>
    template<Attribute Attr>
    inline query_relation<std::vector<Derived>, std::tuple<const Attr*>> model<Derived>::where(const Attr&& attr) {
        return where(attr.to_equ_condition());
    }

    template<typename Derived>
    template<Tuple SrcBindAttrs>
    inline query_relation<std::vector<Derived>, SrcBindAttrs> model<Derived>::where(query_condition<SrcBindAttrs>&& cond) {
        query_relation<std::vector<Derived>, SrcBindAttrs> ret;
        
        ret.operation = query_operation::condition;
        ret.query_op_arg.push_back(detail::model_column_full_names_to_string<Derived>());
        ret.query_table.push_back(active_record::string{ "\"" } + active_record::string{ Derived::table_name } + "\"");
        ret.query_condition = std::move(cond.condition);
        ret.temporary_attrs = std::move(cond.temporary_attrs);
        detail::set_bind_attrs_ptr<0>(ret.bind_attrs, ret.temporary_attrs);

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

    namespace detail {
        template<Model ReferModel>
        struct get_reference_attr{
            template<Attribute Attr>
            requires std::same_as<typename Attr::foreign_key_type::model_type, ReferModel>
            auto operator()([[maybe_unused]]std::tuple<Attr&>) {
                return Attr{};
            }
            template<Attribute Attr>
            auto operator()([[maybe_unused]]std::tuple<Attr&>) {
                return std::false_type{};
            }
            template<Attribute Head, Attribute... Tail>
            auto operator()([[maybe_unused]]std::tuple<Head&, Tail&...>) {
                using head_type = std::invoke_result_t<get_reference_attr<ReferModel>, std::tuple<Head&>>;
                if constexpr(std::is_same_v<head_type, std::false_type>) {
                    using tail_type = std::invoke_result_t<get_reference_attr<ReferModel>, std::tuple<Tail&...>>;
                    return tail_type{};
                }
                else {
                    return head_type{};
                }
            }
        };
    }

    template<typename Derived>
    template<typename ReferModel>
    requires std::derived_from<ReferModel, model<ReferModel>>
    inline query_relation<std::vector<Derived>, std::tuple<>> model<Derived>::join() {
        query_relation<std::vector<Derived>, std::tuple<>> ret;

        using ReferenceAttribute = std::invoke_result_t<detail::get_reference_attr<ReferModel>, decltype(Derived{}.attributes)>;

        static_assert(!std::is_same_v<ReferenceAttribute, std::false_type>, "Derived model does not have reference to given model");
        
        ret.operation = query_operation::select;
        ret.query_op_arg.push_back(detail::model_column_full_names_to_string<Derived>());
        ret.query_table.push_back(
            active_record::string{ "\"" } + active_record::string{ Derived::table_name }
            + "\" INNER JOIN \"" + active_record::string{ ReferenceAttribute::foreign_key_type::model_type::table_name }
            + "\" ON " + detail::column_full_names_to_string<typename ReferenceAttribute::foreign_key_type>()
            + " = " + detail::column_full_names_to_string<ReferenceAttribute>()
        );
        
        return ret;  
    }

    template<typename Derived>
    template<typename ReferModel>
    requires std::derived_from<ReferModel, model<ReferModel>>
    inline query_relation<std::vector<Derived>, std::tuple<>> model<Derived>::left_join() {
        query_relation<std::vector<Derived>, std::tuple<>> ret;

        using ReferenceAttribute = std::invoke_result_t<detail::get_reference_attr<ReferModel>, decltype(Derived{}.attributes)>;

        static_assert(!std::is_same_v<ReferenceAttribute, std::false_type>, "Derived model does not have reference to given model");
        
        ret.operation = query_operation::select;
        ret.query_op_arg.push_back(detail::model_column_full_names_to_string<Derived>());
        ret.query_table.push_back(
            active_record::string{ "\"" } + active_record::string{ Derived::table_name }
            + "\" LEFT OUTER JOIN \"" + active_record::string{ ReferenceAttribute::foreign_key_type::model_type::table_name }
            + "\" ON " + detail::column_full_names_to_string<typename ReferenceAttribute::foreign_key_type>()
            + " = " + detail::column_full_names_to_string<ReferenceAttribute>()
        );
        
        return ret;  
    }

    template<typename Derived>
    inline query_relation<std::size_t, std::tuple<>> model<Derived>::count() {
        query_relation<std::size_t, std::tuple<>> ret;

        ret.operation = query_operation::select;
        ret.query_op_arg.push_back("count(*)");
        ret.query_table.push_back(
            active_record::string{ "\"" } + active_record::string{ Derived::table_name } + "\""
        );
        
        return ret;  
    }

    template<typename Derived>
    template<Attribute Attr>
    requires std::integral<typename Attr::value_type> || std::floating_point<typename Attr::value_type>
    inline query_relation<typename Attr::value_type, std::tuple<>> model<Derived>::sum(){
        query_relation<typename Attr::value_type, std::tuple<>> ret;

        ret.operation = query_operation::select;
        ret.query_op_arg.push_back("sum(" + detail::column_full_names_to_string<Attr>() +")");
        ret.query_table.push_back(
            active_record::string{ "\"" } + active_record::string{ Derived::table_name } + "\""
        );
        
        return ret;  
    }

    template<typename Derived>
    template<Attribute Attr>
    requires std::integral<typename Attr::value_type> || std::floating_point<typename Attr::value_type>
    inline query_relation<typename Attr::value_type, std::tuple<>> model<Derived>::avg(){
        query_relation<typename Attr::value_type, std::tuple<>> ret;

        ret.operation = query_operation::select;
        ret.query_op_arg.push_back("avg(" + detail::column_full_names_to_string<Attr>() +")");
        ret.query_table.push_back(
            active_record::string{ "\"" } + active_record::string{ Derived::table_name } + "\""
        );
        
        return ret;  
    }

    template<typename Derived>
    template<Attribute Attr>
    requires std::integral<typename Attr::value_type> || std::floating_point<typename Attr::value_type>
    inline query_relation<typename Attr::value_type, std::tuple<>> model<Derived>::max(){
        query_relation<typename Attr::value_type, std::tuple<>> ret;

        ret.operation = query_operation::select;
        ret.query_op_arg.push_back("max(" + detail::column_full_names_to_string<Attr>() +")");
        ret.query_table.push_back(
            active_record::string{ "\"" } + active_record::string{ Derived::table_name } + "\""
        );
        
        return ret;  
    }

    template<typename Derived>
    template<Attribute Attr>
    requires std::integral<typename Attr::value_type> || std::floating_point<typename Attr::value_type>
    inline query_relation<typename Attr::value_type, std::tuple<>> model<Derived>::min(){
        query_relation<typename Attr::value_type, std::tuple<>> ret;

        ret.operation = query_operation::select;
        ret.query_op_arg.push_back("min(" + detail::column_full_names_to_string<Attr>() +")");
        ret.query_table.push_back(
            active_record::string{ "\"" } + active_record::string{ Derived::table_name } + "\""
        );
        
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

        return active_record::string{"CREATE TABLE "} + (create_if_not_exist ? "IF NOT EXISTS " : "")
            + active_record::string{ Derived::table_name }
            + "(" + col_defs + ");";
    }
}