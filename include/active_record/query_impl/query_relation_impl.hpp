#pragma once
#include "query_relation.hpp"
#include "query_utils.hpp"
#include "query_condition_impl.hpp"

namespace active_record {
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

    template<Container Result, Tuple BindAttrs>
    query_relation<aggregate_attribute<std::size_t>, std::tuple<>> query_relation<Result, BindAttrs>::count() const {
        query_relation<aggregate_attribute<std::size_t>, std::tuple<>> ret;

        ret.operation = query_operation::select;
        ret.query_op_arg.push_back("count(*)");
        ret.query_table = this->query_table;
        ret.query_condition = this->query_condition;
        ret.temporary_attrs = this->temporary_attrs;
        ret.query_options = this->query_options;
        detail::set_bind_attrs_ptr<0>(ret.bind_attrs, ret.temporary_attrs);
        
        return ret;  
    }

    template<Container Result, Tuple BindAttrs>
    template<Attribute Attr>
    requires std::integral<typename Attr::value_type> || std::floating_point<typename Attr::value_type>
    query_relation<aggregate_attribute<typename Attr::value_type>, std::tuple<>> query_relation<Result, BindAttrs>::sum() const {
        query_relation<aggregate_attribute<typename Attr::value_type>, std::tuple<>> ret;

        ret.operation = query_operation::select;
        ret.query_op_arg.push_back("sum(" + detail::column_full_names_to_string<Attr>() +")");
        ret.query_table = this->query_table;
        ret.query_condition = this->query_condition;
        ret.temporary_attrs = this->temporary_attrs;
        ret.query_options = this->query_options;
        detail::set_bind_attrs_ptr<0>(ret.bind_attrs, ret.temporary_attrs);
        
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