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

namespace active_record {
    /*
     * return type == std::unordered_map<Attribute, specialized_from<std::tuple> or Attribute>
     */

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    struct query_relation<Result, BindAttrs> : public query_relation_common<BindAttrs> {
        using mapped_type = typename Result::mapped_type;

        using query_relation_common<BindAttrs>::query_relation_common;

        template<std::derived_from<adaptor> Adaptor>
        [[nodiscard]] auto exec(Adaptor& adapt) const {
            return adapt.exec(*this);
        }

        template<AttributeAggregator... Attrs>
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, std::tuple<typename Attrs::attribute_type...>>, BindAttrs> select() &&;
        template<AttributeAggregator... Attrs>
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, std::tuple<typename Attrs::attribute_type...>>, BindAttrs> select() const &;

        template<AttributeAggregator Attr>
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, typename Attr::attribute_type>, BindAttrs> pluck() &&;
        template<AttributeAggregator Attr>
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, typename Attr::attribute_type>, BindAttrs> pluck() const &;

        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, tuptup::tuple_cat_t<BindAttrs, std::tuple<Attr>>> where(const Attr&) &&;
        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, tuptup::tuple_cat_t<BindAttrs, std::tuple<Attr>>> where(const Attr&) const &;

        template<specialized_from<std::tuple> SrcBindAttrs>
        [[nodiscard]] query_relation<Result, tuptup::tuple_cat_t<BindAttrs, SrcBindAttrs>> where(query_condition<SrcBindAttrs>&&) &&;
        template<specialized_from<std::tuple> SrcBindAttrs>
        [[nodiscard]] query_relation<Result, tuptup::tuple_cat_t<BindAttrs, SrcBindAttrs>> where(query_condition<SrcBindAttrs>&&) const&;

        [[nodiscard]] query_relation<Result, BindAttrs> limit(const std::size_t) &&;
        [[nodiscard]] query_relation<Result, BindAttrs> limit(const std::size_t) const &;

        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, BindAttrs> order_by(const active_record::order = active_record::order::asc) &&;
        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, BindAttrs> order_by(const active_record::order = active_record::order::asc) const &;

        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, std::size_t>, BindAttrs> count() &&;
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, std::size_t>, BindAttrs> count() const&;

        template<Attribute Attr>
        requires requires{ typename Attr::sum; }
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, typename Attr::sum::attribute_type>, BindAttrs> sum() &&;
        template<Attribute Attr>
        requires requires{ typename Attr::sum; }
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, typename Attr::sum::attribute_type>, BindAttrs> sum() const&;

        template<Attribute Attr>
        requires requires{ typename Attr::avg; }
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, typename Attr::avg::attribute_type>, BindAttrs> avg() &&;
        template<Attribute Attr>
        requires requires{ typename Attr::avg; }
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, typename Attr::avg::attribute_type>, BindAttrs> avg() const&;

        template<Attribute Attr>
        requires requires{ typename Attr::max; }
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, typename Attr::max::attribute_type>, BindAttrs> max() &&;
        template<Attribute Attr>
        requires requires{ typename Attr::max; }
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, typename Attr::max::attribute_type>, BindAttrs> max() const&;

        template<Attribute Attr>
        requires requires{ typename Attr::min; }
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, typename Attr::min::attribute_type>, BindAttrs> min() &&;
        template<Attribute Attr>
        requires requires{ typename Attr::min; }
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, typename Attr::min::attribute_type>, BindAttrs> min() const&;
    };

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<AttributeAggregator... Attrs>
    query_relation<std::unordered_map<typename Result::key_type, std::tuple<typename Attrs::attribute_type...>>, BindAttrs> query_relation<Result, BindAttrs>::select() const & {
        query_relation<std::unordered_map<typename Result::key_type, std::tuple<typename Attrs::attribute_type...>>, BindAttrs> ret{ query_operation::select };
        ret.op_args.push_back(detail::column_full_names_to_string<typename Result::key_type, Attrs...>());
        ret.tables = this->tables;

        ret.conditions = this->conditions;
        ret.options = this->options;
        ret.bind_attrs = this->bind_attrs;
        return ret;
    }
    template<typename Result, specialized_from<std::tuple> BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<AttributeAggregator... Attrs>
    query_relation<std::unordered_map<typename Result::key_type, std::tuple<typename Attrs::attribute_type...>>, BindAttrs> query_relation<Result, BindAttrs>::select() && {
        query_relation<std::unordered_map<typename Result::key_type, std::tuple<typename Attrs::attribute_type...>>, BindAttrs> ret{ query_operation::select };
        ret.op_args.push_back(detail::column_full_names_to_string<typename Result::key_type, Attrs...>());
        ret.tables = std::move(this->tables);

        ret.conditions = std::move(this->conditions);
        ret.options = std::move(this->options);
        ret.bind_attrs = std::move(this->bind_attrs);
        return ret;
    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<AttributeAggregator Attr>
    query_relation<std::unordered_map<typename Result::key_type, typename Attr::attribute_type>, BindAttrs> query_relation<Result, BindAttrs>::pluck() const & {
        query_relation<std::unordered_map<typename Result::key_type, typename Attr::attribute_type>, BindAttrs> ret{ query_operation::select };
        ret.op_args.push_back(detail::column_full_names_to_string<typename Result::key_type, Attr>());
        ret.tables = this->tables;

        ret.conditions = this->conditions;
        ret.options = this->options;
        ret.bind_attrs = this->bind_attrs;
        return ret;
    }
    template<typename Result, specialized_from<std::tuple> BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<AttributeAggregator Attr>
    query_relation<std::unordered_map<typename Result::key_type, typename Attr::attribute_type>, BindAttrs> query_relation<Result, BindAttrs>::pluck() && {
        query_relation<std::unordered_map<typename Result::key_type, typename Attr::attribute_type>, BindAttrs> ret{ query_operation::select };
        ret.op_args.push_back(detail::column_full_names_to_string<typename Result::key_type, Attr>());
        ret.tables = std::move(this->tables);

        ret.conditions = std::move(this->conditions);
        ret.options = std::move(this->options);
        ret.bind_attrs = std::move(this->bind_attrs);
        return ret;
    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<Attribute Attr>
    query_relation<Result, tuptup::tuple_cat_t<BindAttrs, std::tuple<Attr>>> query_relation<Result, BindAttrs>::where(const Attr& attr) && {
        return std::move(*this).where(Attr::cmp == attr);
    }
    template<typename Result, specialized_from<std::tuple> BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<Attribute Attr>
    query_relation<Result, tuptup::tuple_cat_t<BindAttrs, std::tuple<Attr>>> query_relation<Result, BindAttrs>::where(const Attr& attr) const& {
        return this->where(Attr::cmp == attr);
    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<specialized_from<std::tuple> SrcBindAttrs>
    query_relation<Result, tuptup::tuple_cat_t<BindAttrs, SrcBindAttrs>> query_relation<Result, BindAttrs>::where(query_condition<SrcBindAttrs>&& cond) &&{
        query_relation<Result, tuptup::tuple_cat_t<BindAttrs, SrcBindAttrs>> ret{ this->operation };
        ret.op_args = std::move(this->op_args);
        ret.tables = std::move(this->tables);

        if(!this->conditions.empty()){
            ret.conditions = std::move(this->conditions);
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

        ret.options = std::move(this->options);
        ret.bind_attrs = std::tuple_cat(std::move(this->bind_attrs), std::move(cond.bind_attrs));
        return ret;
    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<specialized_from<std::tuple> SrcBindAttrs>
    query_relation<Result, tuptup::tuple_cat_t<BindAttrs, SrcBindAttrs>> query_relation<Result, BindAttrs>::where(query_condition<SrcBindAttrs>&& cond) const&{
        query_relation<Result, tuptup::tuple_cat_t<BindAttrs, SrcBindAttrs>> ret{ this->operation };
        ret.op_args = this->op_args;
        ret.tables = this->tables;
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

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    query_relation<Result, BindAttrs> query_relation<Result, BindAttrs>::limit(const std::size_t lim) && {
        this->options.push_back(concat_strings(" LIMIT ", std::to_string(lim)));
        return *this;
    }
    template<typename Result, specialized_from<std::tuple> BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
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

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<Attribute Attr>
    query_relation<Result, BindAttrs> query_relation<Result, BindAttrs>::order_by(const active_record::order order) && {
        this->options.push_back(concat_strings(
            " ORDER BY ", detail::column_full_names_to_string<Attr>(),
            order == active_record::order::asc ? " ASC" : " DESC"
        ));

        return *this;
    }
    template<typename Result, specialized_from<std::tuple> BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
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

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    query_relation<std::unordered_map<typename Result::key_type, std::size_t>, BindAttrs> query_relation<Result, BindAttrs>::count() && {
        query_relation<std::unordered_map<typename Result::key_type, std::size_t>, BindAttrs> ret{ query_operation::select };
        ret.op_args.push_back(detail::column_full_names_to_string<typename Result::key_type>() + ",count(*)");
        ret.tables = std::move(this->tables);

        ret.conditions = std::move(this->conditions);
        ret.options = std::move(this->options);
        ret.bind_attrs = std::move(this->bind_attrs);
        return ret;
    }
    template<typename Result, specialized_from<std::tuple> BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    query_relation<std::unordered_map<typename Result::key_type, std::size_t>, BindAttrs> query_relation<Result, BindAttrs>::count() const& {
        query_relation<std::unordered_map<typename Result::key_type, std::size_t>, BindAttrs> ret{ query_operation::select };
        ret.op_args.push_back(detail::column_full_names_to_string<typename Result::key_type>() + ",count(*)");
        ret.tables = this->tables;

        ret.conditions = this->conditions;
        ret.options = this->options;
        ret.bind_attrs = this->bind_attrs;
        return ret;
    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<Attribute Attr>
    requires requires{ typename Attr::sum; }
    query_relation<std::unordered_map<typename Result::key_type, typename Attr::sum::attribute_type>, BindAttrs> query_relation<Result, BindAttrs>::sum() && {
        return std::move(*this).template pluck<typename Attr::sum>();
    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<Attribute Attr>
    requires requires{ typename Attr::sum; }
    query_relation<std::unordered_map<typename Result::key_type, typename Attr::sum::attribute_type>, BindAttrs> query_relation<Result, BindAttrs>::sum() const& {
        return this->pluck<typename Attr::sum>();
    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<Attribute Attr>
    requires requires{ typename Attr::avg; }
    query_relation<std::unordered_map<typename Result::key_type, typename Attr::avg::attribute_type>, BindAttrs> query_relation<Result, BindAttrs>::avg() && {
        return std::move(*this).template pluck<typename Attr::avg>();
    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<Attribute Attr>
    requires requires{ typename Attr::avg; }
    query_relation<std::unordered_map<typename Result::key_type, typename Attr::avg::attribute_type>, BindAttrs> query_relation<Result, BindAttrs>::avg() const& {
        return this->pluck<typename Attr::avg>();
    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<Attribute Attr>
    requires requires{ typename Attr::max; }
    query_relation<std::unordered_map<typename Result::key_type, typename Attr::max::attribute_type>, BindAttrs> query_relation<Result, BindAttrs>::max() && {
        return std::move(*this).template pluck<typename Attr::max>();
    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<Attribute Attr>
    requires requires{ typename Attr::max; }
    query_relation<std::unordered_map<typename Result::key_type, typename Attr::max::attribute_type>, BindAttrs> query_relation<Result, BindAttrs>::max() const& {
        return this->pluck<typename Attr::max>();
    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<Attribute Attr>
    requires requires{ typename Attr::min; }
    query_relation<std::unordered_map<typename Result::key_type, typename Attr::min::attribute_type>, BindAttrs> query_relation<Result, BindAttrs>::min() && {
        return std::move(*this).template pluck<typename Attr::min>();
    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    template<Attribute Attr>
    requires requires{ typename Attr::min; }
    query_relation<std::unordered_map<typename Result::key_type, typename Attr::min::attribute_type>, BindAttrs> query_relation<Result, BindAttrs>::min() const& {
        return this->pluck<typename Attr::min>();
    }

}