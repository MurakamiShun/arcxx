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
            std::get<I>(bind_attrs) = std::any_cast<std::tuple_element<I, BindAttr>>(&temp_attr[I]);
            if constexpr(I + 1 < std::tuple_size_v<BindAttr>) set_bind_attrs_ptr<I + 1>(bind_attrs, temp_attr);
        }
    }

    /*
     * return type == Container<model or Tuple>
     */

    template<Container Result, Tuple BindAttrs>
    template<Attribute... Attrs>
    query_relation<std::vector<std::tuple<Attrs...>>, BindAttrs> query_relation<Result, BindAttrs>::select() const & {
        query_relation<std::vector<std::tuple<Attrs...>>, BindAttrs> ret;
        
        ret.operation = query_operation::select;
        ret.query_op_arg.push_back(detail::column_full_names_to_string<Attrs...>());
        ret.query_table = this->query_table;

        ret.query_condition = this->query_condition;
        ret.query_options = this->query_options;
        ret.temporary_attrs = this->temporary_attrs;
        detail::set_bind_attrs_ptr<0>(ret.bind_attrs, ret.temporary_attrs);
        return ret;
    }
    template<Container Result, Tuple BindAttrs>
    template<Attribute... Attrs>
    query_relation<std::vector<std::tuple<Attrs...>>, BindAttrs> query_relation<Result, BindAttrs>::select() && {
        query_relation<std::vector<std::tuple<Attrs...>>, BindAttrs> ret;

        ret.operation = query_operation::select;
        ret.query_op_arg.push_back(detail::column_full_names_to_string<Attrs...>());
        ret.query_table = this->query_table;

        ret.query_condition = std::move(this->query_condition);
        ret.query_options = std::move(this->query_options);
        ret.temporary_attrs = std::move(this->temporary_attrs);
        detail::set_bind_attrs_ptr<0>(ret.bind_attrs, ret.temporary_attrs);
        return ret;
    }

    template<Container Result, Tuple BindAttrs>
    template<Attribute Attr>
    query_relation<std::vector<Attr>, BindAttrs> query_relation<Result, BindAttrs>::pluck() const & {
        query_relation<std::vector<Attr>, BindAttrs> ret;
        
        ret.operation = query_operation::select;
        ret.query_op_arg.push_back(detail::column_full_names_to_string<Attr>());
        ret.query_table.push_back(active_record::string{ "\"" } + active_record::string{ Result::value_type::table_name } + "\"");

        ret.query_condition = this->query_condition;
        ret.query_options = this->query_options;
        ret.temporary_attrs = this->temporary_attrs;
        detail::set_bind_attrs_ptr<0>(ret.bind_attrs, ret.temporary_attrs);
        return ret;
    }
    template<Container Result, Tuple BindAttrs>
    template<Attribute Attr>
    query_relation<std::vector<Attr>, BindAttrs> query_relation<Result, BindAttrs>::pluck() && {
        query_relation<std::vector<Attr>, BindAttrs> ret;
        
        ret.operation = query_operation::select;
        ret.query_op_arg.push_back(detail::column_full_names_to_string<Attr>());
        ret.query_table.push_back(active_record::string{ "\"" } + active_record::string{ Result::value_type::table_name } + "\"");

        ret.query_condition = std::move(this->query_condition);
        ret.query_options = std::move(this->query_options);
        ret.temporary_attrs = std::move(this->temporary_attrs);
        detail::set_bind_attrs_ptr<0>(ret.bind_attrs, ret.temporary_attrs);
        return ret;
    }


    template<Container Result, Tuple BindAttrs>
    template<Attribute... Attrs>
    query_relation<Result, std::invoke_result_t<decltype(std::tuple_cat<BindAttrs, std::tuple<const Attrs*...>>)>> query_relation<Result, BindAttrs>::where(const Attrs... attrs) && {
        query_relation<Result, std::invoke_result_t<decltype(std::tuple_cat<BindAttrs, std::tuple<const Attrs*...>>)>> ret;
        
        ret.operation = query_operation::condition;
        ret.query_op_arg = std::move(this->query_op_arg);
        ret.query_table = std::move(this->query_table);
        ret.query_condition = std::move(this->query_condition);
        ret.query_options = std::move(this->query_options);
        ret.temporary_attrs = std::move(this->temporary_attrs);
        detail::set_bind_attrs_ptr<0>(ret.bind_attrs, ret.temporary_attrs);
        detail::attributes_to_condition_string<std::tuple_size_v<BindAttrs>>(ret, attrs...);
        return ret;
    }
    template<Container Result, Tuple BindAttrs>
    template<Attribute... Attrs>
    query_relation<Result, std::invoke_result_t<decltype(std::tuple_cat<BindAttrs, std::tuple<const Attrs*...>>)>> query_relation<Result, BindAttrs>::where(const Attrs... attrs) const& {
        query_relation<Result, std::invoke_result_t<decltype(std::tuple_cat<BindAttrs, std::tuple<const Attrs*...>>)>> ret;
        
        ret.operation = query_operation::condition;
        ret.query_op_arg = this->query_op_arg;
        ret.query_table = this->query_table;
        ret.query_condition = this->query_condition;
        ret.query_options = this->query_options;
        ret.temporary_attrs = this->temporary_attrs;
        detail::set_bind_attrs_ptr<0>(ret.bind_attrs, ret.temporary_attrs);
        detail::attributes_to_condition_string<std::tuple_size_v<BindAttrs>>(ret, attrs...);
        return ret;
    }

    template<Container Result, Tuple BindAttrs>
    query_relation<Result, BindAttrs>& query_relation<Result, BindAttrs>::limit(const std::size_t lim) && {
        this->query_options.push_back(active_record::string{ " LIMIT " } + std::to_string(lim));
        return *this;
    }
    template<Container Result, Tuple BindAttrs>
    query_relation<Result, BindAttrs> query_relation<Result, BindAttrs>::limit(const std::size_t lim) const& {
        query_relation<Result, BindAttrs> ret;
        
        ret.operation = this->operation;
        ret.query_op_arg = this->query_op_arg;
        ret.query_table = this->query_table;
        ret.query_condition = this->query_condition;
        ret.query_options = this->query_options;
        ret.query_options.push_back(active_record::string{ " LIMIT " } + std::to_string(lim));
        ret.temporary_attrs = this->temporary_attrs;
        detail::set_bind_attrs_ptr<0>(ret.bind_attrs, ret.temporary_attrs);

        return ret;
    }

    template<Container Result, Tuple BindAttrs>
    template<Attribute Attr>
    query_relation<Result, BindAttrs>& query_relation<Result, BindAttrs>::order_by(const active_record::order order) && {
        this->query_options.push_back(
            active_record::string{ " ORDER BY " }
            + detail::column_full_names_to_string<Attr>()
            + (order == active_record::order::asc ? " ASC" : " DESC" )
        );

        return *this;
    }
    template<Container Result, Tuple BindAttrs>
    template<Attribute Attr>
    query_relation<Result, BindAttrs> query_relation<Result, BindAttrs>::order_by(const active_record::order order) const & {
        query_relation<Result, BindAttrs> ret;
        
        ret.operation = this->operation;
        ret.query_op_arg = this->query_op_arg;
        ret.query_table = this->query_table;
        ret.query_condition = this->query_condition;
        ret.temporary_attrs = this->temporary_attrs;
        ret.query_options = this->query_options;
        detail::set_bind_attrs_ptr<0>(ret.bind_attrs, ret.temporary_attrs);

        ret.query_options.push_back(
            active_record::string{ " ORDER BY " }
            + detail::column_full_names_to_string<Attr>()
            + (order == active_record::order::asc ? " ASC" : " DESC" )
        );

        return ret;
    }

    /*
     * raw query implementation
     */
    template<typename T>
    query_relation<T, std::tuple<>> raw_query(const active_record::string_view query_str) {
        query_relation<T, std::tuple<>> ret;
        ret.operation     = query_operation::unspecified,
        ret.query_op_arg.push_back(active_record::string{ query_str });

        return ret;
    }
}