#pragma once
/*
 * Copyright 2021 akisute514
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "../query_relation.hpp"
#include "../query_utils.hpp"
#include "../query_condition_impl.hpp"

namespace active_record {
    /*
     * return type == Container<model or Tuple>
     */

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::vector<typename Result::value_type>>
    template<Attribute... Attrs>
    query_relation<std::vector<std::tuple<Attrs...>>, BindAttrs> query_relation<Result, BindAttrs>::select() const & {
        query_relation<std::vector<std::tuple<Attrs...>>, BindAttrs> ret{ query_operation::select };

        ret.op_args.push_back(detail::column_full_names_to_string<Attrs...>());
        ret.tables = this->tables;

        ret.conditions = this->conditions;
        ret.options = this->options;
        ret.bind_attrs = this->bind_attrs;
        return ret;
    }
    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::vector<typename Result::value_type>>
    template<Attribute... Attrs>
    query_relation<std::vector<std::tuple<Attrs...>>, BindAttrs> query_relation<Result, BindAttrs>::select() && {
        query_relation<std::vector<std::tuple<Attrs...>>, BindAttrs> ret{ query_operation::select };

        ret.op_args.push_back(detail::column_full_names_to_string<Attrs...>());
        ret.tables = std::move(this->tables);

        ret.conditions = std::move(this->conditions);
        ret.options = std::move(this->options);
        ret.bind_attrs = std::move(this->bind_attrs);
        return ret;
    }

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::vector<typename Result::value_type>>
    template<AttributeAggregator... Attrs>
    query_relation<std::tuple<typename Attrs::attribute_type...>, BindAttrs> query_relation<Result, BindAttrs>::select() const & {
        query_relation<std::tuple<typename Attrs::attribute_type...>, BindAttrs> ret{ query_operation::select };

        ret.op_args.push_back(detail::column_full_names_to_string<Attrs...>());
        ret.tables = this->tables;

        ret.conditions = this->conditions;
        ret.options = this->options;
        ret.bind_attrs = this->bind_attrs;
        return ret;
    }
    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::vector<typename Result::value_type>>
    template<AttributeAggregator... Attrs>
    query_relation<std::tuple<typename Attrs::attribute_type...>, BindAttrs> query_relation<Result, BindAttrs>::select() && {
        query_relation<std::tuple<typename Attrs::attribute_type...>, BindAttrs> ret{ query_operation::select };

        ret.op_args.push_back(detail::column_full_names_to_string<Attrs...>());
        ret.tables = std::move(this->tables);

        ret.conditions = std::move(this->conditions);
        ret.options = std::move(this->options);
        ret.bind_attrs = std::move(this->bind_attrs);
        return ret;
    }

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::vector<typename Result::value_type>>
    template<Attribute Attr>
    query_relation<std::vector<Attr>, BindAttrs> query_relation<Result, BindAttrs>::pluck() const & {
        query_relation<std::vector<Attr>, BindAttrs> ret{ query_operation::select };

        ret.op_args.push_back(detail::column_full_names_to_string<Attr>());
        ret.tables = this->tables;

        ret.conditions = this->conditions;
        ret.options = this->options;
        ret.bind_attrs = this->bind_attrs;
        return ret;
    }
    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::vector<typename Result::value_type>>
    template<Attribute Attr>
    query_relation<std::vector<Attr>, BindAttrs> query_relation<Result, BindAttrs>::pluck() && {
        query_relation<std::vector<Attr>, BindAttrs> ret{ query_operation::select };

        ret.op_args.push_back(detail::column_full_names_to_string<Attr>());
        ret.tables = std::move(this->tables);

        ret.conditions = std::move(this->conditions);
        ret.options = std::move(this->options);
        ret.bind_attrs = std::move(this->bind_attrs);
        return ret;
    }

    namespace detail {
        template<std::size_t N, typename Query, Attribute Last>
        void set_update_op_args(Query& query, Last&& last) {
            query.op_args.push_back(concat_strings("\"", Last::column_name, "\" = "));
            query.op_args.push_back(N);
            std::get<N>(query.bind_attrs) = std::move(last);
        }
        template<std::size_t N, typename Query, Attribute Head, Attribute... Tails>
        void set_update_op_args(Query& query, Head&& head, Tails&&... tails) {
            set_update_op_args<N>(query, std::move(head));
            query.op_args.push_back(",");
            set_update_op_args<N + 1>(query, std::move(tails)...);
        }
    }

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::vector<typename Result::value_type>>
    template<Attribute... Attrs>
    requires Model<typename Result::value_type>
    query_relation<bool, tuptup::tuple_cat_t<BindAttrs, std::tuple<Attrs...>>> query_relation<Result, BindAttrs>::update(const Attrs&... attrs) {
        query_relation<bool, tuptup::tuple_cat_t<BindAttrs, std::tuple<Attrs...>>> ret{ query_operation::update };

        ret.tables.push_back(concat_strings("\"", Result::value_type::table_name, "\""));
        ret.conditions = std::move(this->conditions);
        ret.options = std::move(this->options);
        ret.bind_attrs = std::tuple_cat(std::move(this->bind_attrs), std::make_tuple(attrs...));
        detail::set_update_op_args<std::tuple_size_v<BindAttrs>>(ret, std::move(attrs)...);
        return ret;
    }

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::vector<typename Result::value_type>>
    template<Attribute Attr>
    query_relation<Result, tuptup::tuple_cat_t<BindAttrs, std::tuple<Attr>>> query_relation<Result, BindAttrs>::where(const Attr& attr) && {
        return std::move(*this).where(Attr::cmp == attr);
    }
    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::vector<typename Result::value_type>>
    template<Attribute Attr>
    query_relation<Result, tuptup::tuple_cat_t<BindAttrs, std::tuple<Attr>>> query_relation<Result, BindAttrs>::where(const Attr& attr) const& {
        return this->where(Attr::cmp == attr);
    }

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::vector<typename Result::value_type>>
    template<Tuple SrcBindAttrs>
    query_relation<Result, tuptup::tuple_cat_t<BindAttrs, SrcBindAttrs>> query_relation<Result, BindAttrs>::where(query_condition<SrcBindAttrs>&& cond) &&{
        query_relation<Result, tuptup::tuple_cat_t<BindAttrs, SrcBindAttrs>> ret{ this->operation };
        ret.op_args = std::move(this->op_args);
        ret.tables = std::move(this->tables);
        if(!this->conditions.empty()){
            ret.conditions = std::move(this->conditions);
            ret.conditions.reserve(ret.conditions.size() + cond.condition.size() + 1);
            ret.conditions.push_back(" AND ");
        }
        else ret.conditions.reserve(cond.condition.size());
        struct {
            decltype(ret.conditions)& ret_cond;
            void operator()(active_record::string&& str) { ret_cond.push_back(std::move(str)); }
            void operator()(std::size_t idx) { ret_cond.push_back(idx + std::tuple_size_v<BindAttrs>); }
        } visitor{ ret.conditions };
        for(auto& cond : cond.condition){
            std::visit(visitor, std::move(cond));
        }

        ret.options = std::move(this->options);
        ret.bind_attrs = std::tuple_cat(std::move(this->bind_attrs), std::move(cond.bind_attrs));
        return ret;
    }

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::vector<typename Result::value_type>>
    template<Tuple SrcBindAttrs>
    query_relation<Result, tuptup::tuple_cat_t<BindAttrs, SrcBindAttrs>> query_relation<Result, BindAttrs>::where(query_condition<SrcBindAttrs>&& cond) const&{
        query_relation<Result, tuptup::tuple_cat_t<BindAttrs, SrcBindAttrs>> ret{ this->operation };
        ret.op_args = this->op_args;
        ret.tables = this->tables;
        ret.conditions.reserve(this->conditions.size() + cond.condition.size() + !this->conditions.empty());
        if(!this->conditions.empty()){
            ret.conditions = this->conditions;
            ret.conditions.push_back(" AND ");
        }
        struct {
            decltype(ret.conditions)& ret_cond;
            void operator()(active_record::string&& str) { ret_cond.push_back(std::move(str)); }
            void operator()(std::size_t idx) { ret_cond.push_back(idx + std::tuple_size_v<BindAttrs>); }
        } visitor{ ret.conditions };
        for(auto& cond : cond.condition){
            std::visit(visitor, std::move(cond));
        }
        ret.options = this->options;
        ret.bind_attrs = std::tuple_cat(this->bind_attrs, std::move(cond.bind_attrs));
        return ret;
    }

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::vector<typename Result::value_type>>
    query_relation<Result, BindAttrs>& query_relation<Result, BindAttrs>::limit(const std::size_t lim) && {
        this->options.push_back(concat_strings(" LIMIT ", std::to_string(lim)));
        return *this;
    }
    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::vector<typename Result::value_type>>
    query_relation<Result, BindAttrs> query_relation<Result, BindAttrs>::limit(const std::size_t lim) const& {
        query_relation<Result, BindAttrs> ret{ this->operation };

        ret.op_args = this->op_args;
        ret.tables = this->tables;
        ret.conditions = this->conditions;
        ret.options = this->options;
        ret.options.push_back(concat_strings(" LIMIT ", std::to_string(lim)));
        ret.bind_attrs = this->bind_attrs;

        return ret;
    }

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::vector<typename Result::value_type>>
    template<Attribute Attr>
    query_relation<Result, BindAttrs>& query_relation<Result, BindAttrs>::order_by(const active_record::order order) && {
        this->options.push_back(concat_strings(
            " ORDER BY ", detail::column_full_names_to_string<Attr>(),
            order == active_record::order::asc ? " ASC" : " DESC"
        ));

        return *this;
    }
    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::vector<typename Result::value_type>>
    template<Attribute Attr>
    query_relation<Result, BindAttrs> query_relation<Result, BindAttrs>::order_by(const active_record::order order) const & {
        query_relation<Result, BindAttrs> ret{ this->operation };

        ret.op_args = this->op_args;
        ret.tables = this->tables;
        ret.conditions = this->conditions;
        ret.bind_attrs = this->bind_attrs;
        ret.options = this->options;

        ret.options.push_back(concat_strings(
            " ORDER BY ", detail::column_full_names_to_string<Attr>(),
            order == active_record::order::asc ? " ASC" : " DESC"
        ));

        return ret;
    }

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::vector<typename Result::value_type>>
    query_relation<std::size_t, BindAttrs> query_relation<Result, BindAttrs>::count() && {
        query_relation<std::size_t, BindAttrs> ret{ query_operation::select };

        ret.op_args.push_back("count(*)");
        ret.tables = std::move(this->tables);
        ret.conditions = std::move(this->conditions);
        ret.bind_attrs = std::move(this->bind_attrs);
        ret.options = std::move(this->options);

        return ret;
    }
    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::vector<typename Result::value_type>>
    query_relation<std::size_t, BindAttrs> query_relation<Result, BindAttrs>::count() const& {
        query_relation<std::size_t, BindAttrs> ret{ query_operation::select };

        ret.op_args.push_back("count(*)");
        ret.tables = this->tables;
        ret.conditions = this->conditions;
        ret.bind_attrs = this->bind_attrs;
        ret.options = this->options;

        return ret;
    }

    namespace detail {
        template<typename Result, Tuple BindAttrs, AttributeAggregator T>
        query_relation<typename T::attribute_type, BindAttrs> vectored_aggregate_query(const query_relation<Result, BindAttrs>& src) {
            query_relation<typename T::attribute_type, BindAttrs> ret{ query_operation::select };

            ret.op_args.push_back(T::column_full_name());
            ret.tables = src.tables;
            ret.conditions = src.conditions;
            ret.bind_attrs = src.bind_attrs;
            ret.options = src.options;

            return ret;
        }

        template<typename Result, Tuple BindAttrs, AttributeAggregator T>
        query_relation<typename T::attribute_type, BindAttrs> vectored_aggregate_query(query_relation<Result, BindAttrs>&& src) {
            query_relation<typename T::attribute_type, BindAttrs> ret{ query_operation::select };

            ret.op_args.push_back(T::column_full_name());
            ret.tables = std::move(src.tables);
            ret.conditions = std::move(src.conditions);
            ret.bind_attrs = std::move(src.bind_attrs);
            ret.options = std::move(src.options);

            return ret;
        }
    }

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::vector<typename Result::value_type>>
    template<Attribute Attr>
    requires requires{ typename Attr::sum; }
    query_relation<typename Attr::sum::attribute_type, BindAttrs> query_relation<Result, BindAttrs>::sum() && {
        return detail::vectored_aggregate_query<Result, BindAttrs, typename Attr::sum>(
            std::move(*this)
        );
    }
    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::vector<typename Result::value_type>>
    template<Attribute Attr>
    requires requires{ typename Attr::sum; }
    query_relation<typename Attr::sum::attribute_type, BindAttrs> query_relation<Result, BindAttrs>::sum() const& {
        return detail::vectored_aggregate_query<Result, BindAttrs, typename Attr::sum>(
            *this
        );
    }

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::vector<typename Result::value_type>>
    template<Attribute Attr>
    requires requires{ typename Attr::avg; }
    query_relation<typename Attr::avg::attribute_type, BindAttrs> query_relation<Result, BindAttrs>::avg() && {
        return detail::vectored_aggregate_query<Result, BindAttrs, typename Attr::avg>(
            std::move(*this)
        );
    }
    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::vector<typename Result::value_type>>
    template<Attribute Attr>
    requires requires{ typename Attr::avg; }
    query_relation<typename Attr::avg::attribute_type, BindAttrs> query_relation<Result, BindAttrs>::avg() const& {
        return detail::vectored_aggregate_query<Result, BindAttrs, typename Attr::avg>(
            *this
        );
    }

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::vector<typename Result::value_type>>
    template<Attribute Attr>
    requires requires{ typename Attr::max; }
    query_relation<typename Attr::max::attribute_type, BindAttrs> query_relation<Result, BindAttrs>::max() && {
        return detail::vectored_aggregate_query<Result, BindAttrs, typename Attr::max>(
            std::move(*this)
        );
    }
    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::vector<typename Result::value_type>>
    template<Attribute Attr>
    requires requires{ typename Attr::max; }
    query_relation<typename Attr::max::attribute_type, BindAttrs> query_relation<Result, BindAttrs>::max() const& {
        return detail::vectored_aggregate_query<Result, BindAttrs, typename Attr::max>(
            *this
        );
    }

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::vector<typename Result::value_type>>
    template<Attribute Attr>
    requires requires{ typename Attr::min; }
    query_relation<typename Attr::min::attribute_type, BindAttrs> query_relation<Result, BindAttrs>::min() && {
        return detail::vectored_aggregate_query<Result, BindAttrs, typename Attr::min>(
            std::move(*this)
        );
    }
    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::vector<typename Result::value_type>>
    template<Attribute Attr>
    requires requires{ typename Attr::min; }
    query_relation<typename Attr::min::attribute_type, BindAttrs> query_relation<Result, BindAttrs>::min() const& {
        return detail::vectored_aggregate_query<Result, BindAttrs, typename Attr::min>(
            *this
        );
    }
}