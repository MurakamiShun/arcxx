#pragma once
#include "../query_relation.hpp"
#include "../query_utils.hpp"
#include "../query_condition_impl.hpp"

namespace active_record {
    /*
     * return type == std::unordered_map<Attribute, Tuple or Attribute>
     */

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<AttributeAggregator... Attrs>
    query_relation<std::unordered_map<typename Result::key_type, std::tuple<typename Attrs::attribute_type...>>, BindAttrs> query_relation<Result, BindAttrs>::select() const & {
        query_relation<std::unordered_map<typename Result::key_type, std::tuple<typename Attrs::attribute_type...>>, BindAttrs> ret;
        
        ret.operation = query_operation::select;
        ret.query_op_arg.push_back(detail::column_full_names_to_string<typename Result::key_type, Attrs...>());
        ret.query_table = this->query_table;

        ret.query_condition = this->query_condition;
        ret.query_options = this->query_options;
        ret.temporary_attrs = this->temporary_attrs;
        detail::set_bind_attrs_ptr<0>(ret.bind_attrs, ret.temporary_attrs);
        return ret;
    }
    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<AttributeAggregator... Attrs>
    query_relation<std::unordered_map<typename Result::key_type, std::tuple<typename Attrs::attribute_type...>>, BindAttrs> query_relation<Result, BindAttrs>::select() && {
        query_relation<std::unordered_map<typename Result::key_type, std::tuple<typename Attrs::attribute_type...>>, BindAttrs> ret;

        ret.operation = query_operation::select;
        ret.query_op_arg.push_back(detail::column_full_names_to_string<typename Result::key_type, Attrs...>());
        ret.query_table = std::move(this->query_table);

        ret.query_condition = std::move(this->query_condition);
        ret.query_options = std::move(this->query_options);
        ret.temporary_attrs = std::move(this->temporary_attrs);
        detail::set_bind_attrs_ptr<0>(ret.bind_attrs, ret.temporary_attrs);
        return ret;
    }

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<AttributeAggregator Attr>
    query_relation<std::unordered_map<typename Result::key_type, typename Attr::attribute_type>, BindAttrs> query_relation<Result, BindAttrs>::pluck() const & {
        query_relation<std::unordered_map<typename Result::key_type, typename Attr::attribute_type>, BindAttrs> ret;
        
        ret.operation = query_operation::select;
        ret.query_op_arg.push_back(detail::column_full_names_to_string<typename Result::key_type, Attr>());
        ret.query_table = this->query_table;

        ret.query_condition = this->query_condition;
        ret.query_options = this->query_options;
        ret.temporary_attrs = this->temporary_attrs;
        detail::set_bind_attrs_ptr<0>(ret.bind_attrs, ret.temporary_attrs);
        return ret;
    }
    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<AttributeAggregator Attr>
    query_relation<std::unordered_map<typename Result::key_type, typename Attr::attribute_type>, BindAttrs> query_relation<Result, BindAttrs>::pluck() && {
        query_relation<std::unordered_map<typename Result::key_type, typename Attr::attribute_type>, BindAttrs> ret;
        
        ret.operation = query_operation::select;
        ret.query_op_arg.push_back(detail::column_full_names_to_string<typename Result::key_type, Attr>());
        ret.query_table = std::move(this->query_table);

        ret.query_condition = std::move(this->query_condition);
        ret.query_options = std::move(this->query_options);
        ret.temporary_attrs = std::move(this->temporary_attrs);
        detail::set_bind_attrs_ptr<0>(ret.bind_attrs, ret.temporary_attrs);
        return ret;
    }

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<Attribute Attr>
    query_relation<Result, std::invoke_result_t<decltype(std::tuple_cat<BindAttrs, std::tuple<const Attr*>>), BindAttrs, std::tuple<const Attr*>>> query_relation<Result, BindAttrs>::where(const Attr& attr) && {
        return std::move(*this).where(attr.to_equ_condition());
    }
    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<Attribute Attr>
    query_relation<Result, std::invoke_result_t<decltype(std::tuple_cat<BindAttrs, std::tuple<const Attr*>>), BindAttrs, std::tuple<const Attr*>>> query_relation<Result, BindAttrs>::where(const Attr& attr) const& {
        return this->where(attr.to_equ_condition());
    }

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<Tuple SrcBindAttrs>
    query_relation<Result, std::invoke_result_t<decltype(std::tuple_cat<BindAttrs, SrcBindAttrs>), BindAttrs, SrcBindAttrs>> query_relation<Result, BindAttrs>::where(query_condition<SrcBindAttrs>&& cond) &&{
        query_relation<Result, std::invoke_result_t<decltype(std::tuple_cat<BindAttrs, SrcBindAttrs>), BindAttrs, SrcBindAttrs>> ret;
        
        ret.operation = this->operation;
        ret.query_op_arg = std::move(this->query_op_arg);
        ret.query_table = std::move(this->query_table);

        if(!this->query_condition.empty()){
            ret.query_condition = std::move(this->query_condition);
            ret.query_condition.push_back(" AND ");
        }
        for(auto& cond : cond.condition){
            struct {
                std::variant<active_record::string, std::size_t> operator()(active_record::string&& str){ return std::move(str); }
                std::variant<active_record::string, std::size_t> operator()(std::size_t idx){ return idx + std::tuple_size_v<BindAttrs>; }
            } visitor;
            
            ret.query_condition.push_back(
                std::visit(visitor, std::move(cond))
            );
        }

        ret.query_options = std::move(this->query_options);
        ret.temporary_attrs = std::move(this->temporary_attrs);
        ret.temporary_attrs.insert(
            ret.temporary_attrs.end(),
            std::make_move_iterator(cond.temporary_attrs.begin()),
            std::make_move_iterator(cond.temporary_attrs.end())
        );
        detail::set_bind_attrs_ptr<0>(ret.bind_attrs, ret.temporary_attrs);
        return ret;
    }
    
    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<Tuple SrcBindAttrs>
    query_relation<Result, std::invoke_result_t<decltype(std::tuple_cat<BindAttrs, SrcBindAttrs>), BindAttrs, SrcBindAttrs>> query_relation<Result, BindAttrs>::where(query_condition<SrcBindAttrs>&& cond) const&{
        query_relation<Result, std::invoke_result_t<decltype(std::tuple_cat<BindAttrs, SrcBindAttrs>), BindAttrs, SrcBindAttrs>> ret;
        
        ret.operation = this->operation;
        ret.query_op_arg = this->query_op_arg;
        ret.query_table = this->query_table;
        if(!this->query_condition.empty()){
            ret.query_condition = this->query_condition;
            ret.query_condition.push_back(" AND ");
        }
        for(auto& cond : cond.condition){
            struct {
                std::variant<active_record::string, std::size_t> operator()(active_record::string&& str){ return std::move(str); }
                std::variant<active_record::string, std::size_t> operator()(std::size_t idx){ return idx + std::tuple_size_v<BindAttrs>; }
            } visitor;
            
            ret.query_condition.push_back(
                std::visit(visitor, std::move(cond))
            );
        }
        ret.query_options = this->query_options;
        ret.temporary_attrs = this->temporary_attrs;
        ret.temporary_attrs.insert(
            ret.temporary_attrs.end(),
            std::make_move_iterator(cond.temporary_attrs.begin()),
            std::make_move_iterator(cond.temporary_attrs.end())
        );
        detail::set_bind_attrs_ptr<0>(ret.bind_attrs, ret.temporary_attrs);
        return ret;
    }

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    query_relation<Result, BindAttrs>& query_relation<Result, BindAttrs>::limit(const std::size_t lim) && {
        this->query_options.push_back(active_record::string{ " LIMIT " } + std::to_string(lim));
        return *this;
    }
    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
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

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<Attribute Attr>
    query_relation<Result, BindAttrs>& query_relation<Result, BindAttrs>::order_by(const active_record::order order) && {
        this->query_options.push_back(
            active_record::string{ " ORDER BY " }
            + detail::column_full_names_to_string<Attr>()
            + (order == active_record::order::asc ? " ASC" : " DESC" )
        );

        return *this;
    }
    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
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

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    query_relation<std::unordered_map<typename Result::key_type, std::size_t>, BindAttrs> query_relation<Result, BindAttrs>::count() && {
        return detail::aggregate_query<std::size_t>(std::move(*this), "count(*)");
    }
    template<typename Result, Tuple BindAttrs>
    requires std::same_as<std::unordered_map<Result::key_type, std::size_t>, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    query_relation<std::size_t, BindAttrs> query_relation<Result, BindAttrs>::count() const& {
        return detail::aggregate_query<std::size_t>(*this, "count(*)");
    }

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<Attribute Attr>
    requires std::integral<typename Attr::value_type> || std::floating_point<typename Attr::value_type>
    query_relation<Attr, BindAttrs> query_relation<Result, BindAttrs>::sum() && {
        return detail::aggregate_query<Attr>(
            std::move(*this),
            active_record::string{ "sum(" } + detail::column_full_names_to_string<Attr>() +")"
        );
    }
    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<Attribute Attr>
    requires std::integral<typename Attr::value_type> || std::floating_point<typename Attr::value_type>
    query_relation<Attr, BindAttrs> query_relation<Result, BindAttrs>::sum() const& {
        return detail::aggregate_query<Attr>(
            *this,
            active_record::string{ "sum(" } + detail::column_full_names_to_string<Attr>() +")"
        );
    }

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<Attribute Attr>
    requires std::integral<typename Attr::value_type> || std::floating_point<typename Attr::value_type>
    query_relation<Attr, BindAttrs> query_relation<Result, BindAttrs>::avg() && {
        return detail::aggregate_query<Attr>(
            std::move(*this),
            active_record::string{ "avg(" } + detail::column_full_names_to_string<Attr>() +")"
        );
    }
    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<Attribute Attr>
    requires std::integral<typename Attr::value_type> || std::floating_point<typename Attr::value_type>
    query_relation<Attr, BindAttrs> query_relation<Result, BindAttrs>::avg() const& {
        return detail::aggregate_query<Attr>(
            *this,
            active_record::string{ "avg(" } + detail::column_full_names_to_string<Attr>() +")"
        );
    }

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<Attribute Attr>
    requires std::integral<typename Attr::value_type> || std::floating_point<typename Attr::value_type>
    query_relation<Attr, BindAttrs> query_relation<Result, BindAttrs>::max() && {
        return detail::aggregate_query<Attr>(
            std::move(*this),
            active_record::string{ "max(" } + detail::column_full_names_to_string<Attr>() +")"
        );
    }
    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<Attribute Attr>
    requires std::integral<typename Attr::value_type> || std::floating_point<typename Attr::value_type>
    query_relation<Attr, BindAttrs> query_relation<Result, BindAttrs>::max() const& {
        return detail::aggregate_query<Attr>(
            *this,
            active_record::string{ "max(" } + detail::column_full_names_to_string<Attr>() +")"
        );
    }

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<Attribute Attr>
    requires std::integral<typename Attr::value_type> || std::floating_point<typename Attr::value_type>
    query_relation<Attr, BindAttrs> query_relation<Result, BindAttrs>::min() && {
        return detail::aggregate_query<Attr>(
            std::move(*this),
            active_record::string{ "min(" } + detail::column_full_names_to_string<Attr>() +")"
        );
    }
    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<Attribute Attr>
    requires std::integral<typename Attr::value_type> || std::floating_point<typename Attr::value_type>
    query_relation<Attr, BindAttrs> query_relation<Result, BindAttrs>::min() const& {
        return detail::aggregate_query<Attr>(
            *this,
            active_record::string{ "min(" } + detail::column_full_names_to_string<Attr>() +")"
        );
    }
    */
}