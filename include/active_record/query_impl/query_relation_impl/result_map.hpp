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
    query_relation<Result, active_record::tuple_cat_t<BindAttrs, std::tuple<const Attr*>>> query_relation<Result, BindAttrs>::where(const Attr& attr) && {
        return std::move(*this).where(Attr::cmp == attr);
    }
    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<Attribute Attr>
    query_relation<Result, active_record::tuple_cat_t<BindAttrs, std::tuple<const Attr*>>> query_relation<Result, BindAttrs>::where(const Attr& attr) const& {
        return this->where(Attr::cmp == attr);
    }

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<Tuple SrcBindAttrs>
    query_relation<Result, active_record::tuple_cat_t<BindAttrs, SrcBindAttrs>> query_relation<Result, BindAttrs>::where(query_condition<SrcBindAttrs>&& cond) &&{
        query_relation<Result, active_record::tuple_cat_t<BindAttrs, SrcBindAttrs>> ret;

        ret.operation = this->operation;
        ret.query_op_arg = std::move(this->query_op_arg);
        ret.query_table = std::move(this->query_table);

        if(!this->query_condition.empty()){
            ret.query_condition = std::move(this->query_condition);
            ret.query_condition.push_back(" AND ");
        }
        const struct {
            std::variant<active_record::string, std::size_t> operator()(active_record::string&& str) const { return std::move(str); }
            std::variant<active_record::string, std::size_t> operator()(std::size_t idx) const noexcept { return idx + std::tuple_size_v<BindAttrs>; }
        } visitor;
        for(auto& cond : cond.condition){
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
    query_relation<Result, active_record::tuple_cat_t<BindAttrs, SrcBindAttrs>> query_relation<Result, BindAttrs>::where(query_condition<SrcBindAttrs>&& cond) const&{
        query_relation<Result, active_record::tuple_cat_t<BindAttrs, SrcBindAttrs>> ret;

        ret.operation = this->operation;
        ret.query_op_arg = this->query_op_arg;
        ret.query_table = this->query_table;
        if(!this->query_condition.empty()){
            ret.query_condition = this->query_condition;
            ret.query_condition.push_back(" AND ");
        }
        const struct {
            std::variant<active_record::string, std::size_t> operator()(active_record::string&& str) const { return std::move(str); }
            std::variant<active_record::string, std::size_t> operator()(std::size_t idx) const noexcept { return idx + std::tuple_size_v<BindAttrs>; }
        } visitor;
        for(auto& cond : cond.condition){
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
        this->query_options.push_back(concat_strings(" LIMIT ", std::to_string(lim)));
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
        ret.query_options.push_back(concat_strings(" LIMIT ", std::to_string(lim)));
        ret.temporary_attrs = this->temporary_attrs;
        detail::set_bind_attrs_ptr<0>(ret.bind_attrs, ret.temporary_attrs);

        return ret;
    }

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<Attribute Attr>
    query_relation<Result, BindAttrs>& query_relation<Result, BindAttrs>::order_by(const active_record::order order) && {
        this->query_options.push_back(concat_strings(
            " ORDER BY ", detail::column_full_names_to_string<Attr>(),
            order == active_record::order::asc ? " ASC" : " DESC"
        ));

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

        ret.query_options.push_back(concat_strings(
            " ORDER BY ", detail::column_full_names_to_string<Attr>(),
            order == active_record::order::asc ? " ASC" : " DESC"
        ));

        return ret;
    }

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    query_relation<std::unordered_map<typename Result::key_type, std::size_t>, BindAttrs> query_relation<Result, BindAttrs>::count() && {
        query_relation<std::unordered_map<typename Result::key_type, std::size_t>, BindAttrs> ret;

        ret.operation = query_operation::select;
        ret.query_op_arg.push_back(detail::column_full_names_to_string<typename Result::key_type>() + ",count(*)");
        ret.query_table = std::move(this->query_table);

        ret.query_condition = std::move(this->query_condition);
        ret.query_options = std::move(this->query_options);
        ret.temporary_attrs = std::move(this->temporary_attrs);
        detail::set_bind_attrs_ptr<0>(ret.bind_attrs, ret.temporary_attrs);
        return ret;
    }
    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    query_relation<std::unordered_map<typename Result::key_type, std::size_t>, BindAttrs> query_relation<Result, BindAttrs>::count() const& {
        query_relation<std::unordered_map<typename Result::key_type, std::size_t>, BindAttrs> ret;

        ret.operation = query_operation::select;
        ret.query_op_arg.push_back(detail::column_full_names_to_string<typename Result::key_type>() + ",count(*)");
        ret.query_table = this->query_table;

        ret.query_condition = this->query_condition;
        ret.query_options = this->query_options;
        ret.temporary_attrs = this->temporary_attrs;
        detail::set_bind_attrs_ptr<0>(ret.bind_attrs, ret.temporary_attrs);
        return ret;
    }

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<Attribute Attr>
    requires requires{ typename Attr::sum; }
    query_relation<std::unordered_map<typename Result::key_type, typename Attr::sum::attribute_type>, BindAttrs> query_relation<Result, BindAttrs>::sum() && {
        return std::move(*this).template pluck<typename Attr::sum>();
    }

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<Attribute Attr>
    requires requires{ typename Attr::sum; }
    query_relation<std::unordered_map<typename Result::key_type, typename Attr::sum::attribute_type>, BindAttrs> query_relation<Result, BindAttrs>::sum() const& {
        return this->pluck<typename Attr::sum>();
    }

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<Attribute Attr>
    requires requires{ typename Attr::avg; }
    query_relation<std::unordered_map<typename Result::key_type, typename Attr::avg::attribute_type>, BindAttrs> query_relation<Result, BindAttrs>::avg() && {
        return std::move(*this).template pluck<typename Attr::avg>();
    }

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<Attribute Attr>
    requires requires{ typename Attr::avg; }
    query_relation<std::unordered_map<typename Result::key_type, typename Attr::avg::attribute_type>, BindAttrs> query_relation<Result, BindAttrs>::avg() const& {
        return this->pluck<typename Attr::avg>();
    }

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<Attribute Attr>
    requires requires{ typename Attr::max; }
    query_relation<std::unordered_map<typename Result::key_type, typename Attr::max::attribute_type>, BindAttrs> query_relation<Result, BindAttrs>::max() && {
        return std::move(*this).template pluck<typename Attr::max>();
    }

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<Attribute Attr>
    requires requires{ typename Attr::max; }
    query_relation<std::unordered_map<typename Result::key_type, typename Attr::max::attribute_type>, BindAttrs> query_relation<Result, BindAttrs>::max() const& {
        return this->pluck<typename Attr::max>();
    }

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<Attribute Attr>
    requires requires{ typename Attr::min; }
    query_relation<std::unordered_map<typename Result::key_type, typename Attr::min::attribute_type>, BindAttrs> query_relation<Result, BindAttrs>::min() && {
        return std::move(*this).template pluck<typename Attr::min>();
    }

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<Attribute Attr>
    requires requires{ typename Attr::min; }
    query_relation<std::unordered_map<typename Result::key_type, typename Attr::min::attribute_type>, BindAttrs> query_relation<Result, BindAttrs>::min() const& {
        return this->pluck<typename Attr::min>();
    }

}