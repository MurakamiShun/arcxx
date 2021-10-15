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
     * return type == Scalar
     */

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    struct query_relation : public query_relation_common<BindAttrs> {
        using query_relation_common<BindAttrs>::query_relation_common;
        template<std::derived_from<adaptor> Adaptor>
        [[nodiscard]] auto exec(Adaptor& adapt) const {
            return adapt.exec(*this);
        }

        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, tuptup::tuple_cat_t<BindAttrs, std::tuple<Attr>>> where(const Attr&) &&;
        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, tuptup::tuple_cat_t<BindAttrs, std::tuple<Attr>>> where(const Attr&) const &;

        template<specialized_from<std::tuple> SrcBindAttrs>
        [[nodiscard]] query_relation<Result, tuptup::tuple_cat_t<BindAttrs, SrcBindAttrs>> where(query_condition<SrcBindAttrs>&&) &&;
        template<specialized_from<std::tuple> SrcBindAttrs>
        [[nodiscard]] query_relation<Result, tuptup::tuple_cat_t<BindAttrs, SrcBindAttrs>> where(query_condition<SrcBindAttrs>&&) const&;
    };

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    template<Attribute Attr>
    query_relation<Result, tuptup::tuple_cat_t<BindAttrs, std::tuple<Attr>>> query_relation<Result, BindAttrs>::where(const Attr& attr) && {
        return std::move(*this).where(Attr::cmp == attr);
    }
    template<typename Result, specialized_from<std::tuple> BindAttrs>
    template<Attribute Attr>
    query_relation<Result, tuptup::tuple_cat_t<BindAttrs, std::tuple<Attr>>> query_relation<Result, BindAttrs>::where(const Attr& attr) const& {
        return this->where(Attr::cmp == attr);
    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
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
}